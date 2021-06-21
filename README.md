# ascii-table-tool
A minimal tool to generate .tex code for ASCII tables

## Installation
The tool consists of a single C file and requires minimal dependencies. 
Just run ```gcc ascii.c -o ascii -lm``` to compile.

## Usage
The available options are:

### Examples
Compile a table in standalone mode and directly compile it:
```./ascii -s -o table.tex && pdflatex "table.tex" ```


![table-animation](https://github.com/stgloorious/ascii-table-tool/blob/master/doc/animation.gif)
