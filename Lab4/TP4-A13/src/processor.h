///////////////////////////////////////////////////////////////////////////////
//
//	processor.h
//
///////////////////////////////////////////////////////////////////////////////
#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include <systemc.h>
#include "stdio.h"
#include "Application_Definition.h"
#include "LMBIF.h"

// Classe pour les instructions
class cInst 
{
	public:
		unsigned int OPCODE;
		unsigned int RD;
		unsigned int RA;
		unsigned int RB;
		short IMM;
		
		// Pour le fonctionnement des étages
		unsigned int PC;
		int RESULT;
		unsigned int ADDR;

		friend ostream& operator<<(ostream& out, const cInst& inst)
		{
			out << 	" OPCODE: " << inst.OPCODE <<
					" RD: " 	<< inst.RD <<
					" RA: " 	<< inst.RA <<
					" RB: " 	<< inst.RB <<
					" IMM: " 	<< inst.IMM << endl;

			return out;
		}
}; 



///////////////////////////////////////////////////////////////////////////////
//
//	Classe processor
//
///////////////////////////////////////////////////////////////////////////////
class processor: public sc_module
{
	public: 
		
		// Variable
		sc_in_clk				ClockPort;    
		sc_port<LMBIF>			InstPort;
		sc_port<LMBIF>			DataPort;
		sc_out<unsigned int>	Processor_Data_OutPort;
		sc_in<unsigned int>		Processor_Data_InPort;
		sc_out<unsigned int>	Processor_Address_OutPort;
		sc_out<bool>			Processor_Enable_OutPort;
		sc_in<bool>				Processor_Ready_InPort;
		sc_out<bool>			Processor_RW_OutPort;
		
		// Constructeur
		processor( sc_module_name zName, unsigned long ulInstRAMSize = 0, unsigned long ulDataRAMSize = 0, bool bVerbose = false );
		
		// Destructeur
		~processor();

	private:	

		// Processus SystemC
		SC_HAS_PROCESS(processor);
		
		// Méthode
		void assign_inst(cInst* inst, unsigned int data);
		void exec_inst(cInst* inst);
		void save_inst(cInst* inst);
		
		// Variable
		unsigned long m_ulInstRAMSize;
		unsigned long m_ulDataRAMSize;
		bool m_bVerbose;
		bool m_bFin;
		
		// Étage du pipeline
		void load(void);  
		void execute(void);  
		void store(void);  
				
		// Registres
		sc_signal<int> reg[15];
		sc_signal<unsigned int> PC;
		
		// Pour fonctionement interne
		sc_fifo<bool> isBranching_STG1_STG2;
		sc_fifo<bool> isBranching_STG2_STG3;
		unsigned int PC_Branching;
		
		// Fifo entre les étages du pipeline
		sc_fifo<cInst> STG1_STG2_buffer;
		sc_fifo<cInst> STG2_STG3_buffer;
};

#endif
