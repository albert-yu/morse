#ifndef RECEIVE_H
#define RECEIVE_H

#include "endpoints.h"
#include "statuscodes.h"


#ifdef __cplusplus
extern "C" {
#endif

int morse_list_folders();

void list_last_n(char *box_name, size_t n);

#ifdef __cplusplus
}
#endif

#endif
