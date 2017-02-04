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

const char tt_time_format = "%Y-%m-%d %H:%M:%S";

/* little helper
   TODO:
   - read in buf 
   - reallocing buf if
   - necessary
*/
char* bufline(char* oldbuf, int bl, int fd);

struct chunk{
  char* start;
  char* end;
  char stopsign;
};

int parse_id(struct chunk);
char* parse_name(struct chunk);
time_t parse_time(struct chunk);

int parse_line(char* buf, tt_db_t* db);
/* read and parse */
tt_db_t* tt_db_read_file( const char* file_name);
