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
		/* 

		À compléter 
		
		*/

		// Constructor
		Control( sc_module_name zName, unsigned int DictSize, unsigned int maxErrors );
		
		// Destructor
		~Control();
		
	private:
		// Process SystemC
		SC_HAS_PROCESS(Control);
		/* 

		À compléter 
		
		*/
};

#endif
