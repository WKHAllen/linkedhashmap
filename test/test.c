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

int main(void)
{
    // Begin
    printf("Beginning tests\n");

    // test initialization
    // { }
    LinkedHashMap* map = linkedhashmap_new();
    TEST_ASSERT_EQ(linkedhashmap_length(map), (size_t)0);
    TEST_ASSERT(linkedhashmap_is_empty(map));
    TEST_ASSERT_EQ(map->length, (size_t)0);
    TEST_ASSERT_EQ(map->capacity, (size_t)LINKEDHASHMAP_MIN_SIZE);

    // test hash function
    int int_key = 123;
    size_t int_hash = linkedhashmap_hash(map, &int_key, sizeof(int));
    TEST_ASSERT_EQ(int_hash, (size_t)(123 % map->capacity));
    char* str_key = "abcde";
    size_t str_hash = linkedhashmap_hash(map, str_key, STR_SIZE(str_key));
    TEST_ASSERT_EQ(str_hash, (size_t)(495 % map->capacity));

    // test set
    // { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225 }
    int indices[17];
    int squares[17];
    for (int i = 0; i < 17; i++)
    {
        indices[i] = i;
        squares[i] = i * i;
        if (i < 16)
            linkedhashmap_set(map, &(indices[i]), sizeof(indices[i]), &(squares[i]));
    }
    TEST_ASSERT_EQ(linkedhashmap_length(map), (size_t)16);
    TEST_ASSERT(!linkedhashmap_is_empty(map));
    TEST_ASSERT_EQ(map->length, (size_t)16);
    TEST_ASSERT_EQ(map->capacity, (size_t)16);

    // test get
    // { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225 }
    for (int i = 0; i < 16; i++)
        TEST_ASSERT_INT_EQ(*(int*)linkedhashmap_get(map, &(indices[i]), sizeof(indices[i])), squares[i]);

    // test resize_up
    // { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225, 16: 256 }
    linkedhashmap_set(map, &(indices[16]), sizeof(indices[16]), &(squares[16]));
    TEST_ASSERT_EQ(linkedhashmap_length(map), (size_t)17);
    TEST_ASSERT_EQ(map->length, (size_t)17);
    TEST_ASSERT_EQ(map->capacity, (size_t)32);
    for (int i = 0; i < 17; i++)
        TEST_ASSERT_INT_EQ(*(int*)linkedhashmap_get(map, &(indices[i]), sizeof(indices[i])), squares[i]);

    // test pop and resize_down
    // { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225 }
    TEST_ASSERT_INT_EQ(*(int*)linkedhashmap_pop(map, &(indices[16]), sizeof(indices[16])), squares[16]);
    TEST_ASSERT_EQ(linkedhashmap_length(map), (size_t)16);
    TEST_ASSERT_EQ(map->length, (size_t)16);
    TEST_ASSERT_EQ(map->capacity, (size_t)16);
    for (int i = 0; i < 16; i++)
        TEST_ASSERT_INT_EQ(*(int*)linkedhashmap_get(map, &(indices[i]), sizeof(indices[i])), squares[i]);

    // test copy and equal
    // { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225 }
    LinkedHashMap* map2 = linkedhashmap_copy(map);
    for (int i = 0; i < 16; i++)
        TEST_ASSERT_INT_EQ(*(int*)linkedhashmap_get(map2, &(indices[i]), sizeof(indices[i])), squares[i]);
    TEST_ASSERT(linkedhashmap_equal(map, map2));

    // test extend
    // { 0: 0, 1: 1, 2: 4, 3: 21, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225, 16: 37 }
    int map3value1 = 21;
    int map3value2 = 37;
    LinkedHashMap* map3 = linkedhashmap_new();
    linkedhashmap_set(map3, &(indices[3]), sizeof(map3value1), &(map3value1));
    linkedhashmap_set(map3, &(indices[16]), sizeof(map3value2), &(map3value2));
    linkedhashmap_extend(map2, map3);
    TEST_ASSERT(!linkedhashmap_equal(map, map2));
    TEST_ASSERT(!linkedhashmap_equal(map, map3));
    TEST_ASSERT(!linkedhashmap_equal(map2, map3));
    TEST_ASSERT_INT_EQ(*(int*)linkedhashmap_get(map2, &(indices[3]), sizeof(indices[3])), map3value1);
    TEST_ASSERT_INT_EQ(*(int*)linkedhashmap_get(map2, &(indices[16]), sizeof(indices[16])), map3value2);
    TEST_ASSERT_EQ(linkedhashmap_length(map2), (size_t)17);

    // test keys, values, and entries
    // { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225 }
    int key_total = 0;
    int value_total = 0;
    int entries_total = 0;
    for (int i = 0; i < 16; i++)
    {
        indices[i] = i;
        squares[i] = i * i;
    }
    void** map_keys = linkedhashmap_keys(map);
    void** map_values = linkedhashmap_values(map);
    void** map_entries = linkedhashmap_entries(map);
    for (size_t i = 0; i < linkedhashmap_length(map); i++)
    {
        key_total += *(int*)(map_keys[i]);
        value_total += *(int*)(map_values[i]);
        entries_total += *(int*)(map_entries[2 * i]);
        entries_total += *(int*)(map_entries[2 * i + 1]);
    }
    TEST_ASSERT_INT_EQ(key_total, 120);
    TEST_ASSERT_INT_EQ(value_total, 1240);
    TEST_ASSERT_INT_EQ(entries_total, key_total + value_total);
    free(map_keys);
    free(map_values);
    free(map_entries);

    // test delete
    // { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225, 16: 256 }
    linkedhashmap_set(map, &(indices[16]), sizeof(indices[16]), &(squares[16]));
    TEST_ASSERT_EQ(linkedhashmap_length(map), (size_t)17);
    TEST_ASSERT_EQ(map->length, (size_t)17);
    TEST_ASSERT_EQ(map->capacity, (size_t)32);
    linkedhashmap_delete(map, &(indices[16]), sizeof(indices[16]));
    TEST_ASSERT_EQ(linkedhashmap_length(map), (size_t)16);
    TEST_ASSERT_EQ(map->length, (size_t)16);
    TEST_ASSERT_EQ(map->capacity, (size_t)16);
    for (int i = 0; i < 16; i++)
        TEST_ASSERT_INT_EQ(*(int*)linkedhashmap_get(map, &(indices[i]), sizeof(indices[i])), squares[i]);

    // test contains
    // { 0: 0, 1: 1, 2: 4, 3: 9, 4: 16, 5: 25, 6: 36, 7: 49, 8: 64, 9: 81, 10: 100, 11: 121, 12: 144, 13: 169, 14: 196, 15: 225 }
    for (int i = 0; i < 16; i++)
        TEST_ASSERT(linkedhashmap_contains(map, &(indices[i]), sizeof(indices[i])));
    TEST_ASSERT(!linkedhashmap_contains(map, &(indices[16]), sizeof(indices[16])));

    // test clear
    // { }
    linkedhashmap_clear(map);
    TEST_ASSERT_EQ(linkedhashmap_length(map), (size_t)0);
    TEST_ASSERT(linkedhashmap_is_empty(map));
    TEST_ASSERT_EQ(map->length, (size_t)0);
    TEST_ASSERT_EQ(map->capacity, (size_t)LINKEDHASHMAP_MIN_SIZE);

    // test foreach
    int intvals[] = { 1, 3, 7, 9 };
    int arraylen = sizeof(intvals) / sizeof(intvals[0]);
    int expected = 0;
    for (int i = 0; i < arraylen; i++)
    {
        linkedhashmap_set(map, &(indices[i]), sizeof(indices[i]), &(intvals[i]));
        expected += intvals[i];
    }
    int result = 0;
    linkedhashmap_foreach(map, sum_ints, &result);
    TEST_ASSERT_INT_EQ(result, expected);

    // test memory deallocation
    linkedhashmap_free(map);

    // Done
    printf("\nCompleted tests\n");
}
