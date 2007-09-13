#ifndef lint
static char sccsid[] = "@(#) $Id: inet_ntoa.c,v 1.1.1.1 2003/02/01 18:55:07 lombard Exp $";
#endif

/* Private implementation of inet_ntoa for os/9.	*/
/* Apparent undefined symbols when linking netdb.l	*/

#include <types.h>
#include <inet/netdb.h>
#include <inet/in.h>

char *inet_ntoa (struct in_addr in)
{
	static char str[80];
	union {
		unsigned long int l;
		unsigned char b[4];
	} u;
	char *p = str;
	int i;
	u.l = in.s_addr;
	sprintf (str, "%d.%d.%d.%d", u.b[0], u.b[1], u.b[2], u.b[3]);	
	return (str);
}
