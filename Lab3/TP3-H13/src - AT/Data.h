///////////////////////////////////////////////////////////////////////////////
//
//	Data.h
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _DATA_H_
#define _DATA_H_

#include <systemc.h>
#include <stdio.h>
#include "InterfaceData.h"

///////////////////////////////////////////////////////////////////////////////
//
//	Class Data
//
///////////////////////////////////////////////////////////////////////////////
class Data : public sc_module, public InterfaceData
{
	public: 
		// Ports  

		// Constructor
		Data( sc_module_name zName );
		
		// Destructor
		~Data();
		
	private:
		// Process SystemC
		SC_HAS_PROCESS(Data);
		std::string word;
		unsigned int wordLength;
		unsigned int GetWord(unsigned int wordPosition); //Return la longueur du mot sélectionné
		unsigned char GetChar(unsigned int charPosition); //Return la lettre data[charPosition]
};

#endif
