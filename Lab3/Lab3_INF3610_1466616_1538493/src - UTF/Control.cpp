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
//	StartGame
//
// Utiliser un random pour la sélection du mot (Attention à DICTIONARYSIZE)
// Recherche caractère par caractère pour compléter le mot
// Initalise les variables de Print et fait un premier affichage
// Retourne false si la préparation de la partie s'est mal déroulée (Si la longueur du mot est vide)
//
///////////////////////////////////////////////////////////////////////////////
bool Control::StartGame(void)
{
	int number;
	srand ( time(NULL) );
	number = rand() % DictSize_;
	wordLength_ = dataPort->GetWord(number);
	if (wordLength_ == 0) return false;
	for (int i = 0; i < wordLength_; i++) word_[i] = dataPort->GetChar(i);
	printPort->SetWordLength(wordLength_);
	printPort->UpdatePrint();
	return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//	TryNewLetter
//
//  Retourne un false si la partie est terminé
//
///////////////////////////////////////////////////////////////////////////////
bool Control::TryNewLetter(char a)
{
	bool found = false;
	bool win = true;
	for (int i = 0; i < wordLength_; i++){
		if(word_[i] == a){
			printPort->CharacterFound(i,a);
			foundBool_[i] = true;
			found = true;
		}
	}
	if (found == false){
		printPort->AddError(); 
		nbErrors_++;
	}
	if (nbErrors_ == maxErrors_){
		printPort->EndGame(false);
		return false;
	}
	for (int i = 0; i < wordLength_; i++){
		if (foundBool_[i] == false)
			win = false;
	}
	if (win == true){
		printPort->EndGame(true);
		return false;
	}
	printPort->UpdatePrint();
	return true;
}

