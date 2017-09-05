#include "tt.h"
#include "ttf.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

const char* default_file = "tt.csv";

typedef enum args{
  help,
  p_ls,
  p_lsr,
  p_add,
  p_rm,
  t_ls,
  t_add,
  t_rm
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

  if(argc > 2){
    if(0 == strcmp("-p", argv[1])){
      
      if(0 == strcmp("ls", argv[2])){
        /*   if(argc == 2)
             return help;*/
        return p_ls;
      }
  
      if(0 == strcmp("lsr", argv[2]))
        return p_lsr;
  
      if(0 == strcmp("add", argv[2])){
        if(argc < 4)
          return help;
        return p_add;
      }
      
      if(0 == strcmp("rm", argv[2]))
        return p_rm;
      
    }
    else if(0 == strcmp("-t", argv[1])){
    
      if(0 == strcmp("ls", argv[2])){
        /*   if(argc == 2)
             return help;*/
        return t_ls;
      }
      if(0 == strcmp("add", argv[2]))
        return t_add;
      
      if(0 == strcmp("rm", argv[2]))
        return t_rm;
    }
  }
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
 

  printf(" -t rm <project> <task> [<task2> <task3> ...]\n");
  printf("                                    - remove tasks from a project.\n");
 
            
}

void do_p_lsr(int argc, char** argv, tt_db_t* db){
  if(argc == 3)
    tt_db_lsR(db, stdout);
  else{
    for( int i = 3; i < argc; i++){
      
      tt_p_t* p = tt_db_find_project( db, argv[i]);
      if(p)
        tt_p_lsR( p, stdout);
      else
        fprintf(stderr, "project '%s' not found.\n", argv[i]);
    }
  }
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

void do_p_ls(int argc, char** argv, tt_db_t* db){
  if(argc == 3){
    
    for(int i = 0; i < db->nprojects; i++){
      printf("%s\n", db->projects[i]->name);
    }
  }
  else{
    for( int i = 3; i < argc; i++){
      tt_p_t* p = tt_db_find_project( db,argv[i]);
      if(p)
        tt_p_ls( p, stdout);
      else
        fprintf(stderr, "project '%s' not found.\n", argv[i]);
    }
  }
}


void do_add_p(int argc, char** argv, tt_db_t* db){

  tt_p_t* p = NULL;
  
  /*TODO: error handling */
  for( int i = 3; i < argc; i++){
    if(NULL == tt_db_find_project(db,argv[i])){
      p = tt_p_new(argv[i]);
      tt_db_add_project(db,p);
    }
  }
  
}

void do_rm_p(int argc, char** argv, tt_db_t* db){
  tt_p_t* p = NULL;
  
#ifdef DEBUG
  fprintf(stderr,"%s:%d:   remove pr '%s'\n",__FILE__, __LINE__, argv[3]);
#endif

  for( int i = 3; i < argc; i++){

#ifdef DEBUG
    fprintf(stderr,"%s:%d:   tt_db_rm_project(db '%s')\n",__FILE__, __LINE__, argv[i]);
#endif

    p = tt_db_rm_project(db, argv[i]);

#ifdef DEBUG
    if(NULL == p)
      fprintf(stderr,"%s:%d:   pr '%s' not found!\n",__FILE__, __LINE__, argv[i]);
#endif

    tt_p_free(p);
  }

}



int main(int argc, char** argv){
  const char* dbfile = get_db_fname();
  tt_db_t* db = NULL;
  
  db = tt_db_new();
  tt_db_read_file(db, dbfile);
  

  switch(args(argc, argv)){
  case help:
    pr_help(argv[0]);
    break;
  case p_ls:
    do_p_ls(argc, argv, db);
    break;
  case p_lsr:
    do_p_lsr( argc, argv, db);
    break;
  case p_add:
    do_add_p(argc, argv, db);
    break;
  case p_rm:
    do_rm_p(argc,argv, db);
    break;
  case t_ls:
  case t_add:
  case t_rm:
    fprintf(stderr, "%s:%d Not yet implemented.\n", __FILE__, __LINE__);
    pr_help(argv[0]);
    break;
  default:
    pr_help(argv[0]);
    break;
  }

  
  tt_db_write_file(db);
  tt_db_free(db);
  return 0;
}
