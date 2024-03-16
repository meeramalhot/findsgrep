#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include "apue.h"
#include "pathalloc.h"
#include "error.h"

/* Function type that is called for each filename */
typedef int Myfunc(const char *, const struct stat *, int);
static Myfunc myfunc;
static int myftw(char *, Myfunc *);
static int dopath(Myfunc *);
extern char *optarg;
bool validSuffix(const char *pathname);
bool matches(char* text, size_t lentext, char* regex, size_t lenreg);

//sfind is the string to search for                                                 
const char *sFind;
//holds directory name                                                              
const char *directoryname;

//search for all files                                                              
int suffix = 1;
//search for just regular files                                                     
bool justReg = true;

int main(int argc, char *argv[]) {
  // printf("in main\n");                                                           
  int opt;

  while ((opt = getopt(argc, argv, "p:f:ls:")) != -1)
    {
     switch (opt)
     {
      case 'p':
        directoryname = optarg;
        break;
       case 'f': ;
        char* suff = optarg;
        if(strcmp(suff,"c")==0) {
            //search for only c files                                               
            suffix = 2;
	    //printf("%d", suffix);
        }else if(strcmp(suff, "h")==0) {
            //search for only header files                                          
            suffix = 3;
	    //printf("%d", suffix);

        }else if(strcmp(suff, "S")==0) {
            //search for only S files                                               
            suffix = 4;
	    //printf("%d", suffix);

        }else{
          printf("Wrong argument given for suffix\n");
	  exit(1);
        }
          break;
        case 'l':
          //look for symbolic links and regular links                               
          justReg = false;
          break;
        case 's':
          sFind = optarg;
	  if(strcmp(sFind, "")==0) {
	    printf("string is empty, provide string arg /n");
	    exit(1);
	  }
          break;
     }
  }
  int ret;
    ret = myftw(directoryname, myfunc); /* does it all */
    exit(ret);
}

//taken from textbok and linked with library from textbook

/* Descend through the hierarchy, starting at "pathname".                                                                                  
 * The caller's func() is called for every file.                                                                                           
 */
#define FTW_F   1   /* file other than directory */
#define FTW_D   2   /* directory */
#define FTW_DNR 3   /* directory that can't be read */
#define FTW_NS  4   /* file that we can't stat */

static char *fullpath; /* contains full pathname for every file */


//takes in each file and turns filepath into a string                                                                                      
static int myftw(char *pathname, Myfunc *func) {
    int len;
    fullpath = path_alloc(&len); /* malloc's for PATH_MAX+1 bytes */

    // writes path into pathname buffer                                                                                                    
    strncpy(fullpath, pathname, len); /* protect against */
    fullpath[len-1] = 0; /* buffer overrun */

    int x = dopath(func);
    return(x);
}

/* Descend through the hierarchy, starting at "fullpath".                                                                                  
 * If "fullpath" is anything other than a directory, we lstat() it,                                                                        
 * call func(), and return. For a directory, we call ourself                                                                               
 * recursively for each name in the directory.                                                                                             
 */
static int dopath(Myfunc* func) {
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    int ret;
    char *ptr;
    if (lstat(fullpath, &statbuf) < 0) /* stat error */
        return(func(fullpath, &statbuf, FTW_NS));

    if (S_ISDIR(statbuf.st_mode) == 0) /* not a directory */
        return(func(fullpath, &statbuf, FTW_F));

    /* It's a directory. First call func() for the directory,                                                                              
     * then process each filename in the directory.                                                                                        
     */
    //printf("we have a directory %s \n", fullpath);
    if ((ret = func(fullpath, &statbuf, FTW_D)) != 0){
      //printf("branch 1 \n");
      return(ret);
    }
    ptr = fullpath + strlen(fullpath); /* point to end of fullpath */
    *ptr++ = '/';
    *ptr = 0;
    if ((dp = opendir(fullpath)) == NULL) /* can't read directory */
      {     //printf("branch 2 \n");                                                                                                       
        return(func(fullpath, &statbuf, FTW_DNR));
      }

    while ((dirp = readdir(dp)) != NULL) {
      //printf("%s \n", dirp->d_name);                                                                                                     
        if (strcmp(dirp->d_name, ".") == 0 ||
            strcmp(dirp->d_name, "..") == 0)
            continue; /* ignore dot and dot-dot */

        strcpy(ptr, dirp->d_name); /* append name after slash */

        if ((ret = dopath(func)) != 0) /* recursive */
            break; /* time to leave */
    }

    ptr[-1] = 0; /* erase everything from slash onwards */

    if (closedir(dp) < 0)
        err_ret("can't close directory %s", fullpath);

    return(ret);
}

bool validSuffix(const char *pathname){
  bool valid = false;
  int len = strlen(pathname);

  //any suffix, -f not included                                                                                                            
  if(suffix == 1) {
    valid = true;
  }
  //check for right suffix if .h file                                                                                                      
  else if(suffix == 2) {
    if (pathname[len-1]=='c' && pathname[len-2]=='.'){
      valid = true;
      // printf("suffix is 2 and c\n");
    }
  }
  //check for right suffix if .h file                                                                                                      
  else if(suffix == 3) {
      if (pathname[len-1]=='h' && pathname[len-2]=='.'){
      valid = true;
      //printf("suffix is 3 and h\n");

    }
  }
  //check for right suffix if .Sfile                                                                                                       
  else  if(suffix == 4) {
    if (pathname[len-1]=='S' && pathname[len-2]=='.'){
      valid = true;
    }
  }
  else{
      valid = false;
  }

  return valid;
}

static int myfunc(const char *pathname, const struct stat *statptr, int type) {

FILE *fileToSearch;
  //printf("we foud a file \n");                                                                                                           
if(type == FTW_F){
  //if -l flag is not specified, and just want to look at regular files                                                                    
  if(justReg == true && validSuffix(pathname)==true) {
    if ((statptr->st_mode & S_IFMT)==S_IFREG) {

    fileToSearch = fopen(pathname, "r");

    if (fileToSearch == NULL) {
      fprintf(stderr, "Cannot open file for reading: %s\n", pathname);
      return 2;
    }
    
    //call matches                                                                                                                        

    //replaces null with the address it malloc's                                                                                           
    char *buffer = NULL;
    //allocates line size for you                                                                                                          
    size_t bufsize = 0;
    size_t chars;


    size_t linesize;

    linesize = getline(&buffer, &bufsize, fileToSearch);

    //while loop for each line in folder
    //print if matches regex
    while(linesize != -1) {
      for(size_t i = 0; i<linesize; i++){
        if(matches(&buffer[i], linesize-i, sFind, strlen(sFind))) {
          printf("line is: %s", buffer);
          printf("path: %s\n", pathname);
	  printf("\n");
          break;
        }
      }
      //alloactes space for next line
      free(buffer);
      buffer = NULL;
      bufsize = 0;
      linesize = getline(&buffer, &bufsize, fileToSearch);
    }


    fclose(fileToSearch);
    return 0;
    }
  }

  if (justReg == false && validSuffix(pathname)==true){
    //if -l flag is specified and we want both regular and symbolic link files                 \
                                                                                                
    if ((statptr->st_mode & S_IFMT)==S_IFREG || (statptr->st_mode & S_IFMT)==S_IFLNK) {
      fileToSearch = fopen(pathname, "r");

    if (fileToSearch == NULL) {
      fprintf(stderr, "Cannot open file for reading: %s\n", pathname);
      return 2;
      // call matches                                                                          \
                                                                                                
    }

    //replaces null with the address it malloc's                                            
                                                                                                
    char *buffer = NULL;
    //allocates line size for you                                                              \
                                                                                                
    size_t bufsize = 0;
    size_t chars;


    size_t linesize;

    linesize = getline(&buffer, &bufsize, fileToSearch);

    //while loop for each line                                                               
    //loop through file and print if matches                                                           
    while(linesize != -1) {
      for(size_t i = 0; i<linesize; i++){
        if(matches(&buffer[i], linesize-i, sFind, strlen(sFind))) {
          printf("line is: %s", buffer);
          printf("path: %s\n", pathname);
	  printf("\n");

          break;
        }
      }
      free(buffer);
      buffer = NULL;
      bufsize = 0;
      linesize = getline(&buffer, &bufsize, fileToSearch);
    }

    fclose(fileToSearch);

    return 0;
    }
  }
} else {
  //printf("%s was not a specified file, or is a directory", pathname);                         
  return 0;
}
 return 0;
}

bool matches(char* text, size_t lentext, char* regex, size_t lenreg){
  //whle we have a regex to search for                                                                                                                              \
                                                                                                                                                                     
  if (lenreg != 0){
    //switch on girst character                                                                                                                                      
    switch(regex[0]){
    case '.': ;
      //for text processing -> make sure we are not at the end of the buffer, there should be some text to process                                                   
      bool textmatch = (lentext != 0);
      //if the first char is a ?                                                                                                                                     
      if(lenreg>=2 && regex[1]=='?'){
        //ignore the ? mark                                                                                                                                                                                                                                                 
        return (textmatch && matches(text+1, lentext-1, regex+2, lenreg-2))||matches(text, lentext, regex+2, lenreg-2);

      }else if(lenreg>=2 && regex[1]=='*'){
        //first character after . is a *                                                                                                                             
        return matches(text, lentext, regex+2, lenreg-2)||(textmatch && matches(text+1, lentext-1, regex, lenreg));
      }
      else{
        //move to the second character                                                                                                                              \
                                                                                                                                                                     
        return matches(text+1,lentext-1, regex+1, lenreg-1);
      }
      break;

    case '(': ;
      //earliest the closed parenthesis can be                                                                                                                       
      char *end=regex+1;      //end points to end of parenthesis                                                                                                     
      while(end[0] != ')') {
        end++;
      }

      bool textmatches = true;
      //length = everything within the parenthesis                                                                                                                   
      int length= (int)(end - (regex+1));

      //checks if we need to option to consume characters in () -> (ef)?g, g                                                                                         
      if(lentext < length) {
        for(int i = 0; i<length; i++) {
          if(text[i] != regex[i+1]){
            textmatches = false;
            break;
          }
        }
      }
      //length + parenthesis + extra characters                                                                                                                      
      if(lenreg>=length+3 && end[1]=='?'){
        //ignore the ? mark                                                                                                                                          
        //first we consume the character(ab case) and then b case                                                                                                    
        return (textmatches)&& matches(text+length, lentext-length, end+2, lenreg-(length+3))||matches(text, lentext, end+2, lenreg-(length+3));

      }else if(lenreg>=length+3 && end[1]=='*'){
        //if string is ef is f, case 2 is eeeeef                                                                                                                     
      return matches(text, lentext, end+2, lenreg-(length+3)) ||(textmatches)&&matches(text+length, lentext-length, regex, lenreg);

      }
      else{
        //no special char, if we can match the text make sure everything else matches                                                                                
        return (textmatches) && matches(text+length,lentext-length, end+2, lenreg-(length+2));
      }

      break;

    default: ;
          //first character is alphanumeric                                                                                                                              
      bool match = (lentext !=0) && (text[0]==regex[0]);
      //for text processing purposes, make sure there is text to compare to                                                                                          
      if(lentext == 0) {
        return false;
      }else if(lenreg>=2 && regex[1]=='?'){
        //ignore the ? mark                                                                                                                                          
        //first we consume the character(ab case) and then b case                                                                                                    
        return (match)&& matches(text+1, lentext-1, regex+2, lenreg-2)||matches(text, lentext, regex+2, lenreg-2);

      }else if(lenreg>=2 && regex[1]=='*'){
        //if string is ef is f, case 2 is eeeeef                                                                                                                     
        return matches(text, lentext, regex+2, lenreg-2) || ((match)&& matches(text+1, lentext-1, regex, lenreg));
      }
      else{
        //no special character, so we check to make sure the first character is the correct character and the rest of string matches                             
                                                                                                                                                                   
	    return (match) && matches(text+1,lentext-1, regex+1, lenreg-1);
      }

      break;
    }

    return true;
  }
}
