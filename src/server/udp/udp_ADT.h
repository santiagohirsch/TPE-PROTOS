#ifndef _UDP_ADT_H_
#define _UDP_ADT_H_

#include "../../selector/selector.h"

typedef struct udp * udp_ADT;

udp_ADT init_udp();

int get_udp_socket();

int validate_credentials(const char * username, const char * password);

fd_handler * get_udp_fd_handler();

void set_fd_handler(void (*handle_read)(struct selector_key * key), void (*handle_write)(struct selector_key * key));

#endif