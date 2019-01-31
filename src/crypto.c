#include <string.h>
#include "crypto.h"
#include "secrets.h"
#include "file.h"

// libsodium.a
#include "sodium.h"

#define INT_UPPER_BOUND UINT32_MAX
#define CRYPTO_FILEPATH "./temp"
#define CRYPTO_NONCE_FILENAME "state.txt"
#define CRYPTO_SECRET_FILENAME "creds.txt"
#define CRYPTO_DECRYPT_LEN_FILENAME "len.txt"
#define CRYPTO_FILEPATH_SIZE 32
#define NUM_BYTES_IN_UNSIGNED_LONG 4
#define UNSIGNED_LONG_HEX_LEN (NUM_BYTES_IN_UNSIGNED_LONG * 2)


/*
 * Convert bytes to hex string by parsing two chars at a time.
 * Need to free resulting pointer's memory.
 * Adapted from https://stackoverflow.com/a/46210658
 */
char* bytes_to_hex(unsigned char *bytes, size_t bytes_len) {
    char *hex = calloc(bytes_len * 2, sizeof(*hex));

    for (size_t i = 0, j = 0; i < bytes_len; ++i, j += 2) {
        sprintf(hex + j, "%02x", bytes[i] & 0xff);
    }
    return hex;
}


/*
 * Convert hex string to bytes by parsing two chars at a time
 * Need to free resulting pointer's memory.
 * Adapted from https://stackoverflow.com/a/46210658
 */
unsigned char* hex_to_bytes_with_len(char *hex_string, size_t hex_length) {
    // not handling the odd case--yet
    if ((hex_length % 2) != 0) {
        unsigned char *empty_buf = calloc(1, sizeof(*empty_buf));
        return empty_buf;
    }

    size_t ascii_length = hex_length / 2;
    unsigned char *byte_array = calloc(ascii_length + 1, sizeof(*byte_array));
    size_t i = 0;
    size_t j = 0;

    for (; j < hex_length; ++i, j += 2) {
        int val [1];
        sscanf(hex_string + j, "%2x", val);
        byte_array[i] = val[0];
        byte_array[i + 1] = '\0';
    }
    return byte_array;
}


unsigned char* hex_to_bytes(char *hex_string) {
    size_t hex_length = strlen(hex_string);
    return hex_to_bytes_with_len(hex_string, hex_length);
}


/**
 * Convert unsigned long to null-terminated byte array.
 * Caller must free returned buffer.
 * Adapted from: https://stackoverflow.com/a/3784478/9555588
 */
unsigned char* unsigned_long_to_bytes(unsigned long n) {
    int size = NUM_BYTES_IN_UNSIGNED_LONG + 1; // 4 + 1
    unsigned char *bytes = calloc(size, sizeof(*bytes));
    bytes[0] = (n >> 24) & 0xff;
    bytes[1] = (n >> 16) & 0xff;
    bytes[2] = (n >> 8) & 0xff;
    bytes[3] = n & 0xFF;
    return bytes;
}


/**
 * Convert bytes to unsigned long
 */
unsigned long bytes_to_unsigned_long(unsigned char *bytes) {
    unsigned long value = 0;
    value += (bytes[0] << 24) & 0xffff;
    value += (bytes[1] << 16) & 0xffff;
    value += (bytes[2] << 8) & 0xffff;
    value += (bytes[3]) & 0xffff;
    return value;
}


/**
 * Convert hex string to unsigned long. Will truncate anything longer than 8 chars.
 */
unsigned long hex_bytes_to_unsigned_long(char *hex_bytes) {
    unsigned char *bytes = hex_to_bytes_with_len(hex_bytes, UNSIGNED_LONG_HEX_LEN);
    return bytes_to_unsigned_long(bytes);
}


uint32_t gen_random_int() {
    uint32_t ret_val;
    if (sodium_init() < 0) {
        /* panic! the library couldn't be initialized, it is not safe to use */
        fprintf(stderr, "Could not initialize sodium! Exiting...\n");
        exit(1); 
    }

    ret_val = randombytes_uniform(INT_UPPER_BOUND);
    return ret_val;
}


/*
 * Get the temporary file location
 */
void get_nonce_fileloc(char *buffer) {
    sprintf(buffer, "%s/%s", CRYPTO_FILEPATH, CRYPTO_NONCE_FILENAME);
}


void get_secret_fileloc(char *buffer) {
    sprintf(buffer, "%s/%s", CRYPTO_FILEPATH, CRYPTO_SECRET_FILENAME);
}


void populate_strbuffer(char *buffer, size_t buf_size) {
    /* buffer will not be null-terminated */        
    randombytes_buf(buffer, buf_size);
}


unsigned char* generate_random_key() {
    unsigned char *key = 
        calloc(crypto_secretbox_KEYBYTES + 1, sizeof(*key));
    crypto_secretbox_keygen(key);
    return key;
}


/**
 * Writes the nonce to a file. Overwrites any existing contents
 */
void set_nonce(unsigned char *nonce) {
    // make printable with hex chars
    char *hex_str = bytes_to_hex(nonce, crypto_secretbox_NONCEBYTES);

    // write to file
    file_write(hex_str, CRYPTO_FILEPATH, CRYPTO_NONCE_FILENAME);

    free(hex_str);
}


size_t byteslen(unsigned char *bytes) {
    size_t ret_val = 0;
    unsigned char *ptr;
    while (*ptr) {
        ret_val++;
        ptr++;
    }

    return ret_val;
}


/**
 * Gets the nonce from a file and populates the
 * specified buffer.
 * Returns 0 if success.
 */
int get_nonce(unsigned char *nonce) {
    char nonce_file [CRYPTO_FILEPATH_SIZE];
    get_nonce_fileloc(nonce_file);

    FileData *filestruct = file_load(nonce_file);
    if (filestruct == NULL) {
        printf("File does not exist.\n");
        return 1;
    }

    // file should be stored as hex string
    // bytes should be null-terminated
    unsigned char *bytes = hex_to_bytes_with_len(filestruct->data, filestruct->size);

    // copy result to buffer
    memcpy(nonce, bytes, crypto_secretbox_NONCEBYTES);

    // gc
    free(bytes);
    bytes = NULL;
    file_free(filestruct);

    return 0;
}


/**
 * Generates a new nonce and writes it to a file if 
 * caller specifies
 */
void generate_nonce(unsigned char *nonce, int write_to_file) {
    randombytes_buf(nonce, sizeof nonce);
    if (write_to_file) {
        set_nonce(nonce);
    }
}


/**
 * Populates the buffer with the secret key
 */
void init_secret_key(unsigned char *key_buf) {
    // get secret key and copy to buffer
    char *hex_key = SECRET_KEY; /* from SECRETS_H */
    unsigned char *key_read = hex_to_bytes(hex_key);
    memcpy(key_buf, key_read, crypto_secretbox_KEYBYTES);
    free(key_read);
}


char* crypto_encrypt(char *message, size_t msg_len) {
    const unsigned char *bytes = (const unsigned char *)message;
    size_t CIPHERTEXT_LEN = crypto_secretbox_MACBYTES + msg_len;
    unsigned char key [crypto_secretbox_KEYBYTES];
    unsigned char nonce [crypto_secretbox_NONCEBYTES];
    unsigned char ciphertext [CIPHERTEXT_LEN];

    // get secret key and copy to buffer
    init_secret_key(key);

    // generate nonce and write it to a file
    generate_nonce(nonce, 1);

    // encrypt!
    crypto_secretbox_easy(ciphertext, bytes, msg_len, nonce, key);

    char *returned_buf = calloc(CIPHERTEXT_LEN + 1, sizeof(*returned_buf));
    memcpy(returned_buf, ciphertext, CIPHERTEXT_LEN);

    return returned_buf;
}


void crypto_encrypt_to_file(char *message, size_t msg_len) {
    char *encrypted = crypto_encrypt(message, msg_len);
    size_t CIPHERTEXT_LEN = crypto_secretbox_MACBYTES + msg_len;

    // make printable with hex chars
    char *hex_str = bytes_to_hex((unsigned char*)encrypted, CIPHERTEXT_LEN);

    // encode msg_len (unsigned long) into first four bytes 
    unsigned char *length = unsigned_long_to_bytes(msg_len);
    char *len_as_hex = bytes_to_hex(length, NUM_BYTES_IN_UNSIGNED_LONG);

    // prepend encrypted text with decrypted length 
    // represented as hex string
    size_t size = UNSIGNED_LONG_HEX_LEN + strlen(hex_str) + 2;
    if (size < CIPHERTEXT_LEN) {
        fprintf(stderr, "Integer overflow detected. The message we are encrypting may be too large. Exiting...\n");
        exit(1);
    }

    char string_to_write [size];
    strcpy(string_to_write, len_as_hex);
    strcat(string_to_write, hex_str);

    // write to file
    file_write(string_to_write, CRYPTO_FILEPATH, CRYPTO_SECRET_FILENAME);

    // gc
    free(encrypted);
    free(length);
    free(len_as_hex);
    free(hex_str);
}


char* crypto_decrypt(unsigned char *ciphertext, size_t ciphertext_len, size_t decrypted_len) {
    unsigned char decrypted [decrypted_len];
    unsigned char nonce [crypto_secretbox_NONCEBYTES];
    unsigned char key [crypto_secretbox_KEYBYTES];

    init_secret_key(key);
    int nonce_error = get_nonce(nonce);
    if (nonce_error) {
        return NULL;
    }

    if (crypto_secretbox_open_easy(
        decrypted, ciphertext, ciphertext_len, nonce, key) != 0) {
        /* message forged! */
        return NULL;
    }

    char *ret_val = calloc(decrypted_len + 1, sizeof(*ret_val));
    memcpy(ret_val, decrypted, decrypted_len);
    return ret_val;
}


char* crypto_decrypt_from_file() {
    char filepath [CRYPTO_FILEPATH_SIZE];
    get_secret_fileloc(filepath);

    FileData *filestruct = file_load(filepath);
    if (filestruct == NULL) {
        return NULL;
    }

    char *len_and_encrypted_hex = (char*)filestruct->data;

    // exclude the length at the beginning
    char *encrypted_hex_only = len_and_encrypted_hex + UNSIGNED_LONG_HEX_LEN;
    int size_of_encrypted_hex = filestruct->size - UNSIGNED_LONG_HEX_LEN;

    unsigned char *bytes = hex_to_bytes_with_len(encrypted_hex_only, size_of_encrypted_hex);
    size_t decrypted_len = hex_bytes_to_unsigned_long(len_and_encrypted_hex);
    char *decrypted = crypto_decrypt(bytes, size_of_encrypted_hex / 2, decrypted_len);

    // gc
    free(bytes);
    file_free(filestruct);

    return decrypted;
}

