#include "tt.h"
#include <unistd.h>
#include <stdio.h>

void test_d(void){
  tt_d_t* d = NULL;

  d = tt_d_new(0,0);
  tt_d_start(d);
  tt_d_stop(d);
  tt_d_free(d);
}

void test_t1(void){
  tt_t_t* t = NULL;

  t = tt_t_new("t");
  tt_t_start_run(t);
  tt_t_stop_run(t);
  tt_t_free(t);
}

void test_p1(void){
  tt_p_t* p = NULL;
  tt_t_t* t = NULL;
  
  p = tt_p_new("p");
  t = tt_t_new("t");
  tt_p_add_task(p, t);
  tt_p_free(p);
}


void test_db1(void){
  tt_db_t* db = NULL;
  tt_p_t* p = NULL;
  tt_t_t* t = NULL;

  db = tt_db_new();
  
  p = tt_p_new("p");
  t = tt_t_new("t");
  tt_p_add_task(p, t);
  tt_db_add_project(db, p);
  
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

  tt_d_start(d);
  tt_t_add_run(t, d);
  tt_p_add_task(p, t);
  tt_db_add_project(db, p);

  {
    unsigned int rest = 0;
    if(0 != (rest = sleep(5)))
      fprintf(stderr, "sleep interrupted %ds early\n", rest);
  }
  tt_d_stop(d);

  /* TODO: somewhere between here and tt_t_add_task 
     a stupid mistake screws up the tasklist array in p. 
     The problem seems to start in start_run, after it screws t, 
     it seems to spread to the tasklist. */
  tt_t_start_run(t);
  
  {
    unsigned int rest = 0;
    if(0 != (rest = sleep(5)))
      fprintf(stderr, "sleep interrupted %ds early\n", rest);
  }
  tt_t_stop_run(t);

  t = tt_t_new("aufgabe2");
  tt_p_add_task(p, t);
  tt_t_start_run(t);
  
  {
    unsigned int rest = 0;
    if(0 != (rest = sleep(5)))
      fprintf(stderr, "sleep interrupted %ds early\n", rest);
  }
  tt_t_stop_run(t);

  tt_db_free(db);

  test_d();
  test_t1();
  test_p1();
  test_db1();
  
  return 0;
}
