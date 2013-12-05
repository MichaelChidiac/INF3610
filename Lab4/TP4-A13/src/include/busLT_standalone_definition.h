///////////////////////////////////////////////////////////////////////////////
//
//	busLT_standalone_definition.h
//
///////////////////////////////////////////////////////////////////////////////

#include <tlm.h>

#ifndef busLT_standalone_definition_
#define busLT_standalone_definition_

#define PROCESSOR_TARGET_ID						0
#define COPROCESSOR_INITIATOR_ID				0
#define CONSOLE_INITIATOR_ID					1

#define DATA_TRANSACTION_SIZE					4

//Mettez ici les plages d'adresse des esclaves busLT
#define ADRESSE_CONSOLE_DEBUT      0xFFF
#define ADRESSE_CONSOLE_FIN        0x1002

#define ADRESSE_COPROCESSOR_DEBUT				0x2000
#define ADRESSE_COPROCESSOR_FIN					0x3000

typedef tlm::tlm_generic_payload                 				transaction_type;
typedef tlm::tlm_phase                           				phase_type;
typedef tlm::tlm_sync_enum                   				    sync_enum_type;
typedef tlm::tlm_command										command_type;

#endif /* busLT_standalone_definition_ */

