#ifndef TT_H
#define TT_H
#include <time.h>

#define TT_BUF_LEN 2

/** When working on a task 
    start and stop the time for this task.
 */
typedef struct tt_duration_struct{
  time_t start;
  time_t finished;
} tt_d_t;

/* malloc a tt_duration_struct:
   start, finished default to 0 */
tt_d_t* tt_d_new(time_t start, time_t finished);

/* recursively free a tt_duration_struct.
 */
void tt_d_free(tt_d_t* d);

/* ask the system clock for the current time and start a run. 
   return 0 on error or start time else.
*/
time_t tt_d_start(tt_d_t* d);

/* ask the system clock for the current time and stop a run. 
   return 0 on error or stop time else.
*/
time_t tt_d_stop(tt_d_t* d);


/** A task model:
    A task has a name and is worked on through 
    different periods of time.
*/
typedef struct tt_task_struct{
  char* name;
  tt_d_t** runs; 
  unsigned nruns; /* number of registered runs  */
  unsigned len; /* current len of the array 'runs'. */
} tt_t_t;

/* malloc a new tt_task_struct
   expect a 0-terminated string which will be copied. */
tt_t_t* tt_t_new( const char* name);

/* recursively free a tt_task_struct.
 */
void tt_t_free(tt_t_t* task);

/* add a duration to a task
   return index of duration or below 0 on error
*/
int tt_t_add_run(tt_t_t* task, tt_d_t* duration);

/* start a run on a given task
   return below 0 on error
   or the start time else.
*/
int tt_t_start_run(tt_t_t* task);

/* finish a run on a given task
   return below 0 on error
   or the finish time else.
*/
int tt_t_stop_run(tt_t_t* task);

/** A project:
    A project has a name and a list of tasks.
 */
typedef struct tt_project_struct{
  char* name;
  tt_t_t** tasklist;
  unsigned ntasks; /* number of registered tasks */
  unsigned len; /* current len of the array 'tasklist'. */
} tt_p_t;

/* malloc a new tt_project_struct
   expect a 0-terminated string which will be copied. */
tt_p_t* tt_p_new(char* name);

/* recursively free a tt_project_struct.
 */
void tt_p_free(tt_p_t* p);

/* add a task to a project 
   return index of task or below 0 on error
*/
int tt_p_add_task(tt_p_t* project, tt_t_t* task);

/** The Registry:
    Registers an array of projects and the time periods
    they have been worked on.
*/
typedef struct tt_db_struct{
  tt_p_t** projects; /* an array of projects*/
  unsigned nprojects; /* The number of registered projects, index of NULL terminator */
  unsigned len; /* current len of the array 'projects'. */
} tt_db_t;

/* malloc a tt_db_struct.
   return NULL on error.
 */
tt_db_t* tt_db_new();

/* recursively free a tt_db_struct.
 */
void tt_db_free(tt_db_t* p);

/* add a project 
   return index of project or below 0 on error
*/
int tt_db_add_project(tt_db_t* db, tt_p_t* project);

#endif
