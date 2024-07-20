/************************************************************
			Course 		: 	CSC/ECE506
			Source 		:	write_through.cc
			Instructor	:	Ed Gehringer
			Email Id	:	efg@ncsu.edu
------------------------------------------------------------
	 Please do not replicate this code without consulting
		the owner & instructor! Thanks!
*************************************************************/ 
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
using namespace std;
#include "main.h"
#include "write_through.h"

cache_line * Write_through::allocate_line_dir(ulong addr) {
        return NULL;
}
void Write_through::signalRd(ulong addr, int processor_number){
}
void Write_through::signalRdX(ulong addr, int processor_number){
}
void Write_through::signalUpgr(ulong addr, int processor_number){
}
void Write_through::Inv(ulong addr) {
}
void Write_through::Int(ulong addr) {
}
void Write_through::PrRdDir(ulong addr, int processor_number) {
}
void Write_through::PrWrDir(ulong addr, int processor_number) {
}
//-------------------------------------------------------------
//Section ends here. edit the functions in the section below 
//-----------------------------------------------------------

void Write_through::PrRd(ulong addr, int processor_number) {
	cache_state state;
    current_cycle++;
    reads++;
    cache_line *line = find_line(addr);
	// No action taken if V; BusRd for I -> V
	if (line == NULL) {
		read_misses++;
		cache_line *newline = allocate_line(addr);
		memory_transactions++;
		newline->set_state(V);
		bus_reads++;
		sendBusRd(addr, processor_number);
	}
	else {
		state = line->get_state();
		if (state == I) {
			read_misses++;
			cache_line *newline = allocate_line(addr);
			memory_transactions++;
			newline->set_state(V);
			bus_reads++;
			sendBusRd(addr, processor_number);
		}
		else if (state == V) {
			update_LRU(line);
		}
	}
}

void Write_through::PrWr(ulong addr, int processor_number) {
   	cache_state state;
    current_cycle++;
    writes++;
    cache_line *line = find_line(addr);
	// BusWr for V -> V and I -> I
	if (line == NULL) { // Write directly to main memory
		write_misses++;
		memory_transactions++;
		bus_writes++;
		sendBusWr(addr, processor_number);
	}
	else {
		state = line->get_state();
		if (state == I) {
			memory_transactions++;
			write_misses++;
			bus_writes++;
			sendBusWr(addr, processor_number);
		}
		else if (state == V) {
			memory_transactions++;
			bus_writes++;
			sendBusWr(addr, processor_number);
		}
	}
}

cache_line * Write_through::allocate_line(ulong addr) {
	ulong tag;
    cache_state state;

    cache_line *victim = find_line_to_replace(addr);
    assert(victim != 0);
    state = victim->get_state();
    if (writeback_needed(state))
    {
        write_backs++;
        memory_transactions++;
    }
    tag = tag_field(addr);
    victim->set_tag(tag);
    victim->set_state(I);
    return victim;//change it according to the protocol
}
//
void Write_through::BusRd(ulong addr) {
	// No action taken and maintain currect state
}
//
void Write_through::BusWr(ulong addr) {
	cache_state state;
	cache_line *line = find_line(addr);
	// No action taken; V -> I; I -> I
	if (line != NULL) {
		state = line->get_state();
		if (state == V) {
			line->set_state(I);
		}
	}
}

bool Write_through::writeback_needed(cache_state state) 
{
	return false; //change it according to the protocol
}
void Write_through::BusUpgr(ulong addr) 
{
}
