///////////////////////////////////////////////////////////////////////////////
//
//	Print.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include "Print.h"


///////////////////////////////////////////////////////////////////////////////
//
//	Constructeur
//
///////////////////////////////////////////////////////////////////////////////
Print::Print( sc_module_name zName )
: sc_module(zName)
{
	
}


///////////////////////////////////////////////////////////////////////////////
//
//	Destructeur
//
///////////////////////////////////////////////////////////////////////////////
Print::~Print()
{
}

///////////////////////////////////////////////////////////////////////////////
//
//	EndGame
//
//  Appel PrintWin() et PrintLose() selon le bool
//
///////////////////////////////////////////////////////////////////////////////
void Print::EndGame(bool winner)
{
	/*

			� compl�ter

		*/
}

///////////////////////////////////////////////////////////////////////////////
//
//	SetWordLength
//
//  Initialise les variables
//  Assigne la valeur '_' � currentWord[]
//
///////////////////////////////////////////////////////////////////////////////
void Print::SetWordLength(unsigned int wordLength)
{
	/*

			� compl�ter

		*/
	nbError_ = 0;
	wordLength_ = wordLength;
	currentWord = "";
	for (int i = 0; i < wordLength; i++){
		currentWord[i] = '_';
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	CharacterFound
//
///////////////////////////////////////////////////////////////////////////////
void Print::CharacterFound(unsigned int position, unsigned char charFound)
{
	/*

			� compl�ter

		*/
}

///////////////////////////////////////////////////////////////////////////////
//
//	AddError
//
//  Incr�mente le nombre d'erreur
//
///////////////////////////////////////////////////////////////////////////////
void Print::AddError()
{
	/*

			� compl�ter

		*/
}

///////////////////////////////////////////////////////////////////////////////
//
//	UpdatePrint
//
//  Affiche le nombre d'erreurs actuel ainsi que le en cours (Ex. Mot : _ B _ _ _ )
//
///////////////////////////////////////////////////////////////////////////////
void Print::UpdatePrint()
{
	/*

		� compl�ter

	*/
}

///////////////////////////////////////////////////////////////////////////////
//
//	PrintWin
//
///////////////////////////////////////////////////////////////////////////////
void Print::PrintWin()
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
void Print::PrintLose()
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