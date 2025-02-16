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

/// @brief A representation of a linked hashmap key. This contains the type-erased pointer to the key and the key size in bytes.
typedef struct _LinkedHashMapKey
{
    void* key;
    size_t key_size;
} LinkedHashMapKey;

/// @brief A representation of a linked hashmap value. This contains the type-erased pointer to the value and the value size in bytes.
typedef struct _LinkedHashMapValue
{
    void* value;
    size_t value_size;
} LinkedHashMapValue;

/// @brief A representation of a linked hashmap entry. This contains the type-erased pointers to the key and value, as well as the size of each in bytes.
typedef struct _LinkedHashMapEntry
{
    void* key;
    size_t key_size;
    void* value;
    size_t value_size;
} LinkedHashMapEntry;

/// @brief A single node in a linked hashmap.
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

/// @brief A linked hashmap. This is like a normal hashmap, but keeps track of insertion order by having each node store a pointer to the previous and next nodes.
typedef struct _LinkedHashMap
{
    size_t length;
    size_t capacity;
    LinkedHashMapNode* nodes;
    LinkedHashMapNode* head;
    LinkedHashMapNode* tail;
} LinkedHashMap;

/// @brief Constructs a new linked hashmap with the default capacity. When done with the map, `linkedhashmap_free` will need to be called to free the memory.
/// @return The newly constructed linked hashmap.
LINKEDHASHMAP_EXPORT LinkedHashMap* linkedhashmap_new(void);

/// @brief Constructs a new linked hashmap with the given capacity. When done with the map, `linkedhashmap_free` will need to be called to free the memory.
/// @param capacity The starting capacity. `LINKEDHASHMAP_MIN_SIZE` will be used instead if `capacity` is less than it.
/// @return The newly constructed linked hashmap.
LINKEDHASHMAP_EXPORT LinkedHashMap* linkedhashmap_new_with_capacity(size_t capacity);

/// @brief Calculates the hash of a given key. This is only intended to be used internally.
/// @param map The linked hashmap. This is necessary because this function needs to mod by the map's capacity.
/// @param key A pointer to the key to hash.
/// @param key_size The size of the key in bytes.
/// @return A `size_t` representing the calculated hash.
LINKEDHASHMAP_TEST_EXPORT size_t linkedhashmap_hash(LinkedHashMap* map, void* key, size_t key_size);

/// @brief Checks if two regions of memory are equal. Used for comparing keys and values. This is only intended to be used internally.
/// @param p1 The pointer to the first region of memory.
/// @param size1 The size of the first region of memory in bytes.
/// @param p2 The pointer to the second region of memory.
/// @param size2 The size of the second region of memory in bytes.
/// @return `true` if the memory regions contain identical data.
LINKEDHASHMAP_TEST_EXPORT bool linkedhashmap_mem_equal(void* p1, size_t size1, void* p2, size_t size2);

/// @brief Locates the index within the allocated block where a given key resides. This is only intended to be used internally.
/// @param map The linked hashmap.
/// @param key The key to locate.
/// @param key_size The size of the key in bytes.
/// @return The index of the given key. Returns `~0` if the key does not exist.
LINKEDHASHMAP_TEST_EXPORT size_t linkedhashmap_find_key(LinkedHashMap* map, void* key, size_t key_size);

/// @brief Returns the number of items currently stored in the linked hashmap.
/// @param map The linked hashmap.
/// @return The number of items.
LINKEDHASHMAP_EXPORT size_t linkedhashmap_length(LinkedHashMap* map);

/// @brief Returns whether the linked hashmap is empty.
/// @param map The linked hashmap.
/// @return `true` if the linked hashmap has no items.
LINKEDHASHMAP_EXPORT bool linkedhashmap_is_empty(LinkedHashMap* map);

/// @brief Returns a collection of all keys in the map, in the order in which they were inserted. The returned pointer points to the first key. The number of keys is equal to the number returned by `linkedhashmap_length`. When done with the returned pointer, `free` must be called on it.
/// @param map The linked hashmap.
/// @return A pointer to the start of a collection of all keys in the map.
LINKEDHASHMAP_EXPORT LinkedHashMapKey* linkedhashmap_keys(LinkedHashMap* map);

/// @brief Returns a collection of all values in the map, in the order in which they were inserted. The returned pointer points to the first value. The number of values is equal to the number returned by `linkedhashmap_length`. When done with the returned pointer, `free` must be called on it.
/// @param map The linked hashmap.
/// @return A pointer to the start of a collection of all values in the map.
LINKEDHASHMAP_EXPORT LinkedHashMapValue* linkedhashmap_values(LinkedHashMap* map);

/// @brief Returns a collection of all entries (key-value pairs) in the map, in the order in which they were inserted. The returned pointer points to the first entry. The number of entries is equal to the number returned by `linkedhashmap_length`. When done with the returned pointer, `free` must be called on it.
/// @param map The linked hashmap.
/// @return A pointer to the start of a collection of all entries in the map.
LINKEDHASHMAP_EXPORT LinkedHashMapEntry* linkedhashmap_entries(LinkedHashMap* map);

/// @brief Reallocates the map to the specified capacity, copying the contents to the new allocation. This is only intended to be used internally.
/// @param map The linked hashmap.
/// @param new_size The new capacity.
LINKEDHASHMAP_TEST_EXPORT void linkedhashmap_resize(LinkedHashMap* map, size_t new_size);

/// @brief Reallocates the map, doubling its capacity. This is only intended to be used internally.
/// @param map The linked hashmap.
LINKEDHASHMAP_TEST_EXPORT void linkedhashmap_resize_up(LinkedHashMap* map);

/// @brief Reallocates the map, halving its capacity. This is only intended to be used internally.
/// @param map The linked hashmap.
LINKEDHASHMAP_TEST_EXPORT void linkedhashmap_resize_down(LinkedHashMap* map);

/// @brief Retrieves the entry at the given key. Returns `NULL` if the key does not exist. When done with the returned pointer, `free` must be called on it.
/// @param map The linked hashmap.
/// @param key The lookup key. Keys are compared using value comparison, not reference comparison. This means that lookup can still be performed even if done via a pointer to a copy of the key, and not the exact same pointer that was inserted into the map in the first place.
/// @param key_size The size of the given key.
/// @return A pointer to a representation of the requested entry.
LINKEDHASHMAP_EXPORT LinkedHashMapEntry* linkedhashmap_get(LinkedHashMap* map, void* key, size_t key_size);

/// @brief Retrieves the entry at the given insertion order index. Returns `NULL` if the index is invalid. Note that this is an `O(n)` lookup operation, unlike the `O(1)` operation that is retrieval by key. When done with the returned pointer, `free` must be called on it.
/// @param map The linked hashmap.
/// @param index The insertion order index.
/// @return A pointer to a representation of the requested entry.
LINKEDHASHMAP_EXPORT LinkedHashMapEntry* linkedhashmap_get_by_index(LinkedHashMap* map, size_t index);

/// @brief Gets the insertion order index of the given key. Returns `~0` if the key does not exist.
/// @param map The linked hashmap.
/// @param key The lookup key. Keys are compared using value comparison, not reference comparison. This means that lookup can still be performed even if done via a pointer to a copy of the key, and not the exact same pointer that was inserted into the map in the first place.
/// @param key_size The size of the given key.
/// @return The insertion order index.
LINKEDHASHMAP_EXPORT size_t linkedhashmap_get_index(LinkedHashMap* map, void* key, size_t key_size);

/// @brief Sets a key-value pair in the map. Returns the previous entry at the given key, or `NULL` if the key did not already exist. When done with the returned pointer, `free` must be called on it, unless the pointer is `NULL`.
/// @param map The linked hashmap.
/// @param key The lookup key. Keys are compared using value comparison, not reference comparison. This means that lookup can still be performed even if done via a pointer to a copy of the key, and not the exact same pointer that was inserted into the map in the first place.
/// @param key_size The size of the given key.
/// @param value The value.
/// @param value_size The size of the given value.
/// @return The previous entry at the given key, or `NULL` if the key did not already exist.
LINKEDHASHMAP_EXPORT LinkedHashMapEntry* linkedhashmap_set(LinkedHashMap* map, void* key, size_t key_size, void* value, size_t value_size);

/// @brief Extends `map1` with the contents of `map2`. Insertion order of `map2` carries over to `map1`.
/// @param map1 The map to extend.
/// @param map2 The map to extend from.
LINKEDHASHMAP_EXPORT void linkedhashmap_extend(LinkedHashMap* map1, LinkedHashMap* map2);

/// @brief Pops an entry from the map. Returns `NULL` if the key does not exist. When done with the returned pointer, `free` must be called on it, unless the pointer is `NULL`.
/// @param map The linked hashmap.
/// @param key The lookup key. Keys are compared using value comparison, not reference comparison. This means that lookup can still be performed even if done via a pointer to a copy of the key, and not the exact same pointer that was inserted into the map in the first place.
/// @param key_size The size of the given key.
/// @return The entry at the given key, or `NULL` if the key did not exist.
LINKEDHASHMAP_EXPORT LinkedHashMapEntry* linkedhashmap_pop(LinkedHashMap* map, void* key, size_t key_size);

/// @brief Deletes an entry from the map. This is equivalent to calling `linkedhashmap_pop` ignoring the returned entry.
/// @param map The linked hashmap.
/// @param key The lookup key. Keys are compared using value comparison, not reference comparison. This means that lookup can still be performed even if done via a pointer to a copy of the key, and not the exact same pointer that was inserted into the map in the first place.
/// @param key_size The size of the given key.
LINKEDHASHMAP_EXPORT void linkedhashmap_delete(LinkedHashMap* map, void* key, size_t key_size);

/// @brief Checks whether a map contains a given key.
/// @param map The linked hashmap.
/// @param key The lookup key. Keys are compared using value comparison, not reference comparison. This means that lookup can still be performed even if done via a pointer to a copy of the key, and not the exact same pointer that was inserted into the map in the first place.
/// @param key_size The size of the given key.
/// @return `true` if the key exists in the map.
LINKEDHASHMAP_EXPORT bool linkedhashmap_contains(LinkedHashMap* map, void* key, size_t key_size);

/// @brief Checks if two maps contain the same set of key-value pairs. This does not take insertion order into account. For checking equality including insertion order, use `linkedhashmap_equal_with_insertion_order`.
/// @param map1 The first map.
/// @param map2 The second map.
/// @return `true` if both maps contain the same set of key-value pairs.
LINKEDHASHMAP_EXPORT bool linkedhashmap_equal(LinkedHashMap* map1, LinkedHashMap* map2);

/// @brief Checks if two maps contain the same set of key-value pairs, and have the same insertion order. For checking equality without taking insertion order into account, use `linkedhashmap_equal`.
/// @param map1 The first map.
/// @param map2 The second map.
/// @return `true` if both maps contain the same set of key-value pairs in the same order.
LINKEDHASHMAP_EXPORT bool linkedhashmap_equal_with_insertion_order(LinkedHashMap* map1, LinkedHashMap* map2);

/// @brief Empties the map and reallocates down to the default capacity.
/// @param map The linked hashmap.
LINKEDHASHMAP_EXPORT void linkedhashmap_clear(LinkedHashMap* map);

/// @brief Copies the contents of a map. This is a shallow copy. The keys and values themselves are not duplicated in this operation. Their pointers are duplicated instead. When done with the copy of the map, `linkedhashmap_free` will need to be called to free the memory.
/// @param map The linked hashmap.
/// @return The new copy of the map.
LINKEDHASHMAP_EXPORT LinkedHashMap* linkedhashmap_copy(LinkedHashMap* map);

/// @brief Applies a function to each key-value pair in the map, in the order in which they were inserted.
/// @param map The linked hashmap.
/// @param fn The function to run on each key-value pair. The function should take the following arguments: A pointer to the key, the size of the key, a pointer to the value, the size of the value, and the additional `void*` argument.
/// @param arg An additional `void*` argument to pass to the function.
LINKEDHASHMAP_EXPORT void linkedhashmap_foreach(LinkedHashMap* map, void (*fn)(void*, size_t, void*, size_t, void*), void* arg);

/// @brief Frees the memory used by the map. This does not free the keys and values in the map, as it is assumed that they may still be referenced elsewhere in the application.
/// @param map The linked hashmap.
LINKEDHASHMAP_EXPORT void linkedhashmap_free(LinkedHashMap* map);

#endif // __LINKEDHASHMAP_H__
