/*   Configuration file utility module.
*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <termio.h>
#include <fcntl.h>
#include <sys/types.h>
#include "stuff.h"

/* Find the separator character in the source string, the portion to the
   right is moved into "right". The portion to the left remains in "src".
   The separator itself is removed. If the separator is not found then
   "src" is unchanged and "right" is a null string.
*/
  void comserv_split (pchar src, pchar right, char sep)
    begin
      pchar tmp ;

      tmp = strchr (src, sep) ;
      if (tmp)
        then
          begin
            str_right (right, tmp) ;
            *tmp = '\0' ;
          end
        else
          right[0] = '\0' ;
    end
