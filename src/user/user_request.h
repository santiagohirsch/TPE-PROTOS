#ifndef _USER_REQUEST_H_
#define _USER_REQUEST_H_

struct request * get_request(int argc, char * argv[]);

void build_request(struct request * req, char * buf);

#endif