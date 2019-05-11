#include "imap_response.h"


/*
 * Allocates memory for this DTO.
 */
ImapResponse* imap_response_new() {
    ImapResponse* new_resp = NULL;
    MemoryStruct *inner_data = memstruct_new();
    if (inner_data) {
        new_resp = malloc(sizeof(ImapResponse));
        if (new_resp) {
            new_resp->data = inner_data;
            new_resp->status = 0;
        }
        else {
            memstruct_free(inner_data);
        }
    }
    return new_resp;
}


void imap_response_free(ImapResponse* response) {
    memstruct_free(response->data);
    free(response);
}
