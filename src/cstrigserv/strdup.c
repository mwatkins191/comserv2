#ifndef lint
static char sccsid[] = "@(#) $Id: strdup.c,v 1.1.1.1 2003/02/01 18:55:07 lombard Exp $";
#endif

/* strdup - return copy of input string.	*/
char *strdup (char *instr)
{
	char *outstr;
	outstr = (char *)malloc(strlen(instr)+1);
	if (outstr) strcpy (outstr, instr);
	return (outstr);
}

