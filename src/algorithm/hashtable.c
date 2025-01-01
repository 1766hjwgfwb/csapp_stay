/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */



#include<stdio.h>
#include<assert.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<stdbool.h>
#include<headers/common.h>
#include<headers/algorithm.h>



//internal function
static int hash_function_index(hash_table_t *ht, int key);
static uint64_t lowbits_n(uint64_t num, int length);
static bucket_t* create_bucket(int depth, int capacity);
static void split_bucket(hash_table_t *table, int index);
static void merge_buckets(hash_table_t *table);
static void free_bucket(bucket_t *bucket);

static void hashtable_print_sorted(hash_table_t *tab);
static int compare_int(const void *a, const void *b);



/*static uint64_t hash_function(char *str)
{
    int p = 31;
    int m = 1000000007;

    int k = p;
    int v = 0;
    for (int i = 0; i < strlen(str); ++ i)
    {
        v = (v + ((int)str[i] * k) % m) % m;
        k = (k * p) % m;
    }
    return v;
}*/
static int hash_function_index(hash_table_t *ht, int key) {
    int mask = (1 << ht->globaldepth) - 1;  // Mask based on global depth
    return key & mask;
}



static uint64_t lowbits_n(uint64_t num, int length)
{
    uint64_t mask = ~(0xffffffffffffffff << length);
    return num & mask;
}


static bucket_t* create_bucket(int depth, int capacity) {
    bucket_t *bucket = (bucket_t*)malloc(sizeof(bucket_t));

    // set localdepth
    bucket->localdepth = depth;

    // malloc pair
    bucket->data = (pair_t*)malloc(capacity * sizeof(pair_t));

    bucket->counter = 0;
    bucket->capacity = capacity;

    return bucket;
}


static void free_bucket(bucket_t *bucket) {
    if (bucket != NULL) {
        if (bucket->data != NULL) {
            free(bucket->data);
        }
        free(bucket);
    }
}


hash_table_t *hashtable_construct(int bsize) {
    hash_table_t *tab = (hash_table_t*)malloc(sizeof(hash_table_t));

    tab->globaldepth = 1;

    // max number of buckets in each level
    tab->bucket_capacity = bsize;

    // entry -> 2, 4, 8... 2^globaldepth
    tab->directory_size = 1 << tab->globaldepth;

    tab->directory = (bucket_t**)malloc(tab->directory_size * sizeof(bucket_t*));

    // init directory
    tab->directory[0] = create_bucket(1, bsize);
    tab->directory[1] = create_bucket(1, bsize);

    /*
        globaldepth = 1;
        entry
        [0]  -> bucket(localdepth=1, capacity=bsize)
        [1]  -> bucket(localdepth=1, capacity=bsize)
    */

   return tab;
}


void hashtable_free(hash_table_t *tab) {
    assert(tab != NULL);

    for (int i = 0; i < tab->directory_size; ++ i) {
        if (tab->directory[i] != NULL) {
             // multiplicity directory pointer bucket
             // check same bucket, if same mark
             bool found = false;
             for (int j = 0; j < i; ++ j) {
                if (tab->directory[i] == tab->directory[j]) {
                    found = true;
                    break;
                }
             }

             // not same pointer address -> bucket, free
             if (!found)
                free_bucket(tab->directory[i]);
        }
    }


    free(tab->directory);
    free(tab);
}


int hashtable_insert(hash_table_t *tab_addr, int key, int val) {
    // get index
    int index = hash_function_index(tab_addr, key);
    bucket_t *bucket = tab_addr->directory[index];

    // bucket is full, split
    if (bucket->counter >= bucket->capacity) {
        // first split
        split_bucket(tab_addr, index);

        // hashtable_insert(tab_addr, key, val);
        while (hashtable_insert(tab_addr, key, val) == 0) {}
    } else {
        // insert k-v pair
        bucket->data[bucket->counter].key = key;
        bucket->data[bucket->counter].val = val;
        bucket->counter++;
        return 1;
    }

    return 0;
}


static void merge_buckets(hash_table_t *table) {
    // todo: merge buckets
}


static void split_bucket(hash_table_t *table, int index) {
    // get old bucket
    bucket_t *old_bucket = table->directory[index];
    int old_localdepth = old_bucket->localdepth;

    // check if need to double capacity
    if (old_localdepth == table->globaldepth) {
        // double capacity size
        table->directory_size *= 2;
        table->directory = (bucket_t**)realloc(table->directory, table->directory_size * sizeof(bucket_t*));


        // copy old bucket to new bucket
        for (int i = 0; i < (1 << table->globaldepth); ++ i) {
            table->directory[i + (1 << table->globaldepth)] = table->directory[i];
        }

        // if globaldepth = 1
        /*
            00 -> 00 localdepth=1
            01
            10
            11 -> 01 localdepth=1
        */

       table->globaldepth++;
    }

    // create new bucket
    // current old bucket split to two new bucket
    bucket_t *new_bucket = create_bucket(old_localdepth + 1, table->bucket_capacity);

    old_bucket->localdepth++;

    // rehash all key-value pair to new bucket
    

    // * store k-v pair in the old bucket
    // 1101  `01`
    // bit_to_check = 2^localdepth
    int bit_to_check = 1 << old_localdepth;
    /*pair_t *keys_to_rehash_pair = malloc(sizeof(pair_t) * old_bucket->counter);
    
    int rehash_counter = 0;
    for (int i = 0; i < old_bucket->counter; i++) {
        keys_to_rehash_pair[rehash_counter++]= old_bucket->data[i];
    }*/

    int old_couter = old_bucket->counter;
    old_bucket->counter = 0;

    // * rehash all key-value pair to old or new bucket
    for (int i = 0; i < old_couter; i++) {
        // int key = keys_to_rehash_pair[i].key;
        // pair_t local_pair = keys_to_rehash_pair[i];
        pair_t local_pair = old_bucket->data[i];    // copy old data to local_pair, reallocate key-value pair
        int key = local_pair.key;
        // reallocate key-value pair
        if (key & bit_to_check) {
            // rehash to new bucket
            new_bucket->data[new_bucket->counter].key = local_pair.key;
            new_bucket->data[new_bucket->counter].val = local_pair.val;
            new_bucket->counter++;
        } else {
            // rehash to old bucket
            old_bucket->data[old_bucket->counter].key = local_pair.key;
            old_bucket->data[old_bucket->counter].val = local_pair.val;
            old_bucket->counter++;
        }
    }

    // free(keys_to_rehash_pair);

    // * update directory
    for (int i = 0; i < table->directory_size; i++) {
        if (table->directory[i] == old_bucket && (i & bit_to_check)) {
            table->directory[i] = new_bucket;
        }
    }
}


int hashtable_get(hash_table_t *tab, int key, int *val) {
    // get index
    int index = hash_function_index(tab, key);
    bucket_t *bucket = tab->directory[index];

    for (int i = 0; i < bucket->counter; ++ i) {
        if (bucket->data[i].key == key) {
            *val = bucket->data[i].val;
            return 1;
        }
    }

    // not found
    return 0;
}


int hashtable_delete(hash_table_t *tab, int key) {
    // get index
    int idnex = hash_function_index(tab, key);
    bucket_t *bucket = tab->directory[idnex];

    for (int i = 0; i < bucket->counter; ++ i) {
        if (bucket->data[i].key == key) {
            // delete k-v pair

            // scan and shift all key-value pair
            /* for (int j = i; j < bucket->counter - 1; ++ j) {
                bucket->data[j] = bucket->data[j + 1];
            }

            bucket->counter--;
            return 1;*/

            // swap with last pair
            bucket->data[i] = bucket->data[bucket->counter - 1];
            bucket->counter--;
            return 1;
        }
    }

    return 0;
}


void hashtable_print(hash_table_t *tab) {
    // printf("Global: %d\n", tab->globaldepth);
    // for (int i = 0; i < tab->directory_size; ++ i) {
    //     printf("(bucket:%d, localdepth:%d)", i, tab->directory[i]->localdepth);
    //     for (int j = 0; j < tab->directory[i]->counter; ++ j) {
    //         printf(" %d ", tab->directory[i]->data[j].key);
    //     }
    //     // printf("counter:%d\n", tab->directory[i]->counter);
    //     printf("\n");
    // }

    hashtable_print_sorted(tab);
}


static void hashtable_print_sorted(hash_table_t *tab) {
    assert(tab != NULL);

    printf("Global: %d\n", tab->globaldepth);

    for (int i = 0; i < tab->directory_size; ++i) {
        printf("(bucket:%d, localdepth:%d) ", i, tab->directory[i]->localdepth);

        // copy keys to temporary array
        int *keys = (int *) malloc(tab->directory[i]->counter * sizeof(int));
        assert(keys != NULL); // 断言 malloc 成功
        for (int j = 0; j < tab->directory[i]->counter; ++j) {
            keys[j] = tab->directory[i]->data[j].key;
        }

        // sort keys, use qsort
        qsort(keys, tab->directory[i]->counter, sizeof(int), compare_int); 

        // print sorted keys
        for (int j = 0; j < tab->directory[i]->counter; ++j) {
            printf("%d ", keys[j]);
        }
        printf("\n");

        free(keys);
    }
}


// uesd by qsort
static int compare_int(const void *a, const void *b) {
    return (*(int *) a - *(int *) b);
}