/* Main source file for BFCI, yet another Brainf*** interpreter.
 *
 * This project can be edited and redistributed freely; it is to be realeased
 * under the GNU GPLv3 license, contained in the main project source tree.
 *
 * <https://www.gnu.org/licenses/gpl-3.0.en.html> */


#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "src/interpreter.h"


#define VERSION "1.0.0"


int main(int argc, char** argv)
{
  char* program = argv[0]; /* Always entered of course so no seg fault. */
  /* If being run directy, set pointer past "./", this looks nicer when being
   * used to print errors: "bfci: error" instead of "./bfci: error". */
  if(*argv[0] == '.' && *argv[0] + 1 == '/') program = argv[0] + 2;

  if(argc < 2) {
    fprintf(stderr, "%s: error: invalid option count \"%i\"\n", program, argc);
    return 1;
  }

  int option;
  char* bf_source_file_name = {0};
  char* bf_source_opt_arg = {0};
  char* bf_file_contents_array = {0};
  FILE* bf_source_file_handle;

  bool interpret_arg_opt = false;

  opterr = 0; /* Do not print default getopt error messages. */

  while((option = getopt(argc, argv, "hf:i:v")) != -1 && option != 1)
    switch(option) {
    case 'h': printf("usage: %s [options] [target]\n", program); break;
    case 'f': bf_source_file_name = optarg; break; /* Source file name. */
    case 'i': /* Instead of reading file, interpret argument bf code. */
      interpret_arg_opt = true;
      bf_source_opt_arg = optarg;
      break;
    case 'v': printf("%s: current version %s\n", program, VERSION); return 0;
    case '?': /* Getopt returns '?' is returned when either a required argument
	       * is missing/ if a non-valid argument is entered. For more
               * information check man pages for getopt(3) errors. */
      fprintf(stderr, "%s: error: invalid option/ required argument missing\n"
	      "see usage: %s -h\n", program, program );
      return 1;
    }

  /* If -i option is used interpret from argument & skip file checks below. */
  if(interpret_arg_opt == true) {
    if(bf_interpreter(bf_source_opt_arg) == 1) {
      fprintf(stderr, "%s: error: interpreter failed to execute\n", program);
      return 1;
    }
    return 0;
  }

  /* Silently exit if bf_source_file_name is not set due to invalid opt. */
  if(!bf_source_file_name) return 1;

  /* Ensure the file can first be opened for reading before proceeding. */
  if(!(bf_source_file_handle = fopen(bf_source_file_name, "r"))) {
    fprintf(stderr, "%s: error: specified file \"%s\" does not exist\n",
	    program, bf_source_file_name);
    return 1;
  }

  /* Ensure extention of file is of a valid bf type. */
  if(check_file_ext(bf_source_file_name) == false) {
    fprintf(stderr, "%s: error: the file \"%s\" is of an invalid type\n",
	    program, bf_source_file_name);
    return 1;
  }

  bf_file_contents_array = bf_file_to_array(bf_source_file_handle);

  if(bf_interpreter(bf_file_contents_array) == 1) {
    fprintf(stderr, "%s: error: interpreter failed to execute\n", program);
    return 1;
  }

  free(bf_file_contents_array);

  return 0;
}
