/***********************************************************

File Name : 
        mservutils.h


Programmer:
        Phil Maechling

Description:
        These are mserv utilities. They should isolate certain routines
        which may change from the primary mserv and mcomlink code.

Creation Date:
        22 Feb 1999


Usage Notes:



Modification History:


**********************************************************/
#ifndef MSERVUTILS_H
#define MSERVUTILS_H

#include <stdio.h>
#ifndef cs_quanstrc_h
#include "quanstrc.h"
#endif

int classify_packet(seed_fixed_data_record_header* sh);

double header_to_double_time(seed_fixed_data_record_header* sh);

#endif
