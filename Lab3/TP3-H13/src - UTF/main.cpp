///////////////////////////////////////////////////////////////////////////////
//
//	main.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include <systemc.h>
#include "Control.h"
#include "Print.h"
#include "Data.h"
#include "User.h"

const int DICTIONARYSIZE = 30;
const int NUMBEROFERRORSMAXIMUM = 5;
// Global variables
bool m_bError = false;

///////////////////////////////////////////////////////////////////////////////
//
//	Main
//
///////////////////////////////////////////////////////////////////////////////
int sc_main(int arg_count, char **arg_value)
{
	// Variables
	int sim_units = 2; //SC_NS 
	
	sc_module_name control_name("instance_control");
	sc_module_name print_name("instance_print");
	sc_module_name data_name("instance_data");
	Control instance_control(control_name,DICTIONARYSIZE,NUMBEROFERRORSMAXIMUM);
	Print instance_print(print_name);
	Data instance_data(data_name);

	instance_control.dataPort(instance_data);
	instance_control.printPort(instance_print);

	// Démarrage de l'application
	if (!m_bError)
	{
		cout << "Démarrage de la simulation." << endl;
		sc_start( -1, sc_core::sc_time_unit(sim_units) );
		cout << endl << "Simulation s'est terminée à " << sc_time_stamp() << " ns";
	}
	// Fin du programme
	return 0;
}
