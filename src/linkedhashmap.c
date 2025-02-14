#include "linkedhashmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

LinkedHashMap* linkedhashmap_new(void)
{
    return linkedhashmap_new_with_capacity(LINKEDHASHMAP_MIN_SIZE);
}

LinkedHashMap* linkedhashmap_new_with_capacity(size_t capacity)
{
    if (capacity < LINKEDHASHMAP_MIN_SIZE)
        capacity = LINKEDHASHMAP_MIN_SIZE;

    LinkedHashMap* map = (LinkedHashMap*)malloc(sizeof(LinkedHashMap));
    map->length = 0;
    map->capacity = capacity;
    map->nodes = (LinkedHashMapNode*)malloc(capacity * sizeof(LinkedHashMapNode));
    map->head = NULL;
    map->tail = NULL;

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

bool linkedhashmap_mem_equal(void* p1, size_t size1, void* p2, size_t size2)
{
    if (size1 != size2)
        return false;

    return memcmp(p1, p2, size1) == 0;
}

size_t linkedhashmap_length(LinkedHashMap* map)
{
    return map->length;
}

bool linkedhashmap_is_empty(LinkedHashMap* map)
{
    return map->length == 0;
}

LinkedHashMapKey* linkedhashmap_keys(LinkedHashMap* map)
{
    LinkedHashMapKey* map_keys = (LinkedHashMapKey*)malloc(map->length * sizeof(LinkedHashMapKey));
    size_t found = 0;
    LinkedHashMapNode* current = map->head;

    while (current != NULL)
    {
        map_keys[found].key = current->key;
        map_keys[found++].key_size = current->key_size;
        current = current->next;
    }

    return map_keys;
}

LinkedHashMapValue* linkedhashmap_values(LinkedHashMap* map)
{
    LinkedHashMapValue* map_values = (LinkedHashMapValue*)malloc(map->length * sizeof(LinkedHashMapValue));
    size_t found = 0;
    LinkedHashMapNode* current = map->head;

    while (current != NULL)
    {
        map_values[found].value = current->value;
        map_values[found++].value_size = current->value_size;
        current = current->next;
    }

    return map_values;
}

LinkedHashMapEntry* linkedhashmap_entries(LinkedHashMap* map)
{
    LinkedHashMapEntry* map_entries = (LinkedHashMapEntry*)malloc(map->length * sizeof(LinkedHashMapEntry));
    size_t found = 0;
    LinkedHashMapNode* current = map->head;

    while (current != NULL)
    {
        map_entries[found].key = current->key;
        map_entries[found].key_size = current->key_size;
        map_entries[found].value = current->value;
        map_entries[found++].value_size = current->value_size;
        current = current->next;
    }

    return map_entries;
}

void linkedhashmap_resize(LinkedHashMap* map, size_t new_size)
{
    LinkedHashMapEntry* map_entries = (LinkedHashMapEntry*)malloc(map->length * sizeof(LinkedHashMapEntry));
    size_t found = 0;
    LinkedHashMapNode* current = map->head;

    while (current != NULL)
    {
        map_entries[found].key = current->key;
        map_entries[found].key_size = current->key_size;
        map_entries[found].value = current->value;
        map_entries[found++].value_size = current->value_size;
        current = current->next;
    }

    map->length = 0;
    map->capacity = new_size;
    map->nodes = (LinkedHashMapNode*)realloc(map->nodes, new_size * sizeof(LinkedHashMapNode));
    map->head = NULL;
    map->tail = NULL;

    for (size_t i = 0; i < map->capacity; i++)
        map->nodes[i].is_allocated = false;

    for (size_t i = 0; i < found; i++)
        linkedhashmap_set(map, map_entries[i].key, map_entries[i].key_size, map_entries[i].value, map_entries[i].value_size);

    free(map_entries);
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

LinkedHashMapEntry* linkedhashmap_get(LinkedHashMap* map, void* key, size_t key_size)
{
    size_t hashvalue = linkedhashmap_hash(map, key, key_size);
    size_t current;

    for (size_t i = 0; i < map->capacity; i++)
    {
        current = (hashvalue + i) % map->capacity;

        if (linkedhashmap_mem_equal(key, key_size, map->nodes[current].key, map->nodes[current].key_size)
            && map->nodes[current].is_allocated)
        {
            LinkedHashMapEntry* res = (LinkedHashMapEntry*)malloc(sizeof(LinkedHashMapEntry));
            res->key = map->nodes[current].key;
            res->key_size = map->nodes[current].key_size;
            res->value = map->nodes[current].value;
            res->value_size = map->nodes[current].value_size;
            return res;
        }
    }

    return NULL;
}

LinkedHashMapEntry* linkedhashmap_get_by_index(LinkedHashMap* map, size_t index)
{
    LinkedHashMapNode* current = map->head;

    for (size_t i = 0; i < index && current != NULL; i++)
        current = current->next;

    if (current == NULL)
        return NULL;

    LinkedHashMapEntry* res = (LinkedHashMapEntry*)malloc(sizeof(LinkedHashMapEntry));
    res->key = current->key;
    res->key_size = current->key_size;
    res->value = current->value;
    res->value_size = current->value_size;
    return res;
}

size_t linkedhashmap_get_index(LinkedHashMap* map, void* key, size_t key_size)
{
    LinkedHashMapNode* current = map->head;

    for (size_t i = 0; current != NULL; i++)
    {
        if (linkedhashmap_mem_equal(current->key, current->key_size, key, key_size))
            return i;

        current = current->next;
    }

    return ~0;
}

LinkedHashMapEntry* linkedhashmap_set(LinkedHashMap* map, void* key, size_t key_size, void* value, size_t value_size)
{
    size_t hashvalue = linkedhashmap_hash(map, key, key_size);
    size_t current;

    for (size_t i = 0; i < map->capacity; i++)
    {
        current = (hashvalue + i) % map->capacity;

        if (!map->nodes[current].is_allocated)
        {
            map->nodes[current].key = key;
            map->nodes[current].key_size = key_size;
            map->nodes[current].value = value;
            map->nodes[current].value_size = value_size;
            map->nodes[current].is_allocated = true;
            map->nodes[current].prev = map->tail;
            map->nodes[current].next = NULL;

            if (map->length == 0)
            {
                map->head = &(map->nodes[current]);
            }
            else
            {
                map->tail->next = &(map->nodes[current]);
            }

            map->tail = &(map->nodes[current]);
            map->length++;
            return NULL;
        }
        else if (linkedhashmap_mem_equal(key, key_size, map->nodes[current].key, map->nodes[current].key_size))
        {
            LinkedHashMapEntry* res = (LinkedHashMapEntry*)malloc(sizeof(LinkedHashMapEntry));
            res->key = map->nodes[current].key;
            res->key_size = map->nodes[current].key_size;
            res->value = map->nodes[current].value;
            res->value_size = map->nodes[current].value_size;

            map->nodes[current].value = value;

            return res;
        }
    }

    linkedhashmap_resize_up(map);
    return linkedhashmap_set(map, key, key_size, value, value_size);
}

void linkedhashmap_extend(LinkedHashMap* map1, LinkedHashMap* map2)
{
    LinkedHashMapNode* current = map2->head;

    while (current != NULL)
    {
        LinkedHashMapEntry* res = linkedhashmap_set(map1, current->key, current->key_size, current->value, current->value_size);

        if (res != NULL)
            free(res);

        current = current->next;
    }
}

LinkedHashMapEntry* linkedhashmap_pop(LinkedHashMap* map, void* key, size_t key_size)
{
    size_t hashvalue = linkedhashmap_hash(map, key, key_size);
    size_t current;

    for (size_t i = 0; i < map->capacity; i++)
    {
        current = (hashvalue + i) % map->capacity;

        if (linkedhashmap_mem_equal(key, key_size, map->nodes[current].key, map->nodes[current].key_size)
            && map->nodes[current].is_allocated)
        {
            LinkedHashMapEntry* res = (LinkedHashMapEntry*)malloc(sizeof(LinkedHashMapEntry));
            res->key = map->nodes[current].key;
            res->key_size = map->nodes[current].key_size;
            res->value = map->nodes[current].value;
            res->value_size = map->nodes[current].value_size;

            map->nodes[current].is_allocated = false;
            map->length--;

            if (map->nodes[current].prev != NULL)
                map->nodes[current].prev->next = map->nodes[current].next;
            else
                map->head = map->nodes[current].next;

            if (map->nodes[current].next != NULL)
                map->nodes[current].next->prev = map->nodes[current].prev;
            else
                map->tail = map->nodes[current].prev;

            if (map->length <= (map->capacity >> 1) && map->capacity > LINKEDHASHMAP_MIN_SIZE)
                linkedhashmap_resize_down(map);

            return res;
        }
    }

    return NULL;
}

void linkedhashmap_delete(LinkedHashMap* map, void* key, size_t key_size)
{
    LinkedHashMapEntry* res = linkedhashmap_pop(map, key, key_size);

    if (res != NULL)
        free(res);
}

bool linkedhashmap_contains(LinkedHashMap* map, void* key, size_t key_size)
{
    size_t hashvalue = linkedhashmap_hash(map, key, key_size);
    size_t current;

    for (size_t i = 0; i < map->capacity; i++)
    {
        current = (hashvalue + i) % map->capacity;

        if (linkedhashmap_mem_equal(key, key_size, map->nodes[current].key, map->nodes[current].key_size)
            && map->nodes[current].is_allocated)
            return true;
    }

    return false;
}

bool linkedhashmap_equal(LinkedHashMap* map1, LinkedHashMap* map2)
{
    if (map1->length != map2->length)
        return false;

    for (size_t i = 0; i < map1->capacity; i++)
    {
        if (map1->nodes[i].is_allocated)
        {
            LinkedHashMapEntry* res = linkedhashmap_get(map2, map1->nodes[i].key, map1->nodes[i].key_size);

            if (res != NULL && !linkedhashmap_mem_equal(map1->nodes[i].value, map1->nodes[i].value_size, res->value, res->value_size))
            {
                free(res);
                return false;
            }

            if (res != NULL)
                free(res);
        }
    }

    return true;
}

bool linkedhashmap_equal_with_insertion_order(LinkedHashMap* map1, LinkedHashMap* map2)
{
    if (map1->length != map2->length)
        return false;

    LinkedHashMapNode* current1 = map1->head;
    LinkedHashMapNode* current2 = map2->head;

    while (current1 != NULL)
    {
        if (!linkedhashmap_mem_equal(current1->key, current1->key_size, current2->key, current2->key_size)
            || !linkedhashmap_mem_equal(current1->value, current1->value_size, current2->value, current2->value_size))
            return false;

        current1 = current1->next;
        current2 = current2->next;
    }

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
    LinkedHashMapNode* current = map->head;

    while (current != NULL)
    {
        linkedhashmap_set(new_map, current->key, current->key_size, current->value, current->value_size);
        current = current->next;
    }

    return new_map;
}

void linkedhashmap_foreach(LinkedHashMap* map, void (*fn)(void*, size_t, void*, size_t, void*), void* arg)
{
    LinkedHashMapNode* current = map->head;

    while (current != NULL)
    {
        (*fn)(current->key, current->key_size, current->value, current->value_size, arg);
        current = current->next;
    }
}

void linkedhashmap_free(LinkedHashMap* map)
{
    free(map->nodes);
    free(map);
}
