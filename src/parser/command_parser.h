#ifndef _COMMAND_PARSER_H_
#define _COMMAND_PARSER_H_

/*
 * parser_utils.c -- factory de ciertos parsers típicos
 *
 * Provee parsers reusables, como por ejemplo para verificar que
 * un string es igual a otro de forma case insensitive.
 */
#include "parser.h"
#include <stdlib.h>
#include "../server/server_utils.h"

enum command_states {
    COMMAND,
    ARG1,
    ARG2,
    FINISHED
};

enum command_event_types {
    MAYEQ,
    /** hay posibilidades de que el string sea igual */
    EQ,
    /** NO hay posibilidades de que el string sea igual */
    NEQ
};

struct parser * command_parser_init();

struct parser_event * get_command(struct parser_event * event, struct parser * p, char * buffer, size_t bytes, size_t * bytes_read);

void command_parser_destroy(struct parser * p);

void command_parser_reset(struct parser * p);

struct parser_event * get_command_parser_event(struct parser * p);

#endif
