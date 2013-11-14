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
	
	Control instance_control("instance_control",DICTIONARYSIZE,NUMBEROFERRORSMAXIMUM);
	Print instance_print("instance_print");
	Data instance_data("instance_data");
	User instance_user("instance_user");

	instance_control.dataPort(instance_data);
	instance_control.printPort(instance_print);
	instance_user.readPort(instance_control);

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
