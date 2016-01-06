#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "reverse_polish_notation.h"
#include "../utils/color.h"

int rpn_check_infix_notation(const char* str, const unsigned str_len) {
    char c;
    unsigned i;
    unsigned short bad_syntax;
    short quantity_parethesis_notclosed;

    // In normal expression, zero is required.
    quantity_parethesis_notclosed = 0;

    // In the begin, the expression is correct.
    bad_syntax = 0;

    for(i=0; i<str_len; i++)  {
        c = str[i];

        switch(c) {
            case '(':
                quantity_parethesis_notclosed++;
                break;
            case ')':
                quantity_parethesis_notclosed--;
                if(quantity_parethesis_notclosed < 0) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "You cannot close a parenthesis nonexistent.\n");
                    return EXIT_FAILURE;
                }
                break;

            case '+':
            case '-':
            case '*':
            case '/':
            case '^':
            case 'x':   // variable
                break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '.':
                break;

            case 'p':
                if( (i+1) < str_len ) {
                    if( str[i+1] == 'i' ) {
                        i++;
                    } else {
                        fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                        fprintf(stderr, "Invalid character \'%c\'\n", c);
                        bad_syntax++;
                    }
                }

                break;

            case 'e':
                break;

            case 'c':
                if( (i+2) < str_len ) {
                    if( str[i+1] == 'o' ) {
                        if( str[i+2] == 's' ) {
                            i += 2;
                        }
                    }
                }
                break;

            case 's':
                if( (i+2) < str_len ) {
                    if( str[i+1] == 'i' || str[i+1] == 'e' ) {
                        if( str[i+2] == 'n' ) {
                            i += 2;
                        }
                    }
                }
                break;

            case 't':
                if( (i+2) < str_len ) {
                    if( str[i+1] == 'a' ) {
                        if( str[i+2] == 'n' ) {
                            i += 2;
                        }
                    } else {
                        if( str[i+1] == 'g' ) {
                            i += 1;
                        }
                    }
                }
                break;

            case ' ':
                break;

            default:
                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                fprintf(stderr, "Invalid character \'%c\'\n", c);
                bad_syntax++;
        }
    }

    if( quantity_parethesis_notclosed ) {
        fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
        fprintf(stderr, "Invalid quantity of parentheses.\n");
        return EXIT_FAILURE;
    }

    if( bad_syntax ) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int rpn_infix_notation_to_rpn(rpn_queue_t* queue, const char* infix_notation) {
    unsigned    infix_notation_len;
    unsigned    i;
    unsigned    j;
    unsigned    number_len;
    char        chr;
    char        stack_top_operator;
    char*       auxiliary_str;
    double*     auxiliary_number;

    rpn_elem_t*  auxiliary_elem;
    rpn_stack_t* stack_operators;

    infix_notation_len = strlen(infix_notation);

    if( rpn_check_infix_notation(infix_notation, infix_notation_len) ) {
        fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
        fprintf(stderr, "\"%s\" is not a infix notation.\n", infix_notation);
        exit(EXIT_FAILURE);
    }

    if( queue->size || queue->first != (rpn_elem_t*) NULL ) {
        if( rpn_exit_queue(queue) ) {
            fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
            fprintf(stderr, "There was an error in rpn_exit_queue().\n");
            exit(EXIT_FAILURE);
        }

        if( rpn_init_queue(queue) ) {
            fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
            fprintf(stderr, "There was an error in rpn_init_queue().\n");
            exit(EXIT_FAILURE);
        }
    }

    stack_operators = (rpn_stack_t*) malloc(sizeof(rpn_stack_t));
    if( stack_operators == (rpn_stack_t*) NULL ) {
        fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
        fprintf(stderr, "Could not allocate memory to rpn_stack_t.\n");
        exit(EXIT_FAILURE);
    }

    if( rpn_init_stack(stack_operators) ) {
        fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
        fprintf(stderr, "There was an error in rpn_init_stack().\n");
        exit(EXIT_FAILURE);
    }

    for(i=0; i<infix_notation_len; i++) {
        chr = infix_notation[i];
        number_len = 0;

        switch(chr) {
            case '+':
            case '-':
            case '*':
            case '/':
            case '^':
                auxiliary_str = (char*) malloc(sizeof(char));
                if( auxiliary_str == (char*)NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to string.\n");
                    exit(EXIT_FAILURE);
                }

                *auxiliary_str = chr;

                auxiliary_elem = (rpn_elem_t*) malloc(sizeof(rpn_elem_t));
                if( auxiliary_elem == (rpn_elem_t*)NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to rpn_elem_t.\n");
                    exit(EXIT_FAILURE);
                }

                auxiliary_elem->next = (rpn_elem_t*) NULL;
                auxiliary_elem->element = (void*) auxiliary_str;
                auxiliary_elem->type = (rpn_type_elem_t) OPERATOR;

                if( stack_operators->size ) {

                    while( stack_operators->size &&
                           (stack_top_operator = *(char*)stack_operators->top->element) != '(' ) {

                        if( chr == '+' || chr == '-' ) {

                            if( stack_top_operator == '+' ||
                                stack_top_operator == '-' ||
                                stack_top_operator == '*' ||
                                stack_top_operator == '/' ||
                                stack_top_operator == '^' ||
                                stack_top_operator == 'C' ||
                                stack_top_operator == 'S' ||
                                stack_top_operator == 'T' ) {
                                rpn_push_queue(queue, rpn_pop_stack(stack_operators));
                                continue;
                            }

                        }

                        if( chr == '*' || chr == '/' ) {

                            if( stack_top_operator == '*' ||
                                stack_top_operator == '/' ||
                                stack_top_operator == '^' ||
                                stack_top_operator == 'C' ||
                                stack_top_operator == 'S' ||
                                stack_top_operator == 'T' ) {
                                rpn_push_queue(queue, rpn_pop_stack(stack_operators));
                                continue;
                            }

                        }

                        if( chr == '^' ) {

                            if( stack_top_operator == 'C' ||
                                stack_top_operator == 'S' ||
                                stack_top_operator == 'T' ) {
                                rpn_push_queue(queue, rpn_pop_stack(stack_operators));
                                continue;
                            }

                        }

                        break;
                    }
                }

                rpn_push_stack(stack_operators, auxiliary_elem);

                break;

            case 'x':
                auxiliary_str = (char*) malloc(sizeof(char));
                if( auxiliary_str == (char*)NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to string.\n");
                    exit(EXIT_FAILURE);
                }

                *auxiliary_str = chr;

                auxiliary_elem = (rpn_elem_t*) malloc(sizeof(rpn_elem_t));
                if( auxiliary_elem == (rpn_elem_t*)NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to rpn_elem_t.\n");
                    exit(EXIT_FAILURE);
                }

                auxiliary_elem->next = (rpn_elem_t*) NULL;
                auxiliary_elem->element = (void*) auxiliary_str;
                auxiliary_elem->type = (rpn_type_elem_t) UNKNOWN;
                rpn_push_queue(queue, auxiliary_elem);
                break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                for(j=i; j<infix_notation_len; j++) {
                    chr = infix_notation[j];

                    switch (chr) {
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                        case '.':
                            number_len += 1;
                            break;

                        default:
                            j = infix_notation_len;
                    }
                }

                auxiliary_str = (char*) malloc(sizeof(char)*(number_len+1));
                if( auxiliary_str == (char*)NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to string.\n");
                    exit(EXIT_FAILURE);
                }

                strncpy(auxiliary_str, (infix_notation+i), sizeof(char)*(number_len+1));

                auxiliary_number = (double*) malloc(sizeof(double));
                if( auxiliary_number == (double*)NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to double.\n");
                    exit(EXIT_FAILURE);
                }

                *auxiliary_number = atof(auxiliary_str);

                auxiliary_elem = (rpn_elem_t*) malloc(sizeof(rpn_elem_t));
                if( auxiliary_elem == (rpn_elem_t*)NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to rpn_elem_t.\n");
                    exit(EXIT_FAILURE);
                }

                auxiliary_elem->next = (rpn_elem_t*) NULL;
                auxiliary_elem->element = (void*) auxiliary_number;
                auxiliary_elem->type = NUMBER;

                rpn_push_queue(queue, auxiliary_elem);
                i += (number_len-1);
                break;

            case 'p':
                if( (i+1) < infix_notation_len ) {
                    if( infix_notation[i+1] == 'i' ) {
                        i++;

                        auxiliary_number = (double*) malloc(sizeof(double));
                        if( auxiliary_number == (double*)NULL ) {
                            fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                            fprintf(stderr, "Could not allocate memory to double.\n");
                            exit(EXIT_FAILURE);
                        }

                        *auxiliary_number = M_PI;

                        auxiliary_elem = (rpn_elem_t*) malloc(sizeof(rpn_elem_t));
                        if( auxiliary_elem == (rpn_elem_t*)NULL ) {
                            fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                            fprintf(stderr, "Could not allocate memory to rpn_elem_t.\n");
                            exit(EXIT_FAILURE);
                        }

                        auxiliary_elem->next = (rpn_elem_t*) NULL;
                        auxiliary_elem->element = (void*) auxiliary_number;
                        auxiliary_elem->type = MATH_CONSTANT;

                        rpn_push_queue(queue, auxiliary_elem);
                    }
                }

                break;

            case 'e':
                auxiliary_number = (double*) malloc(sizeof(double));
                if( auxiliary_number == (double*)NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to double.\n");
                    exit(EXIT_FAILURE);
                }

                *auxiliary_number = M_E;

                auxiliary_elem = (rpn_elem_t*) malloc(sizeof(rpn_elem_t));
                if( auxiliary_elem == (rpn_elem_t*)NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to rpn_elem_t.\n");
                    exit(EXIT_FAILURE);
                }

                auxiliary_elem->next = (rpn_elem_t*) NULL;
                auxiliary_elem->element = (void*) auxiliary_number;
                auxiliary_elem->type = MATH_CONSTANT;

                rpn_push_queue(queue, auxiliary_elem);
                break;

            case 'c':
                if( (i+2) < infix_notation_len ) {
                    if( infix_notation[i+1] == 'o' ) {
                        if( infix_notation[i+2] == 's' ) {
                            i += 2;

                            auxiliary_str = (char*) malloc(sizeof(char));
                            if( auxiliary_str == (char*)NULL ) {
                                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                                fprintf(stderr, "Could not allocate memory to char.\n");
                                exit(EXIT_FAILURE);
                            }

                            *auxiliary_str = 'C';

                            auxiliary_elem = (rpn_elem_t*) malloc(sizeof(rpn_elem_t));
                            if( auxiliary_elem == (rpn_elem_t*)NULL ) {
                                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                                fprintf(stderr, "Could not allocate memory to rpn_elem_t.\n");
                                exit(EXIT_FAILURE);
                            }

                            auxiliary_elem->next = (rpn_elem_t*) NULL;
                            auxiliary_elem->element = (void*) auxiliary_str;
                            auxiliary_elem->type = TRIGONOMETRIC;

                            rpn_push_stack(stack_operators, auxiliary_elem);

                        }
                    }
                }
                break;

            case 's':
                if( (i+2) < infix_notation_len ) {
                    if( infix_notation[i+1] == 'i' || infix_notation[i+1] == 'e' ) {
                        if( infix_notation[i+2] == 'n' ) {
                            i += 2;

                            auxiliary_str = (char*) malloc(sizeof(char));
                            if( auxiliary_str == (char*)NULL ) {
                                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                                fprintf(stderr, "Could not allocate memory to char.\n");
                                exit(EXIT_FAILURE);
                            }

                            *auxiliary_str = 'S';

                            auxiliary_elem = (rpn_elem_t*) malloc(sizeof(rpn_elem_t));
                            if( auxiliary_elem == (rpn_elem_t*)NULL ) {
                                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                                fprintf(stderr, "Could not allocate memory to rpn_elem_t.\n");
                                exit(EXIT_FAILURE);
                            }

                            auxiliary_elem->next = (rpn_elem_t*) NULL;
                            auxiliary_elem->element = (void*) auxiliary_str;
                            auxiliary_elem->type = TRIGONOMETRIC;

                            rpn_push_stack(stack_operators, auxiliary_elem);
                        }
                    }
                }
                break;

            case 't':
                if( (i+2) < infix_notation_len ) {
                    if( infix_notation[i+1] == 'a' ) {
                        if( infix_notation[i+2] == 'n' ) {
                            i += 2;

                            auxiliary_str = (char*) malloc(sizeof(char));
                            if( auxiliary_str == (char*)NULL ) {
                                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                                fprintf(stderr, "Could not allocate memory to char.\n");
                                exit(EXIT_FAILURE);
                            }

                            *auxiliary_str = 'T';

                            auxiliary_elem = (rpn_elem_t*) malloc(sizeof(rpn_elem_t));
                            if( auxiliary_elem == (rpn_elem_t*)NULL ) {
                                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                                fprintf(stderr, "Could not allocate memory to rpn_elem_t.\n");
                                exit(EXIT_FAILURE);
                            }

                            auxiliary_elem->next = (rpn_elem_t*) NULL;
                            auxiliary_elem->element = (void*) auxiliary_str;
                            auxiliary_elem->type = TRIGONOMETRIC;

                            rpn_push_stack(stack_operators, auxiliary_elem);
                        }
                    } else {
                        if( infix_notation[i+1] == 'g' ) {
                            i += 1;

                            auxiliary_str = (char*) malloc(sizeof(char));
                            if( auxiliary_str == (char*)NULL ) {
                                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                                fprintf(stderr, "Could not allocate memory to char.\n");
                                exit(EXIT_FAILURE);
                            }

                            *auxiliary_str = 'T';

                            auxiliary_elem = (rpn_elem_t*) malloc(sizeof(rpn_elem_t));
                            if( auxiliary_elem == (rpn_elem_t*)NULL ) {
                                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                                fprintf(stderr, "Could not allocate memory to rpn_elem_t.\n");
                                exit(EXIT_FAILURE);
                            }

                            auxiliary_elem->next = (rpn_elem_t*) NULL;
                            auxiliary_elem->element = (void*) auxiliary_str;
                            auxiliary_elem->type = TRIGONOMETRIC;

                            rpn_push_stack(stack_operators, auxiliary_elem);
                        }
                    }
                }
                break;

            case ' ':
                break;

            case '(':
                auxiliary_str = (char*) malloc(sizeof(char));
                if( auxiliary_str == (char*)NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to string.\n");
                    exit(EXIT_FAILURE);
                }

                *auxiliary_str = chr;

                auxiliary_elem = (rpn_elem_t*) malloc(sizeof(rpn_elem_t));
                if( auxiliary_elem == (rpn_elem_t*)NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to rpn_elem_t.\n");
                    exit(EXIT_FAILURE);
                }

                auxiliary_elem->next = (rpn_elem_t*) NULL;
                auxiliary_elem->element = (void*) auxiliary_str;
                auxiliary_elem->type = (rpn_type_elem_t) OPERATOR;

                rpn_push_stack(stack_operators, auxiliary_elem);
                break;

            case ')':
                while( (stack_top_operator = *(char*)stack_operators->top->element) != '(' ) {
                    rpn_push_queue(queue, rpn_pop_stack(stack_operators));
                }

                free(rpn_pop_stack(stack_operators));

                break;

            default:
                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                fprintf(stderr, "Invalid character \'%c\'\n", chr);
                exit(EXIT_FAILURE);
        }

        auxiliary_elem = (rpn_elem_t*) NULL;
        auxiliary_str = (char*) NULL;
    }

    while( (stack_operators->size) ) {
        auxiliary_elem = rpn_pop_stack(stack_operators);
        rpn_push_queue(queue, auxiliary_elem);
    }

    if( rpn_exit_stack(stack_operators) ) {
        fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
        fprintf(stderr, "There was an error in rpn_exit_stack().\n");
        exit(EXIT_FAILURE);
    }

    free(stack_operators);

    return EXIT_SUCCESS;
}

int rpn_copy_queue(const rpn_queue_t* queue_src, rpn_queue_t* queue_dst) {
    rpn_elem_t* auxiliary_elem;
    rpn_elem_t* new_elem;

    char* auxiliary_str;
    double* auxiliary_number;

    if( queue_dst->size || queue_dst->first != (rpn_elem_t*) NULL ) {
        if( rpn_exit_queue(queue_dst) ) {
            fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
            fprintf(stderr, "There was an error in rpn_exit_queue().\n");
            exit(EXIT_FAILURE);
        }

        if( rpn_init_queue(queue_dst) ) {
            fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
            fprintf(stderr, "There was an error in rpn_init_queue().\n");
            exit(EXIT_FAILURE);
        }
    }

    auxiliary_elem = (rpn_elem_t*) queue_src->first;
    while( auxiliary_elem != (rpn_elem_t*)NULL ) {
        new_elem = (rpn_elem_t*) malloc(sizeof(rpn_elem_t));
        if( new_elem == (rpn_elem_t*) NULL ) {
            fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
            fprintf(stderr, "Could not allocate memory to rpn_elem_t.\n");
            exit(EXIT_FAILURE);
        }

        new_elem->type = (rpn_type_elem_t) auxiliary_elem->type;
        new_elem->next = (rpn_elem_t*)NULL;

        if( new_elem->type == NUMBER || new_elem->type == MATH_CONSTANT ) {
            auxiliary_number = (double*) malloc(sizeof(double));
            if( auxiliary_number == (double*)NULL ) {
                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                fprintf(stderr, "Could not allocate memory to double.\n");
                exit(EXIT_FAILURE);
            }

            *auxiliary_number = *(double*)auxiliary_elem->element;
            new_elem->element = auxiliary_number;
        } else {
            auxiliary_str = (char*) malloc(sizeof(char));
            if( auxiliary_str == (char*)NULL ) {
                fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                fprintf(stderr, "Could not allocate memory to char.\n");
                exit(EXIT_FAILURE);
            }

            *auxiliary_str = *(char*)auxiliary_elem->element;
            new_elem->element = auxiliary_str;
        }

        rpn_push_queue(queue_dst, new_elem);

        new_elem = (rpn_elem_t*) NULL;
        auxiliary_number = (double*) NULL;
        auxiliary_str   = (char*) NULL;
        auxiliary_elem = (rpn_elem_t*) auxiliary_elem->next;
    }

    return EXIT_SUCCESS;
}

double rpn_solve(rpn_queue_t* queue, const double unknown) {
    char operator;

    double final_value;
    double* auxiliary_number;

    rpn_stack_t stack;
    rpn_elem_t* auxiliary_elem;

    rpn_elem_t* elem1;
    rpn_elem_t* elem2;

    rpn_elem_t* new_elem;

    if( rpn_init_stack(&stack) ) {
        fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
        fprintf(stderr, "There was an error in rpn_init_stack().\n");
        exit(EXIT_FAILURE);
    }

    while( queue->size ) {
        auxiliary_elem = (rpn_elem_t*) rpn_pop_queue(queue);

        switch(auxiliary_elem->type) {
            case NUMBER:
            case MATH_CONSTANT:
                rpn_push_stack(&stack, auxiliary_elem);
                break;

            case UNKNOWN:
                auxiliary_number = (double*) malloc(sizeof(double));
                if( auxiliary_number == (double*) NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to double.\n");
                    exit(EXIT_FAILURE);
                }

                *auxiliary_number = unknown;

                auxiliary_elem->type = NUMBER;
                free(auxiliary_elem->element);
                auxiliary_elem->element = (void*) auxiliary_number;

                rpn_push_stack(&stack, auxiliary_elem);
                break;

            case OPERATOR:

                elem2 = (rpn_elem_t*) rpn_pop_stack(&stack);
                elem1 = (rpn_elem_t*) rpn_pop_stack(&stack);

                new_elem = (rpn_elem_t*) malloc(sizeof(rpn_elem_t));
                if( new_elem == (rpn_elem_t*) NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to rpn_elem_t.\n");
                    exit(EXIT_FAILURE);
                }

                new_elem->next = (rpn_elem_t*) NULL;
                new_elem->type = (rpn_type_elem_t) NUMBER;

                auxiliary_number = (double*) malloc(sizeof(double));
                if( auxiliary_number == (double*) NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to double.\n");
                    exit(EXIT_FAILURE);
                }

                operator = * (char*) auxiliary_elem->element;

                switch( operator ) {
                    case '+':
                        *auxiliary_number = ((*(double*)elem1->element) + (*(double*)elem2->element));
                        break;
                    case '-':
                        *auxiliary_number = ((*(double*)elem1->element) - (*(double*)elem2->element));
                        break;
                    case '*':
                        *auxiliary_number = ((*(double*)elem1->element) * (*(double*)elem2->element));
                        break;
                    case '/':
                        *auxiliary_number = ((*(double*)elem1->element) / (*(double*)elem2->element));
                        break;
                    case '^':
                        *auxiliary_number = pow((*(double*)elem1->element), (*(double*)elem2->element));
                        break;
                }

                free(elem1);
                free(elem2);

                new_elem->element = (void*) auxiliary_number;
                rpn_push_stack(&stack, new_elem);

                break;

            case TRIGONOMETRIC:
                elem1 = (rpn_elem_t*) rpn_pop_stack(&stack);

                new_elem = (rpn_elem_t*) malloc(sizeof(rpn_elem_t));
                if( new_elem == (rpn_elem_t*) NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to rpn_elem_t.\n");
                    exit(EXIT_FAILURE);
                }

                new_elem->next = (rpn_elem_t*) NULL;
                new_elem->type = (rpn_type_elem_t) NUMBER;

                auxiliary_number = (double*) malloc(sizeof(double));
                if( auxiliary_number == (double*) NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to double.\n");
                    exit(EXIT_FAILURE);
                }

                operator = * (char*) auxiliary_elem->element;

                switch( operator ) {
                    case 'C':
                        *auxiliary_number = cos( *(double*)elem1->element );
                        break;

                    case 'S':
                        *auxiliary_number = sin( *(double*)elem1->element );
                        break;

                    case 'T':
                        *auxiliary_number = tan( *(double*)elem1->element );
                        break;
                }

                free(elem1);

                new_elem->element = (void*) auxiliary_number;
                rpn_push_stack(&stack, new_elem);

                break;
        }

        auxiliary_number = (double*) NULL;
        elem1 = (rpn_elem_t*) NULL;
        elem2 = (rpn_elem_t*) NULL;
        new_elem = (rpn_elem_t*) NULL;

    }

    auxiliary_elem = rpn_pop_stack(&stack);

    final_value = *(double*) auxiliary_elem->element;

    free(auxiliary_elem);

    if( rpn_exit_stack(&stack) ) {
        fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
        fprintf(stderr, "There was an error in rpn_exit_stack().\n");
        exit(EXIT_FAILURE);
    }

    return final_value;

}

double rpn_solve_light(const rpn_queue_t* queue, const double unknown) {
    double  final_value;
    double  auxiliary_number;
    double* numbers;
    unsigned n_numbers;
    rpn_elem_t* auxiliary_elem;

    n_numbers=0;

    numbers = (double*)NULL;

    auxiliary_elem = queue->first;
    while( auxiliary_elem != (rpn_elem_t*)NULL ) {
        auxiliary_number=0;

        switch( auxiliary_elem->type ) {
            case UNKNOWN:
                numbers = (double*) realloc(numbers, sizeof(double)*(n_numbers+1));
                if( numbers == (double*)NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to double.\n");
                    exit(EXIT_FAILURE);
                }

                numbers[n_numbers] = unknown;
                n_numbers++;
                break;

            case NUMBER:
            case MATH_CONSTANT:
                numbers = (double*) realloc(numbers, sizeof(double)*(n_numbers+1));
                if( numbers == (double*)NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to double.\n");
                    exit(EXIT_FAILURE);
                }

                numbers[n_numbers] = *(double*)auxiliary_elem->element;
                n_numbers++;
                break;

            case OPERATOR:
                switch ( *(char*)auxiliary_elem->element ) {
                    case '+':
                        auxiliary_number = numbers[n_numbers-2] + numbers[n_numbers-1];
                        break;

                    case '-':
                        auxiliary_number = numbers[n_numbers-2] - numbers[n_numbers-1];
                        break;

                    case '*':
                        auxiliary_number = numbers[n_numbers-2] * numbers[n_numbers-1];
                        break;

                    case '/':
                        auxiliary_number = numbers[n_numbers-2] / numbers[n_numbers-1];
                        break;

                    case '^':
                        auxiliary_number = pow(numbers[n_numbers-2], numbers[n_numbers-1]);
                        break;
                }

                n_numbers -= 1;

                numbers = (double*) realloc(numbers, sizeof(double)*n_numbers);
                if( numbers == (double*)NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to double.\n");
                    exit(EXIT_FAILURE);
                }

                numbers[n_numbers-1] = auxiliary_number;
                break;

            case TRIGONOMETRIC:
                switch ( *(char*)auxiliary_elem->element ) {
                    case 'C':
                        auxiliary_number = cos(numbers[n_numbers-1]);
                        break;

                    case 'S':
                        auxiliary_number = sin(numbers[n_numbers-1]);
                        break;

                    case 'T':
                        auxiliary_number = tan(numbers[n_numbers-1]);
                        break;
                }

                numbers = (double*) realloc(numbers, sizeof(double)*n_numbers);
                if( numbers == (double*)NULL ) {
                    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
                    fprintf(stderr, "Could not allocate memory to double.\n");
                    exit(EXIT_FAILURE);
                }

                numbers[n_numbers-1] = auxiliary_number;
                break;
        }

        auxiliary_elem = (rpn_elem_t*) auxiliary_elem->next;
    }

    final_value = *numbers;
    free(numbers);

    return final_value;
}

int rpn_init_queue(rpn_queue_t* queue) {

    queue->size = 0;
    queue->first = (rpn_elem_t*) NULL;
    queue->last  = (rpn_elem_t*) NULL;

    return EXIT_SUCCESS;
}

int rpn_exit_queue(rpn_queue_t* queue) {

    void rpn_down_queue_recursively(unsigned* size, rpn_elem_t* elem) {

        if( elem != (rpn_elem_t*) NULL ) {
            rpn_down_queue_recursively(size, elem->next);
            *size -= 1;
            free(elem);
        }

    }

    rpn_down_queue_recursively(&(queue->size), queue->first);

    if( !(queue->size) ) {
        queue->first = (rpn_elem_t*) NULL;
        queue->last  = (rpn_elem_t*) NULL;

        return EXIT_SUCCESS;
    }

    fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
    fprintf(stderr, "Problem to free RPN's queue.\n");
    exit(EXIT_FAILURE);
}

int rpn_push_queue(rpn_queue_t* queue,  rpn_elem_t* elem) {

    elem->next = (rpn_elem_t*) NULL;

    if( !(queue->size) ) {
        queue->first = (rpn_elem_t*) elem;
    } else {
        queue->last->next = (rpn_elem_t*) elem;
    }

    queue->last = (rpn_elem_t*) elem;
    queue->size += 1;

    return EXIT_SUCCESS;
}

rpn_elem_t* rpn_pop_queue(rpn_queue_t* queue) {
    rpn_elem_t* auxiliary_elem;

    if( !(queue->size) ) {
        fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
        fprintf(stderr, "You cannot get an element with the empty queue.");
        exit(EXIT_FAILURE);
    }

    auxiliary_elem = (rpn_elem_t*) queue->first;
    queue->first = queue->first->next;
    auxiliary_elem->next = (rpn_elem_t*) NULL;
    queue->size -= 1;

    return (rpn_elem_t*) auxiliary_elem;
}

void rpn_show_queue(const rpn_queue_t* queue) {
    rpn_elem_t* auxiliary_elem;

    fprintf(stdout, "[" C_BOLD "INFO" C_RESET "] ");
    fprintf(stdout, "Informations about queue\n");
    fprintf(stdout, "Queue size: %u elements\n\n", queue->size);

    fprintf(stdout, "Content stored in it\n");

    auxiliary_elem = queue->first;
    while( auxiliary_elem != (rpn_elem_t*) NULL ) {

        switch(auxiliary_elem->type) {
            case NUMBER:
                fprintf(stdout, "NUMBER:\t\t%f\n", *(double*)auxiliary_elem->element);
                break;
            case MATH_CONSTANT:
                fprintf(stdout, "MATH_CONSTANT:\t%f\n", *(double*)auxiliary_elem->element);
                break;
            case OPERATOR:
                fprintf(stdout, "OPERATOR:\t%c\n", *(char*)auxiliary_elem->element);
                break;
            case UNKNOWN:
                fprintf(stdout, "UNKNOWN:\t%c\n", *(char*)auxiliary_elem->element);
                break;
            case TRIGONOMETRIC:
                fprintf(stdout, "TRIGONOMETRIC:\t%c\n", *(char*)auxiliary_elem->element);
                break;
        }

        auxiliary_elem = (rpn_elem_t*) auxiliary_elem->next;
    }

    fprintf(stdout, "[" C_BOLD "INFO" C_RESET "] ");
    fprintf(stdout, "End informations about the queue.\n");
}

int rpn_init_stack(rpn_stack_t* stack) {

    stack->size = 0;
    stack->top = (rpn_elem_t*) NULL;

    return EXIT_SUCCESS;
}

int rpn_exit_stack(rpn_stack_t* stack) {

    void rpn_down_stack_recursively(unsigned* size, rpn_elem_t* elem) {

        if( elem != (rpn_elem_t*) NULL ) {
            rpn_down_stack_recursively(size, elem->next);
            *size -= 1;
            free(elem);
        }

    }

    rpn_down_stack_recursively(&(stack->size), stack->top);

    if( !(stack->size) ) {
        stack->top = (rpn_elem_t*) NULL;
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

int rpn_push_stack(rpn_stack_t* stack, rpn_elem_t* elem) {

    if( !(stack->size) ) {
        elem->next = (rpn_elem_t*) NULL;
    } else {
        elem->next = (rpn_elem_t*) stack->top;
    }

    stack->top = (rpn_elem_t*) elem;
    stack->size += 1;

    return EXIT_SUCCESS;
}

rpn_elem_t* rpn_pop_stack(rpn_stack_t* stack) {
    rpn_elem_t* auxiliary_elem;

    if( !(stack->size) ) {
        fprintf(stderr, "[" C_RED "ERROR" C_RESET "] ");
        fprintf(stderr, "You cannot get an element with the empty stack.");
        exit(EXIT_FAILURE);
    }

    auxiliary_elem = (rpn_elem_t*) stack->top;
    stack->top = stack->top->next;
    auxiliary_elem->next = (rpn_elem_t*) NULL;
    stack->size -= 1;

    return (rpn_elem_t*) auxiliary_elem;
}

void rpn_show_stack(const rpn_stack_t* stack) {
    rpn_elem_t* auxiliary_elem;

    fprintf(stdout, "[" C_BOLD "INFO" C_RESET "] ");
    fprintf(stdout, "Informations about stack\n");
    fprintf(stdout, "Stack size: %u elements\n\n", stack->size);

    fprintf(stdout, "Content stored in it\n");

    auxiliary_elem = stack->top;
    while( auxiliary_elem != (rpn_elem_t*) NULL ) {

        switch(auxiliary_elem->type) {
            case NUMBER:
                fprintf(stdout, "NUMBER:\t\t%f\n", *(double*)auxiliary_elem->element);
                break;
            case MATH_CONSTANT:
                fprintf(stdout, "MATH_CONSTANT:\t%f\n", *(double*)auxiliary_elem->element);
                break;
            case OPERATOR:
                fprintf(stdout, "OPERATOR:\t%c\n", *(char*)auxiliary_elem->element);
                break;
            case UNKNOWN:
                fprintf(stdout, "UNKNOWN:\t%c\n", *(char*)auxiliary_elem->element);
                break;
            case TRIGONOMETRIC:
                fprintf(stdout, "TRIGONOMETRIC:\t%c\n", *(char*)auxiliary_elem->element);
                break;
        }

        auxiliary_elem = (rpn_elem_t*) auxiliary_elem->next;
    }

    fprintf(stdout, "[" C_BOLD "INFO" C_RESET "] ");
    fprintf(stdout, "End informations about the queue.\n");
}
