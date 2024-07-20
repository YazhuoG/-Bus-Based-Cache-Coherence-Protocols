/************************************************************
			Course 		: 	CSC/ECE506
			Source 		:	write_through.h
			Instructor	:	Ed Gehringer
			Email Id	:	efg@ncsu.edu
------------------------------------------------------------
	 Please do not replicate this code without consulting
		the owner & instructor! Thanks!
*************************************************************/ 
#include "cache.h"

class Write_through : public Cache{
	public:
		Write_through(int s, int a, int b, int p, int pn):
			Cache(s, a, b, p, pn) {};
		~Write_through() {};
		void PrRd(ulong addr, int processor_number);
		void PrWr(ulong addr, int processor_number);
		cache_line *allocate_line(ulong addr);
		void PrRdDir(ulong addr, int processor_number);
		void PrWrDir(ulong addr, int processor_number);
		bool writeback_needed(cache_state state);
		void BusUpgr(ulong addr);
		void BusRd(ulong addr);
		void signalRd(ulong addr, int processorNumber);
		void signalRdX(ulong addr, int processorNumber);
		void signalUpgr(ulong addr, int processorNumber);
		void Int(ulong addr);
		void Inv(ulong addr);
		cache_line *allocate_line_dir(ulong addr);
		void BusWr(ulong addr);
};
