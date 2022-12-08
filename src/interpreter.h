#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdbool.h>
#include <stdio.h>


#define TAPESIZE 30000

bool check_file_ext(char*); /* Not valid ext: b, B, bf, BF. Error & exit. */
char* bf_file_to_array(FILE*); /* Bf File -> calloce array caller must free. */
int bf_interpreter(char*); /* Bf code interpreter. */
int bf_into_c(char*, char*); /* Translate Bf code into C & write to file. */


#endif
