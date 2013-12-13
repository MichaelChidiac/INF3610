///////////////////////////////////////////////////////////////////////////////
//
//	Console.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include "Console.h"


///////////////////////////////////////////////////////////////////////////////
//
//	Constructeur
//
///////////////////////////////////////////////////////////////////////////////
Console::Console( sc_module_name zName )
: sc_module(zName)
{
	SC_THREAD(thread);
	sensitive<< ClockPort.pos();
		
}


///////////////////////////////////////////////////////////////////////////////
//
//	Destructeur
//
///////////////////////////////////////////////////////////////////////////////
Console::~Console()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//	thread
//
///////////////////////////////////////////////////////////////////////////////
void Console::thread(void)
{
	unsigned int addr, data;
	bool bAfficheString = true;

	while(1)
	{
		if(Console_Enable_InPort.read() == true)
		{
			// Adresse de la console
			addr = Console_Data_InPort.read();

			// Occupé 
			Console_Ready_OutPort.write( false );

			// Next transmission
			wait(Console_Enable_InPort.default_event()); ;		

			data = Console_Data_InPort.read();

			switch(addr)
			{
			case 4095: // Réception d’un caractère d’une chaîne
				if(bAfficheString)
				{
					cout << "Mot actuel: ";
					bAfficheString = false;
				}
				putchar(data);
				break;
			case 4096: // Réception de la quantité d’erreurs 
				cout << endl << "Nombre d'erreurs : " << data << endl;
				bAfficheString = true;
				break;
			case 4097: // Demande d’affichage de la fonction PrintWin()
				PrintWin();
				break;
			case 4098: // Demande d’affichage de la fonction PrintLose()
				PrintLose();
				break;
			default:
				break;
			}
			Console_Ready_OutPort.write( true );
		}
		Console_Ready_OutPort.write( true );
		wait(Console_Enable_InPort.default_event()); 
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	PrintWin
//
///////////////////////////////////////////////////////////////////////////////
void Console::PrintWin()
{
    cout << "@@@               @@@      @@@@@         @@@@@         @@@@"  << endl;
	cout << "@@@      @@@      @@@      @@@@@         @@@@@@@       @@@@"  << endl;
	cout << " @@@     @@@     @@@                     @@@@ @@@      @@@@"  << endl;
	cout << "  @@@    @@@    @@@        @@@@@         @@@@   @@@    @@@@"  << endl;
	cout << "   @@@   @@@   @@@         @@@@@         @@@@     @@@  @@@@"  << endl;
	cout << "   @@@@@@@@@@@@@@@         @@@@@         @@@@      @@@ @@@@"  << endl;
	cout << "    @@@@@@ @@@@@@          @@@@@         @@@@       @@@@@@@"  << endl;
	cout << "     @@@@   @@@@           @@@@@         @@@@         @@@@@"  << endl;

}


///////////////////////////////////////////////////////////////////////////////
//
//	PrintLose
//
///////////////////////////////////////////////////////////////////////////////
void Console::PrintLose()
{ 
	cout << "@@@@          @@@@@@@@@@@@  @@@@@@@@@@@   @@@@@@@@@@@@@    "  << endl;
	cout << "@@@@          @@@@@@@@@@@@  @@@@@@@@@@@   @@@@@@@@@@@@@   "  << endl;
	cout << "@@@@          @@@@    @@@@  @@@@@         @@@@            "  << endl;
	cout << "@@@@          @@@@    @@@@  @@@@@@@@@@@   @@@@@@@@        "  << endl;
	cout << "@@@@          @@@@    @@@@  @@@@@@@@@@@   @@@@@@@@        "  << endl;
	cout << "@@@@          @@@@    @@@@         @@@@   @@@@            "  << endl;
	cout << "@@@@@@@@@@@@  @@@@@@@@@@@@  @@@@@@@@@@@   @@@@@@@@@@@@@   "  << endl;
	cout << "@@@@@@@@@@@@  @@@@@@@@@@@@  @@@@@@@@@@@   @@@@@@@@@@@@@   "  << endl;
}