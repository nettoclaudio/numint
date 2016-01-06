#ifndef NUMINT_H_
#define NUMINT_H_

    #include "../rpn/reverse_polish_notation.h"

    typedef struct _numint_config_t_ {
        rpn_queue_t* rpn;
        double max_error;
        double interval_start;
        double interval_end;
        unsigned threads;
        unsigned short verbose;
    } numint_config_t;

    typedef struct _numint_thread_t_ {
        numint_config_t* ni_config;
        double from;
        double to;
        unsigned long long  n;
        unsigned t_id;
    } numint_thread_t;

    extern void numint_interface(int argc, char** argv);

    extern void numint_integrate(numint_config_t* ni_config);

    extern double numint_compute_s(numint_config_t* ni_config, const long long n);

    extern void* numint_compute_t(void* t_args);

#endif
