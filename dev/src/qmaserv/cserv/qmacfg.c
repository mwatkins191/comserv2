/*
 * File     :
 *   qmacfg.c
 *
 * Purpose  :
 *  This is used by qma to scan the appropriate comserv config file 
 *  and copy the information needed to a qma config file.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  28 March 2002
 *
 * This program is free software; you can redistribute it and/or modify
 * it with the sole restriction that:
 * You must cause any work that you distribute or publish, that in
 * whole or in part contains or is derived from the Program or any
 * part thereof, to be licensed as a whole at no charge to all third parties.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#include "qmacfg.h"
#include "stuff.h"
#include "cfgutil.h"

#define QMA_FAILURE -1
#define QMA_SUCCESS 1

int getQmacfg(struct qma_cfg* out_cfg)
{

  config_struc cfg ;
  char filename[CFGWIDTH] ;
  char str1[CFGWIDTH] ;
  char str2[CFGWIDTH] ;
  char station_name[CFGWIDTH] ;
  char station_dir[CFGWIDTH] ;
  char station_desc[CFGWIDTH] ;
  char source[CFGWIDTH] ;

  /*
  * clear the default config values
  */
  clearConfig(out_cfg);

  /*
   Start by finding this station in the stations.ini file
   Then look up the QMA infor
  */

  strncpy (station_name, out_cfg->station_code, 5) ; 
  strcpy (filename, "/etc/stations.ini") ;
  if (open_cfg(&cfg, filename, station_name))
       terminate ("Could not find stations.ini\n") ;

/* Try to find the station directory, source, and description */

      do
        {
          read_cfg(&cfg, str1, str2) ;
          if (str1[0] == '\0')
              break ;
          if (strcmp(str1, "DIR") == 0)
              strcpy(station_dir, str2) ;
          else if (strcmp(str1, "DESC") == 0)
              {
                strcpy(station_desc, str2) ;
                station_desc[59] = '\0' ;
                printf ("%s\n", station_desc) ;
              }
          else if (strcmp(str1, "SOURCE") == 0)
              strcpy(source, str2) ;
        }
      while (1) ;
    close_cfg(&cfg);
      
/* Check for that this is a comlink station */
      if (strcasecmp((pchar) &source, "comlink") != 0)
          terminate ("xxx Not a comlink configuration.\n") ;

/* Try to open the station.ini file in this station's directory */
      addslash (station_dir) ;
      strcpy (filename, station_dir) ;
      strcat (filename, "station.ini") ;
      if (open_cfg(&cfg, filename, "mountainair"))
          terminate ("xxx Could not find a valid mountainair station.ini\n") ;

/* Now with file open, scan for QMA info */

      do
      {
          read_cfg(&cfg, str1, str2) ;
          if (str1[0] == '\0')
              break ;
          if (strcmp(str1, "UDPADDR") == 0)
              strcpy(out_cfg->udpaddr, str2) ;

          if (strcmp(str1, "BASEPORT") == 0)
              strcpy(out_cfg->baseport, str2) ;

          if (strcmp(str1, "DATAPORT") == 0)
              strcpy(out_cfg->dataport, str2) ;

          if (strcmp(str1, "SERIALNUMBER") == 0)
              strcpy(out_cfg->serialnumber, str2) ;

          if (strcmp(str1, "AUTHCODE") == 0)
              strcpy(out_cfg->authcode, str2) ;

          if (strcmp(str1, "IPPORT") == 0)
              strcpy(out_cfg->ipport, str2) ;

          if (strcmp(str1, "VERBOSITY") == 0)
              strcpy(out_cfg->verbosity, str2) ;

          if (strcmp(str1, "DIAGNOSTIC") == 0)
              strcpy(out_cfg->diagnostic, str2) ;

          if (strcmp(str1, "STARTMSG") == 0)
              strcpy(out_cfg->startmsg, str2) ;

          if (strcmp(str1, "STATUSINTERVAL") == 0)
              strcpy(out_cfg->statusinterval, str2) ;

          if (strcmp(str1, "DATARATEINTERVAL") == 0)
              strcpy(out_cfg->datarateinterval, str2) ;

	  if (strcmp(str1, "LOGLEVEL") == 0) {
	    strcpy(out_cfg->loglevel, str2);
	  }

      }
      while (1) ;

      return 1;
}


void clearConfig(struct qma_cfg* cfg)
{
  // Don't reset station code, as that is cmd line info,
  // not station.ini info
  //strcpy(cfg->station_code,"");
  strcpy(cfg->udpaddr,"");
  strcpy(cfg->ipport,"");
  strcpy(cfg->baseport,"");
  strcpy(cfg->dataport,"");
  strcpy(cfg->serialnumber,"");
  strcpy(cfg->authcode,"");
  strcpy(cfg->verbosity,"");
  strcpy(cfg->startmsg,"");
  strcpy(cfg->statusinterval,"");
  strcpy(cfg->datarateinterval,"");
  strcpy(cfg->loglevel, "");
}
