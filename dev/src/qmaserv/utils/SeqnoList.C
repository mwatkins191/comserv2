/*
 * File     :
 *  SeqnoList.C
 *
 * Purpose  :
 *  This implements a sequence number routine to eliminate the use of
 *  stdlist in the processing. This is set to fixed max size of 128.
 *
 * Author   :
 *  Phil Maechling
 *
 * Mod Date :
 *  6 July 2002
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
#include <iostream>
#include "QmaLimits.h"
#include "SeqnoList.h"

SeqnoList::SeqnoList()
{
  p_number_in_list = 0;
}

SeqnoList::~SeqnoList()
{

}

void SeqnoList::clearList()
{
  p_number_in_list = 0;
}

qma_uint32 SeqnoList::numberInList()
{
  return p_number_in_list;
}

void SeqnoList::addSeqnoToList(qma_uint32 seqno)
{
  if(p_number_in_list > MAX_SLIDING_WINDOW_SIZE)
  {
    std::cout << "xxx Error adding seqno to list. To Many Seqnos in list"
     << std::endl;
    return;
  }
  p_seqno_list[p_number_in_list] = seqno;
  ++p_number_in_list;
  return;
}

void  SeqnoList::sortList()
{

  if(!true)
  {
    std::cout << "Starting list "  << std::endl;
    for(int x=0;x<p_number_in_list;x++)
    {
      std::cout << " " << p_seqno_list[x] << std::endl;
    }
  }
  /* It sorts in increasing order. It uses 
   * the selection sort method.
   */
  int n = p_number_in_list;
  for ( int where, rh = n-1; rh > 0; rh-- )
  {
    where = 0;  
    for ( int lcv = 1; lcv <= rh; lcv++ ) 
    {
      if ( p_seqno_list[lcv] > p_seqno_list[where] )
      {
        where = lcv;
      }
    }
    int temp = p_seqno_list[where];
    p_seqno_list[where] = p_seqno_list[rh];
    p_seqno_list[rh] = temp;
  }

  if(!true)
  {
    std::cout << "Ending list "  << std::endl;
    for(int x=0;x<p_number_in_list;x++)
    {
      std::cout << " " << p_seqno_list[x] << std::endl;
    }
  }
}
