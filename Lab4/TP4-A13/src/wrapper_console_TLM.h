///////////////////////////////////////////////////////////////////////////////
//
//	wrapper_console_TLM.h
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _WRAPPER_CONSOLE_TLM_H_
#define _WRAPPER_CONSOLE_TLM_H_

#include <systemc.h>
#include "include/busLT_standalone_definition.h"
#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>


///////////////////////////////////////////////////////////////////////////////
//
//	Classe wrapper_console_TLM
//
///////////////////////////////////////////////////////////////////////////////
class wrapper_console_TLM : public sc_module
{
public:
    // Ports
  	/*
		À compléter
	*/


	// TLM-2 socket
	tlm_utils::simple_target_socket<wrapper_console_TLM> socket;
	
	// Constructeur
	wrapper_console_TLM(sc_module_name zName,unsigned long ulStartAdress, unsigned long ulEndAdress);

	// Destructeur
	~wrapper_console_TLM();


	// Méthode
	unsigned int busLT_start_address() const {return m_ulStartAdress;};
	unsigned int busLT_end_address() const {return m_ulEndAdress;};
	void busLT_slave_read(sc_dt::uint64 add, unsigned char* ptrData, unsigned int len);
	void busLT_slave_write(sc_dt::uint64 add, unsigned char* ptrData, unsigned int len);
	
	void b_transport(transaction_type& trans, sc_time& delay );

private:
	// Variable
	unsigned long m_ulStartAdress;
	unsigned long m_ulEndAdress;
};

#endif
