///////////////////////////////////////////////////////////////////////////////
//
//	DataRAM.h
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DATARAM_H_
#define DATARAM_H_

#include <systemc.h>
#include "RAM.h"

///////////////////////////////////////////////////////////////////////////////
//
//	Classe DataRAM
//
///////////////////////////////////////////////////////////////////////////////
class DataRAM : public RAM
{
	public: 

		// Constructeur
		DataRAM( sc_module_name zName, const char *zDataFileName, unsigned long ulRAMSize = 0, bool bVerbose = false );
		
		// Destructeur
		~DataRAM();

		// Méthode
		unsigned char Read(unsigned int uiAddr);
		void Write(unsigned int uiAddr, unsigned char ucData);

	private:	

		// Variable 
		
		// Méthode
		void end_of_simulation();
};

#endif
