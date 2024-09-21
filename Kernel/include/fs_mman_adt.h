#ifndef MMAN_ADT_H
#define MMAN_ADT_H

typedef struct fs_mman_cdt* fs_mman_adt;

fs_mman_adt new_fs_mmap();
char* fs_malloc(fs_mman_adt fs_mman);
char* fs_free(fs_mman_adt fs_mman, void* pointer);

#endif
