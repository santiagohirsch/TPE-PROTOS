#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "command_parser.h"

#define N(x) (sizeof(x)/sizeof((x)[0]))

static void copy_command(struct parser_event *ret, const uint8_t c) {
    ret->type                = MAYEQ;
    ret->command[ret->idx++] = c;
    ret->command_len++;
    ret->command[ret->idx]   = '\0';
}

static void copy_arg1(struct parser_event *ret, const uint8_t c) {
    ret->type                 = MAYEQ;
    ret->arg1[ret->idx++]     = c;
    ret->arg1_len++;
    ret->arg1[ret->idx]       = '\0';
}

static void copy_arg2(struct parser_event *ret, const uint8_t c) {
    ret->type                 = MAYEQ;
    ret->arg2[ret->idx++]     = c;
    ret->arg2_len++;
    ret->arg2[ret->idx]       = '\0';
}

static void next_arg(struct parser_event *ret, const uint8_t c) {
    ret->type                 = MAYEQ;
    ret->idx                  = 0;
}

static void finishing(struct parser_event *ret, const uint8_t c) {
    ret->type                 = MAYEQ;
    ret->idx                  = 0;
    ret->command_len++;
    ret->arg1_len++;
    ret->arg2_len++;
}

static void finished(struct parser_event *ret, const uint8_t c) {
    ret->type                 = EQ;
}

static const struct parser_state_transition ST_COMMAND[] =  {
    {.when = '\r',    .dest = FINISHING, .action = finishing},
    {.when = ' ',     .dest = ARG1,     .action = next_arg},
    {.when = ANY,     .dest = COMMAND,  .action = copy_command},
};

static const struct parser_state_transition ST_ARG1[] =  {
    {.when = '\r',    .dest = FINISHING, .action = finishing},
    {.when = ' ',     .dest = ARG2,     .action = next_arg},
    {.when = ANY,     .dest = ARG1,     .action = copy_arg1},
};

static const struct parser_state_transition ST_ARG2[] =  {
    {.when = '\r',    .dest = FINISHING, .action = finishing},
    {.when = ANY,     .dest = ARG2,     .action = copy_arg2},
};

static const struct parser_state_transition ST_FINISHING[] =  {
    {.when = '\n',    .dest = FINISHED, .action = finished},
};  

static const struct parser_state_transition *states[] = {ST_COMMAND, ST_ARG1, ST_ARG2, ST_FINISHING};

static const size_t states_n[] = {N(ST_COMMAND), N(ST_ARG1), N(ST_ARG2), N(ST_FINISHING)};

static const struct parser_definition command_parser_definition = {
    .states_count = N(states),
    .states       = states,
    .states_n     = states_n,
    .start_state  = COMMAND,
};

struct parser * command_parser_init() {
    return parser_init(parser_no_classes(), &command_parser_definition);
}

struct parser_event * get_command(struct parser_event * event, struct parser * p, char * buffer, size_t bytes, size_t * bytes_read) {
    int i;
    for(i = 0; i < bytes && event->type == MAYEQ; i++) {
        event = parser_feed(p, buffer[i]);
    }

    *bytes_read = i;
    return event;
}

void command_parser_destroy(struct parser * p) {
    if (p != NULL) {
        parser_destroy(p);
    }
}

void command_parser_reset(struct parser * p){
    parser_reset(p);
    struct parser_event * event = get_parser_event(p);
    event->type = MAYEQ;
    event->idx = 0;
    event->command_len = 0;
    event->arg1_len = 0;
    event->arg2_len = 0;
    memset(event->command, 0, sizeof(event->command));
    memset(event->arg1, 0, sizeof(event->arg1));
    memset(event->arg2, 0, sizeof(event->arg2));
}

struct parser_event * get_command_parser_event(struct parser * p){
    return get_parser_event(p);
}