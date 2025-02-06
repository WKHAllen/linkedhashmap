#include "linkedhashmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

LinkedHashMap* linkedhashmap_new(void)
{
    LinkedHashMap* map = (LinkedHashMap*)malloc(sizeof(LinkedHashMap));
    map->length = 0;
    map->capacity = LINKEDHASHMAP_MIN_SIZE;
    map->nodes = (LinkedHashMapNode*)malloc(LINKEDHASHMAP_MIN_SIZE * sizeof(LinkedHashMapNode));

    for (size_t i = 0; i < map->capacity; i++)
        map->nodes[i].is_allocated = false;

    return map;
}

size_t linkedhashmap_hash(LinkedHashMap* map, void* key, size_t key_size)
{
    size_t hashvalue = 0;

    for (size_t i = 0; i < key_size; i++)
        hashvalue += (size_t)(*((char*)key + i));

    return hashvalue % map->capacity;
}

bool linkedhashmap_keys_equal(void* key1, size_t key_size1, void* key2, size_t key_size2)
{
    if (key_size1 != key_size2)
        return false;

    return memcmp(key1, key2, key_size1) == 0;

    // for (size_t i = 0; i < key_size1; i++)
    //     if (*((char*)key1 + i) != *((char*)key2 + i))
    //         return false;

    // return true;
}

size_t linkedhashmap_length(LinkedHashMap* map)
{
    return map->length;
}

bool linkedhashmap_is_empty(LinkedHashMap* map)
{
    return map->length == 0;
}

void** linkedhashmap_keys(LinkedHashMap* map)
{
    void** map_keys = (void**)malloc(map->length * sizeof(void*));
    size_t found = 0;

    for (size_t i = 0; i < map->capacity; i++)
        if (map->nodes[i].is_allocated)
            map_keys[found++] = map->nodes[i].key;

    return map_keys;
}

void** linkedhashmap_values(LinkedHashMap* map)
{
    void** map_values = malloc(map->length * sizeof(void*));
    size_t found = 0;

    for (size_t i = 0; i < map->capacity; i++)
        if (map->nodes[i].is_allocated)
            map_values[found++] = map->nodes[i].value;

    return map_values;
}

void** linkedhashmap_entries(LinkedHashMap* map)
{
    void** map_entries = (void**)malloc(map->length * 2 * sizeof(void*));
    size_t found = 0;

    for (size_t i = 0; i < map->capacity; i++)
    {
        if (map->nodes[i].is_allocated)
        {
            map_entries[found++] = map->nodes[i].key;
            map_entries[found++] = map->nodes[i].value;
        }
    }

    return map_entries;
}

void linkedhashmap_resize(LinkedHashMap* map, size_t new_size)
{
    void** map_keys = malloc(map->length * sizeof(void*));
    void** map_values = malloc(map->length * sizeof(void*));
    size_t* map_key_sizes = malloc(map->length * sizeof(size_t));
    size_t found = 0;

    for (size_t i = 0; i < map->capacity; i++)
    {
        if (map->nodes[i].is_allocated)
        {
            map_keys[found] = map->nodes[i].key;
            map_values[found] = map->nodes[i].value;
            map_key_sizes[found] = map->nodes[i].key_size;
            found++;
        }
    }

    map->nodes = (LinkedHashMapNode*)realloc(map->nodes, new_size * sizeof(LinkedHashMapNode));
    map->capacity = new_size;
    map->length = 0;

    for (size_t i = 0; i < map->capacity; i++)
        map->nodes[i].is_allocated = false;

    for (size_t i = 0; i < found; i++)
        linkedhashmap_set(map, map_keys[i], map_key_sizes[i], map_values[i]);

    free(map_keys);
    free(map_values);
    free(map_key_sizes);
}

void linkedhashmap_resize_up(LinkedHashMap* map)
{
    size_t new_size = map->capacity << 1;
    linkedhashmap_resize(map, new_size);
}

void linkedhashmap_resize_down(LinkedHashMap* map)
{
    size_t new_size = map->capacity >> 1;

    if (new_size < LINKEDHASHMAP_MIN_SIZE)
        new_size = LINKEDHASHMAP_MIN_SIZE;

    linkedhashmap_resize(map, new_size);
}

void* linkedhashmap_get(LinkedHashMap* map, void* key, size_t key_size)
{
    size_t hashvalue = linkedhashmap_hash(map, key, key_size);
    size_t current;

    for (size_t i = 0; i < map->capacity; i++)
    {
        current = (hashvalue + i) % map->capacity;

        if (linkedhashmap_keys_equal(key, key_size, map->nodes[current].key, map->nodes[current].key_size)
            && map->nodes[current].is_allocated)
            return map->nodes[current].value;
    }

    return NULL;
}

void linkedhashmap_set(LinkedHashMap* map, void* key, size_t key_size, void* value)
{
    size_t hashvalue = linkedhashmap_hash(map, key, key_size);
    size_t current;

    for (size_t i = 0; i < map->capacity; i++)
    {
        current = (hashvalue + i) % map->capacity;

        if (!map->nodes[current].is_allocated)
        {
            map->nodes[current].key = key;
            map->nodes[current].value = value;
            map->nodes[current].is_allocated = true;
            map->nodes[current].key_size = key_size;
            map->length++;
            return;
        }
        else if (linkedhashmap_keys_equal(key, key_size, map->nodes[current].key, map->nodes[current].key_size))
        {
            map->nodes[current].value = value;
            return;
        }
    }

    linkedhashmap_resize_up(map);
    linkedhashmap_set(map, key, key_size, value);
}

void linkedhashmap_extend(LinkedHashMap* map1, LinkedHashMap* map2)
{
    for (size_t i = 0; i < map2->capacity; i++)
        if (map2->nodes[i].is_allocated)
            linkedhashmap_set(map1, map2->nodes[i].key, map2->nodes[i].key_size, map2->nodes[i].value);
}

void* linkedhashmap_pop(LinkedHashMap* map, void* key, size_t key_size)
{
    size_t hashvalue = linkedhashmap_hash(map, key, key_size);
    size_t current;

    for (size_t i = 0; i < map->capacity; i++)
    {
        current = (hashvalue + i) % map->capacity;

        if (linkedhashmap_keys_equal(key, key_size, map->nodes[current].key, map->nodes[current].key_size)
            && map->nodes[current].is_allocated)
        {
            void* value = map->nodes[current].value;

            map->nodes[current].is_allocated = false;
            map->length--;

            if (map->length <= (map->capacity >> 1) && map->capacity > LINKEDHASHMAP_MIN_SIZE)
                linkedhashmap_resize_down(map);

            return value;
        }
    }

    return NULL;
}

void linkedhashmap_delete(LinkedHashMap* map, void* key, size_t key_size)
{
    linkedhashmap_pop(map, key, key_size);
}

bool linkedhashmap_contains(LinkedHashMap* map, void* key, size_t key_size)
{
    size_t hashvalue = linkedhashmap_hash(map, key, key_size);
    size_t current;

    for (size_t i = 0; i < map->capacity; i++)
    {
        current = (hashvalue + i) % map->capacity;

        if (linkedhashmap_keys_equal(key, key_size, map->nodes[current].key, map->nodes[current].key_size)
            && map->nodes[current].is_allocated)
            return true;
    }

    return false;
}

bool linkedhashmap_equal(LinkedHashMap* map1, LinkedHashMap* map2)
{
    if (map1->length != map2->length)
        return 0;

    for (size_t i = 0; i < map1->capacity; i++)
        if (map1->nodes[i].is_allocated
            && (!linkedhashmap_contains(map2, map1->nodes[i].key, map1->nodes[i].key_size)
                || map1->nodes[i].value != linkedhashmap_get(map2, map1->nodes[i].key, map1->nodes[i].key_size)))
            return false;

    return true;
}

void linkedhashmap_clear(LinkedHashMap* map)
{
    map->nodes = (LinkedHashMapNode*)realloc(map->nodes, LINKEDHASHMAP_MIN_SIZE * sizeof(LinkedHashMapNode));
    map->length = 0;
    map->capacity = LINKEDHASHMAP_MIN_SIZE;

    for (size_t i = 0; i < map->capacity; i++)
        map->nodes[i].is_allocated = false;
}

LinkedHashMap* linkedhashmap_copy(LinkedHashMap* map)
{
    LinkedHashMap* new_map = linkedhashmap_new();

    for (size_t i = 0; i < map->capacity; i++)
        if (map->nodes[i].is_allocated)
            linkedhashmap_set(new_map, map->nodes[i].key, map->nodes[i].key_size, map->nodes[i].value);

    return new_map;
}

void linkedhashmap_foreach(LinkedHashMap* map, void (*fptr)(void*, size_t, void*, void*), void* arg)
{
    for (size_t i = 0; i < map->capacity; i++)
        if (map->nodes[i].is_allocated)
            (*fptr)(map->nodes[i].key, map->nodes[i].key_size, map->nodes[i].value, arg);
}

void linkedhashmap_free(LinkedHashMap* map)
{
    free(map->nodes);
    free(map);
}
