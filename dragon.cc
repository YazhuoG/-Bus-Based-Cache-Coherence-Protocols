/************************************************************
                        Course          :       CSC506
                        Source          :       msi.cc
                        Instructor      :       Ed Gehringer
                        Email Id        :       efg@ncsu.edu
------------------------------------------------------------
         Please do not replicate this code without consulting
                the owner & instructor! Thanks!
Code Integration : Tarun Govind Kesavamurthi
*************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
using namespace std;
#include "main.h"
#include "dragon.h"


/*
------------------------------------------------------
Don't modify the fucntions included in this section
------------------------------------------------------
*/
cache_line * Dragon::allocate_line_dir(ulong addr) {
        return NULL;
}
void Dragon::signalRd(ulong addr, int processor_number){
}
void Dragon::signalRdX(ulong addr, int processor_number){
}
void Dragon::signalUpgr(ulong addr, int processor_number){
}
void Dragon::Inv(ulong addr) {
}
void Dragon::Int(ulong addr) {
}
void Dragon::PrRdDir(ulong addr, int processor_number) {
}
void Dragon::PrWrDir(ulong addr, int processor_number) {
}
//-------------------------------------------------------------
//Section ends here. edit the functions in the section below 
//-------------------------------------------------------------

void Dragon::PrRd(ulong addr, int processor_number) 
{
        //Processor Read
        cache_state state;
        cache_line *line = find_line(addr);
        reads++;
        current_cycle++;

        if (line == NULL) { // Cache Miss on Processsor Read
                bus_reads++;
                sendBusRd(addr, processor_number);
                read_misses++;
                cache_line *newline = allocate_line(addr);
                int count = sharers(addr);
                if (count){
                        // Place block into cache in shared clean state if block is in some other cache
                        newline->set_state(Sc);
                        cache2cache++;
                }
                else{
                        // Place block into cache in exclusive state if block not in any other cache
                        newline->set_state(E);
                        memory_transactions++;
                }
        }
        else { // Cache Hit on Processor Read
                state = line->get_state();
                if (state == E) {
                        update_LRU(line);
                }
                else if (state == Sc){
                        update_LRU(line);
                }
                else if (state == Sm){
                        update_LRU(line);
                }
                else if (state == M){
                        update_LRU(line);
                }
        }
}

void Dragon::PrWr(ulong addr, int processor_number) 
{
        cache_state state;
        cache_line *line = find_line(addr);
        writes++;
        current_cycle++;

        if (line == NULL) { // Cache Miss on Processor Write
                write_misses++;
                cache_line *newline = allocate_line(addr);
                int count = sharers_exclude(addr, processor_number);
                // int count = sharers(addr);
                if (count){
                        // Place block into cache in shared modified state if block is in another cache
                        newline->set_state(Sm);
                        cache2cache++;
                        bus_writes++;
                        sendBusWr(addr, processor_number);
                } else {
                        // Place block into cache in modified state if block not in any other cache
                        newline->set_state(M);
                        memory_transactions++;
                        bus_reads++;
                        sendBusRd(addr, processor_number);
                }
        }
        else { // Cache Hit on Processor Write
                state = line->get_state();
                if (state == E) { // Change state to modified
                        update_LRU(line);
                        line->set_state(M);
                }
                else if (state == Sc){ 
                        update_LRU(line);
                        int count = sharers_exclude(addr, processor_number);
                        // int count = sharers(addr);
                        if (count){ // Change state to shared modified and propogate new value to other caches
                                line->set_state(Sm);
                        }
                        else { // Change state to modified because no other cache has this value
                                line->set_state(M);
                        }
                        bus_writes++;
                        sendBusWr(addr, processor_number);
                }
                else if (state == Sm){ 
                        update_LRU(line);
                        int count = sharers_exclude(addr, processor_number);
                        // int count = sharers(addr);
                        if (count){ // Remains owner of block so state does not change
                        }
                        else{ // State changes to modified because no other cache has this value
                                line->set_state(M);
                        }
                        // Update other caches with this new value
                        bus_writes++;
                        sendBusWr(addr, processor_number);
                }
                else if (state == M){
                        update_LRU(line);
                }
        }
}


void Dragon::BusRd(ulong addr)
{
        cache_state state;
        cache_line *line = find_line(addr);

        if (line != NULL) { //read hits in this cache
                state = line->get_state();
                if (state == E){ // Block is supplied from main memory so the state transitions to shared-clean
                        line->set_state(Sc);
                }
                else if (state == Sc){ // Block is supplied to requestor by main memory or cache owner so no action is taken
                }
                else if (state == Sm){ // Flush block on bus since this cache is the owner of this value
                        flushes++;
                        write_backs++;
                }
                else if (state == M){ // Flush block since this is the only cache with this value in it, change state to shared modified
                        flushes++;
                        write_backs++;
                        line->set_state(Sm);
                }
        }
}


void Dragon::BusRdX(ulong addr) 
{
}


void Dragon::BusUpgr(ulong addr) 
{
}

void Dragon::BusWr(ulong addr) 
{
        cache_state state;
        cache_line *line = find_line(addr);

        if (line != NULL) { //read hits in this cache
                state = line->get_state();
                if (state == E){ // Bus update cannot occur in this state as no other processor has this value
                }
                else if (state == Sc){ // Update for the word is picked up and used to update currently cached block
                }
                else if (state == Sm){ // Requestor of write is the new owner of block so transition state to shared-clean
                        line->set_state(Sc);
                }
                else if (state == M){ // Bus update cannot occur in this state as no other processor has this value
                }
        }
}

bool Dragon::writeback_needed(cache_state state) 
{
	//change if required according to the protocol
        if (state == Sm){
                return true;
        }
        else if (state == M){
                return true;
        }
        else{
                return false; 
        }   
}
