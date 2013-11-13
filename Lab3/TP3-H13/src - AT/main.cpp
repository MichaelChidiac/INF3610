///////////////////////////////////////////////////////////////////////////////
//
//	main.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include <systemc.h>
#include "Central.h"
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
	sc_clock clk("clk", 4000, SC_NS, 0.5);

	/*

	À compléter

	*/

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
