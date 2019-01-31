#ifndef _NET_H_
#define _NET_H_


#ifdef __cplusplus
extern "C" {
#endif

void *get_in_addr(struct sockaddr *sa);
int get_listener_socket(char *port);


#ifdef __cplusplus
}
#endif

#endif