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
int readfilebuf(char** oldbuf, int bl, int fd);

struct chunk{
  char* start;
  char* end;
  char coldelim;
  char rowdelim;
};

int parse_id(struct chunk *sc, char delim);
char* parse_name(struct chunk *sc, char delim);
time_t parse_time(struct chunk *sc, char delim);

int parse_line(char* buf, tt_db_t* db, struct chunk* sc);
/* read and parse */
tt_db_t* tt_db_read_file( const char* file_name);
time_t tt_timegm(struct tm *tm);
int tt_d_tocsv( tt_d_t* d, int fd, tt_p_t* curpr, tt_t_t* curtsk);
char* tt_strchar(char* buf, char delim);

char* tt_strdelim(char* buf, int* cnt, char delim, char esc);


#endif
