#ifndef TYPES_H
#define TYPES_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Type to keep track of recipients
 */
typedef struct recipient_list_t {
    char **recipients;
    size_t count;
} RecipientList; 


#ifdef __cplusplus
}
#endif

#endif

