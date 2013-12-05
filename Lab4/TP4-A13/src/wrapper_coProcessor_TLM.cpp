///////////////////////////////////////////////////////////////////////////////
//
//	wrapper_coProcessor_TLM.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include "wrapper_coProcessor_TLM.h"

///////////////////////////////////////////////////////////////////////////////
//
//	Constructeur
//
///////////////////////////////////////////////////////////////////////////////
wrapper_coProcessor_TLM::wrapper_coProcessor_TLM(sc_module_name zName, unsigned long ulStartAdress, unsigned long ulEndAdress)
: sc_module(zName)
{
	//Assigne les variables
	m_ulStartAdress = ulStartAdress;
	m_ulEndAdress = ulEndAdress;
	/*
		À compléter
	*/
}


///////////////////////////////////////////////////////////////////////////////
//
//	Destructeur
//
///////////////////////////////////////////////////////////////////////////////
wrapper_coProcessor_TLM::~wrapper_coProcessor_TLM()
{
}

///////////////////////////////////////////////////////////////////////////////
//
//	b_transport : répond aux requêtes envoyées par le initiatot
//
///////////////////////////////////////////////////////////////////////////////
void wrapper_coProcessor_TLM::b_transport( transaction_type& trans, sc_time& delay )
{
	command_type cmd = trans.get_command();
	sc_dt::uint64    add = trans.get_address();
	unsigned char*   ptr = trans.get_data_ptr();
	unsigned int     len = trans.get_data_length();
	unsigned char*   byt = trans.get_byte_enable_ptr();
	unsigned int     wid = trans.get_streaming_width();
	sc_time dly = sc_time(30, SC_NS);

	//Vérification 
	if (byt != 0 || wid < len)
	  SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");
	
	// On vérifie si la requête est de lecture ou écriture
	if ( cmd == tlm::TLM_READ_COMMAND ){
		busLT_slave_read(add, ptr, len);
	}
	else if ( cmd == tlm::TLM_WRITE_COMMAND ){
		busLT_slave_write(add, ptr, len);
	}
	
	//Temps d'attente
	delay += dly;
	
	// Aquittement
	trans.set_response_status( tlm::TLM_OK_RESPONSE );
	
}
///////////////////////////////////////////////////////////////////////////////
//
//	busLT_slave_read
//
///////////////////////////////////////////////////////////////////////////////
void wrapper_coProcessor_TLM::busLT_slave_read(sc_dt::uint64 add, unsigned char* ptrData, unsigned int len)
{
	unsigned int data ;
	
	/*
		À compléter
	*/
	
	memcpy(ptrData, &data, len);
}


///////////////////////////////////////////////////////////////////////////////
//
//	opb_slave_write
//
///////////////////////////////////////////////////////////////////////////////
void wrapper_coProcessor_TLM::busLT_slave_write(sc_dt::uint64 add, unsigned char* ptrData, unsigned int     len)
{
	unsigned int data ;
	memcpy(&data, ptrData, len);

	/*
		À compléter
	*/
}


