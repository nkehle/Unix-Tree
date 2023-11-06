#ifndef TREE_H
#define TREE_H

#include <stdlib.h>   // used for printing
#include <dirent.h>   // used for readdir
#include <stdio.h>    // used for files in-out
#include <string.h>   // used for strcmp
#include <sys/stat.h> // used for stat call o
#include <unistd.h>   // used for chdir
                  

struct Folder{
  int level;
  int count;
  int capacity;
  int a_switch;
  int s_switch;
  char **array;
};


void tree(struct Folder *current, char *indent, int *num_files, int *num_directories);
void rem_indent(char *indent);
void get_folder(struct Folder *current, char *directory, int level, int a_switch, int s_switch);
int sort_comparator(const void *element1, const void *element2);
void print_level(int level, int spaces, int dashes, int last);
void free_elements(struct Folder *current);

#endif
