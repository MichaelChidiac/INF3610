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

	// Register callback for incoming b_transport interface method call
    socket.register_b_transport(this, &wrapper_coProcessor_TLM::b_transport);
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
//	b_transport : r�pond aux requ�tes envoy�es par le initiatot
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

	//V�rification 
	if (byt != 0 || wid < len)
	  SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");
	
	// On v�rifie si la requ�te est de lecture ou �criture
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
	unsigned int data;

	// On indique que la transaction est une lecture 
	Wrapper_CoProcessor_RW_OutPort.write( true ); 
 
	// L'adresse du CoProcesseur
	Wrapper_CoProcessor_Data_OutPort.write( add );

	// On indique que la donn�e est pr�te 
	Wrapper_CoProcessor_Enable_OutPort.write( true ); 
 
	// Indique qu'il n'y a pas de donn�e valide
	wait(Wrapper_CoProcessor_Ready_InPort.default_event());

	Wrapper_CoProcessor_Enable_OutPort.write( false );

	// Lecture pour envoyer sur le bus
	data = Wrapper_CoProcessor_Data_InPort.read(); 

	// La lecteur a �t� effectu�
	wait(Wrapper_CoProcessor_Ready_InPort.default_event());
 
	memcpy(ptrData, &data, len);
}


///////////////////////////////////////////////////////////////////////////////
//
//	busLT_slave_write
//
///////////////////////////////////////////////////////////////////////////////
void wrapper_coProcessor_TLM::busLT_slave_write(sc_dt::uint64 add, unsigned char* ptrData, unsigned int     len)
{
	unsigned int data;
	memcpy(&data, ptrData, len);

	// Demande d'�criture
	Wrapper_CoProcessor_RW_OutPort.write( false );

	// L'addresse du CoProcesseur
	Wrapper_CoProcessor_Data_OutPort.write( add );

	// Donn�e valide
	Wrapper_CoProcessor_Enable_OutPort.write( true );

	// Acquitement de la r�ception
	wait(Wrapper_CoProcessor_Ready_InPort.default_event());

	Wrapper_CoProcessor_Enable_OutPort.write( false );

	// Donn�e�� �crire
	Wrapper_CoProcessor_Data_OutPort.write( data );

	// Inique qu'il n'y a plus de donn�e valide
	wait(Wrapper_CoProcessor_Ready_InPort.default_event());
}


