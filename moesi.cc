/************************************************************
                        Course          :       CSC506
                        Source          :       msi.cc
                        Instructor      :       Ed Gehringer
                        Email Id        :       efg@ncsu.edu
------------------------------------------------------------
         Please do not replicate this code without consulting
                the owner & instructor! Thanks!
*************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
using namespace std;
#include "main.h"
#include "moesi.h"


/*
------------------------------------------------------
Don't modify the fucntions included in this section
------------------------------------------------------
*/
cache_line * MOESI::allocate_line_dir(ulong addr) {
	return NULL;
}
void MOESI::signalRd(ulong addr, int processor_number){
}
void MOESI::signalRdX(ulong addr, int processor_number){
}
void MOESI::signalUpgr(ulong addr, int processor_number){
}
void MOESI::Inv(ulong addr) {
}
void MOESI::Int(ulong addr) {
}
void MOESI::PrRdDir(ulong addr, int processor_number) {
}
void MOESI::PrWrDir(ulong addr, int processor_number) {
}
//-------------------------------------------------------------
//Section ends here. edit the functions in the section below 
//-------------------------------------------------------------

void MOESI::PrRd(ulong addr, int processor_number) 
{
     cache_state state;
     current_cycle++; 
     reads++;
     cache_line * line = find_line(addr); 
     if(line == NULL)
  	{ 
             read_misses++;
             cache_line *newline = allocate_line(addr);
		if((c2c_supplier(addr,processor_number)==0))
		  memory_transactions++;
		else
		 cache2cache++;
		if((sharers_exclude(addr,processor_number)==0))
		   {			
		   I2E++;
		   bus_reads++;
		   newline->set_state(E);			
		   }	
		else
		{
 		  I2S++; 
		  newline->set_state(S);
		  bus_reads++;		   
		}
              sendBusRd(addr, processor_number);
       }
     else if(line->get_state()==I)
       {
         read_misses++;
             cache_line *newline = allocate_line(addr);
		if((c2c_supplier(addr,processor_number)==0))
		  memory_transactions++;
		else
		 cache2cache++;
		if((sharers_exclude(addr,processor_number)==0))
		   {			
		   I2E++;
		   bus_reads++;
		   newline->set_state(E);			
		   }	
		else
		{
 		  I2S++; 
		  newline->set_state(S);
		  bus_reads++;		   
		}
              sendBusRd(addr, processor_number);
       }

     else
	 {
           state=line->get_state();
	     if(state == M || state == O || state == E || state == S)
               { 
	         update_LRU(line);
               }
         }
}


void MOESI::PrWr(ulong addr, int processor_number) 
{
    cache_state state;
    current_cycle++;
    writes++;
    cache_line * line = find_line(addr);
    if(line == NULL)
      { 
	write_misses++;
        cache_line *newline = allocate_line(addr); 		
	if((c2c_supplier(addr,processor_number)==0))
           memory_transactions++;            
	else
	   cache2cache++;
	I2M++;
        newline->set_state(M);
	bus_readxs++;
	sendBusRdX(addr, processor_number);
      }
    else if(line->get_state() == I)
      {
        write_misses++;
        cache_line *newline = allocate_line(addr); 		
	if((c2c_supplier(addr,processor_number)==0)){
           memory_transactions++;
    }            
	else{
	       cache2cache++;
      }
	       I2M++;
        newline->set_state(M);
	bus_readxs++;
	sendBusRdX(addr, processor_number);
      }

    else 
	{
	  state=line->get_state();
	  if (state == M)
	    {
	       update_LRU(line);
            }
          else if (state == O)
              {
		 O2M++;  
		 line->set_state(M);
                 update_LRU(line);
                 bus_upgrades++;
                 sendBusUpgr(addr, processor_number);
              }
          else if (state == E)
            {
              E2M++;
              line->set_state(M);
              update_LRU(line);
             }
	   else if (state == S)
             {
	       S2M++;      
	       line->set_state(M);
               update_LRU(line);
               bus_upgrades++;
               sendBusUpgr(addr, processor_number);
              }
           
     }
}


void MOESI::BusRd(ulong addr)
{
  cache_state state;    
  cache_line * line=find_line(addr);
    if (line!=NULL) 
       { 		
	state = line->get_state();
	  if (state == M)
            {
              flushes++;
	      interventions++;
	      line->set_state(O);
	      M2O++;
            }
           else if (state == O)
            {
              flushes++;
	    }
           else if (state == E)
            {
               line->set_state(S);
	        E2S++;
            }

         }
}

void MOESI::BusRdX(ulong addr) 
{
   cache_line * line=find_line(addr);
   if (line!=NULL)	
     { 
	cache_state state; 
        state = line->get_state();
	if (state == M)
          {
           invalidations++;
      	  flushes++;
          memory_transactions++;
          write_backs++;
	         line->set_state(I);
	       }
        else if (state == O)
          {
	   invalidations++;
           write_backs++;
	   memory_transactions++;
	   flushes++;
	   line->set_state(I);
          }
         else if(state == E)
          {
            line->set_state(I);
            invalidations++;
          }
        else if(state == S)
          {
            line->set_state(I);
            invalidations++;
          }
       
      } 
}


void MOESI::BusUpgr(ulong addr) 
{
    cache_line *line = find_line(addr);
    if(line!=NULL)
    {
      cache_state state;
      state = line->get_state();
      if(state == O || state == S)
       {
        line->set_state(I);
        invalidations++;
       }
     }
}


bool MOESI::writeback_needed(cache_state state) 
{
         if(state == O || state == M)
	//edit this function to return the correct boolean value
	   return true;
          else
           return false;
}

