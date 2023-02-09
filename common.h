#ifndef COMMON_H
#define COMMON_H

/*
https://www.rfc-editor.org/rfc/rfc868
[...]
The time is the number of seconds since 00:00 (midnight) 1 January 1900
GMT, such that the time 1 is 12:00:01 am on 1 January 1900 GMT; this
base will serve until the year 2036.

For example:

   the time  2,208,988,800 corresponds to 00:00  1 Jan 1970 GMT,

             2,398,291,200 corresponds to 00:00  1 Jan 1976 GMT,

             2,524,521,600 corresponds to 00:00  1 Jan 1980 GMT,

             2,629,584,000 corresponds to 00:00  1 May 1983 GMT,

        and -1,297,728,000 corresponds to 00:00 17 Nov 1858 GMT.
*/
#define TIME_1900_TO_1970 2208988800

#define BUFFER_SIZE 4096

#endif