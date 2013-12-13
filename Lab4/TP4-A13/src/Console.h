///////////////////////////////////////////////////////////////////////////////
//
//	Console.h
//
///////////////////////////////////////////////////////////////////////////////
#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <systemc.h>
#include <stdio.h>


///////////////////////////////////////////////////////////////////////////////
//
//	Classe Console
//
///////////////////////////////////////////////////////////////////////////////
class Console : public sc_module
{
	public: 
		sc_in_clk				ClockPort;
		sc_in<unsigned int>		Console_Data_InPort;
		sc_in<bool>				Console_Enable_InPort;
		sc_out<bool>			Console_Ready_OutPort;

		// Constructeur
		Console( sc_module_name zName );
		
		// Destructeur
		~Console();
		
	private:
		// Processus SystemC
		SC_HAS_PROCESS(Console);
		void PrintWin();
		void PrintLose();
		// Méthode
		void thread(void);
};

#endif
