# ascii-table-tool
A minimal tool to generate .tex code for ASCII tables

## Installation
The tool consists of a single C file and requires minimal dependencies. 
Just run ```gcc ascii.c -o ascii -lm``` to compile.

## Usage
The available options are:

    ./ascii --help
    Usage: ascii [OPTION...] A minimal tool to generate .tex code for ASCII tables

      -a, --standalone           Creates standalone latex files
      -C, --clabel=STRING        Modify the char label
      -d, --dec                  Only create the decimal column
      -D, --dlabel=STRING        Modify the decimal label
      -e, --end=n                Character code to end with
      -h, --hex                  Only create the hexadecimal column
      -H, --hlabel=STRING        Modify the hexadecimal label
      -o, --output=FILE          Output filename, default is ascii.tex
      -s, --start=n              Character code to start with
      -w, --wrap=n               Number of lines before the table wraps to the next
                                 column
      -?, --help                 Give this help list
          --usage                Give a short usage message
      -V, --version              Print program version



### Examples
Create a table in standalone mode and directly compile it:
```./ascii -s -o table.tex && pdflatex "table.tex" ```


![table-animation](https://github.com/stgloorious/ascii-table-tool/blob/master/doc/animation.gif)
