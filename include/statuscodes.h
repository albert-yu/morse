/*
 * This file defines the status codes for various actions
 */

#ifndef MORSE_STATUS_CODES_H
#define MORSE_STATUS_CODES_H

typedef enum morse_status_code_t {
    
    /* Everything is OK */	
    MorseStatus_OK,

    /* When executing an IMAP command, the input is bad */
    MorseStatus_BadInput,
    
    /* Unable to authenticate with the bearer token */
    MorseStatus_InvalidCreds,
   
    MorseStatus_InvalidArg, 
    MorseStatus_MemoryError,
    
    /* Error occurred executing CURL command */
    MorseStatus_CurlError,
    /* Catch-all */
    MorseStatus_ErrUnknown
} MorseStatusCode;

#endif
