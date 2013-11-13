///////////////////////////////////////////////////////////////////////////////
//
//	Print.h
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _PRINT_H_
#define _PRINT_H_

#include <systemc.h>
#include <stdio.h>
#include "InterfacePrint.h"
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
//
//	Class Print
//
///////////////////////////////////////////////////////////////////////////////
class Print : public sc_module, public InterfacePrint
{
	public: 
		// Ports    
		// Constructor
		Print( sc_module_name zName );
		
		// Destructor
		~Print();

	private:
		// Process SystemC
		SC_HAS_PROCESS(Print);
		void EndGame(bool winner);
		void SetWordLength(unsigned int wordLength);
		void CharacterFound(unsigned int position, unsigned char charFound);
		void AddError();
		void UpdatePrint();
		void PrintWin();
		void PrintLose();

		//Variables
		std::string currentWord;
		int wordLength_;
		int nbError_;
};

#endif
