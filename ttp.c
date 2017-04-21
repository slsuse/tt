#include "tt.h"
#include "ttf.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

const char* default_file = "tt.csv";

typedef enum args{
  help,
  ls,
  lsr,
  t,
  add,
  rm
} args_t;

args_t args(int argc, char** argv){
  if(argc == 1)
    return help;
  
  if(0 == strcmp("-h", argv[1]))
    return help;
  
  if(0 == strcmp("--help", argv[1]))
    return help;
  
  if(0 == strcmp("h", argv[1]))
    return help;
  
  if(0 == strcmp("help", argv[1]))
    return help;
  
  if(0 == strcmp("ls", argv[1])){
    /*   if(argc == 2)
         return help;*/
    return ls;
  }
  
  if(0 == strcmp("lsr", argv[1]))
    return lsr;
  
  if(0 == strcmp("t", argv[1]))
    return t; /*FIXME: printing tasks - give CLI some thought. */
  
  if(0 == strcmp("add", argv[1]))
    return add;
  
  if(0 == strcmp("rm", argv[1]))
    return rm;
  
  return help;
}

void pr_help(const char* name){
  /* TODO: At this point GNU gengetopt might become worth the hassle. */
  printf("%s [<command>]\n", name);
  printf("where <command> is:\n");

  printf("[-h|--help|h|help]                  - print this screen.\n");

  printf(" -p ls [<project1> <project2> ...]\n");
  printf("                                    - list registered projects.\n");
  printf(" -p lsr [<project1> <project2> ...] - list recursively, i.e. all data down to clocked times.\n");
  printf(" -p add <project1> [<project2> <project3> ...]\n");
  printf("                                    - register a project.\n");
  printf(" -p rm  <project>  [<project2> <project3> ...]\n");
  printf("                                    - rm a project.\n");
  
  /*FIXME: printing tasks - give CLI some thought. */
  
  printf(" -t ls <project> <task> [<task2> <task3> ...]\n");
  printf("                                    - print task times.\n");


  printf(" -t add <project> <task> [<task2> <task3> ...]\n");
  printf("                                    - register tasks in a project.\n");
 
            
}

void ls_r(int argc, char** argv, const char* fname){
  tt_db_t* db = NULL;
  
  db = tt_db_new();
  tt_db_read_file(db, fname);

  if(argc == 2)
    tt_db_lsR(db, stdout);
  else{
    for( int i = 2; i < argc; i++){
      tt_p_t* p = tt_db_find_project( db, argv[i]);
      if(p)
        tt_p_lsR( p, stdout);
      else
        fprintf(stderr, "project '%s' not found.\n", argv[i]);
    }
  }
  tt_db_free(db);
}

/* WARNING: see man 3 getenv! */
/* return the filename we are working with,
   currently only queries environment,
   a config file might be added at a later stage. */

const char* get_db_fname(void){
  char* ret = NULL;

  if(NULL !=(ret = getenv("TT_DB")))
    return ret;
  
  return default_file;
}

void plain_ls(int argc, char** argv){
  tt_db_t* db = NULL;

  db = tt_db_new();
  tt_db_read_file(db, get_db_fname());

  if(argc == 2){
    for(int i = 0; i < db->nprojects; i++){
      printf("%s\n", db->projects[i]->name);
    }
  }
  else{
    for( int i = 2; i < argc; i++){
      tt_p_t* p = tt_db_find_project( db,argv[i]);
      if(p)
        tt_p_ls( p, stdout);
      else
        fprintf(stderr, "project '%s' not found.\n", argv[i]);
    }
  }
  tt_db_free(db);
}


int main(int argc, char** argv){
    
  fprintf(stderr, "%s: %d\n", "argc", argc);

  switch(args(argc, argv)){
  case help:
    pr_help(argv[0]);
    break;
  case ls:
    plain_ls(argc, argv);
    break;
  case lsr:
    ls_r( argc, argv, get_db_fname());
    break;
  case add:
    break;
  case rm:
    break;
  default:
    pr_help(argv[0]);
    break;
  }

  return 0;
}
