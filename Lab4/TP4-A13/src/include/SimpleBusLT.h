///////////////////////////////////////////////////////////////////////////////
//
//	SimpleBusLT.h
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SIMPLEBUSLT_H__
#define __SIMPLEBUSLT_H__

#include "busLT_standalone_definition.h"
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <systemc.h>
#include <stdio.h>

template<unsigned int N_TARGETS>
class SimpleBusLT : public sc_core::sc_module
{
	typedef tlm_utils::simple_target_socket_tagged<SimpleBusLT>		target_socket_type;
	typedef tlm_utils::simple_initiator_socket_tagged<SimpleBusLT>*	initiator_socket_type;
	
	public:
		target_socket_type target_socket;
		initiator_socket_type initiator_socket[N_TARGETS];
		sc_port<DataRAM> DataPort;

		SC_HAS_PROCESS(SimpleBusLT);	
		//	Constructeur
		SimpleBusLT(sc_core::sc_module_name name) : sc_module(name), target_socket("socket")
		{
			target_socket.register_b_transport(this, &SimpleBusLT::initiatorBTransport, 0);
			for (unsigned int i = 0; i < N_TARGETS; i++)
			{
				char txt[20];
				sprintf(txt, "socket_%d", i);
				initiator_socket[i] = new tlm_utils::simple_initiator_socket_tagged<SimpleBusLT>(txt);
			}
		}

		//	Decode
		unsigned int decode(const sc_dt::uint64& address)
		{
			unsigned int target_nr;
			if(address >= ADRESSE_COPROCESSOR_DEBUT && address <= ADRESSE_COPROCESSOR_FIN)
				target_nr = 0;
			else if(address >= ADRESSE_CONSOLE_DEBUT && address <= ADRESSE_CONSOLE_FIN)
				target_nr = 1;
			return target_nr;
		}

		//	Interface method(LT protocol)
		//	Forward each call to the target/initiator
		void initiatorBTransport(int SocketId, transaction_type& trans, sc_time& t)
		{	
			unsigned int target_nr = target_nr = decode(trans.get_address());;
			// Forward transaction to appropriate target
			( *initiator_socket[target_nr] )->b_transport( trans, t );
		}
};

#endif
