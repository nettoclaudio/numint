#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include "numint.h"
#include "../utils/color.h"
#include "../utils/timer.h"
#include "../utils/log.h"

void numint_interface(int argc, char** argv) {
    int i;
    short required_args[4] = {0,0,0,0};
    short optional_arg[2] = {0,0};

    numint_config_t* ni_config;
    rpn_queue_t* rpn;

    if(argc==1) {
        log_print("ERROR", "You must submit any arguments. See options in \"--help\".");
        exit(EXIT_FAILURE);
    }

    for(i=1; i<argc; i++) {
        if( !(strcmp(argv[i], "--help")) ||
            !(strcmp(argv[i], "-h"))
          ) {
            fprintf(stdout, "Usage:\n\t$ ./numint --function \"<FUNCTION>\" --from <NUMBER> --to <NUMBER>\n");
            fprintf(stdout, "\t\t   --error <NUMBER> [--thread [<NUMBER>|MAX]] [--verbose]\n");

            fprintf(stdout, "\n");
            fprintf(stdout, "Perform a numerical integration using the method Adaptive Quadrature.\n");
            fprintf(stdout, "Return the final value of integral and the elapsed time.\n");

            fprintf(stdout, "\n");
            fprintf(stdout, "Mandatory arguments to long options are mandatory for short options too.\n");

            fprintf(stdout, "  -a, --from\t\t");
            fprintf(stdout, "Integration start interval.\n");

            fprintf(stdout, "  -b, --to\t\t");
            fprintf(stdout, "Integration end interval.\n");

            fprintf(stdout, "  -e, --error\t\t");
            fprintf(stdout, "Maximum error to estimate the integrate.\n");

            fprintf(stdout, "  -f, --function\t");
            fprintf(stdout, "Function that will be evaluated.\n");

            fprintf(stdout, "  -h, --help\t\t");
            fprintf(stdout, "Show this menu.\n");

            fprintf(stdout, "  -t, --thread\t\t");
            fprintf(stdout, "Numbers of threads to concurrent approach.\n");

            fprintf(stdout, "  -v, --verbose\t");
            fprintf(stdout, "Show messages while the application runs.\n");

            exit(EXIT_SUCCESS);
        }
    }

    ni_config = (numint_config_t*) malloc(sizeof(numint_config_t));

    if( ni_config == (numint_config_t*) NULL ) {
        log_print("ERROR", "Could not allocate memory to numint_config_t.");
        exit(EXIT_FAILURE);
    }

    // Optinal's default configuration
    ni_config->verbose=0;
    ni_config->threads=0;

    for(i=1; i<argc; i++) {

        if( !(strcmp(argv[i], "--function")) ||
            !(strcmp(argv[i], "-f")) ) {

            if( required_args[0] ) {
                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                fprintf(stderr, "You have already submitted \"%s\".\n", argv[i]);
                exit(EXIT_FAILURE);
            }

            if( argc < i+2) {
                log_print("ERROR", "Insuficient arguments. You must submit a function.");
                exit(EXIT_FAILURE);
            }

            i++;

            if( !(rpn_check_infix_notation(argv[i], strlen(argv[i]))) ) {

                rpn = (rpn_queue_t*) malloc(sizeof(rpn_queue_t));
                if( rpn == (rpn_queue_t*) NULL ) {
                    log_print("ERROR", "Could not allocate memory to rpn_queue_t.");
                    exit(EXIT_FAILURE);
                }

                if( rpn_init_queue(rpn) ){
                    log_print("ERROR", "Queue was not initialized correctly.");
                    exit(EXIT_FAILURE);
                }

                rpn_infix_notation_to_rpn(rpn, argv[i]);
                ni_config->rpn = (rpn_queue_t*) rpn;

            }

            required_args[0]=1;
            continue;
        }

        if( !(strcmp(argv[i], "--from")) ||
            !(strcmp(argv[i], "-a")) ) {

            if( required_args[1] ) {
                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                fprintf(stderr, "You have already submitted \"%s\".\n", argv[i]);
                exit(EXIT_FAILURE);
            }

            if( argc < i+2 ) {
                log_print("ERROR", "Insuficient arguments.You must submit an integer number for interval start.");
                exit(EXIT_FAILURE);
            }

            i++;

            ni_config->interval_start = atof(argv[i]);
            if( !ni_config->interval_start && strcmp(argv[i], "0") ) {
                log_print("ERROR", "You must submit an integer number for interval start.");
                exit(EXIT_FAILURE);
            }

            required_args[1]=1;
            continue;
        }

        if( !(strcmp(argv[i], "--to")) ||
            !(strcmp(argv[i], "-b")) ) {

            if( required_args[2] ) {
                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                fprintf(stderr, "You have already submitted \"%s\".\n", argv[i]);
                exit(EXIT_FAILURE);
            }

            if( argc < i+2 ) {
                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                fprintf(stderr, "Insuficient arguments. ");
                fprintf(stderr, "You must submit an integer number for interval end.\n");
                exit(EXIT_FAILURE);
            }

            i++;

            ni_config->interval_end = atof(argv[i]);
            if( !ni_config->interval_end && strcmp(argv[i], "0") ) {
                log_print("ERROR", "You must submit an integer number for interval end.");
                exit(EXIT_FAILURE);
            }

            required_args[2]=1;
            continue;
        }

        if( !(strcmp(argv[i], "--error")) ||
            !(strcmp(argv[i], "-e")) ) {

            if( required_args[3] ) {
                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                fprintf(stderr, "You have already submitted \"%s\".\n", argv[i]);
                exit(EXIT_FAILURE);
            }

            if( argc < i+2 ) {
                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                fprintf(stderr, "Insuficient arguments. ");
                fprintf(stderr, "You must submit a positive integer number for error.\n");
                exit(EXIT_FAILURE);
            }

            i++;
            ni_config->max_error = (strtod(argv[i], NULL) < 0) ?
                                   (strtod(argv[i], NULL)*(-1)) :
                                   (strtod(argv[i], NULL));

            if( !ni_config->max_error && strcmp(argv[i], "0") ) {
                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                fprintf(stderr, "The value of argument \"%s\" must be an integer number.\n", argv[i-1]);
                exit(EXIT_FAILURE);
            }

            required_args[3]=1;
            continue;
        }

        if( !(strcmp(argv[i], "--thread")) ||
            !(strcmp(argv[i], "-t")) ) {

            if( optional_arg[0] ) {
                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                fprintf(stderr, "You have already submitted \"%s\".\n", argv[i]);
                exit(EXIT_FAILURE);
            }

            if( argc < i+2 ) {
                log_print("ERROR", "Insuficient arguments. You must submit a positive integer number for threads.");
                exit(EXIT_FAILURE);
            }

            i++;

            if( !(strcmp(argv[i], "MAX")) ) {
                ni_config->threads = sysconf(_SC_NPROCESSORS_ONLN);
            } else {
                ni_config->threads = (atoi(argv[i]) < 0) ?
                                     (atoi(argv[i])*(-1)) :
                                     (atoi(argv[i]));

                if( !ni_config->max_error && strcmp(argv[i], "0") ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "The value of argument \"%s\" must be a positive integer number.\n", argv[i-1]);
                    exit(EXIT_FAILURE);
                }
            }

            optional_arg[0] = 1;

            continue;
        }

        if( !(strcmp(argv[i], "--verbose")) ||
            !(strcmp(argv[i], "-v")) ) {

            if( optional_arg[1] ) {
                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                fprintf(stderr, "You have already submitted \"%s\".\n", argv[i]);
                exit(EXIT_FAILURE);
            }

            ni_config->verbose=1;
            optional_arg[1] = 1;

            continue;
        }

        fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
        fprintf(stderr, "Invalid option \"%s\"\n", argv[i]);
        exit(EXIT_FAILURE);
    }

    for(i=0; i<4; i++) {
        if( required_args[i] != 1 ) {
            log_print("ERROR", "Any arguments are required. See options in \"--help\".");
            exit(EXIT_FAILURE);
        }
    }

    numint_integrate( ni_config );

}

void numint_integrate(numint_config_t* ni_config) {
    double runtime_start;
    double runtime_end;
    double final_result;
    double previous_result;
    double current_result;
    double current_error;
    unsigned long long n=1;
    unsigned    i;
    void* block_result;
    div_t blocks;
    numint_thread_t* t_args;
    pthread_t* threads;

    previous_result = 0;
    current_result  = 0;
    final_result    = 0;

    if( !(ni_config->threads) ) {

        if( ni_config->verbose ) {
            log_print("VERBOSE", "Initializing the sequential computing integration.");
        }

        GET_TIME(runtime_start);
        do {
            previous_result = current_result;
            current_result = numint_compute_s(ni_config, n);

            if( ni_config->verbose ) {
                fprintf(stdout, "[" C_YELLOW "VERBOSE" C_RESET "] ");
                fprintf(stdout, "Parcial result to N=%Lu: %.20f\n", n, current_result);
            }

            current_error = fabs(current_result-previous_result);

            if( ni_config->verbose && n>1 ) {
                fprintf(stdout, "[" C_YELLOW "VERBOSE" C_RESET "] ");
                fprintf(stdout, "Current absolute error: %.20f\n", current_error);
            }

            n <<= 1;
        } while( current_error > ni_config->max_error );
        GET_TIME(runtime_end);

        final_result = current_result;

        if( ni_config->verbose ){
            log_print("VERBOSE", "Finishing the sequential computing integration.");
        }

    } else {

        if( ni_config->verbose ) {
            log_print("VERBOSE", "Initializing the concurrent computing integration.");
        }

        GET_TIME(runtime_start);

        threads = (pthread_t*) malloc(sizeof(pthread_t)*ni_config->threads);
        if( threads == (pthread_t*)NULL ) {
            log_print("ERROR", "Could not allocate memory to pthread_t.");
            exit(EXIT_FAILURE);
        }

        blocks = div(ni_config->interval_end-ni_config->interval_start, ni_config->threads);

        if( ni_config->verbose ) {
            fprintf(stdout, "[" C_YELLOW "VERBOSE" C_RESET "] ");
            fprintf(stdout, "Each thread will work under a size block of %d. ", blocks.quot);
            fprintf(stdout, "And the last thread will get a size block of %d+%d.\n", blocks.quot, blocks.rem);
        }

        for(i=0; i<ni_config->threads; i++) {
            t_args = (numint_thread_t*) malloc(sizeof(numint_thread_t));
            if( t_args == (numint_thread_t*)NULL ) {
                log_print("ERROR", "Could not allocate memory to numint_thread_t.");
                exit(EXIT_FAILURE);
            }

            t_args->t_id = i;
            t_args->from = blocks.quot*i;
            t_args->to   = blocks.quot*(i+1);

            if(i == ni_config->threads-1 ) {
                t_args->to += blocks.rem;
            }

            t_args->n = n;
            t_args->ni_config = ni_config;

            if( ni_config->verbose ) {
                fprintf(stdout, "[" C_YELLOW "VERBOSE" C_RESET "] ");
                fprintf(stdout, "Creating a new thread ID: %u\n", i);
            }

            if( pthread_create(&threads[i], NULL, numint_compute_t, (void*)t_args) ) {
                log_print("ERROR", "Problem to create threads.");
                exit(EXIT_FAILURE);
            }
        }

        final_result = 0;

        for(i=0; i<ni_config->threads; i++) {
            if(pthread_join(threads[i], &block_result)) {
                log_print("ERROR", "Problem to join threads.");
                exit(EXIT_FAILURE);
            }

            if( ni_config->verbose ) {
                fprintf(stdout, "[" C_YELLOW "VERBOSE" C_RESET "] ");
                fprintf(stdout, "End of thread: %u\n", i);
            }

            final_result += *((double*)block_result);
        }

        GET_TIME(runtime_end);

        if( ni_config->verbose ) {
            log_print("VERBOSE", "Finishing the concurrent computing integration.");
        }

        free(threads);
    }

    fprintf(stdout, "[" C_BOLD "INFO" C_RESET "] ");
    fprintf(stdout, "Final value:\t%.20f\n", final_result);

    fprintf(stdout, "[" C_BOLD "INFO" C_RESET "] ");
    fprintf(stdout, "Elapsed time:\t %.20f\n", runtime_end-runtime_start);

    if( rpn_exit_queue(ni_config->rpn) ) {
        log_print("ERROR", "Problem to exit of RPN.");
        exit(EXIT_FAILURE);
    }

    free(ni_config->rpn);
    free(ni_config);
}

double numint_compute_s(numint_config_t* ni_config, const long long n) {
    double dx;
    double x;
    double final_result;
    double i;

    final_result=0;

    dx = (double)(ni_config->interval_end - ni_config->interval_start)/n;

    for(i=ni_config->interval_start; i<ni_config->interval_end; i+=dx) {
        x = (double)(i+(i+dx))/2;
        final_result += rpn_solve_light(ni_config->rpn, x)*dx;
    }

    return final_result;
}

void* numint_compute_t(void* t_args) {
    double  dx;
    double  x;
    double  i;
    double  current_error;
    double  previous_result;
    double  current_result;
    double* final_result;

    numint_thread_t arg;

    previous_result = 0;
    current_result  = 0;

    arg = *(numint_thread_t*)t_args;
    free(t_args);

    final_result  = (double*) malloc(sizeof(double));
    if( final_result == (double*)NULL ) {
        log_print("ERROR", "Could not allocate memory to numint_thread_t.");
        exit(EXIT_FAILURE);
    }

    *final_result = 0;

    if( arg.ni_config->verbose ) {
        fprintf(stdout, "[" C_YELLOW "VERBOSE" C_RESET "] ");
        fprintf(stdout, "Thread %u - Initializing thread\'s processing.\n", arg.t_id);
    }

    do {
        previous_result = current_result;

        dx = (double)(arg.to - arg.from)/arg.n;
        for(i=arg.from; i<arg.to; i+=dx) {
            x = (double)(i+(i+dx))/2;

            if( i==arg.from ) {
                current_result = 0;
            }

            current_result += rpn_solve_light(arg.ni_config->rpn, x) * dx;
        }

        if( arg.ni_config->verbose ) {
            fprintf(stdout, "[" C_YELLOW "VERBOSE" C_RESET "] ");
            fprintf(stdout, "Thread %u - Parcial result to N=%Lu: %.20f\n", arg.t_id, arg.n, current_result);
        }

        current_error = fabs(current_result-previous_result);

        if( arg.ni_config->verbose && arg.n>1 ) {
            fprintf(stdout, "[" C_YELLOW "VERBOSE" C_RESET "] ");
            fprintf(stdout, "Thread %u - Current absolute error: %.20f\n", arg.t_id, current_error);
        }

        arg.n <<= 1;
    } while( current_error > arg.ni_config->max_error );

    *final_result = current_result;

    if( arg.ni_config->verbose ) {
        fprintf(stdout, "[" C_YELLOW "VERBOSE" C_RESET "] ");
        fprintf(stdout, "Thread %u - Finishing thread\'s processing.\n", arg.t_id);
    }

    pthread_exit((void*)final_result);
}
