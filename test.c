#define POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "tt.h"
#include "ttf.h"

void test_strdelim(const char* str){
  char* s = NULL;
  char* start = NULL;
  char* end = NULL;
  int done = 0;
  int cnt = 0;

  s = strdup(str);
  start = s;

  while(!done){
    end = tt_strdelim(start, &cnt, ',',  '\\');
    if(*end == '\0'){
      printf("%s:%d - |%d.) %s|\n", __FILE__, __LINE__, cnt, start);
      done = 1;
    }
    else{
      *end = '\0';
      printf("%s:%d - |%d.) %s|\n", __FILE__, __LINE__, cnt, start);

      if(cnt >0)
        if( '\0' == end[cnt]) /*check for end of buffer, avoid offbyone */
          done = 1;

      start = end+cnt; /* original delim position */
      ++start;
    }      
    
    
  }
}


void test_d(void){
  tt_d_t* d = NULL;

  printf("\ntest_d:\n");
  d = tt_d_new(0,0);
  tt_d_start(d);
  tt_d_stop(d);
  tt_d_free(d);
}

void test_t1(void){
  tt_t_t* t = NULL;

  printf("\ntest_t1:\n");
  t = tt_t_new("t");
  tt_t_start_run(t);
  tt_t_stop_run(t);
  tt_t_ls(t, stdout);

 tt_t_free(t);
}

void test_p1(void){
  tt_p_t* p = NULL;
  tt_t_t* t = NULL;
  char buf[3] = "00";
  
  printf("\ntest_p1:\n");
  p = tt_p_new("p");
  t = tt_t_new("t");
  tt_t_start_run(t);
  tt_p_add_task(p, t);
  printf("\nls:\n");
  tt_p_ls(p, stdout);

  {
    unsigned int rest = 0;
    if(0 != (rest = sleep(5)))
      fprintf(stderr, "sleep interrupted %ds early\n", rest);
  }
  
  for(int i = 0; i < 10; i++){
    snprintf(buf, 3, "%d", i);
    tt_t_t* t2 = tt_t_new(buf);
    tt_p_add_task(p,t2);
    tt_t_start_run(t2);
    
    {
      unsigned int rest = 0;
      if(0 != (rest = sleep(5)))
        fprintf(stderr, "sleep interrupted %ds early\n", rest);
    }
    tt_t_stop_run(t2);
  }
  /* tt_t_stop_run(t);*/
  printf("\nlsR:\n");
  tt_p_lsR(p, stdout);
  tt_p_free(p);
}


void test_db1(void){
  tt_db_t* db = NULL;
  tt_p_t* p = NULL;
  tt_t_t* t = NULL;

  printf("\ntest_db1:\n");
  db = tt_db_new();
  
  p = tt_p_new("p");
  t = tt_t_new("t");
  tt_p_add_task(p, t);
  tt_db_add_project(db, p);
  
  tt_db_free(db);
}

tt_p_t* db_find_project( tt_db_t* db, const char* pname){
  tt_p_t* p = NULL;
  for( int i = 0; i < db->nprojects; i++){
    p = db->projects[i];
    if( 0 == strcmp( p->name, pname)){
      return p;
    }
  }
  return p;
}

tt_t_t* project_find_task( tt_p_t* p, const char* tname){
  tt_t_t* t = NULL;
  for( int i = 0; i < p->ntasks; i++){
    t = p->tasklist[i];
    if( 0 == strcmp( t->name, tname)){
      return t;
    }
  }
  return t;
}

tt_t_t* db_find_task( tt_db_t* db, const char* pname, const char* tname){
  tt_p_t* p = NULL;

  if( NULL != (p = db_find_project(db, pname))){
    return project_find_task(p, tname);
  }
  return NULL;
}


/* stuff that might turn into code someday.
   looks like I'll need a full CRUD interface after all.
*/
int db_add_run( tt_db_t* db, unsigned int pid, unsigned int tid, tt_d_t* run){
  tt_p_t* p = NULL;
  tt_t_t* t = NULL;

  for( int i = 0; i < db->nprojects; i++){
    if( db->projects[i]->id == pid){
      p = db->projects[i];
      for( int j = 0; j < p->ntasks; j++){

	if( p->tasklist[i]->id == tid){
	  t = p->tasklist[i];
	  tt_t_add_run(t, run);
	  return 0;
	}
	/*TODO:
	  create new task and project? Without names? Really?
	*/
	else{
	  return -1; /* not found */
	}
      }
    }
    else{
      return -2; /* not found */
    }
  }
  /* the fuck? */
  return -3;
}

void test_db2(void){
  tt_db_t* db = NULL;
  /*  tt_db_t* res = NULL;*/
  tt_t_t* t = NULL;
  tt_p_t* p = NULL;
  tt_d_t* run = NULL;

  printf("\ntest_db2:\n");
  db = tt_db_new();
  p = tt_p_new("projectX");
  tt_p_setid(p, 1);
  
  t = tt_t_new("taskX");
  tt_t_setid(t, 1);

  run = tt_d_new( 0, 0);

  tt_d_start(run);
  sleep(5);
  tt_d_stop(run);

  tt_db_add_project(db, p);
  tt_p_add_task(p, t);
  /*tt_t_add_run(t, run);*/

  /*res = db_find(db, pname, tname);*/
  db_add_run(db, 1, 1, run);
}

int main(){
  /* tt_db_t* db = NULL;
  tt_p_t* p = NULL;
  tt_t_t* t = NULL;
  tt_d_t* d = NULL;

  db = tt_db_new();
  p = tt_p_new("testprojekt");
  t = tt_t_new("testaufgabe");
  d = tt_d_new( 0, 0);

  if( 0 == tt_d_start(d)){
    fprintf(stderr, "%s:%d: tt_d_start(d) failed\n", __FILE__, __LINE__);
  }
  if( 0 > tt_t_add_run(t, d))
        fprintf(stderr, "%s:%d: tt_t_add_run(t,d) failed\n", __FILE__, __LINE__);
  if( 0 > tt_p_add_task(p, t))
        fprintf(stderr, "%s:%d: tt_p_add_task(p,t) failed\n", __FILE__, __LINE__);
  if( 0 > tt_db_add_project(db, p))
        fprintf(stderr, "%s:%d: tt_db_add_project(db,p) failed\n", __FILE__, __LINE__);

  {
    unsigned int rest = 0;
    if(0 != (rest = sleep(5)))
      fprintf(stderr, "sleep interrupted %ds early\n", rest);
  }
  tt_d_stop(d);
  tt_t_start_run(t);
  
  {
    unsigned int rest = 0;
    if(0 != (rest = sleep(5)))
      fprintf(stderr, "sleep interrupted %ds early\n", rest);
  }
  tt_t_stop_run(t);
  tt_t_ls(t, stdout);

  
  t = tt_t_new("aufgabe2");
  tt_p_add_task(p, t);
  tt_t_start_run(t);
  
  {
    unsigned int rest = 0;
    if(0 != (rest = sleep(5)))
      fprintf(stderr, "sleep interrupted %ds early\n", rest);
  }
  tt_t_stop_run(t);
  tt_t_ls(t, stdout);

  tt_p_ls(p, stdout);

  printf("\nlsR:\n");
  tt_p_lsR(p, stdout);
  
  tt_db_free(db);

  test_d();
  test_t1();
  test_p1();
  test_db1();
  */
  {
    const char* s = "a,b,cde,f,g,hijkl,m,n";
    printf("%s:%d %s\n", __FILE__, __LINE__, s);
    test_strdelim(s);
  }
  {
    const char* s = "a\\,b\\,cde,f,g,hijkl,m\\,n";
    printf("%s:%d %s\n", __FILE__, __LINE__, s);
    test_strdelim(s);
  }
  return 0;
}
