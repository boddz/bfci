/* Interpreter source file for BFCI.
 *
 * Contains the main interpreter for executing Brainf*** code and an assortment
 * of helper functions to go along side it for parsing code/ file IO. */


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"


bool check_file_ext(char* file_name) {
  int before_ext = 0;

  while(*file_name != '.') { /* Push array pointer to start of ext (.bf). */
    ++before_ext;
    ++file_name;
  }

  if(before_ext == 0) return false; /* No proceeding chars, avoid seg fault. */

  /* Return true if pointed to section of array matches valid bf ext. */
  if(!strcmp(file_name, ".b") || !strcmp(file_name, ".bf")) return true;
  if(!strcmp(file_name, ".B") || !strcmp(file_name, ".BF")) return true;
  if(!strcmp(file_name, ".bF") || !strcmp(file_name, ".Bf")) return true;

  return false;
}


char* bf_file_to_array(FILE* input_file)
{
  /* Get file size and rewind pointer for later use. */
  fseek(input_file, 0, SEEK_END);
  unsigned int file_size = ftell(input_file);
  rewind(input_file);

  char* out_array = calloc(file_size + 1, sizeof(char));

  /* Dump the contents of file into the calloced array and append null term. */
  for(unsigned int i = 0; i < file_size; i++) out_array[i] = fgetc(input_file);
  out_array[file_size + 1] = '\0';

  return out_array;
}


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
