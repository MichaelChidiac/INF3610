///////////////////////////////////////////////////////////////////////////////
//
//	CoProcessor.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include "CoProcessor.h"
#include <iostream>
#include <vector>
#include <time.h>
using namespace std;


///////////////////////////////////////////////////////////////////////////////
//
//	Constructeur
//
///////////////////////////////////////////////////////////////////////////////
CoProcessor::CoProcessor( sc_module_name zName )
: sc_module(zName)
{
	SC_THREAD(thread);
	sensitive << ClockPort.pos();
		
	/*
		À compléter
	*/
}


///////////////////////////////////////////////////////////////////////////////
//
//	Destructeur
//
///////////////////////////////////////////////////////////////////////////////
CoProcessor::~CoProcessor()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//	thread
//
///////////////////////////////////////////////////////////////////////////////
void CoProcessor::thread(void)
{
	srand(time(NULL));
	/*
		À compléter
	*/
}

///////////////////////////////////////////////////////////////////////////////
//
//	TryNewLetter
//
///////////////////////////////////////////////////////////////////////////////
void CoProcessor::TryNewLetter(unsigned char* motRecherche, unsigned int longueurMot, unsigned int maximumOfErrors)
{
	//Rand letter.
	//Test in the word
	
	unsigned char temp = 'a' + (rand()%26);
	bool foundLetter = false;
	bool incrementError = false;
	for(int i = 0; i < longueurMot ; i++)
	{
		if(motRecherche[i] == temp)
		{
			if(actualWord[i] != temp)
			{
				actualWord[i] = temp;
				actualNumberGoodLetters ++;
				foundLetter = true;
			}
			else
			{
				actualNumberOfErrors++;
				incrementError = true;
				break;
			}
		}
	}

	if(!incrementError && !foundLetter)
		actualNumberOfErrors++;

	if(longueurMot == actualNumberGoodLetters)
	{
		gameIsOver = true;
		winnedGame = true;
	}
	else if(actualNumberOfErrors == maximumOfErrors)
	{
		gameIsOver = true;
	}


}
