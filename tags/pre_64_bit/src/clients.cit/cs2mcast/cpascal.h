/*$Id: cpascal.h,v 1.1.1.1 2004/06/15 19:07:59 isti Exp $*/
/* File to make C code more readable (i.e. more like Pascal) */

/* Flag this file as included */
#define cs_pascal_h

#define begin {
#define end }
#define land &&
#define lor ||
#define lnot !
#define div /
#define mod %
#define c_and &
#define c_or |
#define eor ^
#define c_not ~
#define then
