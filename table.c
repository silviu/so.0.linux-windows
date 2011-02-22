#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include "hash.c"

#define DIE(assertion, call_description)		\
	do {										\
		if (assertion) {						\
			fprintf(stderr, "(%s, %s, %d): ",	\
			__FILE__, __FUNCTION__, __LINE__);	\
			perror(call_description);	\
			exit(EXIT_FAILURE);					\
		}										\
	} while(0)
	
#define RUNT_TEST 1


typedef struct node {
	char* value;
	struct node* next;
} node;

void read_files(char** files)
{
	printf("READ FILES\n");
}

void read_stdin()
{
	printf("READ STDIN\n");
}

void read_input(int argc, char** argv)
{
	if (argc == 2)
		read_stdin();
	else
		read_files(argv);
}

void init_hash(node** hash_table, int hash_length) 
{
	int i;
	for (i = 0; i < hash_length; i++)
		hash_table[i] = NULL;
}

node* find(char *search_term, node** hash_table, int hash_length)
{
	int index = hash(search_term, hash_length);
	node* np = hash_table[index];
	for(; np != NULL; np = np->next)
		if (np->value != NULL)
			if (strcmp(np->value, search_term) == 0)
				return np;
	return NULL;
}

int add(char* value_to_add, node** hash_table, int hash_length)
{
	int index;
	if (RUNT_TEST)
		printf("ADD [%s]:\n", value_to_add);
		
	node* new_node = find(value_to_add, hash_table, hash_length);
	if (new_node != NULL)
		return -1;
	
		index = hash(value_to_add, hash_length);
		new_node = (node*) malloc(sizeof(node));
		if(new_node == NULL)
			return 0;
			
		new_node->value = malloc(strlen(value_to_add) + 1);
		memcpy(new_node->value, value_to_add, strlen(value_to_add) + 1);
		if(new_node->value == NULL)
			return 0;
			
		new_node->next = hash_table[index];
		hash_table[index] = new_node;
		
	return 1;
}

void rem(char* value, node** hash_table, int hash_length)
{
	if (RUNT_TEST)
		printf("REM [%s]:\n", value);
		
	int index = hash(value, hash_length);
	node* np = find(value, hash_table, hash_length);
	if (NULL == np)
		return;

	np = hash_table[index];
	if (np->value != NULL)
		if (strcmp(np->value, value) == 0) {
			node* aux = np;
			if (np->next == NULL) {
				hash_table[index] = NULL;
				free(aux->value);
				free(aux);
				return;
			}
			np = np->next;
			hash_table[index] = np;
			free(aux->value);
			free(aux);
			return;
		}
	
	for(; np->next != NULL; np = np->next) {
		if (strcmp(np->next->value, value) == 0) {
			node* aux = np->next;
			np->next = np->next->next;
			free(aux->value);
			free(aux);
			return;
		}
	}
}

void clear(node** hash_table, int hash_length)
{
	int i;
	if (RUNT_TEST)
		printf("CLEAR:\n");
	for (i = 0; i < hash_length; i++)
		if (hash_table[i] != NULL) {
			node* np = hash_table[i];
			for (; np != NULL; np= np->next) {
				node* aux = np;
				free(aux->value);
				free(aux);
			}
			hash_table[i] = NULL;
		}
}

void print_bucket(int index, node** hash_table)
{
	node* np = hash_table[index];
	printf("\t[%d] - ", index);
	for(; np != NULL; np = np->next)
		if (np != NULL)
			printf("[%s] ", np->value);
	printf("\n");
}

void print_hash(node** hash_table, int hash_length)
{
	int i;
	printf("PRINT:\n");
	for (i = 0; i < hash_length; i++)
		if (hash_table[i] != NULL)
			print_bucket(i, hash_table);
	printf("----------------------------------\n");
}

node** new_hash(int hash_length)
{
	node** ret = malloc(sizeof(node*) * hash_length);
	init_hash(ret, hash_length);
	return ret;
}

node** resize_double(node** hash_table, int* phash_length)
{
	int i;
	int hash_length = *phash_length;
	node** tmp_hash = new_hash(2 * (*phash_length));
	
	for (i = 0; i < hash_length; i++) {
		node* np = hash_table[i];
 		for (; np != NULL; np = np->next)
 			add(np->value, tmp_hash, 2 * hash_length);
 	}
	clear(hash_table, hash_length);
	free(hash_table);
	*phash_length = 2 * hash_length;
	
	return tmp_hash;
}

node** test(node** hash_table, int *hash_length)
{
	add("mimi", hash_table, *hash_length);
	print_hash(hash_table, *hash_length);
	DIE(!find("mimi", hash_table, *hash_length), "NOT FOUND");
	
	add("mama", hash_table, *hash_length);
	print_hash(hash_table, *hash_length);
	DIE(!find("mama", hash_table, *hash_length), "NOT FOUND");
	
	add("riri", hash_table, *hash_length);
	print_hash(hash_table, *hash_length);
	DIE(!find("riri", hash_table, *hash_length), "NOT FOUND");
	
	add("fofo", hash_table, *hash_length);
	print_hash(hash_table, *hash_length);
	DIE(!find("fofo", hash_table, *hash_length), "NOT FOUND");
	
	add("sasa", hash_table, *hash_length);
	print_hash(hash_table, *hash_length);
	DIE(!find("sasa", hash_table, *hash_length), "NOT FOUND");
	
	add("ywue", hash_table, *hash_length);
	print_hash(hash_table, *hash_length);
	DIE(!find("ywue", hash_table, *hash_length), "NOT FOUND");
	
	
	rem("fofo", hash_table, *hash_length);
	print_hash(hash_table, *hash_length);
	DIE(find("fofo", hash_table, *hash_length), "FOUND INEXISTING");
	
	rem("riri", hash_table, *hash_length);
	print_hash(hash_table, *hash_length);
	DIE(find("riri", hash_table, *hash_length), "FOUND INEXISTING");
	
	/*
	rem("ywue", hash_table, *hash_length);
	print_hash(hash_table, *hash_length);
	DIE(find("ywue", hash_table, *hash_length), "FOUND INEXISTING");
	
	
	rem("mimi", hash_table, *hash_length);
	print_hash(hash_table, *hash_length);
	DIE(find("mimi", hash_table, *hash_length), "FOUND INEXISTING");
	
	clear(hash_table, *hash_length);
	print_hash(hash_table, *hash_length);
	DIE(find("sasa", hash_table, *hash_length), "FOUND AFTER CLEAR");
	*/
	
	hash_table = resize_double(hash_table, hash_length);
	print_hash(hash_table, *hash_length);
	return hash_table;
}

int main(int argc, char** argv)
{
	int hash_length = 0;
	if (argc < 2) {
		fprintf(stderr, "Too few arguments.\n");
		exit(EXIT_FAILURE);
	}
		
	hash_length = atoi(argv[1]);
	node** hash_table = new_hash(hash_length);
	
	/*read_input(argc, argv);*/
	
	if(RUNT_TEST)
		hash_table = test(hash_table, &hash_length);
		
	printf("HASH_LENGTH=[%d]\n", hash_length);
	
	return 0;
}

