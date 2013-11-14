///////////////////////////////////////////////////////////////////////////////
//
//	User.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include "User.h"
#include <conio.h>



///////////////////////////////////////////////////////////////////////////////
//
//	Constructeur
//
///////////////////////////////////////////////////////////////////////////////
User::User( sc_module_name zName )
: sc_module(zName)
{
	SC_THREAD(Process); 
	sensitive << clk.pos();
}


///////////////////////////////////////////////////////////////////////////////
//
//	Destructeur
//
///////////////////////////////////////////////////////////////////////////////
User::~User()
{
}


void User::Process()
{
	cout<< "Appuyez sur Enter pour commencer une partie" <<endl;
	getchar();
	bool gameEnded = false;

	do{
		wait(clk.posedge_event());
	} while(!requestStartPort.read());

	startGamePort.write(true);

	// Synchronisation
	do{
		wait(clk.posedge_event());
	} while(!ackPort.read());

	while(!gameEnded)
	{	
		requestLetterPort.write(true);
		
		do{
			wait(clk.posedge_event());
		} while(!ackPort.read());

		requestLetterPort.write(false);

		cout<< "Entrez une lettre" <<endl;
		char input;
		std::string temp;
		cin >> temp;
		input = temp[0];

		incomingLetterPort.write(input);
		
		do{
			wait(clk.posedge_event());
		} while(!ackPort.read());
		
		gameEnded = endGamePort.read();
	}	
		
	cout<< "Appuyez sur Enter pour terminer la partie" <<endl;
	getch();
	sc_stop();
}