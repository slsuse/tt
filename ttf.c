#define _XOPEN_SOURCE 600 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/file.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "tt.h"
#include "ttf.h"


/* return < 0 on error or new length of buf 
   save position of null terminator in slen
*/
int readfilebuf(char** buf, int* slen, int bl, int fd) {
  int pos = 0;
  int l = bl;
  int max = l-1;
  int nread = -1;
  char* b = *buf;
  errno = 0;
  while(0 < (nread = read(fd, b+pos, max))){ 
    pos += nread;
    
    if(0 == (max -= nread)){
      max = l-1;
      l *=2;
      if(NULL == (b = realloc( b, sizeof(char) * l ))){
        perror("read");
        return -1;
      }
      *buf = b;
    }
    b[pos] = (char) 0x0;
  }
  if(0>nread){
    perror("read");
    return -2;
  }
  *slen = pos;
  return l;
}

int tt_unesc(char* s, char esc){
  char* src = s;
  char* dst = NULL;
  int n = 0;

  while( *src != esc){ /* fast forward */
    if(0x0 == *src)
      return 0;
    ++src;
  }

  dst = src;
  while( 0x0 != *src){
    if( *src == esc){
      ++src;
      ++n;
    }
    *dst = *src;
    ++dst;
    ++src;
  }
  return n;
}

/* return pointer to delimiter or end of string 
   get rid of escaping by removing esc inplace 
   (truncating the delimited string)
   cnt must be allocated and will contain the
   number of removed esc characters.

   SEE:
       -  test_strdelim in test.c for example usage.
*/
char* tt_strdelim(char* buf, int* cnt, char delim, char esc){
  char* src = buf;
  char* dst = NULL;
  *cnt = 0;

  while( *src != esc){ /* fast forward */
    if( 0x0 == *src)
      return src;
    if( delim == *src)
      return src;
    ++src;
  }

  ++(*cnt);
  dst = src;
  ++src;
  if( 0x0 == *src)
    return src;
  *dst = *src;
  ++src;
  ++dst;
  
  while( 0x0 != *src){
    if( *src == esc){
      ++(*cnt);
      ++src;
      if( 0x0 == *src)
        return src; /* we don't escape string ends */
      *dst = *src;
    }
    else{
      if(delim == *src){
        return dst; /*dst points to the end of the cleaned field. */
      }
      *dst = *src;
    }
    ++dst;
    ++src;
  }
  return dst;
}
  

char* tt_strchar(char* buf, char delim){
  char esc = '\\';
  char *tmp = buf;
  
  do{
    if( esc == *tmp){
      ++tmp;
    }
    else if( delim == *tmp){
      return tmp;
    }

  } while(++tmp);
    
  return NULL;
}

/* REALLY int ids, not alnum? */
int parse_id(struct chunk* sc, char delim){
  sc->end = tt_strdelim(sc->start, &(sc->cnt), delim, sc->esc);
 
  if(*(sc->end) == '\0'){
    fprintf(stderr, "%s:%d: corrupt data\n", __FILE__, __LINE__);
 
    return -1;
  }
  *(sc->end) = (char) 0x0;
  return atoi(sc->start);
}

char* parse_name(struct chunk* sc, char delim){  
  sc->end = tt_strdelim(sc->start, &(sc->cnt), delim, sc->esc);
  
  if(*(sc->end) == '\0'){
    
    fprintf(stderr, "%s:%d: corrupt data\n", __FILE__, __LINE__);
    return NULL;
  }
  *(sc->end) = (char) 0x0;
  return sc->start;
}

time_t parse_time(struct chunk* sc, char delim){
  struct tm stm;
  fprintf(stderr, "%s:%d - %s\n", __FILE__, __LINE__, sc->start);
  sc->end = tt_strdelim(sc->start, &(sc->cnt), delim, sc->esc);
   
  if(*(sc->end) == '\0'){
     fprintf(stderr, "%s:%d: corrupt data\n", __FILE__, __LINE__);
    return -1;
  }
  *(sc->end) = (char) 0x0;
  if(sc->start != sc->end){
    strptime( sc->start, tt_time_format, &stm);
    fprintf(stderr, "%s:%d - strptime called\n", __FILE__, __LINE__);
  /* TODO: DST.
     The value specified in the tm_isdst field informs mktime()
     whether or not daylight saving time (DST) is in effect for the
     time supplied in the tm structure: a positive value means DST is
     in effect; zero means that DST is not in effect; and a negative
     value means that mktime() should (use timezone information and
     system databases to) attempt to determine whether DST is in
     effect at the specified time.  */
    stm.tm_isdst = -1; /* FIXME: best solution is to register dst in the csv. */
    return mktime(&stm);
  }
  else
    return 0;
}

/* TODO: clean up.
   function is too long. Break down into simpler pieces.
   interface changed, buf no longer necessary (is referenced in sc)
*/   
int parse_line(char* buf, tt_db_t* db, struct chunk* sc){  
  int pid = 0;
  int tid = 0;
  char* pname = NULL;
  char* tname = NULL;
  time_t dstart = 0;
  time_t dstop  = 0;

  /* project id */
  if( 0> (pid = parse_id(sc, sc->coldelim))){
    return -1;
  }
  sc->start = (sc->end)+(sc->cnt)+1;
  /*sc->start = ++(sc->end);*/
   
  /* project name */
  if(NULL == (pname = parse_name(sc, sc->coldelim))){
    return -2;
  }
  sc->start = (sc->end)+(sc->cnt)+1;
  
  /* task id */
  if( 0> (tid = parse_id(sc, sc->coldelim))){
    return -3;
  }   
  sc->start = (sc->end)+(sc->cnt)+1;
 
  /* task name */
  if(NULL == (tname = parse_name(sc, sc->coldelim))){
    return -4;
  }
  sc->start = (sc->end)+(sc->cnt)+1;
  
  /* duration start */
    
  if(0 > (dstart = parse_time(sc, sc->coldelim))){
    return -5;
  }
  
  sc->start = (sc->end)+(sc->cnt)+1;
    
  /* duration end */
  if(0 > (dstop = parse_time(sc, sc->rowdelim))){
    return -6;
  }
  sc->start = (sc->end)+(sc->cnt)+1;
   
  {
    tt_t_t* tmptsk = NULL;
    tt_p_t* tmppr = NULL;
    tt_d_t* tmpd = NULL;

    /* TODO: error checking.
     */

    tmppr = tt_db_find_project(db, pname);
    if(NULL == tmppr){
      tmppr = tt_p_new(pname);
      tt_p_setid(tmppr, pid);
      tt_db_add_project(db,tmppr);
    }
  
    tmptsk = tt_p_find_task(tmppr,tname);
    if(NULL == tmptsk){
      tmptsk = tt_t_new(tname);
      tt_t_setid(tmptsk, tid);
      tt_p_add_task(tmppr,tmptsk);
    }
    fprintf(stderr, "%s:%d - %d - %d\n", __FILE__, __LINE__, dstart, dstop);
    tmpd = tt_d_new(dstart, dstop);
    if(0 >= tt_t_find_run(tmptsk, tmpd))
      tt_t_add_run(tmptsk, tmpd);
    else
      tt_d_free(tmpd);
  }
  
  return 0;
}


tt_db_t* tt_db_update(tt_db_t* db, int fd){
  errno = 0;   
  {
    char* buf = NULL;
    int bl = 128;
    int slen = 0;
    int l = 0;
    int nparsed = 0;
    struct chunk sc;

    if( NULL == (buf = malloc(sizeof(char)*bl))){
      perror("malloc"); //?
      return NULL;
    }
    
    if( 0 > (l = readfilebuf(&buf, &slen, bl, fd))){
      perror("readfilebuf");
      free(buf);
      return NULL;
    }
   
    sc.start = buf;
    sc.end = buf;
    sc.cnt = 0;
    sc.esc = '\\';
    sc.coldelim = ',';
    sc.rowdelim = '\n';

    while((slen - 1) > nparsed){
      if( 0> parse_line(buf, db, &sc)){
        free(buf);
        
        return NULL;
      }
      nparsed = (sc.end - buf) + sc.cnt;
    }
    free(buf);
  }
  return db;
}

tt_db_t* tt_db_read_file( tt_db_t* db, const char* file_name){
  tt_db_t* ret = NULL;
  int fd = -1;
  errno = 0;
  
  if( 0 > (fd = open( file_name, O_RDONLY))){
    perror("tt_db_read_file");
    return NULL;
  }
  if( 0 > (flock( fd, LOCK_EX))){ //TODO: is lockf(fd,op,0) better?
    perror("tt_db_read_file");
    return NULL;
  }

  ret = tt_db_update(db, fd);
  
  if(0 != close(fd)){
    perror("close");
    
    return NULL;
  }
  return ret;
}

/* i, j, buf[32], curpr, fd*/
void esc_write(int fd, const char* s){
  char buf[64];
  
  /*TODO: test escaping. */
  {
    int j = 0;
    int l =  strlen(s);
    while( j < l){
      int i = 0;
      while(i < 63 && j < l){
        switch( s[j] ){
        case ',':
        case '\\':
          buf[i++] = '\\';
        default:
          buf[i++] = s[j++];
          break;
        }
      }
      write(fd,buf,i);
    }
  }

}

/* TODO:  error checking. 
*/
int tt_d_tocsv( tt_d_t* d, int fd, tt_p_t* curpr, tt_t_t* curtsk){
  char buf[32];
  int rl = 0;
  
  rl = snprintf(buf, 32, "%d,", curpr->id);
  write(fd, buf, rl);
  write(fd, ",", 1);
  esc_write(fd, curpr->name);
  
  /*write(fd, curpr->name, strlen(curpr->name));*/
  /*TODO: test escaping. */
  /* { */
  /*   int j = 0; */
  /*   int l =  strlen(curpr->name); */
  /*   while( j < l){ */
  /*     int i = 0; */
  /*     while(i < 31 && j < l){ */
  /*       switch(curpr->name[j]){ */
  /*       case ',': */
  /*       case '\\': */
  /*         buf[i++] = '\\'; */
  /*       default: */
  /*         buf[i++] = curpr->name[j++]; */
  /*         break; */
  /*       } */
  /*     } */
  /*     write(fd,buf,32); */
  /*   } */
  /* } */

  write(fd, ",", 1);
  if(curtsk){
    rl = snprintf(buf, 32, "%d,", curtsk->id);
    write(fd, buf, rl);
    write(fd, ",", 1);
    esc_write(fd, curtsk->name);
  }
  else{ /*empty fields*/
    write(fd, ",", 1);
  }
 
  
  /* write(fd, curtsk->name, strlen(curtsk->name)); */
  /*TODO: test escaping. */
  /* { */
  /*   int j = 0; */
  /*   int l =  strlen(curtsk->name); */
  /*   while( j < l){ */
  /*     int i = 0; */
  /*     while(i < 31 && j < l){ */
  /*       switch(curtsk->name[j]){ */
  /*       case ',': */
  /*       case '\\': */
  /*         buf[i++] = '\\'; */
  /*       default: */
  /*         buf[i++] = curtsk->name[j++]; */
  /*         break; */
  /*       } */
  /*     } */
  /*     write(fd,buf,32); */
  /*   } */
  /* }  */
  write(fd, ",", 1);

  if(d)
  { /* time_t to struct tm to string */
    char buf[20]; /* strlen("2001-11-12 18:31:01") */
    buf[0] = (char) 0x0;
   
    strftime(buf, 20, tt_time_format, gmtime( &(d->start)));
    write(fd, buf, 20);
    write(fd, ",", 1);
    
    strftime(buf, 20, tt_time_format, gmtime( &(d->finished)));
    write(fd, buf, 20);
  }
  else{
    write(fd, ",", 1);
  }
  return 0; 
}

int tt_t_tocsv( tt_t_t* t, int fd, tt_p_t* curpr){
  if(t->nruns){
    for( int i = 0; i < t->nruns; i++){
      tt_d_tocsv( t->runs[i], fd, curpr, t);
    }
  }
  else
    tt_d_tocsv(NULL, fd, curpr, t);
  return 0;
}

int tt_p_tocsv( tt_p_t* p, int fd){
  if(p->ntasks){
    for( int i = 0; i < p->ntasks; i++){
      tt_t_tocsv( p->tasklist[i], fd, p);
    }
  }
  else
    tt_d_tocsv(NULL, fd, p, NULL);
  return 0;
}

/* safe a task table, csv 
   TODO: test.
*/
int tt_write_file( tt_db_t* t, char* file_name){
  int fd;
  
  if( NULL == t)
    return -1;

  if(NULL == file_name)
    return -2;
  
  /* TODO: error handling
   */

  errno = 0;

  if( 0 > (fd = open( file_name, O_RDWR))){
    perror("tt_db_write_file");
    return -3;
  }
  if( 0 > (flock( fd, LOCK_EX))){ //TODO: is lockf(fd,op,0) better?
    perror("tt_db_write_file");
    return -4;
  }

  if( NULL == tt_db_update(t, fd)){
    perror("tt_db_write_file");
    return -5;
  }
  
  for( int i =  0; i < t->nprojects; i++){
    tt_p_tocsv( t->projects[i], fd);
  }
  close(fd);
  return 0;
}
