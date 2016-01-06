#ifndef REVERSE_POLISH_NOTATION_H_
#define REVERSE_POLISH_NOTATION_H_

    typedef enum _rpn_type_elem_t_ {
        NUMBER, UNKNOWN, OPERATOR, MATH_CONSTANT, TRIGONOMETRIC
    } rpn_type_elem_t;

    typedef struct _rpn_elem_t_ {
        struct _rpn_elem_t_* next;
        rpn_type_elem_t type;
        void* element;
    } rpn_elem_t;

    typedef struct _rpn_queue_t_ {
        rpn_elem_t* first;
        rpn_elem_t* last;
        unsigned size;
    } rpn_queue_t;

    typedef struct _rpn_stack_t_ {
        rpn_elem_t* top;
        unsigned size;
    } rpn_stack_t;

    /*
     * Check whether a string is in a infix notation.
     * Return EXIT_SUCCESS when the expression is a infix notation, otherwise
     * it returns EXIT_FAILURE.
     */
    extern int rpn_check_infix_notation(const char* str, const unsigned str_len);

    extern int rpn_infix_notation_to_rpn(rpn_queue_t* queue, const char* infix_notation);

    extern int rpn_copy_queue(const rpn_queue_t* queue_src, rpn_queue_t* queue_dst);

    extern double rpn_solve(rpn_queue_t* queue, const double unknown);

    extern double rpn_solve_light(const rpn_queue_t* queue, const double unknown);

    extern int rpn_init_queue(rpn_queue_t* queue);

    extern int rpn_exit_queue(rpn_queue_t* queue);

    extern int rpn_push_queue(rpn_queue_t* queue, rpn_elem_t* elem);

    extern rpn_elem_t* rpn_pop_queue(rpn_queue_t* queue);

    extern void rpn_show_queue(const rpn_queue_t* queue);

    extern int rpn_init_stack(rpn_stack_t* stack);

    extern int rpn_exit_stack(rpn_stack_t* stack);

    extern int rpn_push_stack(rpn_stack_t* stack, rpn_elem_t* elem);

    extern rpn_elem_t* rpn_pop_stack(rpn_stack_t* stack);

    extern void rpn_show_stack(const rpn_stack_t* stack);


#endif
