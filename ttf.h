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
