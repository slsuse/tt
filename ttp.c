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
    if(argc == 2)
      return help;
    return ls;
  }
  
  if(0 == strcmp("lsr", argv[1]))
    return lsr;
  
  if(0 == strcmp("add", argv[1]))
    return add;
  
  if(0 == strcmp("rm", argv[1]))
    return rm;
  
  return help;
}

void pr_help(const char* name){
  printf("%s [command]\n", name);
  printf("command is:\n");
  printf("\t[-h|--help|h|help] - print this screen.\n");
  printf("\tls - list registered projects.\n");
  printf("\tlsr - list recursively, i.e. all data down to clocked times.\n");
  printf("\tadd - register a project.\n");
  printf("\trm - rm a project.\n");
            
}

void lsr_file(const char* fname){
  tt_db_t* db = NULL;
  db = tt_db_new();
  tt_db_read_file(db, fname);
  tt_db_lsR(db, stdout);
  tt_db_free(db);
}

/* WARNING: see man 3 getenv! */
/* return the filename we are working with,
   currently only queries environment,
   a config file might be added at a later stage. */

char* get_db_fname(void){
  char* ret = NULL;
  if(NULL ==(ret = getenv("TT_DB")))
    ret = default_file;
  return ret;
}

void ls_all_p(int argc, char** argv){
  tt_db_t* db = NULL;
  db = tt_db_new();
  tt_db_read_file(db, get_db_fname());
  
  for( int i = 2; i < argc; i++){
    tt_p_t* p = tt_db_find_project( db,argv[i]);
    tt_p_ls( p, stdout);
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
    ls_all_p(argc, argv);
    break;
  case lsr:
    lsr_file(get_db_fname());
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
