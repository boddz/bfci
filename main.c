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
  char* c_output_opt_arg = {0};
  FILE* bf_source_file_handle;

  bool file_arg_opt = false;
  bool interp_arg_opt = false;
  bool c_arg_opt = false;

  opterr = 0; /* Do not print default getopt error messages. */

  while((option = getopt(argc, argv, "hf:i:c:v")) != -1 && option != 1)
    switch(option) {
    case 'h': printf("usage: %s [options] [target]\n", program); return 0;
    case 'f': /* Source file name. */
      file_arg_opt = true;
      bf_source_file_name = optarg;
      break;
    case 'i': /* Instead of reading file, interpret argument bf code. */
      interp_arg_opt = true;
      bf_source_opt_arg = optarg;
      break;
    case 'c': /* Translate Brainf*** code into C and write to output file. */
      c_arg_opt = true;
      c_output_opt_arg = optarg;
      break;
      case 'v': printf("%s: current version %s\n", program, VERSION); return 0;
    case '?': /* Getopt returns '?' is returned when either a required argument
	       * is missing/ if a non-valid argument is entered. For more
               * information check man pages for getopt(3) errors. */
      fprintf(stderr, "%s: error: invalid option/ required argument missing\n"
	      "see usage: %s -h\n", program, program );
      return 1;
    }

  /* No options just a filename entered. */
  if(optind == 1 && !optarg) bf_source_file_name = argv[1];

  /* -c option is used paired with -i option, translate from arg array. */
  if(c_arg_opt == true && interp_arg_opt == true) {
    if(bf_into_c(bf_source_opt_arg, c_output_opt_arg) == 1) {
      fprintf(stderr, "%s: error: translator failed to execute\n", program);
      return 1;
    }
    return 0;
  }
  /* -i option is used interpret from argument & skip file checks below. */
  else if(interp_arg_opt == true) {
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

  bf_source_file_handle = fopen(bf_source_file_name, "r");
  bf_file_contents_array = bf_file_to_array(bf_source_file_handle);

  /* -c option is used paired with -f option, translate from Bf file. */
  if(c_arg_opt == true && file_arg_opt == true) {
    if(bf_into_c(bf_file_contents_array, c_output_opt_arg) == 1) {
      fprintf(stderr, "%s: error: translator failed to execute\n", program);
      return 1;
    }
    return 0;
  }
  /* Only the -f option is specified or just filename entered. */
  else if(file_arg_opt == true || optind == 1 && !optarg)
    if (bf_interpreter(bf_file_contents_array) == 1) {
      fprintf(stderr, "%s: error: interpreter failed to execute\n", program);
      return 1;
    }

  /* Clean up! */
  free(bf_file_contents_array);

  return 0;
}
