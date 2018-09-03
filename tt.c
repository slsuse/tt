#define _XOPEN_SOURCE 500 

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "tt.h"
#include <sys/time.h>

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

void seconds_to_hours(time_t t, int* sec, int* min, int* h){
  int s = (int) t;

  *h = s / 3600;
  s = s % 3600;
  *min = s / 60;
  *sec = s % 60;
}

/* list a duration d to stream, if filter applies 
   for a start there's only one filter is_stopped with value 1 */
int tt_d_ls(tt_d_t* d, FILE* stream, char filter){
  char buf1[32];
  char buf2[32];
  char buf3[64];
  int h, m, s;
  char* nl = 0x0;
  time_t end;
   
  if(!d)
    return -1;

  switch(filter){
  case 1:
    if(0 == d->finished)
      break;
  case 0:
    if(0 == d->start){
      fprintf(stream, "N/A -- N/A: 0 seconds.\n");
      return 0;
    }
    if( NULL == ctime_r(&(d->start), buf1))
      return -2;
    nl = strchr(buf1,'\n');
    *nl = 0x0;
    
    if(0 == d->finished){
      /* using the current time to calculate a duration is more useful */
      
      end = time(NULL);
      
      
      sprintf( buf2, "N/A");    
    }
    else{
      end = d->finished;
      
      if( NULL == ctime_r(&end, buf2))
        return -3;
      nl = strchr(buf2,'\n');
      *nl = 0x0;
    }  
    seconds_to_hours(end - d->start, &s, &m, &h);
    snprintf(buf3,63,"%d h, %d min, and %d sec" ,h, m, s);
    
    fprintf(stream, "\t%s -- %s, i.e. %s\n", buf1, buf2, buf3);
    break;
  }
  
  return 0;
}


/* start a run on a given task,
   stopping the last run.
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

  if(task->nruns){
    if( 0 == task->runs[task->nruns - 1]->finished){
      fprintf(stderr, "%s:%d - stopping last run\n", __FILE__, __LINE__);
      if( tt_t_stop_run(task) < 0)
        return -6;
    }
  }
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

  return task->runs[task->nruns - 1]->finished = time(NULL);
}

int tt_t_stop_this_run(tt_t_t* task, unsigned int i){
  if(!task)
    return -2;

  if(!(task->name))
    return -3;

  if(0 == task->len)
    return -4;

  if( task->nruns > i)
    return task->runs[i]->finished = time(NULL);
  else
    return -5;
}
  


  
/* list the runs of a given task, if filter applies 
   for a start there's only one filter is_stopped with value 1 */
int tt_t_ls(tt_t_t* t, FILE* stream, char filter){
  /*  char buf1[26] = "N/A";
  char buf2[26] = "N/A";
  char* tmp = NULL;
  */
  unsigned int decplace = 0;
  unsigned int tmp = 0;
  if( NULL == t)
    return -1;
  if(NULL == stream)
    return -2;
  
  if(0 == t->nruns){
    /*    fprintf(stderr, "%s:%d - no runs\n", __FILE__, __LINE__ );*/
    return 0;
  }
  
  decplace = 1;
  tmp = t->nruns;
  while( tmp/10 >= 1){
    tmp = tmp/10;
    ++decplace;
  }

  for( int i = 0; i < t->nruns; i++){
    fprintf( stream, "\t\t[%*d] ", decplace, i);
    tt_d_ls(t->runs[i], stream, filter);
    /*
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
      else
        strcpy(buf2, "N/A");
      
      if( 0> fprintf( stream, "%s -- %s\n", buf1, buf2))
        return -5;
    }
    */
  }
  return t->nruns;
}

time_t tt_d_duration(tt_d_t* d){
  time_t end;

  if(!d)
    return -1;
  
  if(0 == d->start)
    return 0;

  end = d->finished;
  if(0 == end)
    end = time(NULL);

  return end - d->start;
}

time_t tt_t_sum(tt_t_t* t){
  time_t ret = 0;

  if(!t)
    return -1;

  for(int i = 0; i < t->nruns; i++){
    time_t tmp;
    if(0 > (tmp = tt_d_duration(t->runs[i])))
      return -2;
    ret += tmp;
  }
  return ret;
}

int tt_t_prsum(tt_t_t* t, FILE* stream){
  int h, m, s;
  time_t duration;

  if(0 > (duration = tt_t_sum(t)))
    return -3;
  seconds_to_hours(duration ,&s, &m, &h);
  fprintf(stream, "%d h, %d min, and %d sec\n", h, m, s);
  return duration;
}

time_t tt_p_sum(tt_p_t* p){
  time_t ret =0;
  if(!p)
    return -1;

  for( int i = 0; i < p->ntasks; i++){
    time_t tmp;
    if(0>(tmp=tt_t_sum(p->tasklist[i])))
      return -2;
    ret += tmp;
  }
  return ret;
}

int tt_p_prsum(tt_p_t* p, FILE* stream){
  int h, m, s;
  time_t duration;

  if(0 > (duration = tt_p_sum(p)))
    return -3;
  seconds_to_hours(duration ,&s, &m, &h);
  fprintf(stream, "%d h, %d min, and %d sec\n", h, m, s);
  return duration;
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
  if(NULL == p){
    fprintf(stderr, "%s:%d NULL pointer in tt_p_find_task!\n", __FILE__, __LINE__);
    return NULL;
  }
  if(NULL == tname){
    fprintf(stderr, "%s:%d NULL pointer in tt_p_find_task!\n", __FILE__, __LINE__);
    return NULL;
  }
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
  if(NULL == db){
    fprintf(stderr, "%s:%d NULL pointer in tt_db_find_task!\n", __FILE__, __LINE__);
    return NULL;
  }
  if(NULL == pname){
    fprintf(stderr, "%s:%d NULL pointer in tt_db_find_task!\n", __FILE__, __LINE__);
    return NULL;
  }
  if(NULL == tname){
    fprintf(stderr, "%s:%d NULL pointer in tt_db_find_task!\n", __FILE__, __LINE__);
    return NULL;
  }
  
  if( NULL != (p = tt_db_find_project(db, pname))){
    return tt_p_find_task(p, tname);
  }
  return NULL;
}

int tt_t_is_running(tt_t_t* t){

  for( int i = 0; i < t->nruns; i++){
    if( 0 == t->runs[i]->finished)
      return 0;
  }
  return 1;
}

/* list all tasks of a given project, if filter applies 
   for a start there's only one filter is_stopped 
   with value 1 for stopped and 3 for running */
int tt_p_ls(tt_p_t* p, FILE* stream, char filter){
  if( NULL ==  p)
    return -1;
  if(NULL == stream)
    return -2;

   
  if(0 == p->ntasks)
    return 0;

  switch(filter){
  case 0:
    for( int i = 0; i < p->ntasks; i++){
      if( 0 > fprintf( stream, "\t%s\n", p->tasklist[i]->name))
        return -3;
    }
    break;
  case 3:
    for( int i = 0; i < p->ntasks; i++){
      if(tt_t_is_running( p->tasklist[i])){
        if( 0 > fprintf( stream, "\t%s\n", p->tasklist[i]->name))
          return -4;
      }
    }
    break;
  case 1:
    for( int i = 0; i < p->ntasks; i++){
      if( ! tt_t_is_running( p->tasklist[i])){
        if( 0 > fprintf( stream, "\t%s\n", p->tasklist[i]->name))
          return -5;
      }
    }
  default:
    return -6;
  }
  return p->ntasks;
}


/* list all tasks of a given project recursively, if filter applies 
   for a start there's only one filter is_stopped with value 1 */
int tt_p_lsr(tt_p_t* p, FILE* stream, char filter){
  if( NULL ==  p)
    return -1;
  if(NULL == stream)
    return -2;

   
  if(0 == p->ntasks){
    /* fprintf(stderr, "%s:%d - no tasks\n", __FILE__, __LINE__ );*/
    return 0;
  }
  for( int i = 0; i < p->ntasks; i++){
    if( 0 > fprintf( stream, "\t%s\n", p->tasklist[i]->name))
      return -3;
    if( 0 > tt_t_ls(p->tasklist[i], stream, filter))
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
    if( 0 > fprintf( stream, "%s\n", db->projects[i]->name))
      return -3;
    if( 0 > tt_p_lsr(db->projects[i], stream, (char) 0))
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

  for( int i = 0; i < db->nprojects; i++){
    p = db->projects[i];
    if( 0 == strcmp( p->name, pname)){

      db->projects[i] = NULL;
      
      for(int j = i+1; j < db->nprojects; j++){
        db->projects[i++] = db->projects[j];
      }
      --(db->nprojects);
      return p;
    }
  }
  return NULL;
}


