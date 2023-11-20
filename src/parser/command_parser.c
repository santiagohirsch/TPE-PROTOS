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

static void finish(struct parser_event *ret, const uint8_t c) {
    ret->type                 = EQ;
    ret->idx                  = 0;
    ret->command_len++;
    ret->arg1_len++;
    ret->arg2_len++;
}

static const struct parser_state_transition ST_COMMAND[] =  {
    {.when = ANY,     .dest = COMMAND,  .action = copy_command},
    {.when = '\n',    .dest = FINISHED, .action = finish},
    {.when = ' ',     .dest = ARG1,     .action = next_arg},
};

static const struct parser_state_transition ST_ARG1[] =  {
    {.when = ANY,     .dest = ARG1,     .action = copy_arg1},
    {.when = '\n',    .dest = FINISHED, .action = finish},
    {.when = ' ',     .dest = ARG2,     .action = next_arg},
};

static const struct parser_state_transition ST_ARG2[] =  {
    {.when = ANY,     .dest = ARG2,     .action = copy_arg2},
    {.when = '\n',    .dest = FINISHED, .action = finish},
};

static const struct parser_state_transition *states[] = {ST_COMMAND, ST_ARG1, ST_ARG2};

static const size_t states_n[] = {N(ST_COMMAND), N(ST_ARG1), N(ST_ARG2)};

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
        parser_feed(p, buffer[i]);
    }

    *bytes_read = i;
    return event;
}