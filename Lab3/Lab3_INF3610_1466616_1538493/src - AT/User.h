///////////////////////////////////////////////////////////////////////////////
//
//	User.h
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _USER_H_
#define _USER_H_

#include <systemc.h>
#include <stdio.h>
#include "InterfaceRead.h"

///////////////////////////////////////////////////////////////////////////////
//
//	Class User
//
///////////////////////////////////////////////////////////////////////////////
class User : public sc_module
{
	public: 
		// Ports  
		sc_in_clk		clk;
		sc_out<bool>	startGamePort;
		sc_out<bool>	requestLetterPort;
		sc_out<char>	incomingLetterPort;
		sc_in<bool>		endGamePort;
		sc_in<bool>		requestStartPort;
		sc_in<bool>		ackPort;

		// Constructor
		User( sc_module_name zName );


		// Destructor
		~User();
		
	private:
		// Process SystemC
		SC_HAS_PROCESS(User);
		void Process();
};

#endif
