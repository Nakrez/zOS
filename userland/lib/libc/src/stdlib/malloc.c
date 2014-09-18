#include <stdlib.h>

#include <sys/mman.h>
#include <sys/spinlock.h>

#define PAGE_SIZE 4096
#define MALLOC_WASTE_THRESHOLD 4

#define ALIGN_UP(X, SIZE) ((X + SIZE - 1) & ~(SIZE - 1));

struct malloc_chunk {
    size_t size;

    int free;

    struct malloc_chunk *next;
    struct malloc_chunk *prev;
};

static struct malloc_chunk *chunks = NULL;
static spinlock_t malloc_lock = SPINLOCK_INIT;

static struct malloc_chunk *request_memory(size_t size)
{
    struct malloc_chunk *chunk;

    size = ALIGN_UP(size, PAGE_SIZE);

    if (!(chunk = mmap(0, size, PROT_WRITE, 0, 0, 0)))
        return NULL;

    chunk->size = size - sizeof (struct malloc_chunk);
    chunk->free = 1;
    chunk->next = NULL;
    chunk->prev = NULL;

    return chunk;
}

static void malloc_split_block(struct malloc_chunk *chunk, size_t size)
{
    if (chunk->size >= size + sizeof (struct malloc_chunk) +
        MALLOC_WASTE_THRESHOLD)
    {
        struct malloc_chunk *new_chunk = (void *)((char *)(chunk + 1) + size);

        new_chunk->size = chunk->size - size - sizeof (struct malloc_chunk);
        new_chunk->free = 1;

        new_chunk->next = chunk->next;
        new_chunk->prev = chunk;

        if (new_chunk->next)
            new_chunk->next->prev = new_chunk;

        chunk->next = new_chunk;
        chunk->size = size;
    }

    chunk->free = 0;
}

void *malloc(size_t size)
{
    struct malloc_chunk *tmp;

    if (!size)
        return NULL;

    spinlock_lock(&malloc_lock);

    if (!chunks && !(chunks = request_memory(size)))
    {
        spinlock_unlock(&malloc_lock);

        return NULL;
    }

    tmp = chunks;

    while (tmp)
    {
        if (tmp->free && tmp->size >= size)
        {
            malloc_split_block(tmp, size);

            spinlock_unlock(&malloc_lock);

            return tmp + 1;
        }

        tmp = tmp->next;
    }

    spinlock_unlock(&malloc_lock);

    /* We need a new allocation */
    if (!(tmp = request_memory(size)))
        return NULL;

    spinlock_lock(&malloc_lock);

    tmp->next = chunks;
    chunks->prev = tmp;

    chunks = tmp;

    malloc_split_block(chunks, size);

    spinlock_unlock(&malloc_lock);

    return chunks + 1;
}

void free(void *ptr)
{
    struct malloc_chunk *tmp;

    if (!ptr)
        return;

    tmp = (void *)((char *)ptr - sizeof (struct malloc_chunk));

    spinlock_lock(&malloc_lock);

    tmp->free = 1;

    spinlock_unlock(&malloc_lock);
}
