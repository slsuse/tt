#include "tt.h"
#include "ttf.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

const char* default_file = "tt.csv";

typedef enum args{
  help,
  p_ls,
  p_lsr,
  p_add,
  p_rm,
  p_sum,
  t_ls,
  t_add,
  t_rm,
  t_start,
  t_stop,
  // t_stop_run,
  t_sum
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

      if(0 == strcmp("sum", argv[2])){
        return p_sum;
      }
  

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
    
      if(0 == strcmp("sum", argv[2])){
        return t_sum;
      }
    
      if(0 == strcmp("ls", argv[2])){
        /*   if(argc == 2)
             return help;*/
        return t_ls;
      }
      if(0 == strcmp("add", argv[2]))
        return t_add;
      
      if(0 == strcmp("rm", argv[2]))
        return t_rm;
      
      if(0 == strcmp("start", argv[2]))
        return t_start;

      if(0 == strcmp("b", argv[2]))
        return t_start;

      if(0 == strcmp("begin", argv[2]))
        return t_start;

      if(0 == strcmp("stop", argv[2]))
        return t_stop;

      if(0 == strcmp("e", argv[2]))
        return t_stop;

      if(0 == strcmp("end", argv[2]))
        return t_stop;

    }
  }
  return help;
}

void pr_help(const char* name){
  /* TODO: At this point GNU gengetopt might become worth the hassle. */
  printf("\n%s [<command>] ", name);
  printf("where <command> is:\n");

  printf("\n [-h|--help|h|help]                  - print this screen.\n");

  printf("\n -p ls [<project1> <project2> ...]\n");
  printf("                                    - list registered projects.\n");
  printf("\n -p lsr [<project1> <project2> ...] - list recursively, i.e. all data down to clocked times.\n");
  printf("\n -p add <project1> [<project2> <project3> ...]\n");
  printf("                                    - register a project.\n");
  printf("\n -p rm  <project>  [<project2> <project3> ...]\n");
  printf("                                    - rm a project.\n");
  
  /*FIXME: printing tasks - give CLI some thought. */
  
  printf("\n -t ls <project> <task> [<task2> <task3> ...]\n");
  printf("                                    - print task times.\n");


  printf("\n -t add <project> <task> [<task2> <task3> ...]\n");
  printf("                                    - register tasks in a project.\n");
 

  printf("\n -t rm <project> <task> [<task2> <task3> ...]\n");
  printf("                                    - remove tasks from a project.\n");
 

  printf("\n -t start <project> <task>\n");
  printf("                                    - start a task in a project.\n");
 
 
  printf("\n -t stop <project> <task>\n");
  printf("                                    - stop tasks in a project.\n");
 
  printf("\n -t stop <project> <task> [<index1> <index2> ...]\n");
  printf("                                    - stop task runs with given indices in a project.\n");
 


}

void do_p_lsr(int argc, char** argv, tt_db_t* db){
  if(argc == 3){
    tt_db_lsR(db, stdout);
  }
  else{
    for( int i = 3; i < argc; i++){
      
      tt_p_t* p = tt_db_find_project( db, argv[i]);
      if(p){
        printf("%s:\n", p->name);
        tt_p_lsr( p, stdout, (char) 0);
      }
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
      if(p){
        printf("%s:\n", p->name);
        tt_p_ls( p, stdout, (char) 0);
      }
      else
        fprintf(stderr, "project '%s' not found.\n", argv[i]);
    }
  }
}


void do_t_ls(int argc, char** argv, tt_db_t* db){
  if(argc>3){
    /* ttp -t ls projectname taskname taskname ... */
    tt_p_t* p = tt_db_find_project( db,argv[3]);
    if(p){
      printf("%s:\n", p->name);

      if(argc == 4){
        tt_p_lsr(p, stdout, (char) 0);
      }
      else{
        for( int i = 4; i < argc; i++){
          tt_t_t* t = tt_p_find_task(p,argv[i]);
          if(t){
            fprintf(stdout, "  %s\n", t->name);
            tt_t_ls( t, stdout, (char) 0);
          }
          else{
            fprintf(stderr, "%s no such task\n", argv[i]);
          }
        }
      }
    }
    else
      fprintf(stderr, "project '%s' not found.\n", argv[3]);
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
  for( int i = 3; i < argc; i++){
    p = tt_db_rm_project(db, argv[i]);
    tt_p_free(p);
  }

}

/*TODO error handling */
void do_add_t(int argc, char** argv, tt_db_t* db){
  tt_p_t* p = NULL;
  tt_t_t* t = NULL;
  
  if(argc < 5)
    return;

  if( NULL == (p = tt_db_find_project(db,argv[3]))){
    p = tt_p_new(argv[3]);
    tt_db_add_project(db,p);
  }
  if(NULL == (t = tt_p_find_task(p,argv[4]))){
    t = tt_t_new(argv[4]);
    tt_p_add_task(p,t);
  }
}


void do_rm_t( int argc, char** argv, tt_db_t* db){
  tt_p_t* p = NULL;
 
  if(argc < 5)
    return;
  
  p = tt_db_find_project(db,argv[3]);
  if(p)
    tt_p_rm_task(p,argv[4]);
  
}

void do_start_t(int argc, char** argv, tt_db_t* db){
  tt_t_t* t = NULL;
  tt_p_t* p = NULL;
  int ts = -100;
  
  if(argc < 5){
    pr_help(argv[0]);
    return;
  }

  t = tt_db_find_task(db,argv[3],argv[4]);
  if(NULL == t){
    t = tt_t_new(argv[4]);

    p = tt_db_find_project(db,argv[3]);
    if(NULL == p){
      p = tt_p_new(argv[3]);
      tt_db_add_project(db,p);
    }
    tt_p_add_task(p,t);
  }
  if(0 > (ts = tt_t_start_run(t)))
    fprintf(stderr, "%s:%d error setting timestamp: %d\n", __FILE__, __LINE__, ts);
}


void do_stop_run(int argc, char** argv, tt_db_t* db){
  tt_t_t* t =NULL;
  int ts = -100;
   
  if(argc < 6){
    pr_help(argv[0]);
    return;
  }
  t = tt_db_find_task(db,argv[3],argv[4]);
  if(t){
    unsigned int index = t->nruns - 1;
    unsigned int curarg = 5;
    char* tail = NULL;

    for(curarg=5; curarg < argc; ++curarg){
      errno = 0;
      
      index = strtoul(argv[curarg], &tail, 0);
      if(errno){
        perror("ttui.c/do_stop_run()");
        return;
      }
      if(0 > (ts = tt_t_stop_this_run(t, index)))
        fprintf(stderr, "%s:%d error setting timestamp: %d\n", __FILE__, __LINE__, ts);
    }
  }
}

void do_stop_t(int argc, char** argv, tt_db_t* db){
  tt_t_t* t =NULL;
  int ts = -100;
   
  if(argc < 5){
    pr_help(argv[0]);
    return;
  }
  if(argc > 6)
    do_stop_run(argc, argv, db);
  else{
    t = tt_db_find_task(db,argv[3],argv[4]);
    if(t){
      if(0 > (ts = tt_t_stop_run(t)))
        fprintf(stderr, "%s:%d error setting timestamp: %d\n", __FILE__, __LINE__, ts);
    }
  }
}


void do_p_sum(int argc, char** argv, tt_db_t* db){
  tt_p_t* p = NULL;
  
  if(argc == 3){
    
    for(int i = 0; i < db->nprojects; i++){
      p = db->projects[i];
      printf("%s: ", p->name);
      tt_p_prsum(p,stdout);
    }
  }
  else{
    for( int i = 3; i < argc; i++){
      tt_p_t* p = tt_db_find_project( db,argv[i]);
      if(p){
        printf("%s: ", p->name);
        tt_p_prsum(p,stdout);
      }
      else
        fprintf(stderr, "project '%s' not found.\n", argv[i]);
    }
  }
  
}

void do_t_sum(int argc, char** argv, tt_db_t* db){
  if(argc>3){
    /* ttp -t ls projectname taskname taskname ... */
    tt_p_t* p = tt_db_find_project( db,argv[3]);
    if(p){
      printf("%s:\n", p->name);
      
      if(argc == 4){ /*tt -p sum projectname*/
        for( int i = 0; i < p->ntasks; i++){
          tt_t_t* t =p->tasklist[i]; 
          fprintf(stdout, "  %s: ", t->name);
          tt_t_prsum(t,stdout);
        }
      }
      else{
        for( int i = 4; i < argc; i++){
          tt_t_t* t = tt_p_find_task(p,argv[i]);
          if(t){
            fprintf(stdout, "  %s: ", t->name);
            tt_t_prsum( t, stdout);
          }
          else{
            fprintf(stderr, "%s no such task\n", argv[i]);
          }
        }
      }
    }
    else
      fprintf(stderr, "project '%s' not found.\n", argv[3]);
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
  case p_sum:
    do_p_sum(argc, argv, db);
    break;
  case t_sum:
    do_t_sum(argc, argv, db);
    break;
  case t_ls:
    do_t_ls(argc, argv, db);
    break;
  case t_add:
    do_add_t(argc, argv, db);
    break;
  case t_rm:
    do_rm_t(argc, argv, db);
    break;
  case t_start:
    do_start_t(argc, argv, db);
    break;
  case t_stop:
    do_stop_t(argc, argv, db);
    break;
    /* case t_stop_run:
    do_stop_run(argc, argv, db);
    break;
    */
  default:
    pr_help(argv[0]);
    break;
  }

  
  tt_db_write_file(db);
  tt_db_free(db);
  return 0;
}
