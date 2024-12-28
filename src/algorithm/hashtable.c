/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

#include<stdio.h>
#include<assert.h>
#include<stdlib.h>
#include<string.h>
#include<headers/cpu.h>
#include<headers/memory.h>
#include<headers/common.h>
#include<headers/algorithm.h>



static int mod_pow(int base, int exp, int mod) {
    int result = 1;

    while (exp > 0) {
        if (exp & 1)
            result = (result * base) % mod;     // if exp bit is 1, multiply base to result

        base = (base * base) % mod;  
        exp >>= 1;    // right shift by 1 bit   110 -> 11
    }

    return result;
}


static uint64_t hash_function(char *str) { 
    int p = 31;     // selecte prime number for base
    int m = 1000000007;  // selecte a large prime number for modulus

    // Rolling weight
    int k = p;  
    int v = 0;     // initialize hash value to 0

    int len = strlen(str);

    for (int i = 0; i < len; i++) {
        // k = mod_pow(p, i, m);

        // calculate hash value, using rolling weight
        v = (v + ((int)str[i] * k) % m) % m;
        k = (k * p) % m;
    }
    
    return v;
}


// bsize is the size of the bucket array
hash_table_t *hashtable_construct(int bsize) {
    hash_table_t *tab = (hash_table_t *)malloc(sizeof(hash_table_t));

    // assert(tab != NULL);

    tab->globaldepth = 1;    // set global depth to 1
    tab->size = bsize;      // set maximum number of buckets to bsize


    // Satisfy design hash_value & ( (1 << global_depth) - 1) 
    tab->num = 1 << tab->globaldepth;   // set number of buckets to 2^globaldepth

    // allocate memory for bucket array
    tab->barry = malloc(tab->num * sizeof(bucket_t));

    for (int i = 0; i < tab->num; i++) {
        bucket_t *b = &tab->barry[i];

        // set local bucket depth to 1
        b->localdepth = 1;
        b->counter = 0;

        // allocate memory for key array and value array
        b->karray = malloc(tab->size * sizeof(char *));
        b->varray = malloc(tab->size * sizeof(uint64_t));
    }

    return tab;
}


void hashtable_free(hash_table_t *tab) {
    for (int i = 0; i < tab->num; i++) {
        bucket_t *b = &tab->barry[i];

            for (int k = 0; k < b->counter; k++) {
                free(b->karray[k]);
            }
            
            free(b->karray);
            free(b->varray);
    }

    free(tab->barry);
    free(tab);
}


int hashtable_get(hash_table_t *tab, char *key, uint64_t *val) {
    uint64_t hashkey = hash_function(key);
    // set low_bits, bucket_index
    int mask_bits_low = 64 - tab->globaldepth;
    // e.g hashky = 1101...0011
    // 64 - 1 = 63, hashkey << 63 = 100000.0000
    // hashkey >> 63 = 0000000.0001
    uint64_t hid = (hashkey << mask_bits_low) >> mask_bits_low;

    // 1 -> high_bucket, 0 -> low_bucket
    bucket_t *b = &tab->barry[hid];

    // search key in bucket
    for (int i = 0; i < b->counter; i++) {
        if (strcmp(b->karray[i], key) == 0) {
            // karray[i] is the key, valarray[i] is the value
            *val = b->varray[i];
            return 0;
        }
    }

    // key not found
    return -1;
}


int hashtable_insert(hash_table_t *tab, char *key, uint64_t val) {
    uint64_t hashkey = hash_function(key);
    // set low_bits, bucket_index
    int mask_bits_low = 64 - tab->globaldepth;
    // e.g hashky = 1101...0011
    // 64 - 1 = 63, hashkey << 63 = 100000.0000
    // hashkey >> 63 = 0000000.0001
    uint64_t hid = (hashkey << mask_bits_low ) >> mask_bits_low;

    // 1 -> high_bucket, 0 -> low_bucket
    bucket_t *b = &tab->barry[hid];

    if (b->counter < tab->size) { 
        b->karray[b->counter] = malloc(sizeof(char) * (strlen(key) + 1));
        strcpy(b->karray[b->counter], key);
        b->varray[b->counter] = val;
        
        b->counter++;

        if (b->counter > tab->num * 0.75) {
            // need to rehash
            // TODO: rehashing
        }

        return 1;
    } else {
        // bucket is full
        
    }


    return 0;
}


void hashtable_expand(hash_table_t *tab) {
   
}


void hashtable_print(hash_table_t *tab) {

    for (int i = 0; i < tab->size; i++) {
        bucket_t *b = &tab->barry[i];

        if (b->counter > 0) {
            printf("bucket %d:\n", i);

            for (int j = 0; j < b->counter; j++) 
                printf("key: %s, value: %lx\n", b->karray[j], b->varray[j]);
        }
    }
}