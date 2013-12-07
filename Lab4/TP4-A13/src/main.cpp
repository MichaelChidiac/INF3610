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
//	Entrée principale de l'application
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

	sc_signal<unsigned int> pont_Data_CoProcessor_OutPort;
	sc_signal<unsigned int> pont_Data_CoProcessor_InPort;
	sc_signal<unsigned int> pont_Data_Processor_OutPort;
	sc_signal<unsigned int> pont_Data_Processor_InPort;

	sc_signal<bool>			pont_Enable_CoProcessor_Port;
	sc_signal<bool>			pont_Enable_Processor_Port;
	sc_signal<bool>			pont_Enable_Console_Port;

	sc_signal<bool>			pont_Ready_Console_Port;
	sc_signal<bool>			pont_Ready_CoProcessor_Port;
	sc_signal<bool>			pont_Ready_Processor_Port;

	sc_signal<bool>			pont_RW_CoProcessor_Port;
	sc_signal<bool>			pont_RW_Processor_Port;

	Console instance_console("instance_console");
	processor instance_processor("instance_processor");
	CoProcessor instance_coprocessor("instance_coprocessor");
	wrapper_console_TLM instance_wrapper_console("instance_wrapper_console", ADRESSE_CONSOLE_DEBUT, ADRESSE_CONSOLE_FIN);
	wrapper_coProcessor_TLM instance_wrapper_coProcessor("instance_wrapper_coProcessor", ADRESSE_COPROCESSOR_DEBUT, ADRESSE_COPROCESSOR_FIN);
	wrapper_processor_TLM instance_wrapper_Processor("instance_wrapper_Processor");
	SimpleBusLT<2,1> instance_simplebus("instance_simplebus");

	instance_coprocessor.CoProcessor_Data_InPort(pont_Data_CoProcessor_InPort);
	instance_wrapper_coProcessor.Wrapper_CoProcessor_Data_OutPort(pont_Data_CoProcessor_InPort);

	instance_coprocessor.CoProcessor_Data_OutPort(pont_Data_CoProcessor_OutPort);
	instance_wrapper_coProcessor.Wrapper_CoProcessor_Data_InPort(pont_Data_CoProcessor_OutPort);

	instance_processor.Processor_Data_InPort(pont_Data_CoProcessor_InPort);
	instance_wrapper_Processor.Wrapper_Data_OutPort(pont_Data_CoProcessor_InPort);

	instance_processor.Processor_Data_OutPort(pont_Data_CoProcessor_OutPort);
	instance_wrapper_Processor.Wrapper_Data_InPort(pont_Data_CoProcessor_OutPort);

	instance_coprocessor.CoProcessor_Enable_InPort(pont_Enable_CoProcessor_Port);
	instance_wrapper_coProcessor.Wrapper_CoProcessor_Enable_OutPort(pont_Enable_CoProcessor_Port);

	instance_processor.Processor_Enable_OutPort(pont_Enable_Processor_Port);
	instance_wrapper_Processor.Wrapper_Enable_InPort(pont_Enable_Processor_Port);

	instance_console.Console_Enable_InPort(pont_Enable_Console_Port);
	instance_wrapper_console.Wrapper_Console_Enable_OutPort(pont_Enable_Console_Port);

	instance_console.Console_Ready_OutPort(pont_Ready_Console_Port);
	instance_wrapper_console.Wrapper_Console_Ready_InPort(pont_Ready_Console_Port);

	instance_coprocessor.CoProcessor_Ready_OutPort(pont_Ready_CoProcessor_Port);
	instance_wrapper_coProcessor.Wrapper_CoProcessor_Ready_InPort(pont_Ready_CoProcessor_Port);

	instance_processor.Processor_Ready_InPort(pont_Ready_Processor_Port);
	instance_wrapper_Processor.Wrapper_Ready_OutPort(pont_Ready_Processor_Port);

	instance_coprocessor.CoProcessor_RW_InPort(pont_RW_CoProcessor_Port);
	instance_wrapper_coProcessor.Wrapper_CoProcessor_RW_OutPort(pont_RW_CoProcessor_Port);

	instance_processor.Processor_RW_OutPort(pont_RW_Processor_Port);
	instance_wrapper_Processor.Wrapper_RW_InPort(pont_RW_Processor_Port);

	instance_wrapper_Processor.socket.bind(instance_simplebus.target_socket[0]);
	instance_simplebus.initiator_socket[0].bind(instance_wrapper_coProcessor.socket);
	instance_simplebus.initiator_socket[1].bind(instance_wrapper_console.socket);
	
	// Démarrage de l'application
	if (!m_bError)
	{
		cout << "Démarrage de la simulation." << endl;
		time( &simulation_time_begin );
		sc_start( -1, sc_core::sc_time_unit(sim_units) );
		time( &simulation_time_end );

		cout << endl << "Simulation s'est terminée à " << sc_time_stamp();
		cout << endl << "Durée de la simulation: " << (unsigned long)(simulation_time_end - simulation_time_begin) << " secondes." << endl << endl;
	}
	// Fin du programme
	return 0;
}
