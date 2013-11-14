///////////////////////////////////////////////////////////////////////////////
//
//	Control.h
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _CONTROL_H_
#define _CONTROL_H_

#include <systemc.h>
#include <stdio.h>
#include "InterfaceRead.h"
#include "InterfacePrint.h"
#include "InterfaceData.h"

///////////////////////////////////////////////////////////////////////////////
//
//	Class Control
//
///////////////////////////////////////////////////////////////////////////////
class Control : public sc_module
{
	public: 
		// Ports
		sc_port<InterfacePrint>	printPort;
		sc_port<InterfaceData>	dataPort;
		sc_in_clk				clk;
		sc_in<bool>				startGamePort;
		sc_in<bool>				requestLetterPort;
		sc_in<char>				incomingLetterPort;
		sc_out<bool>			endGamePort;
		sc_out<bool>			requestStartPort;
		sc_out<bool>			ackPort;

		// Constructor
		Control( sc_module_name zName, unsigned int DictSize, unsigned int maxErrors );
		
		// Destructor
		~Control();
		
	private:
		// Process SystemC
		SC_HAS_PROCESS(Control);
		void Threading(void);
		int DictSize_;
		int maxErrors_;
		int wordLength_;
		int nbErrors_;
		char word_[20];
		bool foundBool_[20];
};

#endif
