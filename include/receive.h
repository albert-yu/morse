#ifndef RECEIVE_H
#define RECEIVE_H

#include "endpoints.h"

#ifdef __cplusplus
extern "C" {
#endif


int simple_fetch();

int morse_retrieve_last_n(char *bearertoken, size_t n);

#ifdef __cplusplus
}
#endif

#endif