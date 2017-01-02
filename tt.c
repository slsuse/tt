#include "tt.h"

tt_d_t* tt_d_new(time_t start, time_t finished){
  tt_d_t* ret = NULL;

  if( NULL == (ret = malloc( sizeof(tt_d_t))))
    return NULL;

  ret->start = start;
  ret->finished = finished;
  return ret;
}


void tt_d_free(tt_d_t* d){
  if(NULL != d)
    free(d);
}


time_t tt_d_start(tt_d_t* d){
  if(NULL == d)
    return 0;
  
  return time(&(d->start));
}

time_t tt_d_stop(tt_d_t* d){
  if(NULL == d)
    return 0;
  
  return time(&(d->finished));
}

tt_t_t* tt_t_new( char* name){
  tt_t_t* ret = NULL;
  
  if(NULL == name)
    return NULL;

  if( NULL == (ret = malloc( sizeof(tt_t_t))))
    return NULL;

  if( NULL == (ret->name =  strdup(name))){
    free(ret);
    return NULL;
  }
  ret->runs = NULL;
  ret->nruns = 0;
  ret->len = 0;
  return ret;
}

/* recursively free a tt_task_struct.
 */
void tt_t_free(tt_t_t* task){
  if(NULL == task)
    return;

  if(task->runs)
    free(task->runs);
  if(task->name)
    free(task->name);
  free(task);
}

/* add a duration to a task
   return index of duration or below 0 on error
*/
int tt_t_add_run(tt_t_t* task, tt_d_t* duration){
  unsigned newlen = 0;

  errno = 0;
  if(!task || !duration)
    return -1;
  
  if(0 == task->nruns){
    task->runs = duration;
    task->len = 1;
    task->nruns = 1;
    return 0;
  }
  if( task->len == task->nruns){
    newlen = 2* (task->len);
    if(!(task->runs = realloc( newlen * sizeof(tt_d_t))))
      return -2;
    task->len = newlen;
  }
  task->runs+(task->nruns) = duration;
  ++(task->nruns);
  return task->nruns - 1;
}

/* start a run on a given task
   return below 0 on error
   or the start time else.
*/
int tt_t_start_run(tt_t_t* task){
  tt_d_t* d = NULL;

  if(!task)
    return -1;
  if(!(task->name))
    return -2;

  if(!(d = tt_d_new(0, 0)))
    return -3;

  if( !tt_d_start(d)){
    tt_d_free(d);
    return -4;
  }
  if(0 > tt_t_add_run(d)){
    tt_d_free(d);
    return -5;
  }
  return d->start;
}
/* finish the last (==current) run on a given task
   return below 0 on error
   or the finish time else.
*/
int tt_t_stop_run(tt_t_t* task){
  if(!task)
    return -2;

  if(!(task->name))
    return -3;

  if(0 == task->len)
    return -4;

  return  (task->runs + (task->nruns - 1))->finished = time(NULL);
}


/* malloc a new tt_project_struct
   expect a 0-terminated string which will be copied. */
tt_p_t* tt_p_new(char* name){
  tt_p_t* ret = NULL;
  
  if(NULL == name)
    return NULL;

  if( NULL == (ret = malloc( sizeof(tt_t_t))))
    return NULL;

  if( NULL == (ret->name =  strdup(name))){
    free(ret);
    return NULL;
  }
  ret->tasklist = NULL;
  ret->ntasks = 0;
  ret->len = 0;
  return ret;
}  

/* recursively free a tt_project_struct.
 */
void tt_p_free(tt_p_t* p){
  if(NULL == p)
    return;

  free(p->name);
  if(NULL != p->tasklist)
    free(p->tasklist);
  free(p);
}

/* add a task to a project 
   return index of task or below 0 on error
*/
int tt_p_add_task(tt_p_t* project, tt_t_t* task){
}

