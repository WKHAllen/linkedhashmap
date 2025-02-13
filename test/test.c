#include "../src/linkedhashmap.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#define STR_SIZE(s) ((strlen(s) + 1) * sizeof(char))

#ifdef _WIN32
#  ifdef _WIN64
#    define PRI_SIZE_T PRIu64
#  else
#    define PRI_SIZE_T PRIu32
#  endif
#else
#  define PRI_SIZE_T "zu"
#endif

#define TEST_ASSERT(condition) { \
    if (!(condition)) { \
        fprintf( \
            stderr, \
            "test assertion failed (%s:%d): %s\n", \
            __FILE__, __LINE__, #condition); \
        exit(1); \
    } \
}

#define TEST_ASSERT_EQ(a, b) { \
    if (a != b) { \
        fprintf( \
            stderr, \
            "test equality assertion failed (%s:%d): %s != %s, %" PRI_SIZE_T " != %" PRI_SIZE_T "\n", \
            __FILE__, __LINE__, #a, #b, a, b); \
        exit(1); \
    } \
}

#define TEST_ASSERT_NE(a, b) { \
    if (a == b) { \
        fprintf( \
            stderr, \
            "test inequality assertion failed (%s:%d): %s == %s, %" PRI_SIZE_T " == %" PRI_SIZE_T "\n", \
            __FILE__, __LINE__, #a, #b, a, b); \
        exit(1); \
    } \
}

#define TEST_ASSERT_INT_EQ(a, b) { \
    if (a != b) { \
        fprintf( \
            stderr, \
            "test equality assertion failed (%s:%d): %s != %s, %d != %d\n", \
            __FILE__, __LINE__, #a, #b, a, b); \
        exit(1); \
    } \
}

#define TEST_ASSERT_INT_NE(a, b) { \
    if (a == b) { \
        fprintf( \
            stderr, \
            "test inequality assertion failed (%s:%d): %s == %s, %d == %d\n", \
            __FILE__, __LINE__, #a, #b, a, b); \
        exit(1); \
    } \
}

#define TEST_ASSERT_STR_EQ(a, b) { \
    if (strcmp(a, b) != 0) { \
        fprintf( \
            stderr, \
            "test string equality assertion failed (%s:%d): %s != %s, %s != %s\n", \
            __FILE__, __LINE__, #a, #b, a, b); \
        exit(1); \
    } \
}

#define TEST_ASSERT_STR_NE(a, b) { \
    if (strcmp(a, b) == 0) { \
        fprintf( \
            stderr, \
            "test string inequality assertion failed (%s:%d): %s == %s, %s == %s\n", \
            __FILE__, __LINE__, #a, #b, a, b); \
        exit(1); \
    } \
}

#define TEST_ASSERT_ARRAY_EQ(a, b, size) { \
    for (size_t i = 0; i < size; i++) { \
        if (a[i] != b[i]) { \
            fprintf( \
                stderr, \
                "test array equality assertion failed (%s:%d): %s != %s at index %" PRI_SIZE_T ", with size = %" PRI_SIZE_T "\n", \
                __FILE__, __LINE__, #a, #b, i, size); \
            exit(1); \
        } \
    } \
}

#define TEST_ASSERT_MEM_EQ(a, b, size) { \
    if (memcmp(a, b, size) != 0) { \
        fprintf( \
            stderr, \
            "test memory equality assertion failed (%s:%d): %s != %s, with size = %" PRI_SIZE_T "\n", \
            __FILE__, __LINE__, #a, #b, size); \
        exit(1); \
    } \
}

#define TEST_ASSERT_MEM_NE(a, b, size) { \
    if (memcmp(a, b, size) == 0) { \
        fprintf( \
            stderr, \
            "test memory inequality assertion failed (%s:%d): %s == %s, with size = %" PRI_SIZE_T "\n", \
            __FILE__, __LINE__, #a, #b, size); \
        exit(1); \
    } \
}

#define TEST_ASSERT_ARRAY_MEM_EQ(a, b, size, item_size) { \
    for (size_t i = 0; i < size; i++) { \
        if (memcmp((void *) (a[i]), (void *) (b[i]), item_size) != 0) { \
            fprintf( \
                stderr, \
                "test array memory equality assertion failed (%s:%d): %s != %s at index %" PRI_SIZE_T ", with size = %" PRI_SIZE_T " and item size = %" PRI_SIZE_T "\n", \
                __FILE__, __LINE__, #a, #b, i, size, item_size); \
            exit(1); \
        } \
    } \
}

void print_linkedhashmap_int(void* key, size_t key_size, void* value, void* arg)
{
    (void)key_size;
    (void)arg;

    printf("%d: %d, ", *(int*)key, *(int*)value);
}

void print_linkedhashmap(LinkedHashMap* map)
{
    printf("{ ");
    linkedhashmap_foreach(map, print_linkedhashmap_int, NULL);

    if (map->length == 0)
        printf("}\n");
    else
        printf("\b\b }\n");
}

void sum_ints(void* key, size_t key_size, void* value, void* total)
{
    (void)key;
    (void)key_size;

    *(int*)total += *(int*)value;
}

#define INIT_SQUARES() \
    LinkedHashMap* map = linkedhashmap_new(); \
    int indices[17]; \
    int squares[17]; \
    for (int i = 0; i < 17; i++) \
    { \
        indices[i] = i; \
        squares[i] = i * i; \
        if (i < 16) \
            TEST_ASSERT(linkedhashmap_set(map, &(indices[i]), sizeof(indices[i]), &(squares[i]), sizeof(squares[i])) == NULL); \
    } \

// test initialization and deallocation
// { }
void test_init(void)
{
    LinkedHashMap* map = linkedhashmap_new();

    TEST_ASSERT_EQ(linkedhashmap_length(map), (size_t)0);
    TEST_ASSERT(linkedhashmap_is_empty(map));
    TEST_ASSERT_EQ(map->length, (size_t)0);
    TEST_ASSERT_EQ(map->capacity, (size_t)LINKEDHASHMAP_MIN_SIZE);

    linkedhashmap_free(map);
}

// test initialization and deallocation
// { }
void test_init_with_capacity(void)
{
    LinkedHashMap* map = linkedhashmap_new_with_capacity(100);

    TEST_ASSERT_EQ(linkedhashmap_length(map), (size_t)0);
    TEST_ASSERT(linkedhashmap_is_empty(map));
    TEST_ASSERT_EQ(map->length, (size_t)0);
    TEST_ASSERT_EQ(map->capacity, (size_t)100);

    linkedhashmap_free(map);
}

// test hash function
void test_hash(void)
{
    LinkedHashMap* map = linkedhashmap_new();

    int int_key = 123;
    size_t int_hash = linkedhashmap_hash(map, &int_key, sizeof(int));
    TEST_ASSERT_EQ(int_hash, (size_t)(123 % map->capacity));

    char* str_key = "abcde";
    size_t str_hash = linkedhashmap_hash(map, str_key, STR_SIZE(str_key));
    TEST_ASSERT_EQ(str_hash, (size_t)(495 % map->capacity));

    linkedhashmap_free(map);
}

// test set
// { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225 }
void test_set(void)
{
    INIT_SQUARES();

    TEST_ASSERT_EQ(linkedhashmap_length(map), (size_t)16);
    TEST_ASSERT(!linkedhashmap_is_empty(map));
    TEST_ASSERT_EQ(map->length, (size_t)16);
    TEST_ASSERT_EQ(map->capacity, (size_t)16);

    int mapvalue = 21;
    LinkedHashMapValue* res1 = linkedhashmap_set(map, &(indices[3]), sizeof(indices[3]), &mapvalue, sizeof(mapvalue));
    LinkedHashMapValue* res2 = linkedhashmap_set(map, &(indices[3]), sizeof(indices[3]), &(squares[3]), sizeof(squares[3]));

    TEST_ASSERT_INT_EQ(*(int*)(res1->value), squares[3]);
    TEST_ASSERT_INT_EQ(*(int*)(res2->value), mapvalue);

    free(res1);
    free(res2);
    linkedhashmap_free(map);
}

// test get
// { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225 }
void test_get(void)
{
    INIT_SQUARES();

    for (int i = 0; i < 16; i++)
    {
        LinkedHashMapValue* res = linkedhashmap_get(map, &(indices[i]), sizeof(indices[i]));
        TEST_ASSERT_INT_EQ(*(int*)(res->value), squares[i]);
        free(res);
    }

    linkedhashmap_free(map);
}

// test get by index
// { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225 }
void test_get_by_index(void)
{
    LinkedHashMap* map = linkedhashmap_new();

    int keys[] = { 2, 7, 5, 1, 3 };
    int values[] = { 1, 2, 3, 4, 5 };
    int array_len = sizeof(keys) / sizeof(keys[0]);

    for (int i = 0; i < array_len; i++)
        TEST_ASSERT(linkedhashmap_set(map, &(keys[i]), sizeof(keys[i]), &(values[i]), sizeof(values[i])) == NULL);

    for (int i = 0; i < array_len; i++)
    {
        LinkedHashMapValue* res = linkedhashmap_get_by_index(map, i);
        TEST_ASSERT_INT_EQ(*(int*)(res->value), values[i]);
        free(res);
    }

    linkedhashmap_free(map);
}

// test get index
// { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225 }
void test_get_index(void)
{
    LinkedHashMap* map = linkedhashmap_new();

    int keys[] = { 2, 7, 5, 1, 3 };
    int values[] = { 1, 2, 3, 4, 5 };
    int array_len = sizeof(keys) / sizeof(keys[0]);

    for (int i = 0; i < array_len; i++)
        TEST_ASSERT(linkedhashmap_set(map, &(keys[i]), sizeof(keys[i]), &(values[i]), sizeof(values[i])) == NULL);

    for (int i = 0; i < array_len; i++)
        TEST_ASSERT_EQ(linkedhashmap_get_index(map, &(keys[i]), sizeof(keys[i])), (size_t)i);

    linkedhashmap_free(map);
}

// test resize_up
// { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225, 16: 256 }
void test_resize_up(void)
{
    INIT_SQUARES();

    TEST_ASSERT(linkedhashmap_set(map, &(indices[16]), sizeof(indices[16]), &(squares[16]), sizeof(squares[16])) == NULL);

    TEST_ASSERT_EQ(linkedhashmap_length(map), (size_t)17);
    TEST_ASSERT_EQ(map->length, (size_t)17);
    TEST_ASSERT_EQ(map->capacity, (size_t)32);

    for (int i = 0; i < 17; i++)
    {
        LinkedHashMapValue* res = linkedhashmap_get(map, &(indices[i]), sizeof(indices[i]));
        TEST_ASSERT_INT_EQ(*(int*)(res->value), squares[i]);
        free(res);
    }

    linkedhashmap_free(map);
}

// test pop and resize_down
// { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225 }
void test_pop_resize_down(void)
{
    INIT_SQUARES();

    TEST_ASSERT(linkedhashmap_set(map, &(indices[16]), sizeof(indices[16]), &(squares[16]), sizeof(squares[16])) == NULL);

    LinkedHashMapValue* res1 = linkedhashmap_pop(map, &(indices[16]), sizeof(indices[16]));

    TEST_ASSERT_INT_EQ(*(int*)(res1->value), squares[16]);
    TEST_ASSERT_EQ(linkedhashmap_length(map), (size_t)16);
    TEST_ASSERT_EQ(map->length, (size_t)16);
    TEST_ASSERT_EQ(map->capacity, (size_t)16);

    free(res1);

    for (int i = 0; i < 16; i++)
    {
        LinkedHashMapValue* res2 = linkedhashmap_get(map, &(indices[i]), sizeof(indices[i]));
        TEST_ASSERT_INT_EQ(*(int*)(res2->value), squares[i]);
        free(res2);
    }

    linkedhashmap_free(map);
}

// test copy, equal, and equal with insertion order
// { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225 }
void test_copy_equal(void)
{
    INIT_SQUARES();

    LinkedHashMap* map2 = linkedhashmap_copy(map);

    for (int i = 0; i < 16; i++)
    {
        LinkedHashMapValue* res = linkedhashmap_get(map2, &(indices[i]), sizeof(indices[i]));
        TEST_ASSERT_INT_EQ(*(int*)(res->value), squares[i]);
        free(res);
    }

    TEST_ASSERT(linkedhashmap_equal(map, map2));
    TEST_ASSERT(linkedhashmap_equal_with_insertion_order(map, map2));

    LinkedHashMapValue* res = linkedhashmap_set(map2, &(indices[3]), sizeof(indices[3]), &(squares[3]), sizeof(squares[3]));
    free(res);

    TEST_ASSERT(linkedhashmap_equal_with_insertion_order(map, map2));

    LinkedHashMap* map3 = linkedhashmap_new();

    for (int i = 1; i < 16; i++)
        TEST_ASSERT(linkedhashmap_set(map3, &(indices[i]), sizeof(indices[i]), &(squares[i]), sizeof(squares[i])) == NULL);

    TEST_ASSERT(linkedhashmap_set(map3, &(indices[0]), sizeof(indices[0]), &(squares[0]), sizeof(squares[0])) == NULL);

    TEST_ASSERT(linkedhashmap_equal(map, map3));
    TEST_ASSERT(!linkedhashmap_equal_with_insertion_order(map, map3));

    linkedhashmap_free(map);
    linkedhashmap_free(map2);
    linkedhashmap_free(map3);
}

// test extend
// { 0: 0, 1: 1, 2: 4, 3: 21, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225, 16: 37 }
void test_extend(void)
{
    INIT_SQUARES();

    LinkedHashMap* map2 = linkedhashmap_copy(map);
    int map3value1 = 21;
    int map3value2 = 37;
    LinkedHashMap* map3 = linkedhashmap_new();
    LinkedHashMapValue* res1 = linkedhashmap_set(map3, &(indices[3]), sizeof(indices[3]), &(map3value1), sizeof(map3value1));
    LinkedHashMapValue* res2 = linkedhashmap_set(map3, &(indices[16]), sizeof(indices[16]), &(map3value2), sizeof(map3value2));
    linkedhashmap_extend(map2, map3);
    LinkedHashMapValue* res3 = linkedhashmap_get(map2, &(indices[3]), sizeof(indices[3]));
    LinkedHashMapValue* res4 = linkedhashmap_get(map2, &(indices[16]), sizeof(indices[16]));

    TEST_ASSERT(!linkedhashmap_equal(map, map2));
    TEST_ASSERT(!linkedhashmap_equal(map, map3));
    TEST_ASSERT(!linkedhashmap_equal(map2, map3));
    TEST_ASSERT_INT_EQ(*(int*)(res3->value), map3value1);
    TEST_ASSERT_INT_EQ(*(int*)(res4->value), map3value2);
    TEST_ASSERT_EQ(linkedhashmap_length(map2), (size_t)17);

    free(res1);
    free(res2);
    free(res3);
    free(res4);
    linkedhashmap_free(map);
    linkedhashmap_free(map2);
    linkedhashmap_free(map3);
}

// test keys, values, and entries
// { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225 }
void test_keys_values_entries(void)
{
    INIT_SQUARES();

    int key_total = 0;
    int value_total = 0;
    int entries_total = 0;

    for (int i = 0; i < 16; i++)
    {
        indices[i] = i;
        squares[i] = i * i;
    }

    LinkedHashMapKey* map_keys = linkedhashmap_keys(map);
    LinkedHashMapValue* map_values = linkedhashmap_values(map);
    LinkedHashMapEntry* map_entries = linkedhashmap_entries(map);

    for (size_t i = 0; i < linkedhashmap_length(map); i++)
    {
        key_total += *(int*)(map_keys[i].key);
        value_total += *(int*)(map_values[i].value);
        entries_total += *(int*)(map_entries[i].key);
        entries_total += *(int*)(map_entries[i].value);
    }

    TEST_ASSERT_INT_EQ(key_total, 120);
    TEST_ASSERT_INT_EQ(value_total, 1240);
    TEST_ASSERT_INT_EQ(entries_total, key_total + value_total);

    free(map_keys);
    free(map_values);
    free(map_entries);

    linkedhashmap_free(map);
}

// test delete
// { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225, 16: 256 }
void test_delete(void)
{
    INIT_SQUARES();

    TEST_ASSERT(linkedhashmap_set(map, &(indices[16]), sizeof(indices[16]), &(squares[16]), sizeof(squares[16])) == NULL);

    TEST_ASSERT_EQ(linkedhashmap_length(map), (size_t)17);
    TEST_ASSERT_EQ(map->length, (size_t)17);
    TEST_ASSERT_EQ(map->capacity, (size_t)32);

    linkedhashmap_delete(map, &(indices[16]), sizeof(indices[16]));

    TEST_ASSERT_EQ(linkedhashmap_length(map), (size_t)16);
    TEST_ASSERT_EQ(map->length, (size_t)16);
    TEST_ASSERT_EQ(map->capacity, (size_t)16);

    for (int i = 0; i < 16; i++)
    {
        LinkedHashMapValue* res = linkedhashmap_get(map, &(indices[i]), sizeof(indices[i]));
        TEST_ASSERT_INT_EQ(*(int*)(res->value), squares[i]);
        free(res);
    }

    linkedhashmap_free(map);
}

// test contains
// { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225 }
void test_contains(void)
{
    INIT_SQUARES();

    for (int i = 0; i < 16; i++)
        TEST_ASSERT(linkedhashmap_contains(map, &(indices[i]), sizeof(indices[i])));

    TEST_ASSERT(!linkedhashmap_contains(map, &(indices[16]), sizeof(indices[16])));

    linkedhashmap_free(map);
}

// test clear
// { }
void test_clear(void)
{
    INIT_SQUARES();

    linkedhashmap_clear(map);

    TEST_ASSERT_EQ(linkedhashmap_length(map), (size_t)0);
    TEST_ASSERT(linkedhashmap_is_empty(map));
    TEST_ASSERT_EQ(map->length, (size_t)0);
    TEST_ASSERT_EQ(map->capacity, (size_t)LINKEDHASHMAP_MIN_SIZE);

    linkedhashmap_free(map);
}

// test foreach
void test_foreach(void)
{
    INIT_SQUARES();

    linkedhashmap_clear(map);

    int intvals[] = { 1, 3, 7, 9 };
    int arraylen = sizeof(intvals) / sizeof(intvals[0]);
    int expected = 0;

    for (int i = 0; i < arraylen; i++)
    {
        TEST_ASSERT(linkedhashmap_set(map, &(indices[i]), sizeof(indices[i]), &(intvals[i]), sizeof(intvals[i])) == NULL);
        expected += intvals[i];
    }

    int result = 0;
    linkedhashmap_foreach(map, sum_ints, &result);

    TEST_ASSERT_INT_EQ(result, expected);

    linkedhashmap_free(map);
}

// test order is preserved in keys
void test_order_keys(void)
{
    int values[] = { 1, 2, 3, 4, 5 };

    // int map
    LinkedHashMap* int_map = linkedhashmap_new();

    int int_keys[] = { 2, 7, 5, 1, 3 };
    int int_array_len = sizeof(int_keys) / sizeof(int_keys[0]);

    for (int i = 0; i < int_array_len; i++)
        TEST_ASSERT(linkedhashmap_set(int_map, &(int_keys[i]), sizeof(int_keys[i]), &(values[i]), sizeof(values[i])) == NULL);

    LinkedHashMapKey* int_map_keys1 = linkedhashmap_keys(int_map);

    for (size_t i = 0; i < linkedhashmap_length(int_map); i++)
        TEST_ASSERT_INT_EQ(*(int*)(int_map_keys1[i].key), int_keys[i]);

    free(int_map_keys1);

    free(linkedhashmap_set(int_map, &(int_keys[2]), sizeof(int_keys[2]), &(values[1]), sizeof(values[1])));
    LinkedHashMapKey* int_map_keys2 = linkedhashmap_keys(int_map);

    for (size_t i = 0; i < linkedhashmap_length(int_map); i++)
        TEST_ASSERT_INT_EQ(*(int*)(int_map_keys2[i].key), int_keys[i]);

    free(int_map_keys2);
    linkedhashmap_free(int_map);

    // string map
    LinkedHashMap* str_map = linkedhashmap_new();

    char* str_keys[] = { "hello", "world", "foo", "bar", "baz" };
    int str_array_len = sizeof(str_keys) / sizeof(str_keys[0]);

    for (int i = 0; i < str_array_len; i++)
        TEST_ASSERT(linkedhashmap_set(str_map, &(str_keys[i]), sizeof(str_keys[i]), &(values[i]), sizeof(values[i])) == NULL);

    LinkedHashMapKey* str_map_keys1 = linkedhashmap_keys(str_map);

    for (size_t i = 0; i < linkedhashmap_length(str_map); i++)
        TEST_ASSERT_STR_EQ(*(char**)(str_map_keys1[i].key), str_keys[i]);

    free(str_map_keys1);

    free(linkedhashmap_set(str_map, &(str_keys[2]), sizeof(str_keys[2]), &(values[1]), sizeof(values[1])));
    LinkedHashMapKey* str_map_keys2 = linkedhashmap_keys(str_map);

    for (size_t i = 0; i < linkedhashmap_length(str_map); i++)
        TEST_ASSERT_STR_EQ(*(char**)(str_map_keys2[i].key), str_keys[i]);

    free(str_map_keys2);
    linkedhashmap_free(str_map);
}

// test order is preserved in values
void test_order_values(void)
{
    int values[] = { 1, 2, 3, 4, 5 };

    // int map
    LinkedHashMap* int_map = linkedhashmap_new();

    int int_keys[] = { 2, 7, 5, 1, 3 };
    int int_array_len = sizeof(int_keys) / sizeof(int_keys[0]);

    for (int i = 0; i < int_array_len; i++)
        TEST_ASSERT(linkedhashmap_set(int_map, &(int_keys[i]), sizeof(int_keys[i]), &(values[i]), sizeof(values[i])) == NULL);

    LinkedHashMapValue* int_map_values1 = linkedhashmap_values(int_map);

    for (size_t i = 0; i < linkedhashmap_length(int_map); i++)
        TEST_ASSERT_INT_EQ(*(int*)(int_map_values1[i].value), values[i]);

    free(int_map_values1);

    free(linkedhashmap_set(int_map, &(int_keys[2]), sizeof(int_keys[2]), &(values[1]), sizeof(values[1])));
    LinkedHashMapValue* int_map_values2 = linkedhashmap_values(int_map);

    for (size_t i = 0; i < linkedhashmap_length(int_map); i++)
        if (i != 2)
            TEST_ASSERT_INT_EQ(*(int*)(int_map_values2[i].value), values[i])
        else
            TEST_ASSERT_INT_EQ(*(int*)(int_map_values2[2].value), values[1]);

    free(int_map_values2);
    linkedhashmap_free(int_map);

    // string map
    LinkedHashMap* str_map = linkedhashmap_new();

    char* str_keys[] = { "hello", "world", "foo", "bar", "baz" };
    int str_array_len = sizeof(str_keys) / sizeof(str_keys[0]);

    for (int i = 0; i < str_array_len; i++)
        TEST_ASSERT(linkedhashmap_set(str_map, &(str_keys[i]), sizeof(str_keys[i]), &(values[i]), sizeof(values[i])) == NULL);

    LinkedHashMapValue* str_map_values1 = linkedhashmap_values(str_map);

    for (size_t i = 0; i < linkedhashmap_length(str_map); i++)
        TEST_ASSERT_INT_EQ(*(int*)(str_map_values1[i].value), values[i]);

    free(str_map_values1);

    free(linkedhashmap_set(str_map, &(str_keys[2]), sizeof(str_keys[2]), &(values[1]), sizeof(values[1])));
    LinkedHashMapValue* str_map_values2 = linkedhashmap_values(str_map);

    for (size_t i = 0; i < linkedhashmap_length(str_map); i++)
        if (i != 2)
            TEST_ASSERT_INT_EQ(*(int*)(str_map_values2[i].value), values[i])
        else
            TEST_ASSERT_INT_EQ(*(int*)(str_map_values2[2].value), values[1]);

    free(str_map_values2);
    linkedhashmap_free(str_map);
}

// test order is preserved in entries
void test_order_entries(void)
{
    int values[] = { 1, 2, 3, 4, 5 };

    // int map
    LinkedHashMap* int_map = linkedhashmap_new();

    int int_keys[] = { 2, 7, 5, 1, 3 };
    int int_array_len = sizeof(int_keys) / sizeof(int_keys[0]);

    for (int i = 0; i < int_array_len; i++)
        TEST_ASSERT(linkedhashmap_set(int_map, &(int_keys[i]), sizeof(int_keys[i]), &(values[i]), sizeof(values[i])) == NULL);

    LinkedHashMapEntry* int_map_entries1 = linkedhashmap_entries(int_map);

    for (size_t i = 0; i < linkedhashmap_length(int_map); i++)
    {
        TEST_ASSERT_INT_EQ(*(int*)(int_map_entries1[i].key), int_keys[i]);
        TEST_ASSERT_INT_EQ(*(int*)(int_map_entries1[i].value), values[i]);
    }

    free(int_map_entries1);

    free(linkedhashmap_set(int_map, &(int_keys[2]), sizeof(int_keys[2]), &(values[1]), sizeof(values[1])));
    LinkedHashMapEntry* int_map_entries2 = linkedhashmap_entries(int_map);

    for (size_t i = 0; i < linkedhashmap_length(int_map); i++)
    {
        TEST_ASSERT_INT_EQ(*(int*)(int_map_entries2[i].key), int_keys[i]);

        if (i != 2)
            TEST_ASSERT_INT_EQ(*(int*)(int_map_entries2[i].value), values[i])
        else
            TEST_ASSERT_INT_EQ(*(int*)(int_map_entries2[2].value), values[1]);
    }

    free(int_map_entries2);
    linkedhashmap_free(int_map);

    // string map
    LinkedHashMap* str_map = linkedhashmap_new();

    char* str_keys[] = { "hello", "world", "foo", "bar", "baz" };
    int str_array_len = sizeof(str_keys) / sizeof(str_keys[0]);

    for (int i = 0; i < str_array_len; i++)
        TEST_ASSERT(linkedhashmap_set(str_map, &(str_keys[i]), sizeof(str_keys[i]), &(values[i]), sizeof(values[i])) == NULL);

    LinkedHashMapEntry* str_map_entries1 = linkedhashmap_entries(str_map);

    for (size_t i = 0; i < linkedhashmap_length(str_map); i++)
    {
        TEST_ASSERT_STR_EQ(*(char**)(str_map_entries1[i].key), str_keys[i]);
        TEST_ASSERT_INT_EQ(*(int*)(str_map_entries1[i].value), values[i]);
    }

    free(str_map_entries1);

    free(linkedhashmap_set(str_map, &(str_keys[2]), sizeof(str_keys[2]), &(values[1]), sizeof(values[1])));
    LinkedHashMapEntry* str_map_entries2 = linkedhashmap_entries(str_map);

    for (size_t i = 0; i < linkedhashmap_length(str_map); i++)
    {
        TEST_ASSERT_STR_EQ(*(char**)(str_map_entries2[i].key), str_keys[i]);

        if (i != 2)
            TEST_ASSERT_INT_EQ(*(int*)(str_map_entries2[i].value), values[i])
        else
            TEST_ASSERT_INT_EQ(*(int*)(str_map_entries2[2].value), values[1]);
    }

    free(str_map_entries2);
    linkedhashmap_free(str_map);
}

typedef struct _ForeachOrderState
{
    size_t index;
    void* keys;
    int* values;
    bool modified_value;
} ForeachOrderState;

void verify_foreach_order_int(void* key, size_t key_size, void* value, void* arg)
{
    (void)key_size;

    ForeachOrderState* state = (ForeachOrderState*)arg;

    if (!state->modified_value)
    {
        TEST_ASSERT_INT_EQ(*(int*)key, ((int*)(state->keys))[state->index]);
        TEST_ASSERT_INT_EQ(*(int*)value, state->values[state->index]);
    }
    else
    {
        TEST_ASSERT_INT_EQ(*(int*)key, ((int*)(state->keys))[state->index]);

        if (state->index != 2)
            TEST_ASSERT_INT_EQ(*(int*)value, state->values[state->index])
        else
            TEST_ASSERT_INT_EQ(*(int*)value, state->values[1]);
    }

    state->index += 1;
}

void verify_foreach_order_str(void* key, size_t key_size, void* value, void* arg)
{
    (void)key_size;

    ForeachOrderState* state = (ForeachOrderState*)arg;

    if (!state->modified_value)
    {
        TEST_ASSERT_STR_EQ(*(char**)key, ((char**)(state->keys))[state->index]);
        TEST_ASSERT_INT_EQ(*(int*)value, state->values[state->index]);
    }
    else
    {
        TEST_ASSERT_STR_EQ(*(char**)key, ((char**)(state->keys))[state->index]);

        if (state->index != 2)
            TEST_ASSERT_INT_EQ(*(int*)value, state->values[state->index])
        else
            TEST_ASSERT_INT_EQ(*(int*)value, state->values[1]);
    }

    state->index += 1;
}

// test order is preserved in foreach
void test_order_foreach(void)
{
    int values[] = { 1, 2, 3, 4, 5 };

    // int map
    LinkedHashMap* int_map = linkedhashmap_new();

    int int_keys[] = { 2, 7, 5, 1, 3 };
    int int_array_len = sizeof(int_keys) / sizeof(int_keys[0]);

    for (int i = 0; i < int_array_len; i++)
        TEST_ASSERT(linkedhashmap_set(int_map, &(int_keys[i]), sizeof(int_keys[i]), &(values[i]), sizeof(values[i])) == NULL);

    ForeachOrderState foreach_order_state_int;
    foreach_order_state_int.index = 0;
    foreach_order_state_int.keys = int_keys;
    foreach_order_state_int.values = values;
    foreach_order_state_int.modified_value = false;

    linkedhashmap_foreach(int_map, verify_foreach_order_int, &foreach_order_state_int);

    free(linkedhashmap_set(int_map, &(int_keys[2]), sizeof(int_keys[2]), &(values[1]), sizeof(values[1])));
    foreach_order_state_int.index = 0;
    foreach_order_state_int.modified_value = true;
    linkedhashmap_foreach(int_map, verify_foreach_order_int, &foreach_order_state_int);

    linkedhashmap_free(int_map);

    // string map
    LinkedHashMap* str_map = linkedhashmap_new();

    char* str_keys[] = { "hello", "world", "foo", "bar", "baz" };
    int str_array_len = sizeof(str_keys) / sizeof(str_keys[0]);

    for (int i = 0; i < str_array_len; i++)
        TEST_ASSERT(linkedhashmap_set(str_map, &(str_keys[i]), sizeof(str_keys[i]), &(values[i]), sizeof(values[i])) == NULL);

    ForeachOrderState foreach_order_state_str;
    foreach_order_state_str.index = 0;
    foreach_order_state_str.keys = str_keys;
    foreach_order_state_str.values = values;
    foreach_order_state_str.modified_value = false;

    linkedhashmap_foreach(str_map, verify_foreach_order_str, &foreach_order_state_str);

    free(linkedhashmap_set(str_map, &(str_keys[2]), sizeof(str_keys[2]), &(values[1]), sizeof(values[1])));
    foreach_order_state_str.index = 0;
    foreach_order_state_str.modified_value = true;
    linkedhashmap_foreach(str_map, verify_foreach_order_str, &foreach_order_state_str);

    linkedhashmap_free(str_map);
}

int main(void)
{
    // Begin
    printf("Beginning tests\n");

    // Run tests
    printf("\nTesting initialization...\n");
    test_init();
    printf("\nTesting initialization with capacity...\n");
    test_init_with_capacity();
    printf("\nTesting hash function...\n");
    test_hash();
    printf("\nTesting set...\n");
    test_set();
    printf("\nTesting get...\n");
    test_get();
    printf("\nTesting get by index...\n");
    test_get_by_index();
    printf("\nTesting get index...\n");
    test_get_index();
    printf("\nTesting resize up...\n");
    test_resize_up();
    printf("\nTesting pop and resize down...\n");
    test_pop_resize_down();
    printf("\nTesting copy, equal, and equal with insertion order...\n");
    test_copy_equal();
    printf("\nTesting extend...\n");
    test_extend();
    printf("\nTesting keys, values, and entries...\n");
    test_keys_values_entries();
    printf("\nTesting delete...\n");
    test_delete();
    printf("\nTesting contains...\n");
    test_contains();
    printf("\nTesting clear...\n");
    test_clear();
    printf("\nTesting foreach...\n");
    test_foreach();
    printf("\nTesting that order is preserved in keys...\n");
    test_order_keys();
    printf("\nTesting that order is preserved in values...\n");
    test_order_values();
    printf("\nTesting that order is preserved in entries...\n");
    test_order_entries();
    printf("\nTesting that order is preserved in foreach...\n");
    test_order_foreach();

    // Done
    printf("\nCompleted tests\n");
}
