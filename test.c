#include "tt.h"
#include <unistd.h>
#include <stdio.h>

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
  char buf[3] = "00";
  
  printf("\ntest_db1:\n");
  db = tt_db_new();
  
  p = tt_p_new("p");
  t = tt_t_new("t");
  tt_t_start_run(t);

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
  

  tt_p_add_task(p, t);
  tt_db_add_project(db, p);
  tt_db_lsR(db, stdout);
  
  tt_db_free(db);
}


int main(){
  tt_db_t* db = NULL;
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
  
  return 0;
}
