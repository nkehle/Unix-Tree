/* Noa Kehle
 * Asgn 3 
 * 10/19/32 */

#include "tree.h"

int main(int argc, char *argv[]){
  int a_switch = 0, s_switch = 0, i = 1, num_files = 0, num_directories = 0;
  char *root = NULL;
  
  /* Arg checking */
  while(argv[i] != NULL){                             // checks all the args
    if(strcmp(argv[i], "-a") == 0) {                  // hidden switch
        a_switch = 1;
    } else if (strcmp(argv[i], "-s") == 0) {          // size switch
        s_switch = 1;
    } else if (argv[i]){                
      struct stat file_info;
       if (stat(argv[i], &file_info) == 0){           // if its a valid file
          if (S_ISDIR(file_info.st_mode)){            // if its a valid DIR 
            root = argv[i];
          } else {
            printf("Error: Invalid starting directory -> FILE given\n");    // its some other type of file
            return EXIT_FAILURE;
          }
        } else {
          printf("Error: Invalid starting directory -> Does not exist\n");    // its some other type of file
          return EXIT_FAILURE;
        }
    } i++;
  }


  /* Allocate space for the folder and the indent */ 
  struct Folder *root_folder = NULL;
  void *folder_tmp = (struct Folder *) malloc(sizeof(struct Folder));   // allocate space for root folder
  char *indent = NULL;
  void *indent_tmp = (char*) malloc(1000);                              // the initial size for the indentation

  /* Check if we were given the proper space */
  if ((folder_tmp == NULL) || indent_tmp == NULL){                    // malloc didnt work
    printf("Error: malloc failed\n"); 
    exit(0);
  } else {
    root_folder = folder_tmp;
    indent = indent_tmp;                // safety call
    strcpy(indent, "");                 // init the indent 
  }

   
  /* If no dir given, list current elements */
  if(root == NULL){
    get_folder(root_folder, ".", 0, a_switch, s_switch);             // populate the root folder array 
    for(int i = 0; i < root_folder->count-1; i++){
      printf("%s\n", root_folder->array[i]);
    }
    return 0; 
  }

  /* If a proper dir was giveni recurse into the foders and proceeed */
  get_folder(root_folder, root, 0, a_switch, s_switch);                   // populate the root folder array 
  printf("%s\n", root);                                                   // print the root name
  tree(root_folder, indent, &num_files, &num_directories);                // make initial call to the tree with level 0  
  printf("\n%d directories, %d files\n", num_directories, num_files);     // print the totals
  free(indent);                                                           // free the indent 
  return 0;                                           
}

/* Recursive function that takes a Folder struct and the level 
 * of depth of the directory at a given time
 * switches 0-> none 1-> -a 2-> -s 3-> both */  
void tree(struct Folder *current, char *indent, int *num_files, int *num_directories){
  
  /* Base case -> no more files */
  for(int i=2; i < current->count; i++){                  // skipt the . and .. by starting at 2 
    struct stat file_info;
    char *file_path = current->array[i]; 

    if (stat(file_path, &file_info) == 0){               // if the stat is sucssesful 
      if ((current->a_switch == 0) && (current->array[i][0] == '.')){
        // do nothing and ignore this file
      } else {

          /* Print the file/directory */ 
          printf("%s", indent);
          if(i < current->count -1) {
            printf("|-- ");    // not the last element
          } else {
            printf("`-- ");    // the last element
          }

          /* handle the s-switch */
          if(current->s_switch == 1){
            printf("[%11ld]  %s\n", file_info.st_size, current->array[i]);
          } else {
            printf("%s\n", current->array[i]);
          }
 
        /* If its a FILE */
        if (S_ISREG(file_info.st_mode)){ 
          *num_files += 1;                             // increment total files count
        }           
        /* If its a DIRECTORY */
        if (S_ISDIR(file_info.st_mode)){  
          *num_directories += 1;
          struct Folder *next = (struct Folder *) malloc(sizeof(struct Folder));           // make the next Folder struct for the next one
          get_folder(next, current->array[i], current->level + 1, current->a_switch, current->s_switch);      // initalize the next folder
       
          if(i < current->count - 1){
            strcat(indent, "|   ");        // if itsnot the last then add a |
          } else {
            strcat(indent, "    ");        // if its the last keep it to normal spaces
          }
          tree(next, indent, num_files, num_directories);     // Recursive call to tree
        }
      }  
    } 
  }
  rem_indent(indent);                   // remove 4 chars from the last indent 
  free_elements(current);               // go through and free the strings, array, and folder
  chdir("..");                          // backstep into the previous directory
  return;     
}

/* removes from the indent */
void rem_indent(char *indent){
    int len = strlen(indent);
    if (len >= 4) {
      indent[len - 4] = '\0';
    }
}


/* Determine how to qsort the elements */
int sort_comparator(const void *element1, const void *element2){
  char *entry1 = *(char **)element1;                    // double pointer because qsort expects a point to point of dirent struct
  char *entry2 = *(char **)element2; 
  return strcmp(entry1, entry2);                        // compare by their names, thus . and .. should be first
}

/* Places each element of the directory into an array while alloting the proper
 * space and keeping track of how many element exist within the folder */
void get_folder(struct Folder *current, char *directory, int level, int a_switch, int s_switch){
  /* Initialize the folder struct */
  current->count = 0;
  current->capacity = 0;
  current->level = level;
  current->a_switch = a_switch;
  current->s_switch = s_switch;
  current->array = NULL;
  
  DIR *dir = opendir(directory); 
  struct dirent *entry;             // used for the readdir

  /* read from the directory until you reach the end */
  while ((entry = readdir(dir)) != NULL) {                                  // check until the list is empty
      int size = strlen(entry->d_name) + 1;                                 // +1 for null terminator 
      
      current->array = (char **) realloc(current->array, ((current->count + 1) * sizeof(char *)));// alocate another space for a dirent entry
      if (current->array == NULL){
        printf("Error: realloc failed\n");
        exit(0);      
      }
      
      current->array[current->count] = (char *) malloc(size);               // allocates space for a string and storing the address in the space
      if (current->array[current->count] == NULL){
        printf("Error: malloc failed\n");
        exit(0);
      }   
      strcpy(current->array[current->count], entry->d_name);                // copy the array into the alloted space
      current->count += 1;                                                  // increment the element counter
      current->capacity += 1;
    }

  qsort(current->array, current->count, sizeof(entry), sort_comparator);    // sort the array alphabetically 
  chdir(directory);
  closedir(dir);
  return; 
}

/* Frees the selected folder and all of its entities */
void free_elements(struct Folder *fol){
  for (int i = 0; i < fol->count; i++) {
    free(fol->array[i]);
  }
  free(fol->array);
  free(fol);
}

