#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "wc.h"
#include <string.h>

#define FNV_OFFSET 2166136261
#define FNV_PRIME 16777619

struct key_val_pair{ // must alloc using calloc or wont init used to 0
	char* key; // key = string
	int val; // value = int
	int used; // flag to indicate if this kvp is used by default 0 = no, 1 = yes
	struct key_val_pair* next; // pointer to next kvp in case of collision
};

struct hashtable{ // string:int hashtable
	struct key_val_pair* kvp_arr; // var size array of key value pairs
	unsigned int size; // size of hash table
};

unsigned int hash(char* k){
	unsigned int h = FNV_OFFSET;
	int i = 0;
	while (k[i] != '\0'){
		h = h ^ k[i];
		h = h * FNV_PRIME;
		i++;
	}
	return h;
}

int hash_index(struct hashtable* ht, char* k){
	return hash(k) % ht->size;
}

int get_kvp_value(struct hashtable* ht, char* k){
	struct key_val_pair* kvp = &(ht->kvp_arr[hash_index(ht, k)]);
	if (kvp->used == 0){
		return -1;
	}
	while (strcmp(kvp->key, k) != 0 && kvp->next != NULL){ // keep going through linked list till u find value, or reach end
		kvp = kvp->next;
	}
	if (strcmp(kvp->key, k) == 0 ){
		return kvp->val;
	}
	return -1;
}

int add_kvp(struct hashtable* ht, char *k, int v){
	struct key_val_pair* kvp = &(ht->kvp_arr[hash_index(ht, k)]);
	if (get_kvp_value(ht, k) == -1){
		if (kvp->used == 0){ // empty slot
			kvp->key = malloc((strlen(k) + 1) * sizeof(char)); strcpy(kvp->key, k); 
			kvp->val = v;
			kvp->used = 1;
			kvp->next = NULL;
		} else { // collision
			while (kvp->next != NULL){
				kvp = kvp->next;
			}
			struct key_val_pair* new_kvp = (struct key_val_pair*)malloc(sizeof(struct key_val_pair));
			new_kvp->key = malloc((strlen(k) + 1) * sizeof(char)); strcpy(new_kvp->key, k);
			new_kvp->val = v;
			new_kvp->used = 1;
			new_kvp->next = NULL;
			kvp->next = new_kvp;
		}
		return 0;

	}
	return -1;
}

int change_kvp_value(struct hashtable *ht, char*k, int new_v){  // return status 0 = good, -1=bad
	struct key_val_pair* kvp = &(ht->kvp_arr[hash_index(ht, k)]);
	if (kvp->used == 0){
		return -1;
	}
	while (strcmp(kvp->key, k) != 0 && kvp->next != NULL){ // keep going through linked list till u find value, or reach end
		kvp = kvp->next;
	}
	if (strcmp(kvp->key, k) == 0 ){
		kvp->val = new_v;
		return 0;
	}
	return -1;
}

struct wc {
	struct hashtable* ht; // pointer to the hashtable that holds all data
	char **words; // array of unique words
	int wc_size; // number of unique words
};

struct wc * wc_init(char *word_array, long size)
{
	struct wc *wc = (struct wc *)malloc(sizeof(struct wc));
	wc->words = NULL;
	wc->wc_size = 0;
	assert(wc);

	struct hashtable *ht = (struct hashtable *)malloc(sizeof(struct hashtable));
	ht->size = size; // todo this is an over exaguration but good way to ensure hashtable size never exceeded. would be better to implement dynamic ht sizing but not req here
	ht->kvp_arr = (struct key_val_pair*)calloc(ht->size, sizeof(struct key_val_pair)); // defaults all to zero 
	wc->ht = ht;

	const char *p = word_array;
	const char delims[] = " \n\t\r\v\f"; // all chars caught by isspace()
	while (*p) {
		p += strspn(p, delims); // return 0 if no delimiters found, else returns index of first non-delimiter
		const char *start = p;
        p += strcspn(p, delims); //  returns strlen if no delimiters found, else returns index of first delimiter

        unsigned int len = p - start;
        if (len <= 0) {
			continue; // no word found
		}
            
		char * word = (char *)malloc((len + 1) * sizeof(char));
		memcpy(word, start, len);
        word[len] = '\0';

		if (add_kvp(ht, word, 1) == -1){ // already exists in table
			int curr_count = get_kvp_value(ht, word);
			change_kvp_value(ht, word, curr_count + 1);
		}else{ // new word
			wc->wc_size += 1;
			wc->words = realloc(wc->words, wc->wc_size * sizeof(char*));
			wc->words[wc->wc_size - 1] = malloc((strlen(word) + 1) * sizeof(char));
			strcpy(wc->words[wc->wc_size - 1], word);
		}
    }
	return wc;
}

void wc_output(struct wc *wc)
{
	for (int i=0; i<wc->wc_size; i++){
		int count = get_kvp_value(wc->ht, wc->words[i]);
		printf("%s:%d\n", wc->words[i], count);
	}
}

void wc_destroy(struct wc *wc)
{
	for (int i=0; i<wc->wc_size; i++){
		free(wc->words[i]);
	}
	free(wc->words);
	for (int i=0; i<wc->ht->size; i++){
		struct key_val_pair* kvp = &(wc->ht->kvp_arr[i]);
		if (kvp->used == 1){
			free(kvp->key);
			struct key_val_pair* next_kvp = kvp->next;
			while (next_kvp != NULL){
				struct key_val_pair* temp = next_kvp;
				next_kvp = next_kvp->next;
				free(temp->key);
				free(temp);
			}
		}
	}
	free(wc->ht->kvp_arr);
	free(wc->ht);
	free(wc);
}
