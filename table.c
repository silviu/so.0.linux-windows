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


typedef struct node {
	char* key;
	int value;
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

void init_hash(node* hash_table[], int hash_length) 
{
	int i;
	for (i = 0; i < hash_length; i++)
		hash_table[i] = NULL;
}

node* find(char *search_term, node* hash_table[], int hash_length)
{
	int index = hash(search_term, hash_length);
	node* np = hash_table[index];
	for(; np != NULL; np = np->next)
			if (strcmp(np->key, search_term) == 0)
				return np;
	return NULL;
}

int add(char* key_to_add, node* hash_table[], int hash_length)
{
	int index;
	printf("++ADDING [%s]:\n", key_to_add);
	node* new_node;
	if((new_node = find(key_to_add, hash_table, hash_length)) == NULL){
		index = hash(key_to_add, hash_length);
		new_node = (node*) malloc(sizeof(node));
		if(new_node == NULL)
			return 0;
		new_node->key = strdup(key_to_add);
		if(new_node->key == NULL)
			return 0;
		new_node->next = hash_table[index];
		hash_table[index] = new_node;
	}
	else
		return -1;		
	return 1;
}

void print_bucket(int index, node* hash_table[])
{
	node* np = hash_table[index];
	for(; np != NULL; np = np->next)
		printf("\t[%d] - [%s]\n", np->value, np->key);
}

void print_hash(node* hash_table[], int hash_length)
{
	int i;
	printf("++PRINT:\n");
	for (i = 0; i < hash_length; i++)
		if (hash_table[i] != NULL)
			print_bucket(i, hash_table);
}

void test(node* hash_table[], int hash_length)
{
	add("mimi", hash_table, hash_length);
	print_hash(hash_table, hash_length);
	
	add("mama", hash_table, hash_length);
	print_hash(hash_table, hash_length);
	
	DIE(!find("mimi", hash_table, hash_length), "NOT FOUND");
	DIE(!find("mama", hash_table, hash_length), "NOT FOUND");
}

int main(int argc, char** argv)
{
	int hash_length = 0;
	if (argc < 2) {
		fprintf(stderr, "Too few arguments.\n");
		exit(EXIT_FAILURE);
	}
		
	hash_length = atoi(argv[1]);
	node* hash_table[hash_length];
	init_hash(hash_table, hash_length);
	
	printf("hash_length=[%d]\n", hash_length);
	read_input(argc, argv);
	
	test(hash_table, hash_length);
	
	return 0;
}

