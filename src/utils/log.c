#include <stdio.h>
#include "color.h"
#include <string.h>

void log_print(const char* type, const char* msg){
    if (!strcmp(type,"ERROR")){
        fprintf(stderr, "[" C_RED " %s " C_RESET "] %s\n", type, msg);
    } else if (!strcmp(type,"INFO")) {
        fprintf(stdout, "[" C_BOLD "%s" C_RESET "] %s\n", type, msg);
    } else if (!strcmp(type,"VERBOSE")) {
        fprintf(stdout, "[" C_YELLOW "%s" C_RESET "] %s\n", type, msg);
    } else if (!strcmp(type,"OK")) {
        fprintf(stdout, "[" C_GREEN "%s" C_RESET "] %s\n", type, msg);
    }
}
