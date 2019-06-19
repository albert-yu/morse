#ifndef _FILE_H_
#define _FILE_H_

#ifdef __cplusplus
extern "C" {
#endif

struct file_data {
    int size;
    void *data;
};

typedef struct file_data FileData;

extern struct file_data* file_load(char *filename);
extern void file_free(struct file_data *filedata);

int file_write(char *data, char *destination, char *filename);

#ifdef __cplusplus
}
#endif

#endif

