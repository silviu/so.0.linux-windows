#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
	
#define CHUNK_SIZE 20

typedef struct node {
	char* value;
	struct node* next;
} node;

typedef struct hash_t {
	node** head;
	int length;
} hash_t;

node** new_buckets(int hash_length) 
{
	int i;
	node** buckets = malloc(sizeof(node*) * hash_length);
	if (buckets == NULL) {
		return NULL;
	}
	
	for (i = 0; i < hash_length; i++)
		buckets[i] = NULL;
	return buckets;
}


hash_t* new_hash(int hash_length)
{
	hash_t* hash_table = malloc(sizeof(hash_t));
	if (hash_table == NULL)
		return NULL;
		
	hash_table->length = hash_length;
	hash_table->head = new_buckets(hash_length);
	if (hash_table->head == NULL) {
		free(hash_table);
		return NULL;
	}
	
	return hash_table;
}

node* find(char *search_term, hash_t* hash_table)
{
	int key = hash(search_term, hash_table->length);
	node* np = hash_table->head[key];
	for(; np != NULL; np = np->next)
		if (strcmp(np->value, search_term) == 0)
			return np;
	return NULL;
}

int add(char* value_to_add, hash_t* hash_table)
{
	int key, str_len;
	node* it;
		
	/*Evitare dubluri*/
	node* new_node = find(value_to_add, hash_table);
	if (new_node != NULL)
		return -1;
	
	key = hash(value_to_add, hash_table->length);
	new_node = malloc(sizeof(node));
	if(new_node == NULL)
		return 0;
	
	str_len = strlen(value_to_add) + 1;
	new_node->value = malloc(str_len);
	if(new_node->value == NULL) {
		free(new_node);
		return 0;
	}
	memcpy(new_node->value, value_to_add, str_len);
	
	it = hash_table->head[key];
	if (it == NULL) {
		hash_table->head[key] = new_node;
		hash_table->head[key]->next = NULL;
		return 1;
	}
	
	for (; it->next; it = it->next);
	new_node->next = NULL;
	it->next = new_node;
	
	
	return 1;
}

void rem(char* value, hash_t* hash_table)
{
	int key;
	node* it;
		
	node* np = find(value, hash_table);
	if (NULL == np)
		return;
		
	key = hash(value, hash_table->length);
	
	if (hash_table->head[key] == np) {
		hash_table->head[key] = np->next;
		free(np->value);
		free(np);
		return;
	}
	it = hash_table->head[key];
	for (; it->next != np; it = it->next);
	it->next = np->next;
	free(np->value);
	free(np);
	return;
}

void clear(hash_t* hash_table)
{
	int i;
	for (i = 0; i < hash_table->length; i++)
		if (hash_table->head[i] != NULL) {
			node* np = hash_table->head[i];
			for (; np != NULL;) {
				node* aux = np;
				np = np->next;
				free(aux->value);
				free(aux);
			}
			hash_table->head[i] = NULL;
		}
}


void free_hash(hash_t* hash_table)
{
	clear(hash_table);
	free(hash_table->head);
	free(hash_table);
}

void print_bucket(int key, FILE* f, hash_t* hash_table)
{
	node* np = hash_table->head[key];
	/*printf("[%d] - ", key);*/
	for(; np != NULL; np = np->next)
			fprintf(f, "%s ", np->value);
	fprintf(f, "\n");
}

void print_hash(FILE* f, hash_t* hash_table)
{
	int i;
	for (i = 0; i < hash_table->length; i++)
		if (hash_table->head[i] != NULL)
			print_bucket(i, f, hash_table);
}

void swap_hash_tables(hash_t* h1, hash_t* h2)
{
	hash_t aux;
	aux = *h1;
	*h1 = *h2;
	*h2 = aux;
}

void resize(hash_t* hash_table, int new_length)
{
	int i;
	hash_t* new_hash_table = new_hash(new_length);
	
	for (i = 0; i < hash_table->length; i++) {
		node* np = hash_table->head[i];
 		for (; np != NULL; np = np->next)
 			add(np->value, new_hash_table);
 	}
 	swap_hash_tables(hash_table, new_hash_table);
 	free_hash(new_hash_table);
}

void resize_double(hash_t* hash_table)
{
	resize(hash_table, 2 * hash_table->length);
}

void resize_halve(hash_t* hash_table)
{
	resize(hash_table, hash_table->length/2);
}


void run_commands(char* command, char** args, int nr_args, hash_t* hash_table)
{
	if (!strcmp(command, "add")) {
		if (nr_args == 0) {
			fprintf(stderr, "Parameter missing for \"add\" command.\n");
			return;
		}
		add(args[0], hash_table);
		return;
	}
	
	if (!strcmp(command, "remove")) {
		if (nr_args == 0) {
			fprintf(stderr, "Parameter missing for \"remove\" command.\n");	
			return;
		}
		rem(args[0], hash_table);
		return;
	}
	
	if (!strcmp(command, "find")) {
		FILE* f;
		node* np;
		if (nr_args == 0) {
			fprintf(stderr, "Parameter missing for \"find\" command.\n");
			return;
		}
		if (nr_args > 1)
			f = fopen(args[1], "a");
		else
			f = stdout;
			
		np = find(args[0], hash_table);
		if (np)
			fprintf(f, "True\n");
		else
			fprintf(f, "False\n");
		if (f != stdout)
			fclose(f);
		return;
	}
	
	if (!strcmp(command, "print_bucket")) {
		FILE* f;
		if (nr_args == 0) {
			fprintf(stderr, "Parameter missing for \"print_bucket\" command.\n");
			return;
		}
		
		if (nr_args > 1)
			f = fopen(args[1], "a");
		else
			f = stdout;
		
		print_bucket(atoi(args[0]), f, hash_table);
		if (f != stdout)
			fclose(f);
		return;
	}
	
	if (!strcmp(command, "resize")) {
		if (nr_args == 0) {
			fprintf(stderr, "Parameter missing for \"resize\" command.\n");
			return;
		}
		if (!strcmp(args[0], "double"))
			resize_double(hash_table);
		else if	(!strcmp(args[0], "halve"))
			resize_halve(hash_table);
		else {
			fprintf(stderr, "Invalid argument[%s] for Resize.\n", args[0]);
		}
	}
	
	if (!strcmp(command, "print")) {
		FILE* f;
		if (nr_args == 0)
			f = stdout;
		else
			f = fopen(args[0], "a");
			
		print_hash(f, hash_table);
		if (f != stdout)
			fclose(f);
	}
	
	if (!strcmp(command, "clear")) {
		clear(hash_table);
		return;
	}
}

void parse_line(char* line, hash_t* hash_table)
{
		int nr_args = 0;
	char *command, **args;
	char* word = strtok(line, " ");
	if (word == NULL) {
		/*printf("EMPTY LINE=[%s].\n", line);*/
		return;
	}
	command = word;
	
	args = malloc(nr_args * sizeof(char*));
	while(word) {
		word = strtok(NULL, " ");
		if (word) {
			nr_args++;
			args = realloc(args, nr_args * sizeof(char*));
			args[nr_args - 1] = word;
		}
	}
	run_commands(command, args, nr_args, hash_table);
	free(args);
}

void strip_crlf(char* command)
{
	size_t len = strlen(command);
	if (strlen(command) == 0)
		return;
		
	if (command[len - 1] == '\n' || command[len - 1] == '\r')
		command[len - 1] = '\0';
}

void read_file(FILE* f, hash_t* hash_table)
{
	size_t buff_size = CHUNK_SIZE;
	char* command = malloc(buff_size + 1);
	
	while (!feof(f)) {
		command[0] = '\0';
		for(;;) {
			size_t offset = strlen(command);
			char* ret = fgets(command + offset, buff_size - offset, f);
			if (ret == NULL && feof(f))
				break;
			if (ret == NULL) {
				fprintf(stderr, "ERROR reading from file in read_files().\n");
				return;
			}
			if (strchr(command + offset, '\n'))
				break;
			buff_size *= 2;
			command = realloc(command, buff_size + 1);
		}
		strip_crlf(command);
		parse_line(command, hash_table);
	}
	free(command);
}

void read_files(int argc, char** argv, hash_t* hash_table)
{
	int i;
	int nr_files = argc - 2;
	
	for (i = 0; i < nr_files; i++) {
		FILE* f = fopen(argv[i + 2], "r");
		read_file(f, hash_table);
		fclose(f);
	}
}

void read_stdin(hash_t* hash_table)
{
	read_file(stdin, hash_table);
}

void read_input(int argc, char** argv, hash_t* hash_table)
{
	if (argc == 2)
		read_stdin(hash_table);
	else
		read_files(argc, argv, hash_table);
}

int main(int argc, char** argv)
{
	int hash_length = 0;
	hash_t* hash_table;
	if (argc < 2) {
		fprintf(stderr, "Too few arguments.\n");
		exit(EXIT_FAILURE);
	}
	
	hash_length = atoi(argv[1]);
	
	hash_table = new_hash(hash_length);
	if (hash_table == NULL) {
		fprintf(stderr, "Error creating new Hash Table.\n");
		exit(EXIT_FAILURE);
	}
	
	read_input(argc, argv, hash_table);
	free_hash(hash_table);
	
	return 0;
}

