#define _XOPEN_SOURCE 500 

#include <string.h>
#include <stdlib.h>
#include <errno.h>
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

tt_t_t* tt_t_new( const char* name){
  tt_t_t* ret = NULL;
  
  if(NULL == name)
    return NULL;

  if( NULL == (ret = malloc( sizeof(tt_t_t))))
    return NULL;

  if( NULL == (ret->name = strdup(name))){
    free(ret);
    return NULL;
  }
  ret->id = 0;
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

  for(int i = 0; i < task->nruns; i++)
    tt_d_free(task->runs[i]);
  
  if(task->name)
    free(task->name);
  free(task->runs);
  free(task);
}

unsigned int tt_t_setid( tt_t_t* t, unsigned int id){
  if(NULL == t)
    return 0;
  t->id = id;
  return id;
}


unsigned int tt_t_getid( tt_t_t* t){
  if(NULL == t)
    return 0;
 return t->id;
}

/* add a duration to a task
   return index of duration or below 0 on error
*/
int tt_t_add_run(tt_t_t* task, tt_d_t* duration){
  unsigned newlen = 0;
  tt_d_t** tmp = NULL;

  errno = 0;
  if(!task || !duration)
    return -1;
  
  if(0 == task->nruns){
    task->runs = malloc(sizeof(tt_d_t*));
    *(task->runs) = duration;
    task->len = 1;
    task->nruns = 1;
    return 0;
  }
  if( task->len == task->nruns){
    newlen = 2* (task->len);
    if(NULL == (tmp = realloc( task->runs, sizeof(tt_d_t*) * newlen)))
      return -2;
    task->runs = tmp;
    task->len = newlen;
  }
  task->runs[task->nruns] = duration;
  ++(task->nruns);
  return task->nruns - 1;
}

int tt_t_find_run(tt_t_t* task, tt_d_t* duration){
  if(!task || !duration)
    return -1;
  
  for(int i = 0; i < task->nruns; i++){
    if( 0 < tt_d_same_intervall( task->runs[i], duration))
      return i; 
  }
  
  return -2;
}

int tt_d_same_intervall( tt_d_t* d1,  tt_d_t* d2){
  if( NULL == d1 || NULL == d2){
    fprintf(stderr,"%s:%d:   NULL args!\n",__FILE__, __LINE__);
    return -1;
  }
  return ((d1->start == d2->start) && (d1->finished == d2->finished));
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
  if(0 > tt_t_add_run(task, d)){
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

  return  task->runs[task->nruns - 1]->finished = time(NULL);
}

/* list the runs of a given task */

int tt_t_ls(tt_t_t* t, FILE* stream){
  char buf1[26] = "N/A";
  char buf2[26] = "N/A";
  char* tmp = NULL;

  if( NULL == t)
    return -1;
  if(NULL == stream)
    return -2;
  
  if(0 == t->nruns){
    /*    fprintf(stderr, "%s:%d - no runs\n", __FILE__, __LINE__ );*/
    return 0;
  }
  for( int i = 0; i < t->nruns; i++){
    if( 0 != t->runs[i]->start){
      if( NULL == ctime_r(&(t->runs[i]->start), buf1))
        return -3;
      tmp = strchr(buf1, '\n');
      *tmp = '\0';
      
      if( 0 != t->runs[i]->finished){
        if( NULL == ctime_r(&(t->runs[i]->finished), buf2))
        return -4;
        tmp = strchr(buf2, '\n');
        *tmp = '\0';
      }
      
      if( 0> fprintf( stream, "%s -- %s\n", buf1, buf2))
        return -5;
    }
  }
  return t->nruns;
}


/* malloc a new tt_project_struct
   expect a 0-terminated string which will be copied. */
tt_p_t* tt_p_new(const char* name){
  tt_p_t* ret = NULL;
  
  if(NULL == name)
    return NULL;

  if( NULL == (ret = malloc( sizeof(tt_p_t))))
    return NULL;

  if( NULL == (ret->name = strdup(name))){
    free(ret); 
   return NULL;
  }
  ret->id = 0;
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
  for(int i = 0; i < p->ntasks; i++)
    tt_t_free(p->tasklist[i]);
  free(p->tasklist);
  free(p);
}


unsigned int tt_p_setid( tt_p_t* p, unsigned int id){
  if(NULL == p)
    return 0;
  p->id = id;
  return id;
}


unsigned int tt_p_getid( tt_p_t* p){
  if(NULL == p)
    return 0;
 return p->id;
}


/* add a task to a project 
   return index of task or below 0 on error
*/
int tt_p_add_task(tt_p_t* project, tt_t_t* task){
  tt_t_t** tmp = NULL;
  unsigned ret = 0;
  
  if(!project)
    return -1;
  if(!task)
    return -2;

  if(NULL == project->tasklist){
    project->tasklist = malloc(sizeof(tt_t_t*));
    *(project->tasklist) = task;
    project->len = 1;
    project->ntasks = 1;
    return 0;
  }
  if( project->len == project->ntasks){
    if( NULL == (tmp = realloc( project->tasklist,
				sizeof(tt_t_t*)
				* project->len * 2))){
      return -4;
    }
    project->tasklist = tmp;
    project->len *=2;
  }
  project->tasklist[project->ntasks]=task;
  ret = (project->ntasks)++;
  return ret;
}

/* remove first task named tname from tasklist, 
   return it or NULL otherwise */
tt_t_t* tt_p_rm_task(tt_p_t* p, const char* tname){
  tt_t_t* t = NULL;

  for( int i = 0; i < p->ntasks; i++){
    t = p->tasklist[i];
    if( 0 == strcmp( t->name, tname)){
      p->tasklist[i] = NULL;
      
      for(int j = i+1; j < p->ntasks; j++){
        p->tasklist[i++] = p->tasklist[j];
      }
      --(p->ntasks);
      return t;
    }
  }
  return NULL;
}


tt_t_t* tt_p_find_task( tt_p_t* p, const char* tname){
  tt_t_t* t = NULL;
  for( int i = 0; i < p->ntasks; i++){
    t = p->tasklist[i];
    if( 0 == strcmp( t->name, tname)){
      return t;
    }
  }
  return NULL;
}

/* find me a fresh task id. */
unsigned int tt_db_new_taskid( tt_db_t* db){
  return db->next_tskid++;
}

/* find me a fresh project id. */
unsigned int tt_db_new_projectid( tt_db_t* db){
  return db->next_prid++;
}

tt_p_t* tt_db_find_project( tt_db_t* db, const char* pname){
  tt_p_t* p = NULL;
  for( int i = 0; i < db->nprojects; i++){
    p = db->projects[i];
    if( 0 == strcmp( p->name, pname)){
      return p;
    }
  }
  return NULL;
}

tt_t_t* tt_db_find_task(tt_db_t* db,
			const char* pname,
			const char* tname){
  tt_p_t* p = NULL;

  if( NULL != (p = tt_db_find_project(db, pname))){
    return tt_p_find_task(p, tname);
  }
  return NULL;
}


/* list all tasks of a given project */
int tt_p_ls(tt_p_t* p, FILE* stream){
  if( NULL ==  p)
    return -1;
  if(NULL == stream)
    return -2;

   
  if(0 == p->ntasks)
    return 0;

  for( int i = 0; i < p->ntasks; i++){
    if( 0 > fprintf( stream, "%s\n", p->tasklist[i]->name))
      return -3;
  }
  return p->ntasks;
}


/* list all tasks of a given project recursively */
int tt_p_lsr(tt_p_t* p, FILE* stream){
  if( NULL ==  p)
    return -1;
  if(NULL == stream)
    return -2;

   
  if(0 == p->ntasks){
    /* fprintf(stderr, "%s:%d - no tasks\n", __FILE__, __LINE__ );*/
    return 0;
  }
  for( int i = 0; i < p->ntasks; i++){
    if( 0 > fprintf( stream, "%s\n", p->tasklist[i]->name))
      return -3;
    if( 0 > tt_t_ls(p->tasklist[i], stream))
      return -4;
  }
  return p->ntasks;
}


/* malloc a tt_db_struct.
   return NULL on error.
 */
tt_db_t* tt_db_new(void){
  tt_db_t* ret = NULL;
  
  if( NULL == (ret = malloc( sizeof(tt_db_t))))
    return NULL;

  ret->projects = NULL;
  ret->nprojects = 0;
  ret->len = 0;
  return ret;  
}

/* add a project 
   return index of project or below 0 on error
*/
int tt_db_add_project(tt_db_t* db, tt_p_t* project){
  tt_p_t** tmp = NULL;
  unsigned ret = 0;

  if(!db)
    return -1;
  if(!project)
    return -2;

  if(NULL == db->projects){
    db->projects = malloc(sizeof(tt_p_t*));
    *(db->projects) = project;
    db->len = 1;
    db->nprojects = 1;
    return 0;
  }
  if( db->len == db->nprojects){
    if( NULL == (tmp = realloc( db->projects,
				sizeof(tt_db_t*) * db->len * 2))){
      return -4;
    }
    db->projects = tmp;
    db->len *=2;
  }
  db->projects[db->nprojects]=project;
  ret = db->nprojects++;
  return ret;
}

/* recursively free a tt_db_struct.
 */
void tt_db_free(tt_db_t* d){
  if(NULL == d)
    return;

  for(int i = 0; i < d->nprojects; i++)
    tt_p_free(d->projects[i]);
  free(d->projects);
  free(d);
}

/* list all projects of a given registry */
int tt_db_ls(tt_db_t* db, FILE* stream){

  if( NULL ==  db)
    return -1;
  if(NULL == stream)
    return -2;

   
  if(0 == db->projects)
    return 0;

  for( int i = 0; i < db->nprojects; i++){
    if( 0 > fprintf( stream, "%s\n", db->projects[i]->name))
      return -3;
  }
  return db->nprojects;
}

/* list all projects of a given registry recursively */
int tt_db_lsR(tt_db_t* db, FILE* stream){
  if( NULL ==  db)
    return -1;
  if(NULL == stream)
    return -2;

  if(0 == db->nprojects)
    return 0;

  for( int i = 0; i < db->nprojects; i++){
    if( 0 > fprintf( stream, "%s:\n", db->projects[i]->name))
      return -3;
    if( 0 > tt_p_lsr(db->projects[i], stream))
      return -4;
  }
  return db->nprojects;
}


/* snip the project out, 
   return it 
   or return NULL if not found.
*/
tt_p_t* tt_db_rm_project(tt_db_t* db, const char* pname){
  tt_p_t* p = NULL;

#ifdef DEBUG
  fprintf(stderr,"%s:%d:   tt_db_rm_project(db '%s')\n",__FILE__, __LINE__, pname);
#endif
  
  for( int i = 0; i < db->nprojects; i++){
    p = db->projects[i];
    if( 0 == strcmp( p->name, pname)){

#ifdef DEBUG
      fprintf(stderr,"%s:%d:   pr '%s' FOUND\n",__FILE__, __LINE__, pname);
#endif
      
      db->projects[i] = NULL;
      
      for(int j = i+1; j < db->nprojects; j++){

#ifdef DEBUG
        fprintf(stderr,"%s:%d:   db->projects[%d] = db->projects[%d]\n",__FILE__, __LINE__, i, j);
#endif

        db->projects[i++] = db->projects[j];
      }
      --(db->nprojects);
      return p;
    }
  }
  return NULL;
}


