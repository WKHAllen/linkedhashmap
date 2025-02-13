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

typedef struct _LinkedHashMapKey
{
    void* key;
    size_t key_size;
} LinkedHashMapKey;

typedef struct _LinkedHashMapValue
{
    void* value;
    size_t value_size;
} LinkedHashMapValue;

typedef struct _LinkedHashMapEntry
{
    void* key;
    size_t key_size;
    void* value;
    size_t value_size;
} LinkedHashMapEntry;

typedef struct _LinkedHashMapNode
{
    void* key;
    size_t key_size;
    void* value;
    size_t value_size;
    bool is_allocated;
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

LINKEDHASHMAP_TEST_EXPORT bool linkedhashmap_mem_equal(void* p1, size_t size1, void* p2, size_t size2);

LINKEDHASHMAP_EXPORT size_t linkedhashmap_length(LinkedHashMap* map);

LINKEDHASHMAP_EXPORT bool linkedhashmap_is_empty(LinkedHashMap* map);

LINKEDHASHMAP_EXPORT LinkedHashMapKey* linkedhashmap_keys(LinkedHashMap* map);

LINKEDHASHMAP_EXPORT LinkedHashMapValue* linkedhashmap_values(LinkedHashMap* map);

LINKEDHASHMAP_EXPORT LinkedHashMapEntry* linkedhashmap_entries(LinkedHashMap* map);

LINKEDHASHMAP_TEST_EXPORT void linkedhashmap_resize(LinkedHashMap* map, size_t new_size);

LINKEDHASHMAP_TEST_EXPORT void linkedhashmap_resize_up(LinkedHashMap* map);

LINKEDHASHMAP_TEST_EXPORT void linkedhashmap_resize_down(LinkedHashMap* map);

LINKEDHASHMAP_EXPORT LinkedHashMapValue* linkedhashmap_get(LinkedHashMap* map, void* key, size_t key_size);

LINKEDHASHMAP_EXPORT LinkedHashMapValue* linkedhashmap_get_by_index(LinkedHashMap* map, size_t index);

LINKEDHASHMAP_EXPORT size_t linkedhashmap_get_index(LinkedHashMap* map, void* key, size_t key_size);

LINKEDHASHMAP_EXPORT LinkedHashMapValue* linkedhashmap_set(LinkedHashMap* map, void* key, size_t key_size, void* value, size_t value_size);

LINKEDHASHMAP_EXPORT void linkedhashmap_extend(LinkedHashMap* map1, LinkedHashMap* map2);

LINKEDHASHMAP_EXPORT LinkedHashMapValue* linkedhashmap_pop(LinkedHashMap* map, void* key, size_t key_size);

LINKEDHASHMAP_EXPORT void linkedhashmap_delete(LinkedHashMap* map, void* key, size_t key_size);

LINKEDHASHMAP_EXPORT bool linkedhashmap_contains(LinkedHashMap* map, void* key, size_t key_size);

LINKEDHASHMAP_EXPORT bool linkedhashmap_equal(LinkedHashMap* map1, LinkedHashMap* map2);

LINKEDHASHMAP_EXPORT bool linkedhashmap_equal_with_insertion_order(LinkedHashMap* map1, LinkedHashMap* map2);

LINKEDHASHMAP_EXPORT void linkedhashmap_clear(LinkedHashMap* map);

LINKEDHASHMAP_EXPORT LinkedHashMap* linkedhashmap_copy(LinkedHashMap* map);

LINKEDHASHMAP_EXPORT void linkedhashmap_foreach(LinkedHashMap* map, void (*fptr)(void*, size_t, void*, void*), void* arg);

LINKEDHASHMAP_EXPORT void linkedhashmap_free(LinkedHashMap* map);

#endif // __LINKEDHASHMAP_H__
