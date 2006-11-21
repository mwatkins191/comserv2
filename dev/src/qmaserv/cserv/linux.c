#include "linux.h"

short flip2( short shToFlip ) {

#if defined (LINUX)
 short shSave1, shSave2;

        shSave1 = ((shToFlip & 0xFF00) >> 8);
        shSave2 = ((shToFlip & 0x00FF) << 8);
        return( shSave1 | shSave2 );
#else   /*if it is not LINUX-ported version just return input*/
       return (shToFlip);
#endif
}

int flip4( int iToFlip ) {
#if defined (LINUX)
int iSave1, iSave2, iSave3, iSave4;

        iSave1 = ((iToFlip & 0xFF000000) >> 24);
        iSave2 = ((iToFlip & 0x00FF0000) >> 8);
        iSave3 = ((iToFlip & 0x0000FF00) << 8);
        iSave4 = ((iToFlip & 0x000000FF) << 24);

        return( iSave1 | iSave2 | iSave3 | iSave4 );
#else    /*if it is not LINUX-ported version just return input*/
       return (iToFlip);
#endif
}

int flip4array (long *in, short bytes)  {
/*--------------------------------------------------------
/ Swaps "bytes" bytes of a long array *in: 12345678->43218765 etc.
/ returns 0 in case of success and negative value otherwise.
/ Directly modifies data stored in the memory pointed by *data,
/ therefore could be dangerous if used improperly.
/            Ilya Dricker ISTI (i.dricker@isti.com) 12/09/1999
/ Revisions
/ 12/09/1999 Initial revision
/---------------------------------------------------------*/
unsigned char tmp;
unsigned char *p = (unsigned char *)in;
short i;
short j;

if (sizeof(long) != 4) {
        printf("Warning in flip4array: Sizeof long is not 4 as was assumed\n flip4array is aborted\n ");
        return (-1);
        }

for (i=0; i < bytes; i=i+4)     {
        tmp = *(p + i);
        *(p + i) = *(p + 3 + i);
        *(p + 3 +i) = tmp;
        tmp = *(p + 1 + i);
        *(p+1 + i) = *(p + 2 + i);
        *(p + 2 + i) = tmp;
        }
return (0);
}
