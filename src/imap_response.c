#include "imap_response.h"


/*
 * Allocates memory for this DTO.
 */
ImapResponse* imap_response_new() {
    ImapResponse* new_resp = NULL;
    MemoryStruct *inner_data = memstruct_new();
    
    if (inner_data) {
        new_resp = malloc(sizeof(*new_resp));
        if (new_resp) {
            new_resp->data = inner_data;
            new_resp->status = 0;
            MemoryStruct *verbose_data = memstruct_new();
            if (verbose_data) {
                new_resp->v_data = verbose_data;
            }
        }
        else {
            memstruct_free(inner_data);
        }
    }
    return new_resp;
}


void imap_response_free(ImapResponse* response) {
    // memstruct_free should check for NULL
    memstruct_free(response->data);
    memstruct_free(response->v_data);
    free(response);
}


