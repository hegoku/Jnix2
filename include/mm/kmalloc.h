#ifndef MM_KMALLOC_H
#define MM_KMALLOC_H

void *kzmalloc(unsigned int size);
void kfree(void *obj, unsigned int size);
void init_bucket_desc();

#endif