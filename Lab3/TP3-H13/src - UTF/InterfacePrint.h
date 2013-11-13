///////////////////////////////////////////////////////////////////////////////
//
//	InterfacePrint.h
//
///////////////////////////////////////////////////////////////////////////////
#ifndef INTERFACE_PRINT_H_
#define INTERFACE_PRINT_H_

#include <systemc.h>

///////////////////////////////////////////////////////////////////////////////
//
//	Classe InterfacePrint
//
///////////////////////////////////////////////////////////////////////////////
class InterfacePrint : public virtual sc_interface
{
	public:

		// Méthode
		virtual void EndGame(bool winner) = 0; //Fin de partie
		virtual void SetWordLength(unsigned int wordLength) = 0; //Assigne la longueur du mot
		virtual void CharacterFound(unsigned int position, unsigned char charFound)=0;//Une lettre a été trouvé
		virtual void AddError()=0; //Augmente le nombre d'erreurs
		virtual void UpdatePrint()=0; //Affichage pour mettre à jour l'utilisateur
};

#endif
