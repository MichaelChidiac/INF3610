///////////////////////////////////////////////////////////////////////////////
//
//	wrapper_processor_TLM.h
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _WRAPPER_PROCESSOR_TLM_H_
#define _WRAPPER_PROCESSOR_TLM_H_

#include <systemc.h>
#include "include/busLT_standalone_definition.h"
#include <tlm.h>
#include "tlm_utils/simple_initiator_socket.h"


///////////////////////////////////////////////////////////////////////////////
//
//	Classe wrapper_processor_TLM
//
///////////////////////////////////////////////////////////////////////////////
class wrapper_processor_TLM : public sc_module
{
public:
    // Variable
    sc_in_clk				ClockPort;
	
	sc_in<unsigned int>		Wrapper_Data_InPort;
	sc_out<unsigned int>	Wrapper_Data_OutPort;
	sc_in<unsigned int> 	Wrapper_Address_InPort;
	sc_in<bool>				Wrapper_Enable_InPort;
    sc_out<bool>			Wrapper_Ready_OutPort;
	sc_in<bool>				Wrapper_RW_InPort;
    	
	// TLM-2 socket, defaults to 32-bits wide, base protocol
	tlm_utils::simple_initiator_socket<wrapper_processor_TLM> socket;

 	// Constructeur
	wrapper_processor_TLM(sc_module_name zName, bool bVerbose = false);
	
	// Destructeur
	~wrapper_processor_TLM(); 

private:
	// Thread
	SC_HAS_PROCESS( wrapper_processor_TLM );
	
	// Méthode
	void thread();
	bool busLT_write(unsigned long addressDes, void* Data32, unsigned long DataLength8);
	bool busLT_read(unsigned long addressDes, void* Data32, unsigned long DataLength8);
	
    // Variable
    bool m_bVerbose;
};

#endif
