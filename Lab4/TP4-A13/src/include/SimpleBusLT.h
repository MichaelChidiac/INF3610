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

template <int NR_OF_INITIATORS, int NR_OF_TARGETS>

//Class SimpleBusLT
class SimpleBusLT : public sc_core::sc_module
{
	typedef tlm_utils::simple_target_socket_tagged<SimpleBusLT>		target_socket_type;
	typedef tlm_utils::simple_initiator_socket_tagged<SimpleBusLT>	initiator_socket_type;
	
	public:
		target_socket_type target_socket[NR_OF_INITIATORS];
		initiator_socket_type initiator_socket[NR_OF_TARGETS];

		SC_HAS_PROCESS(SimpleBusLT);	
		//	Constructeur
		SimpleBusLT(sc_core::sc_module_name name) : sc_module(name)
		{
			/*
				À compléter
			*/
		}
		
		//	Decode
		unsigned int decode(const sc_dt::uint64& address)
		{
			/*
				À compléter
			*/

		}

		//	Interface method(LT protocol)
		//	Forward each call to the target/initiator
		void initiatorBTransport(int SocketId, transaction_type& trans, sc_time& t)
		{	
			/*
				À compléter
			*/
		}
				

};

#endif
