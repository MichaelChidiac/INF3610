///////////////////////////////////////////////////////////////////////////////
//
//	InterfaceData.h
//
///////////////////////////////////////////////////////////////////////////////
#ifndef INTERFACE_DATA_H_
#define INTERFACE_DATA_H_

#include <systemc.h>

///////////////////////////////////////////////////////////////////////////////
//
//	Classe InterfaceData
//
///////////////////////////////////////////////////////////////////////////////
class InterfaceData : public virtual sc_interface
{
	public:

		// M�thode
		virtual unsigned int GetWord(unsigned int wordPosition) = 0; //Return la longueur du mot s�lectionn�
		virtual unsigned char GetChar(unsigned int charPosition) = 0; //Return la lettre data[charPosition]
};

#endif
