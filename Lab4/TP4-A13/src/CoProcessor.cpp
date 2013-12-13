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

	MemoireInterne = 0;
	actualNumberGoodLetters = NumLettres = 0;
	actualNumberOfErrors = MaxErreurs = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Destructeur
//
///////////////////////////////////////////////////////////////////////////////
CoProcessor::~CoProcessor()
{
	if(MemoireInterne)
	{
		delete MemoireInterne;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	thread
//
///////////////////////////////////////////////////////////////////////////////
void CoProcessor::thread(void)
{
	unsigned int addr, data, index = 0;

	srand(time(NULL));
	unsigned NumCaracteresLus = 0;

	reg[2] = (0x0000); // État Initial

	while (1)
	{
		// Attendre que le Wrapper soit prêt
		while( CoProcessor_Enable_InPort.read() == false )
			wait(1);

		if( CoProcessor_RW_InPort.read() == false ) // Écriture
		{
			addr = CoProcessor_Data_InPort.read();

			// Fin de la lecture de l'adresse
			CoProcessor_Ready_OutPort.write( true );

			// Propagation des signaux
			wait(CoProcessor_Enable_InPort.default_event());

			CoProcessor_Ready_OutPort.write( false );
			
			data = CoProcessor_Data_InPort.read();

			switch(addr)
				{
				case 8192: // Réception de la quantité de caractères ou d’erreur
					if(NumLettres == 0) // indique la quantité de caractères dans le mot
					{
						reg[2] = (0x0001); // Réception des informations de début de partie
						NumLettres = data;
						reg[0] = (NumLettres);
						actualWord = new unsigned char[NumLettres+1];
						MemoireInterne = new unsigned char[NumLettres+1];
						memset(actualWord, '_', NumLettres+1);
						memset(MemoireInterne, '\0', NumLettres+1);
					}
					else if(MaxErreurs == 0) // indique le nombre d’erreurs maximum permis pour cette partie
					{
						MaxErreurs = data;
						reg[1] = (MaxErreurs);
					}
					break;
				case 8193: // Réception d’un caractère d’une chaîne  
					if(NumCaracteresLus < NumLettres) // le  coprocesseur  doit recevoir  les  caractères  de façon successive un après l’autre
					{
						MemoireInterne[NumCaracteresLus] = (unsigned char)data;
						NumCaracteresLus++;
					}
					if(NumCaracteresLus == NumLettres)
					{
						reg[2] = (0x0002); // La partie est en cours
					}
					break;
				default: break;
				}
		}
		else // Lecture
		{
			addr = CoProcessor_Data_InPort.read();

			// Fin de la lecture de l'adresse
			CoProcessor_Ready_OutPort.write( true );

			switch(addr)
			{
			case 8194: // Lecture de l’état du coprocesseur 
				TryNewLetter(MemoireInterne, NumLettres, MaxErreurs);
				if(NumLettres == actualNumberGoodLetters)
				{
					reg[2] = (0x0003); // La partie est terminée (Gagnée)  
				}
				else if(actualNumberOfErrors == MaxErreurs)
				{
					reg[2] = (0x0004); // La partie est terminée (Perdue)  
				}
				CoProcessor_Data_OutPort.write( reg[2].read() );
				break;
			case 8195: // Lecture de la valeur du coprocesseur 
				if(index == NumLettres)
				{
					index = 0;
				}
				else
				{
					CoProcessor_Data_OutPort.write( (unsigned char)actualWord[ index ] );
					index++;
				}
				break;
			case 8196:
				CoProcessor_Data_OutPort.write( actualNumberOfErrors );
				index = 0;
				break;
			default: break;
			}

			// Propagation des signaux
			wait(CoProcessor_Enable_InPort.default_event());

			CoProcessor_Ready_OutPort.write( false );

		}
		// Propagation des signaux
		wait(CoProcessor_Enable_InPort.default_event());
	}
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
