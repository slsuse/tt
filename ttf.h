#ifndef TTF_H
#define TTF_H

/* according to man 3 strptime on OpenSuse Leap 42.2:
   %Y     The year, including century (for example, 1991).
   %m     The month number (1-12).
   %d     The day of month (1-31).
   %H     The hour (0-23).
   %M     The minute (0-59).
   %S     The second (0-60;
          60  may occur for leap seconds;
          earlier also 61 was allowed).
   
"%Y-%m-%d %H:%M:%S" =>   "2001-11-12 18:31:01"
*/

static const char* tt_time_format = "%Y-%m-%d %H:%M:%S";

/* little helper
   - read in buf 
   - reallocing buf if necessary
*/
int readfilebuf(char** oldbuf, int* slen, int bl, int fd);

struct chunk{
  char* start;
  char* end;
  char coldelim;
  char rowdelim;
  char esc;
  int cnt; /*no of removed esc characters*/
};

int parse_id(struct chunk *sc, char delim);

char* parse_name(struct chunk *sc, char delim);

time_t parse_time(struct chunk *sc, char delim);

int parse_line(char* buf, tt_db_t* db, struct chunk* sc);

/* read and parse, updating db */
tt_db_t* tt_db_update(tt_db_t* db);
tt_db_t* tt_db_read_file( tt_db_t* db, const char* file_name);

/* safe a task table, csv */
int tt_db_write_file( tt_db_t* d);

time_t mktime_pain_in_the_ass(struct tm *tm);
int tt_d_tocsv( tt_d_t* d, int fd, tt_p_t* curpr, tt_t_t* curtsk);

char* tt_strchar(char* buf, char delim);

/* return pointer to delimiter or end of string 
   get rid of escaping by removing esc inplace 
   (truncating the delimited string)
   cnt must be allocated and will contain the
   number of removed esc characters.

   SEE:
       -  test_strdelim in test.c for example usage.
   */
char* tt_strdelim(char* buf, int* cnt, char delim, char esc);


#endif
