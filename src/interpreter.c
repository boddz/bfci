/* Interpreter source file for BFCI.
 *
 * Contains the main interpreter for executing Brainf*** code and an assortment
 * of helper functions to go along side it for parsing code/ file IO. */


#include <stdlib.h>
#include <string.h>

#include "interpreter.h" /* stdbool.h stdio.h */


/* Returns true if the specified file is of a valid extention type.
 * Takes one argument of char*. */
bool check_file_ext(char* file_name) {
  int before_ext = 0;

  while(*file_name != '.') { /* Move array pointer to fullstop/ ext offset. */
    ++before_ext;
    ++file_name;
  }

  if(before_ext == 0) return false; /* No chars before '.' avoid seg fault. */

  /* Allowed Brainf*** extention types (I think this is all). */
  if(!strcmp(file_name, ".b") || !strcmp(file_name, ".bf")) return true;
  if(!strcmp(file_name, ".B") || !strcmp(file_name, ".BF")) return true;
  if(!strcmp(file_name, ".bF") || !strcmp(file_name, ".Bf")) return true;

  return false;
}


/* Returns a pointer to a calloced array containinng the contents of file.
 * Takes one argument of type FILE* and closes handle before return.
 * This function assumes that the caller will free the array manually. */
 char* bf_file_to_array(FILE* input_file)
{
  /* Get file size and rewind cursor/pointer for later use. */
  fseek(input_file, 0, SEEK_END);
  unsigned int file_size = ftell(input_file);
  rewind(input_file);

  /* Calloc and dump contents into the array, return the pointer. */
  char* out_array = calloc(file_size + 1, sizeof(char));
  for(unsigned int i = 0; i < file_size; i++) out_array[i] = fgetc(input_file);
  out_array[file_size + 1] = '\0';

  fclose(input_file);

  return out_array;
}


/* Returns an int execution/ error code to indicate status of execution ~
 * == 0 : the execution of interpreter was successful, no errors.
 * != 0 : encountered an error during execution process.
 * Takes one argument of type char*. */
int bf_interpreter(char* bf_code)
{
  unsigned int loop;

  char tape[TAPESIZE] = {0};
  char* cell = tape;

  while(*bf_code) {
    switch(*bf_code) {
      case '>': ++cell; break;
      case '<': --cell; break;
      case '+': ++*cell; break;
      case '-':	--*cell; break;
      case '.': printf("%c", *cell); break;
      case ',':
	*cell = getchar();
	/* Conditional to eat up trailing '\n' as it is not wanted. */
	if(*cell == '\n') *cell = getchar();
	break;
      case '[':
        /* Once the value in the currently pointed to cell reaches zero, in
	 * crement the current char pointer until it reaches the corresponding
	 * closing brace (if balanced) so that once this case breaks the
	 * pointed to char would no longer be a ']' as it is incremented again
	 * once more when case breaks so the switch can move onto next char
	 * which is not a ']' (below ']' case not applicable so loop ends). */
	if(!*cell) {
	  loop = 1;
	  while(loop && *bf_code) {
	    bf_code++;
	    if(*bf_code == '[') ++loop;
	    if(*bf_code == ']') --loop;
	  }
	  if(loop) { /* Avoid seg fault; ensure balanced braces. */
	    fprintf(stderr, "unmatched loop parentheses exiting\n");
	    return 1;
	  }
	}
	break;
      case ']':
	loop = 1;
	while(loop && *bf_code) {
	  bf_code--;
	  if(*bf_code == '[') loop--;
	  if(*bf_code == ']') loop++;
	}
	if(loop) {
	  fprintf(stderr, "unmatched loop parentheses exiting\n");
	  return 1;
	}
        /* Decrement the current char pointer by one so that it is set to
         * the char just before the corresponding opening brace; once this case
	 * breaks, the current char pointer will again point to the initial
	 * opening brace and another check can then be done to ensure the value
         * in the currently pointed to cell has not yet reached zero, if it has
         * reached zero then the loop will end. */
	bf_code--;
	break;
    }
    bf_code++;
  }
  return 0;
}


/* Returns an int execution/ error code to indicate status of translation.
 * == 0 : translation Brainf*** to C translated succesfully.
 * != 0 : encountered some kind of error during the translation process.
 * Takes two arguments of type char* . */
int bf_into_c(char* bf_code, char* output)
{
  /* Ensure the file name for the output file is actually specified. */
  if(!*output) {
    fprintf(stderr, "aborted: a name for the output C file is required\n");
    return 1;
  }

  FILE* c_file_out = fopen(output, "w");

  fprintf(c_file_out,
	  "#include <stdio.h>\n"
	  "char tape[%i] = {0};\n"
	  "char* p = tape;\n"
	  "int main(void)\n{\n",
	  TAPESIZE);

  while(*bf_code) {
    switch(*bf_code) {
    case '>': fprintf(c_file_out, "++p;\n"); break;
    case '<': fprintf(c_file_out, "--p;\n"); break;
    case '+': fprintf(c_file_out, "++*p;\n"); break;
    case '-': fprintf(c_file_out, "--*p;\n"); break;
    case '.': fprintf(c_file_out, "putchar(*p);\n"); break;
    case ',':
      fprintf(c_file_out, "*p = getchar();\nif(*p == '\n') *p = getchar();\n");
      break;
    case '[': fprintf(c_file_out, "while(*p) {\n"); break;
    case ']': fprintf(c_file_out, "}\n"); break;
    }
    bf_code++;
  }

  fprintf(c_file_out, "return 0;\n}\n");

  fclose(c_file_out);

  return 0;
}
