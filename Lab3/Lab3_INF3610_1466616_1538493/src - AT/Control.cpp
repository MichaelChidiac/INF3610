///////////////////////////////////////////////////////////////////////////////
//
//	Control.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include "Control.h"
#include <time.h>


///////////////////////////////////////////////////////////////////////////////
//
//	Constructeur
//
///////////////////////////////////////////////////////////////////////////////
Control::Control( sc_module_name zName, unsigned int DictSize, unsigned int maxErrors )
: sc_module(zName)
{
	SC_THREAD(Threading); 
	DictSize_ = DictSize;
	maxErrors_ = maxErrors;
	nbErrors_ = 0;
	for (int i = 0; i < 20; i++){
		foundBool_[i] = false;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	Destructeur
//
///////////////////////////////////////////////////////////////////////////////
Control::~Control()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//	Threading
//
//
///////////////////////////////////////////////////////////////////////////////
void Control::Threading(void)
{
	bool startRequest;
	bool found = false;
	bool win = true;
	char a;

	requestStartPort.write(true);

	do{
		wait(clk.posedge_event());
	} while(!startGamePort.read());

	// Synchronisation
	ackPort.write(true);

	int number;
	srand ( time(NULL) );
	number = rand() % DictSize_;
	wordLength_ = dataPort->GetWord(number);
	for (int i = 0; i < wordLength_; i++) 
		word_[i] = dataPort->GetChar(i);
	printPort->SetWordLength(wordLength_);
	
	endGamePort.write(false);
	do
	{
		printPort->UpdatePrint();
		found = false;
		win = true;
		do {
			wait(clk.posedge_event());
		} while(!requestLetterPort.read());

		// Synchronisation
		ackPort.write(true);
		ackPort.write(false);
		do {
			wait(clk.posedge_event());
		} while(!(a = incomingLetterPort.read()));

		//ackPort.write(false);

		for (int i = 0; i < wordLength_; i++)
		{
			a = incomingLetterPort.read();
			if(word_[i] == a)
			{
				printPort->CharacterFound(i,a);
				foundBool_[i] = true;
				found = true;
			}
		}

		if (found == false)
		{
			printPort->AddError(); 
			nbErrors_++;
		}	
			
		for (int i = 0; i < wordLength_; i++)
		{
			if (foundBool_[i] == false)
				win = false;
		}
			
		if (win == true)
		{
			printPort->EndGame(true);
			endGamePort.write(true);
		}

		else if (nbErrors_ == maxErrors_)
		{
			printPort->EndGame(false);
			win = true;
			endGamePort.write(true);
		}
		ackPort.write(true);

	} while(!win);
}


