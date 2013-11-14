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
class Control : public sc_module, public InterfaceRead
{
	public: 
		// Ports
		
		// Port pour le module d'affichage
		sc_port<InterfacePrint> printPort;

		// Port pour la memoire de donnees
		sc_port<InterfaceData> dataPort;
	
		// Constructor
		Control( sc_module_name zName, unsigned int DictSize, unsigned int maxErrors );
		
		// Destructor
		~Control();

		// Méthode
		virtual bool StartGame();
		virtual bool TryNewLetter(char a); //False lorsque la partie est terminée
		
	private:
		// Process SystemC
		SC_HAS_PROCESS(Control);
		int DictSize_;
		int maxErrors_;
		int wordLength_;
		int nbErrors_;
		char word_[20];
		bool foundBool_[20];
};

#endif
