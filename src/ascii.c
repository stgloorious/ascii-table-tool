/**
 * @file ascii.c
 * @brief Creates tex code for an ascii table 
 * @author Stefan Gloor
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <argp.h>
#include <errno.h>
#include "ascii.h"

//Hard-coded maximum of 3 headings (hex,dec,char)
const unsigned int nheadings = 3;
const char *headings[3];

//Default values: 
const unsigned int wrap_default = 32;
const unsigned int start_default = 0;
const unsigned int end_default = 127;
char* filename_default = "ascii.tex";
char* dlabel_default = "Decimal";
char* hlabel_default = "Hex";
char* clabel_default = "Char";

static char doc[] = "A minimal tool to generate .tex code for ASCII tables";
const char *argp_program_version = "ascii-table-tool v0.1";
static struct argp_option options [] = {
	{"wrap", 'w', "n", 0, "Number of lines before the table wraps to the next column"},
	{"start", 's', "n", 0, "Character code to start with"},
	{"end", 'e', "n", 0, "Character code to end with"},
	{"output", 'o',"file",0,"Output filename, default is ascii.tex"},
	{"standalone",'a',0,0,"Creates standalone latex files"},
	{"hex", 'h', 0,0,"Only create the hexadecimal column"},
	{"dec", 'd', 0,0,"Only create the decimal column"},
	{"hlabel",1,"string",0,"Modify the hexadecimal label"},
	{"dlabel",2,"string",0,"Modify the decimal label"},
	{"clabel",3,"string",0,"Modify the char label"}
};

struct arguments{
	unsigned int wrap;
	unsigned int start;
	unsigned int end;
	char* filename;
	bool hex; //indicates hex-only mode
	bool dec; //indicates dec-only mode
	char *hlabel;
	char *dlabel;
	char *clabel;
	bool wrapSet; //indicates if wrap value has been set explicitly
	bool standalone;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state){
	struct arguments *arguments = state->input;
	switch(key){
		case 'w'://wrap value
			arguments->wrap	= strtol(arg,NULL,10);
			if (arguments->wrap <= 0 || arguments->wrap >= 128){
				argp_usage(state);
				return EINVAL;
			}
			arguments->wrapSet=true;
			break;
		case 's'://start value
			arguments->start = strtol(arg,NULL,10);
			if (arguments->start < 0 || arguments->start >= 128){
				argp_usage(state);
				return EINVAL;
			}	
			break;
		case 'e'://end value
			arguments->end = strtol(arg,NULL,10);
			if (arguments->end <= 0 || arguments->end >= 128){
				argp_usage(state);
				return EINVAL;
			}
			break;
		case 'o'://output file
			arguments->filename = arg;
			break;
		case 'a'://standalone mode
			arguments->standalone = true;
			break;
		case 'h'://hex-only flag
			arguments->hex=true;
			if (arguments->dec){
				argp_usage(state);
				return EINVAL;
			}
			break;
		case 'd'://decimal-only flag
			arguments->dec=true;
			if (arguments->hex){
				argp_usage(state);
				return EINVAL;
			}
			break;
		case 1://hex label
			arguments->hlabel=arg;
			break;
		case 2://decimal label
			arguments->dlabel=arg;
			break;
		case 3://char label
			arguments->clabel=arg;
			break;
		case ARGP_KEY_ARG://we don't want arguments
			if (state->arg_num>=0){
				argp_usage (state);
			}
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = {options, parse_opt, doc};

int main (int argc, char *argv[]){

	struct arguments arguments;
	//Default values:
	arguments.wrap = wrap_default;
	arguments.start = start_default;
       	arguments.end = end_default;
	arguments.filename = filename_default;
	arguments.dlabel=dlabel_default;
	arguments.hlabel=hlabel_default;
	arguments.clabel=clabel_default;
	arguments.hex = false;
	arguments.dec = false;
	arguments.wrapSet = false;
	arguments.standalone = false;

	argp_parse(&argp,argc,argv,0,0,&arguments);

	//Fix that values that result in a smaller range than the default wrap value can cause an error
	if ((arguments.wrap > (arguments.end-arguments.start)) && !arguments.wrapSet){
		arguments.wrap = (arguments.end-arguments.start)+1;
	}
	if ((arguments.wrap > (arguments.end-arguments.start)+1) ||
			(arguments.start > arguments.end)){
		printf("Invalid option ranges");
		return EINVAL;
	}

	if (arguments.hex){
		headings[0]=arguments.hlabel;
		headings[1]=arguments.clabel;
	}
	else if (arguments.dec){
		headings[0]=arguments.dlabel;
		headings[1]=arguments.clabel;
	} 
	else {
		headings[0]=arguments.dlabel;
		headings[1]=arguments.hlabel;
		headings[2]=arguments.clabel;
	}

	FILE *f;
	f = fopen(arguments.filename,"w+");//open file, override

	//Prepare table
	openTable(f,nheadings-(arguments.hex||arguments.dec),(unsigned int)ceil(((double)arguments.end-(double)arguments.start+1.0)/(double)arguments.wrap),"l",arguments.standalone);
	putHeader(f,nheadings-(arguments.hex||arguments.dec),(unsigned int)ceil(((double)arguments.end-(double)arguments.start+1.0)/(double)arguments.wrap),headings);

	//Make entries
	unsigned char entry = arguments.start;	
	for (unsigned int line=0; line < arguments.wrap; line++){
		entry=arguments.start+line;
		while(entry <= arguments.end){
			char strEntry[64];//enough memory for potential escape characters and/or commands
			//Generate the table entry
			if (arguments.hex){//hex only mode
				fprintf(f,"%X %s \\texttt{%s} ",entry,TEX_SEPARATOR,cleanString(entry, strEntry));
			}
			else if (arguments.dec){//decimal only mode
				fprintf(f,"%d %s \\texttt{%s} ",entry,TEX_SEPARATOR,cleanString(entry, strEntry));
			}
			else {//output decimal and hex
				fprintf(f,"%d %s %X %s \\texttt{%s} ",entry,TEX_SEPARATOR,entry,TEX_SEPARATOR,cleanString(entry, strEntry));
			}
			if (entry+arguments.wrap <= arguments.end){//tab separator after every entry except for the last of the line
				fprintf(f,"%s ",TEX_SEPARATOR);
			}
 			entry += arguments.wrap;
		}
		if (line!=arguments.wrap){//indicate newline except for the last entry
			fprintf(f,TEX_NEWLINE);
		}
		fprintf(f,"\n");	
	}
	closeTable(f, arguments.standalone);
	return 0;
}

void openTable (FILE* f, const unsigned int nheadings, const unsigned int nwraps, const char *alignment, bool standalone){
	if (standalone){
		fprintf(f,"\\documentclass{standalone}\n\\begin{document}\n\n");
	}
	fprintf(f, "\\begin{tabular}{");
	//Generates tabular argument, e.g. {l|l|l|l} (4 columns, left aligned)
	for (unsigned int w=0; w<nwraps; w++){
		for (unsigned int i=0; i<nheadings; i++){
			fprintf(f, alignment);
		}
		if (w!=nwraps-1){
			fprintf(f, "|");
		}
	}
	fprintf(f,"}\n");
}

void closeTable (FILE *f, bool standalone){
	fprintf(f, "\\end{tabular}\n");
	if (standalone){
		fprintf(f,"\\end{document}\n");
	}
}

void putHeader(FILE *f, const unsigned int nheadings, const unsigned int nwraps, const char *headings[]){
	for (unsigned int w=0; w<nwraps; w++){
		for (unsigned int n=0; n<nheadings; n++){
			fprintf(f,headings[n]);
			if ((w!=nwraps-1) || (n!=nheadings-1)){//not the last element
				fprintf(f," %s ",TEX_SEPARATOR);
			}
		}
	}
	fprintf(f, "%s %s\n",TEX_NEWLINE,TEX_HLINE);
}

char* cleanString (unsigned char entry, char *str){
	//only printable characters
	if (entry < 32 || entry > 126){
	 entry=' ';
	}
 	//Some characters have to be escaped for latex
	switch(entry){
		case '%':
			str = "\\%";
			break;

		case '#':
			str = "\\#";
			break;
		
		case '&':
			str = "\\&";
			break;
	
		case '$':
			str = "\\$";
			break;

		case '{':
			str = "\\{";
			break;
		
		case '}':
			str = "\\}";
			break;
		
		case '_':
			str = "\\_";
			break;

		case '~':
			str = "\\textasciitilde";
			break;

		case '^':
			str = "\\textasciicircum";
			break;
		case '\\':
			str = "\\textbackslash";
			break;
		default:
			//per default str <-- entry
			str[0]=entry;
			str[1]='\0';
	}
	return str;
}




