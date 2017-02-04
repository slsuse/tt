#define _XOPEN_SOURCE 500 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/file.h>
#include <time.h>
#include "tt.h"
#include "ttf.h"


/* Codehalde:
   read and parse file 
   WARNING:
   This is ugly as hell, it's written in leftover minutes 
   without much foxus
   and currently contains unsanitized ideas.
*/


char* bufline(char* oldbuf, int bl, int fd) { /* TODO:
                                                 - read in buf 
                                                 - reallocing buf if necessary
                                              */
  int pos = 0;
  int max = bl-1;
  int nread = -1;
  do{
    nread = read(fd, buf+pos, max);
    if( 0>nread){
      perror("read");
      
      return NULL;
    }
      
    buf[nread] = (char) 0x0;
    pos += nread;
    max -= nread;
  }while(nread > 0);
  return buf;
}

int parse_id(struct chunk* sc){
  int pid = 0;
  sc->end = strchr(sc->start, sc->stopsign); 
  if( NULL == sc->end){
    fprintf(stderr, "%s/%d: corrupt data\n", __FILE__, __LINE__);
    //free(buf);
    //tt_db_free(ret);
    return -1;
  }
  *(sc->end) = (char) 0x0;
  pid = atoi(sc->start);
  sc->start = ++(sc->end);
  return pid;
}

char* parse_name(struct chunk sc){  
  char* name = NULL;
  sc->end = strchr( sc->start, sc->stopsign);
  if( NULL == sc->end){
    fprintf(stderr, "%s/%d: corrupt data\n", __FILE__, __LINE__);
    return NULL;
  }
  *(sc->end) = (char) 0x0;
  name = start;
  sc->start = ++(sc->end);
  return name;
}

time_t parse_time(struct chunk sc){
  time_t ret = 0;
  sc->end = strchr( sc->start, sc->stopsign);
  if( NULL == sc->end){
    fprintf(stderr, "%s/%d: corrupt data\n", __FILE__, __LINE__);
    return 0;
  }
  *(sc->end) = (char) 0x0;
  {
    struct tm stm;
    strptime( sc->start, tt_time_format, &stm);
    ret = tt_timegm(&stm);
  }
  sc->start = ++(sc->end);
  return ret;
}

int parse_line(char* buf, tt_db_t* db){  
  /* TODO:
     Get rid of this ugly code repepepepepepepepepetitition.
  */
  /* parse buf 
     TODO:
     - parse more then one line.
     - break down into simple small functions.
  */
  int pid = 0;
  int tid = 0;
  char* pname = NULL;
  char* tname = NULL;
  time_t dstart = 0;
  time_t dstop  = 0;
  struct chunk sc;

  sc.start = buf;
  sc.end = NULL;
  sc.stopsign = ',';

  /* project id */
  if( 0> (pid = parse_id(&sc))){
    return -1;
  }
      
  /* project name */
  if(NULL == (pname = parse_name(&sc))){
    return -2;
  }
      
  /* task id */
  if( 0> (tid = parse_id(&sc))){
    return -3;
  }
  
  /* task name */
  if(NULL == (tname = parse_name(&sc))){
    return -4;
  }
   
  /* duration start */
  if(0 == (dstart = parse_time(&sc))){
    return -5;
  }
      
  /* duration end */
  sc.stopsign = '\n';
  if(0 == (dstop = parse_time(&sc))){
    return -6;
  }
   
  /* TODO:
     allocate and fill the tt_db_struct.
  */
  {
    tt_t_t* tmptsk = NULL;
    tt_p_t* tmppr = NULL;
      
    tmptsk = tt_db_find_task(db, pname, tname);
    if(NULL == tmptsk){
      tmptsk = tt_t_new(tname);
      tt_t_setid(tmptsk, tid);

      if( NULL == (tmppr = tt_db_find_project(db, pname))){
        tmppr = tt_p_new(pname);
        tt_p_setid(tmppr, pid);
      }
	
      tt_p_add_task(tmppr, tmptsk);	
    }

    tt_db_add_project(db, tmppr);
    { /* TODO:
         sanitize here, i.e. 
         check for identical start-stop pairs?
      */
      tt_d_t* tmpd = NULL;
      tmpd = tt_d_new(dstart, dstop);
      tt_t_add_run(tmptsk, tmpd);
    }
  }
    
  return 0;
}



tt_db_t* tt_db_read_file( const char* file_name){
  tt_db_t* ret = NULL;
  char* buf = NULL;
  int bl = 128;
  int fd = -1;
  errno = 0;

  if( NULL == (buf = malloc(sizeof(char)*bl))){
    perror("malloc"); //?
    return NULL;
  }
  
  /* open and lock */
  if( 0 > (fd = open( file_name, O_RDONLY))){
    free(buf);
    perror("tt_db_read_file");
    return NULL;
  }
  if( 0 > (flock( fd, LOCK_EX))){ //TODO: is lockf(fd,op,0) better?
    free(buf);
    perror("tt_db_read_file");
    return NULL;
  }
    
  /* create ret */
  if( NULL == (ret = (tt_db_new(file_name)))){
    free(buf);
    return NULL;
  }
  
  /* TODO:
     parse and fill 
     for heaven's sake, clean this up, Stephan.
  */
  {
    char* b = buf;
    if( NULL == (b = bufline(buf, bl, fd))){
      free(buf);
      tt_db_free(ret);
    }
  }
          
  if( 0> parse_line(buf, ret)){
    free(buf);
    tt_db_free(ret);
  }
    
  } /* end parse buffer */
  /* TODO:
     close and unlock 
  */
}

/* see man timegm on Opensuse Leap 42.2 */
  
time_t tt_timegm(struct tm *tm)
{
  time_t ret;
  char *tz;
  
  tz = getenv("TZ");
  if (tz)
    tz = strdup(tz);
  setenv("TZ", "", 1);
  tzset();
  ret = mktime(tm);
  if (tz) {
    setenv("TZ", tz, 1);
    free(tz);
  } else
    unsetenv("TZ");
  tzset();
  return ret;
}

/* TODO: 
    - error checking 
    - escaping ','
*/
int tt_d_tocsv( tt_d_t* d, int fd, tt_p_t* curpr, tt_t_t* curtsk){
  
  snprintf(buf, 32, "%d,", curpr->id);
  write(fd, buf, 32);
  write(fd, curpr->name, strlen(curpr->name));
  write(fd, ',', 1);
  snprintf(buf, 32, "%d,", curtsk->id);
  write(fd, buf, 32);
  write(fd, curtsk->name, strlen(curtsk->name));
  write(fd, ',', 1);
  { /* time_t to struct tm to string */
    char buf[20]; /* strlen("2001-11-12 18:31:01") */
    buf[0] = (char) 0x0;
   
    strftime(buf, 20, tt_time_format, gmtime( &(d->start)));
    write(fd, buf, 20);
    write(fd, ",", "1")
       
    strftime(buf, 20, tt_time_format, gmtime( &(d->finished)));
    write(fd, buf, 20);
    write(fd, ",", "1")
  }
}

int tt_t_tocsv( tt_t_t* t, int fd, tt_p_t* curpr){
  for( int i = 0; i < t->nruns; i++){
    tt_d_tocsv( t->runs[i], fd, curpr, t);
  }
}

int tt_p_tocsv( tt_p_t* p, int fd){
  
  for( int i = 0; i < p->ntasks; i++){
    tt_t_tocsv( p->tasklist[i], fd, p);
  }
}

int tt_db_update( tt_db_t* db, int fd){
  /* TODO:
      - read buf
      - strchr '\n'
      - strtok ','
  */
  const int len = 128;
  char buf[128];
  int first, last, nread = 0;

  nread = read(fd, buf+first, len);
  
}

/* safe a task register, csv */
int tt_write_file( tt_db_t* t, int fd){
  int fd = -1;

  if( NULL == t)
    return -1;

  /* TODO
     open readwrite
     lockf / flock
     parse file and update t
  */

  errno = 0;

  /* open and lock */
  if( 0 > (fd = open( file_name, O_RDWR))){
    perror("tt_db_write_file");
    return NULL;
  }
  if( 0 > (flock( fd, LOCK_EX))){ //TODO: is lockf(fd,op,0) better?
      perror("tt_db_write_file");
      return NULL;
  }

  

  for( int i =  0; i < t->nprojects; i++){
    tt_p_tocsv( t->projects[i], fd);
  }
  /* TODO
     close and unlock
  */
}
