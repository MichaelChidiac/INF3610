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

	reg[2] = (0x0000); // �tat Initial

	while (1)
	{
		// Attendre que le Wrapper soit pr�t
		while( CoProcessor_Enable_InPort.read() == false )
			wait(1);

		if( CoProcessor_RW_InPort.read() == false ) // �criture
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
				case 8192: // R�ception�de�la�quantit�de�caract�res�ou�d�erreur
					if(NumLettres == 0) // indique�la�quantit�de�caract�res�dans�le�mot
					{
						reg[2] = (0x0001); // R�ception�des�informations�de�d�but�de�partie
						NumLettres = data;
						reg[0] = (NumLettres);
						actualWord = new unsigned char[NumLettres+1];
						MemoireInterne = new unsigned char[NumLettres+1];
						memset(actualWord, '_', NumLettres+1);
						memset(MemoireInterne, '\0', NumLettres+1);
					}
					else if(MaxErreurs == 0) // indique�le�nombre�d�erreurs�maximum�permis�pour�cette�partie
					{
						MaxErreurs = data;
						reg[1] = (MaxErreurs);
					}
					break;
				case 8193: // R�ception�d�un�caract�re�d�une�cha�ne� 
					if(NumCaracteresLus < NumLettres) // le� coprocesseur� doit�recevoir� les� caract�res� de�fa�on�successive�un�apr�s�l�autre
					{
						MemoireInterne[NumCaracteresLus] = (unsigned char)data;
						NumCaracteresLus++;
					}
					if(NumCaracteresLus == NumLettres)
					{
						reg[2] = (0x0002); // La�partie�est�en�cours
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
			case 8194: // Lecture�de�l��tat�du�coprocesseur�
				TryNewLetter(MemoireInterne, NumLettres, MaxErreurs);
				if(NumLettres == actualNumberGoodLetters)
				{
					reg[2] = (0x0003); // La�partie�est�termin�e�(Gagn�e)� 
				}
				else if(actualNumberOfErrors == MaxErreurs)
				{
					reg[2] = (0x0004); // La�partie�est�termin�e�(Perdue)� 
				}
				CoProcessor_Data_OutPort.write( reg[2].read() );
				break;
			case 8195: // Lecture�de�la�valeur�du�coprocesseur�
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
