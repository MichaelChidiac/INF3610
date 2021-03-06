///////////////////////////////////////////////////////////////////////////////
//
//	main.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include <systemc.h>
#include <time.h>
#include "processor.h"
#include "CoProcessor.h"
#include "DataRAM.h"
#include "InstRAM.h"
#include "Console.h"
#include "wrapper_console_TLM.h"
#include "wrapper_processor_TLM.h"
#include "wrapper_coProcessor_TLM.h"
#include "include/SimpleBusLT.h"
#include "include/busLT_standalone_definition.h"


#define RAMSIZE 0x400

// Variable gloable
bool m_bError = false;

///////////////////////////////////////////////////////////////////////////////
//
//	Entr�e principale de l'application
//
///////////////////////////////////////////////////////////////////////////////
int sc_main(int arg_count, char **arg_value)
{
	// Variable
	time_t simulation_time_begin = 0;
	time_t simulation_time_end = 0;
	int sim_units = 2; //SC_NS 
	
	// Horloge
	sc_clock clk( "SysClock", 40, SC_NS, 0.5 );

	DataRAM instance_dataram("instance_dataram", "src\\database.txt", RAMSIZE);
	InstRAM instance_instram("instance_instram", "Compilateur_ISS\\app.elf", RAMSIZE);
	Console instance_console("instance_console");
	processor instance_processor("instance_processor", RAMSIZE, RAMSIZE);
	CoProcessor instance_coprocessor("instance_coprocessor");
	wrapper_console_TLM instance_wrapper_console("instance_wrapper_console", ADRESSE_CONSOLE_DEBUT, ADRESSE_CONSOLE_FIN);
	wrapper_coProcessor_TLM instance_wrapper_coProcessor("instance_wrapper_coProcessor", ADRESSE_COPROCESSOR_DEBUT, ADRESSE_COPROCESSOR_FIN);
	wrapper_processor_TLM instance_wrapper_Processor("instance_wrapper_Processor");
	SimpleBusLT<2> instance_simplebus("instance_simplebus");

	instance_console.ClockPort(clk);
	instance_processor.ClockPort(clk);
	instance_coprocessor.ClockPort(clk);
	instance_wrapper_Processor.ClockPort(clk);

	/* Connexions entre wrapper Processeur et Processeur */
	sc_signal<unsigned int>	pont_Processor_Data_OutPort;
	sc_signal<unsigned int>	pont_Processor_Data_InPort;
	sc_signal<unsigned int>	pont_Processor_Address_OutPort;
	sc_signal<bool>			pont_Processor_Enable_OutPort;
	sc_signal<bool>			pont_Processor_Ready_InPort;
	sc_signal<bool>			pont_Processor_RW_OutPort;
	
	instance_processor.InstPort(instance_instram);
	instance_processor.DataPort(instance_dataram);

	instance_processor.Processor_Data_OutPort(pont_Processor_Data_OutPort);
	instance_wrapper_Processor.Wrapper_Data_InPort(pont_Processor_Data_OutPort);

	instance_processor.Processor_Data_InPort(pont_Processor_Data_InPort);
	instance_wrapper_Processor.Wrapper_Data_OutPort(pont_Processor_Data_InPort);

	instance_processor.Processor_Address_OutPort(pont_Processor_Address_OutPort);
	instance_wrapper_Processor.Wrapper_Address_InPort(pont_Processor_Address_OutPort);

	instance_processor.Processor_Enable_OutPort(pont_Processor_Enable_OutPort);
	instance_wrapper_Processor.Wrapper_Enable_InPort(pont_Processor_Enable_OutPort);

	instance_processor.Processor_Ready_InPort(pont_Processor_Ready_InPort);
	instance_wrapper_Processor.Wrapper_Ready_OutPort(pont_Processor_Ready_InPort);

	instance_processor.Processor_RW_OutPort(pont_Processor_RW_OutPort);
	instance_wrapper_Processor.Wrapper_RW_InPort(pont_Processor_RW_OutPort);

	/* Connexions entre wrapper CoProcesseur et CoProcesseur */
	sc_signal<unsigned int>	pont_CoProcessor_Data_OutPort;
	sc_signal<unsigned int>	pont_CoProcessor_Data_InPort;
	sc_signal<bool>			pont_CoProcessor_Enable_InPort;
	sc_signal<bool>			pont_CoProcessor_Ready_OutPort;
	sc_signal<bool>			pont_CoProcessor_RW_InPort;
	
	instance_coprocessor.CoProcessor_Data_OutPort(pont_CoProcessor_Data_OutPort);
	instance_wrapper_coProcessor.Wrapper_CoProcessor_Data_InPort(pont_CoProcessor_Data_OutPort);

	instance_coprocessor.CoProcessor_Data_InPort(pont_CoProcessor_Data_InPort);
	instance_wrapper_coProcessor.Wrapper_CoProcessor_Data_OutPort(pont_CoProcessor_Data_InPort);

	instance_coprocessor.CoProcessor_Enable_InPort(pont_CoProcessor_Enable_InPort);
	instance_wrapper_coProcessor.Wrapper_CoProcessor_Enable_OutPort(pont_CoProcessor_Enable_InPort);

	instance_coprocessor.CoProcessor_Ready_OutPort(pont_CoProcessor_Ready_OutPort);
	instance_wrapper_coProcessor.Wrapper_CoProcessor_Ready_InPort(pont_CoProcessor_Ready_OutPort);

	instance_coprocessor.CoProcessor_RW_InPort(pont_CoProcessor_RW_InPort);
	instance_wrapper_coProcessor.Wrapper_CoProcessor_RW_OutPort(pont_CoProcessor_RW_InPort);

	/* Connexions de wrapper Console et de la console */
	sc_signal<unsigned int>	pont_Console_Data_InPort;
	sc_signal<bool>			pont_Console_Enable_InPort;
	sc_signal<bool>			pont_Console_Ready_OutPort;

	instance_console.Console_Data_InPort(pont_Console_Data_InPort);
	instance_wrapper_console.Wrapper_Console_Data_OutPort(pont_Console_Data_InPort);

	instance_console.Console_Enable_InPort(pont_Console_Enable_InPort);
	instance_wrapper_console.Wrapper_Console_Enable_OutPort(pont_Console_Enable_InPort);

	instance_console.Console_Ready_OutPort(pont_Console_Ready_OutPort);
	instance_wrapper_console.Wrapper_Console_Ready_InPort(pont_Console_Ready_OutPort);

	/* Bind les targets sockets au initiators correspondant */
	instance_wrapper_Processor.socket.bind(instance_simplebus.target_socket);
	instance_simplebus.DataPort(instance_dataram);
	instance_simplebus.initiator_socket[0]->bind(instance_wrapper_coProcessor.socket);
	instance_simplebus.initiator_socket[1]->bind(instance_wrapper_console.socket);
	
	// D�marrage de l'application
	if (!m_bError)
	{
		cout << "D�marrage de la simulation." << endl;
		time( &simulation_time_begin );
		sc_start( -1, sc_core::sc_time_unit(sim_units) );
		time( &simulation_time_end );

		cout << endl << "Simulation s'est termin�e � " << sc_time_stamp();
		cout << endl << "Dur�e de la simulation: " << (unsigned long)(simulation_time_end - simulation_time_begin) << " secondes." << endl << endl;
	}
	// Fin du programme
	system("PAUSE");
	return 0;
}
