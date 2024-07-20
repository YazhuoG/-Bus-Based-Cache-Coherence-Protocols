/************************************************************
                        Course          :       CSC506
                        Source          :       MSI_up.cc
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
#include "MSI_up.h"
#include "cache.h"
#include "io_function.h"
void MSI_up::PrRd(ulong addr, int processor_number) {
	// Per-cache global counter to maintain LRU order among 
	// cache ways, updated on every cache access
	current_cycle++; 
	cache_state state;
    reads++;
    cache_line * line = find_line(addr);
    if (line == NULL) { 
		// This is a miss
		read_misses++;
		cache_line *newline = allocate_line(addr);
		memory_transactions++;
		newline->set_state(S);
		// Read miss --> BusRd
		bus_reads++;
		sendBusRd(addr, processor_number);
    }
    else { 
		state=line->get_state();
		if (state == I){
			// This is a hit but the state is invalid
			read_misses++;
			line->set_state(S);
			bus_reads++;
			sendBusRd(addr, processor_number);
		}
		update_LRU(line);
	}
}


void MSI_up::PrWr(ulong addr, int processor_number) {
   cache_state state;
    current_cycle++;
    writes++;
    cache_line * line = find_line(addr);
    if (line == NULL) { 
		/* This is a miss */
		write_misses++;
		cache_line *newline = allocate_line(addr);
		memory_transactions++;
		newline->set_state(M); 
		bus_readxs++;
		sendBusRdX(addr, processor_number);
	}
    else {
		/* Since it's a hit, update LRU and update state */
		update_LRU(line);
		state=line->get_state();
//
		if (state!=M){
			if (state == I)
				write_misses++;
			line->set_state(M); 
			bus_upgrades++;
			sendBusUpgr(addr, processor_number);
		}
	}
}


void MSI_up::BusRd(ulong addr) {
    cache_state state;
    cache_line * line=find_line(addr);
    if (line!=NULL)
       {
                state = line->get_state();
                if (state == M)
                    {
                      flushes++;
                      write_backs++;
                      memory_transactions++;
                      interventions++;
                      line->set_state(S);
                      M2S++;
                    }
        }
}


void MSI_up::BusRdX(ulong addr) {
    cache_line * line=find_line(addr);
    if (line!=NULL)     {
                cache_state state;
                state=line->get_state();
                if (state == S )
                {
                invalidations++;
                line->set_state(I);
                }
                else if (state == M )
                {
                flushes++;
                invalidations++;
                write_backs++;
                memory_transactions++;
                line->set_state(I);
                }
        }
}
void MSI_up::BusUpgr(ulong addr) {
    cache_line * line=find_line(addr);
    if (line!=NULL)	{ 
		// If true, it was a hit, so change state appropriately
		cache_state state = line->get_state();
		if (state== S){
			line->set_state(I);
			invalidations++;
		}
	}
}

bool MSI_up::writeback_needed(cache_state state) {
    if (state == M)
    {
	
        return true;
    } else {
        return false;
    }
}

