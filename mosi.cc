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
#include "cache.h"
#include "mosi.h"

void MOSI::PrRd(ulong addr, int processor_number) 
{
    cache_state state;
    current_cycle++; 
    reads++;
    cache_line * line = find_line(addr);
    if (line == NULL)
  	{ 
         read_misses++;
         cache_line *newline = allocate_line(addr);
	 if((c2c_supplier(addr,processor_number)==0))
	    memory_transactions++;
         else
            cache2cache++;
	 I2S++; 
	 newline->set_state(S);
	 bus_reads++;		   
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
	  I2S++; 
	  newline->set_state(S);
	  bus_reads++;		   
	  sendBusRd(addr, processor_number);
        }
       
    else
	{
            state=line->get_state();
	      if(state == M || state == O || state == S)
                { 
	           update_LRU(line);
                }
         }
}

void MOSI::PrWr(ulong addr, int processor_number) 
{
      cache_state state;
      current_cycle++;
      writes++;
      cache_line * line = find_line(addr);
      if (line == NULL)
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
       else if (line->get_state() == I)
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

void MOSI::BusRd(ulong addr) 
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
	}
}

void MOSI::BusRdX(ulong addr) 
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
		else if(state == S )
             {
                line->set_state(I);
                invalidations++;  
             }

      }
}

void MOSI::BusUpgr(ulong addr)
{
cache_line *line = find_line(addr);
    if(line!=NULL)
    {
        cache_state state;
        state = line->get_state();
        if(state == O)
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

bool MOSI::writeback_needed(cache_state state) {
//edit this function to return the correct boolean value
 if(state == M || state == O)
        return true;
    else
return false;
	
}


