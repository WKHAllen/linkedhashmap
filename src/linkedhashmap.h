#ifndef __LINKEDHASHMAP_H__
#define __LINKEDHASHMAP_H__

#include <stdlib.h>
#include <stdbool.h>

#ifdef _WIN32
#  define LINKEDHASHMAP_EXPORT __declspec(dllexport)
#else
#  define LINKEDHASHMAP_EXPORT __attribute__((visibility("default")))
#endif

#ifdef LINKEDHASHMAP_TEST
#  define LINKEDHASHMAP_TEST_EXPORT LINKEDHASHMAP_EXPORT
#else
#  define LINKEDHASHMAP_TEST_EXPORT
#endif

#define LINKEDHASHMAP_MIN_SIZE 16

typedef struct _LinkedHashMapNode
{
    void* key;
    void* value;
    bool is_allocated;
    size_t key_size;
    struct _LinkedHashMapNode* prev;
    struct _LinkedHashMapNode* next;
} LinkedHashMapNode;

typedef struct _LinkedHashMap
{
    size_t length;
    size_t capacity;
    LinkedHashMapNode* nodes;
    LinkedHashMapNode* head;
    LinkedHashMapNode* tail;
} LinkedHashMap;

LINKEDHASHMAP_EXPORT LinkedHashMap* linkedhashmap_new(void);

LINKEDHASHMAP_TEST_EXPORT size_t linkedhashmap_hash(LinkedHashMap* map, void* key, size_t key_size);

LINKEDHASHMAP_TEST_EXPORT bool linkedhashmap_keys_equal(void* key1, size_t key_size1, void* key2, size_t key_size2);

LINKEDHASHMAP_EXPORT size_t linkedhashmap_length(LinkedHashMap* map);

LINKEDHASHMAP_EXPORT bool linkedhashmap_is_empty(LinkedHashMap* map);

LINKEDHASHMAP_EXPORT void** linkedhashmap_keys(LinkedHashMap* map);

LINKEDHASHMAP_EXPORT void** linkedhashmap_values(LinkedHashMap* map);

LINKEDHASHMAP_EXPORT void** linkedhashmap_entries(LinkedHashMap* map);

LINKEDHASHMAP_TEST_EXPORT void linkedhashmap_resize(LinkedHashMap* map, size_t new_size);

LINKEDHASHMAP_TEST_EXPORT void linkedhashmap_resize_up(LinkedHashMap* map);

LINKEDHASHMAP_TEST_EXPORT void linkedhashmap_resize_down(LinkedHashMap* map);

LINKEDHASHMAP_EXPORT void* linkedhashmap_get(LinkedHashMap* map, void* key, size_t key_size);

LINKEDHASHMAP_EXPORT void* linkedhashmap_get_by_index(LinkedHashMap* map, size_t index);

LINKEDHASHMAP_EXPORT size_t linkedhashmap_get_index(LinkedHashMap* map, void* key, size_t key_size);

LINKEDHASHMAP_EXPORT void* linkedhashmap_set(LinkedHashMap* map, void* key, size_t key_size, void* value);

LINKEDHASHMAP_EXPORT void linkedhashmap_extend(LinkedHashMap* map1, LinkedHashMap* map2);

LINKEDHASHMAP_EXPORT void* linkedhashmap_pop(LinkedHashMap* map, void* key, size_t key_size);

LINKEDHASHMAP_EXPORT void linkedhashmap_delete(LinkedHashMap* map, void* key, size_t key_size);

LINKEDHASHMAP_EXPORT bool linkedhashmap_contains(LinkedHashMap* map, void* key, size_t key_size);

LINKEDHASHMAP_EXPORT bool linkedhashmap_equal(LinkedHashMap* map1, LinkedHashMap* map2);

LINKEDHASHMAP_EXPORT bool linkedhashmap_equal_with_insertion_order(LinkedHashMap* map1, LinkedHashMap* map2);

LINKEDHASHMAP_EXPORT void linkedhashmap_clear(LinkedHashMap* map);

LINKEDHASHMAP_EXPORT LinkedHashMap* linkedhashmap_copy(LinkedHashMap* map);

LINKEDHASHMAP_EXPORT void linkedhashmap_foreach(LinkedHashMap* map, void (*fptr)(void*, size_t, void*, void*), void* arg);

LINKEDHASHMAP_EXPORT void linkedhashmap_free(LinkedHashMap* map);

#endif // __LINKEDHASHMAP_H__
