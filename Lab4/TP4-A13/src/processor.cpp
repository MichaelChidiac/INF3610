///////////////////////////////////////////////////////////////////////////////
//
//	processor.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include "processor.h"


///////////////////////////////////////////////////////////////////////////////
//
//	Constructeur
//
///////////////////////////////////////////////////////////////////////////////
processor::processor( sc_module_name zName, unsigned long ulInstRAMSize, unsigned long ulDataRAMSize, bool bVerbose )
: 	sc_module(zName), STG1_STG2_buffer(1), STG2_STG3_buffer(1)
{
	// Initialisation
	m_bVerbose = bVerbose;
	m_ulInstRAMSize = ulInstRAMSize;
	m_ulDataRAMSize = ulDataRAMSize;
	
	// Thread
	SC_THREAD(load);
		sensitive << ClockPort.pos();
		
	SC_THREAD(execute);
		sensitive << ClockPort.pos();
		
	SC_THREAD(store);
		sensitive << ClockPort.pos();
		
	// Initialisation
	for (int i = 0; i <= 15; i++)
		reg[i] = 0;
		
	PC = 0;	
	PC_Branching = 0;
	isBranching_STG1_STG2 = false;
	isBranching_STG2_STG3 = false;
	
	// Indique que la fin n'est pas encore arrivé
	m_bFin = false;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Destructeur
//
///////////////////////////////////////////////////////////////////////////////
processor::~processor()
{
}

///////////////////////////////////////////////////////////////////////////////
//
//	load
//
///////////////////////////////////////////////////////////////////////////////
void processor::load(void)
{
	// Variable
	cInst Instruction;
	unsigned int uiData;
	bool bValid;
	bool isBranching;
	
	// Boucle infinie
	while(!m_bFin)
	{
		// On regarde si on a fait un branchement
		bValid = isBranching_STG1_STG2.nb_read( isBranching );
		
		if ( isBranching && bValid )
		{
			// On doit vider le fifo entre l'étage 1 et 2
			while( STG1_STG2_buffer.num_available() > 0 )
				STG1_STG2_buffer.nb_read( Instruction );
				
			if ( m_bVerbose )
				printf("PROC: Vide le pipeline entre STG1 et STG2 effectué par STG1\n");
				
			// On assigne le nouveau PC
			PC = PC_Branching;
			
			// Délai pour vider le pipeline
			wait(1);
		}
		
		// On s'assure qu'il y a de la place
		if ( STG1_STG2_buffer.num_free() > 0 )
		{				
			// Lecture dans la mémoire des instructions
			uiData = InstPort->ReadInt( PC );
			
			if ( m_bVerbose )
				printf("PROC: Lecture d'une instruction à l'adresse %x avec contenu %x\n", (unsigned int)PC.read(), uiData );
			
			// Assigne les éléments de la structure
			assign_inst( &Instruction, uiData );
			
			if ( m_bVerbose )
				cout << "PROC:" << Instruction;
			
			// On écrit l'instruction pour le prochain étage
			STG1_STG2_buffer.write( Instruction );
			
			// Prochaine instruction
			PC = (unsigned int)PC.read() + INST_SIZE;
		}
		else
		{
			if ( m_bVerbose )
				cout << "PROC: En attente qu'une place se libère vers STG2\n";
		}
		
		// Latence de l'étage
		wait(1);
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	execute
//
///////////////////////////////////////////////////////////////////////////////
void processor::execute(void)
{
	// Variable
	bool bValid;
	cInst Instruction;
	
	// Boucle infinie
	while(!m_bFin)
	{		
		// On récupère une instruction
		bValid = STG1_STG2_buffer.nb_read( Instruction );
		
		// On détermine si on a une instruction valide
		if (bValid)
		{
			// Exécution
			exec_inst( &Instruction );
			
			// On s'assure d'une place
			while ( STG2_STG3_buffer.num_free() == 0 ) 
			{
				if ( m_bVerbose )
					cout << "PROC: En attente qu'une place se libère vers STG3\n";
				wait(1); 
			}
			
			// On écrit l'instruction pour le prochain étage
			STG2_STG3_buffer.write( Instruction );		
		}
		else
			wait(1);
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	store
//
///////////////////////////////////////////////////////////////////////////////
void processor::store(void)
{
	// Variable
	bool bValid;
	bool isBranching;
	cInst Instruction;
	
	// Boucle infinie
	while(!m_bFin)
	{
		// On regarde si on a fait un branchement
		bValid = isBranching_STG2_STG3.nb_read( isBranching );
		
		if ( isBranching && bValid )
		{
			// On doit vider le fifo entre l'étage 2 et 3
			while( STG2_STG3_buffer.num_available() > 0 )
				STG2_STG3_buffer.nb_read( Instruction );
				
			if ( m_bVerbose )
				printf("PROC: Vide le pipeline entre STG2 et STG3 effectué par STG3\n");
		
			// Délai pour vider le pipeline
			wait(1);
		}
		
		// On récupère une instruction
		bValid = STG2_STG3_buffer.nb_read( Instruction );
		
		// On détermine si on a une instruction valide
		if (bValid)
			save_inst( &Instruction );	// Sauvegarde
		
		// Latence de l'étage
		wait(1);
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	assign_inst
//
///////////////////////////////////////////////////////////////////////////////
void processor::assign_inst(cInst* inst, unsigned int data)
{
	// On récupère le opcode
	inst->OPCODE = (data & 0x3F);
	inst->RD = ( (data >> 6) & 0x1F);
	inst->RA = ( (data >> 11) & 0x1F);
	inst->RB = ( (data >> 16) & 0x1F);
	inst->IMM = ( (data >> 16) & 0xFFFF);
	
	// On assighe le PC
	inst->PC = (unsigned int)PC.read();		
}


///////////////////////////////////////////////////////////////////////////////
//
//	exec_inst
//
///////////////////////////////////////////////////////////////////////////////
void processor::exec_inst(cInst* inst)
{
	switch ( inst->OPCODE )
	{
		case 0x0:	// add rd, ra, rb
			inst->RESULT = reg[ inst->RA ] + reg[ inst->RB ];
			wait(1);
			break;
			
		case 0x1:	// addi  rd, ra, imm
			inst->RESULT = reg[ inst->RA ] + inst->IMM;
			wait(1);
			break;
			
		case 0x2:	// and rd, ra, rb
			inst->RESULT = reg[ inst->RA ] & reg[ inst->RB ];
			wait(1);
			break;
			
		case 0x3:	// andi rd, ra, imm
			inst->RESULT = reg[ inst->RA ] & inst->IMM;
			wait(1);
			break;
			
		case 0x4:	// beq ra, rb
			if ( reg[ inst->RA ] == 0 )
			{	
				PC_Branching = inst->PC + reg[ inst->RB ];
				isBranching_STG1_STG2.write( true );
				isBranching_STG2_STG3.write( true );
				
				if ( m_bVerbose )
					printf("PROC: Branchement détecté à l'adresse %x\n", PC_Branching);
					
				wait(3);
			}
			else
				wait(1);
			break;
			
		case 0x5:	// beqi ra, imm
			if ( reg[ inst->RA ] == 0 )
			{				
				PC_Branching = inst->PC + inst->IMM;
				isBranching_STG1_STG2.write( true );
				isBranching_STG2_STG3.write( true );
				
				if ( m_bVerbose )
					printf("PROC: Branchement détecté à l'adresse %x\n", PC_Branching);
					
				wait(3);
			}
			else
				wait(1);
			break;
			
		case 0x6:	// bge ra, rb
			if ( reg[ inst->RA ] >= 0 )
			{
				PC_Branching = inst->PC + reg[ inst->RB ];
				isBranching_STG1_STG2.write( true );
				isBranching_STG2_STG3.write( true );
				
				if ( m_bVerbose )
					printf("PROC: Branchement détecté à l'adresse %x\n", PC_Branching);
					
				wait(3);
			}
			else
				wait(1);
			break;
			
		case 0x7:	// bgei ra, imm
			if ( reg[ inst->RA ] >= 0 )
			{
				PC_Branching = inst->PC + inst->IMM;
				isBranching_STG1_STG2.write( true );
				isBranching_STG2_STG3.write( true );
				
				if ( m_bVerbose )
					printf("PROC: Branchement détecté à l'adresse %x\n", PC_Branching);
					
				wait(3);
			}
			else
				wait(1);
			break;
			
		case 0x8:	// bgt ra, rb
			if ( reg[ inst->RA ] > 0 )
			{
				PC_Branching = inst->PC + reg[ inst->RB ];
				isBranching_STG1_STG2.write( true );
				isBranching_STG2_STG3.write( true );
				
				if ( m_bVerbose )
					printf("PROC: Branchement détecté à l'adresse %x\n", PC_Branching);
					
				wait(3);
			}
			else
				wait(1);
			break;
			
		case 0x9:	// bgti ra, imm
			if ( reg[ inst->RA ] > 0 )
			{
				PC_Branching = inst->PC + inst->IMM;
				isBranching_STG1_STG2.write( true );
				isBranching_STG2_STG3.write( true );
				
				if ( m_bVerbose )
					printf("PROC: Branchement détecté à l'adresse %x\n", PC_Branching);
					
				wait(3);
			}
			else
				wait(1);
			break;
			
		case 0xA:	// ble ra, rb
			if ( reg[ inst->RA ] <= 0 )
			{
				PC_Branching = inst->PC + reg[ inst->RB ];
				isBranching_STG1_STG2.write( true );
				isBranching_STG2_STG3.write( true );
				
				if ( m_bVerbose )
					printf("PROC: Branchement détecté à l'adresse %x\n", PC_Branching);
					
				wait(3);
			}
			else
				wait(1);
			break;
			
		case 0xB:	// blei ra, imm
			if ( reg[ inst->RA ] <= 0 )
			{					
				PC_Branching = inst->PC + inst->IMM;
				isBranching_STG1_STG2.write( true );
				isBranching_STG2_STG3.write( true );
				
				if ( m_bVerbose )
					printf("PROC: Branchement détecté à l'adresse %x\n", PC_Branching);
					
				wait(3);
			}
			else
				wait(1);
			break;
			
		case 0xC:	// blt ra, rb
			if ( reg[ inst->RA ] < 0 )
			{
				PC_Branching = inst->PC + reg[ inst->RB ];
				isBranching_STG1_STG2.write( true );
				isBranching_STG2_STG3.write( true );
				
				if ( m_bVerbose )
					printf("PROC: Branchement détecté à l'adresse %x\n", PC_Branching);
					
				wait(3);
			}
			else
				wait(1);
			break;
			
		case 0xD:	// blti ra, imm
			if ( reg[ inst->RA ] < 0 )
			{					
				PC_Branching = inst->PC + inst->IMM;
				isBranching_STG1_STG2.write( true );
				isBranching_STG2_STG3.write( true );
				
				if ( m_bVerbose )
					printf("PROC: Branchement détecté à l'adresse %x\n", PC_Branching);
					
				wait(3);
			}
			else
				wait(1);
			break;
			
		case 0xE:	// bne ra, rb
			if ( reg[ inst->RA ] != 0 )
			{					
				PC_Branching = inst->PC + reg[ inst->RB ];
				isBranching_STG1_STG2.write( true );
				isBranching_STG2_STG3.write( true );
				
				if ( m_bVerbose )
					printf("PROC: Branchement détecté à l'adresse %x\n", PC_Branching);
					
				wait(3);
			}
			else
				wait(1);
			break;
			
		case 0xF:	// bnei ra, imm
			if ( reg[ inst->RA ] != 0 )
			{					
				PC_Branching = inst->PC + inst->IMM;
				isBranching_STG1_STG2.write( true );
				isBranching_STG2_STG3.write( true );
				
				if ( m_bVerbose )
					printf("PROC: Branchement détecté à l'adresse %x\n", PC_Branching);
					
				wait(3);
			}
			else
				wait(1);
			break;
			
		case 0x10:	// bra rb			
			PC_Branching = reg[ inst->RB ];
			isBranching_STG1_STG2.write( true );
			isBranching_STG2_STG3.write( true );
			
			if ( m_bVerbose )
					printf("PROC: Branchement détecté à l'adresse %x\n", PC_Branching);
					
			wait(3);
			break;
			
		case 0x11:	// brai imm					
			PC_Branching = inst->IMM;
			isBranching_STG1_STG2.write( true );
			isBranching_STG2_STG3.write( true );
			
			if ( m_bVerbose )
					printf("PROC: Branchement détecté à l'adresse %x\n", PC_Branching);
					
			wait(3);
			break;
			
		case 0x12:	// cmp rd, ra, rb
			if ( reg[ inst->RA ] > reg[ inst->RB ] )
				inst->RESULT = 1;
				
			else if ( reg[ inst->RA ] < reg[ inst->RB ] )
				inst->RESULT = -1;
				
			else if ( reg[ inst->RA ] == reg[ inst->RB ] )
				inst->RESULT = 0;
				
			wait(1);
			break;
			
		case 0x13:	// idiv rd, ra, rb
			if ( reg[ inst->RA ] == 0 )
			{
				inst->RESULT = 0;
				wait(1);
			}
			else
			{
				inst->RESULT = reg[ inst->RB ] / reg[ inst->RA ];
				wait(32);
			}
			break;
			
		case 0x14:	// lw rd, ra, rb
			inst->ADDR = reg[ inst->RA ] + reg[ inst->RB ];			
			wait(2);
			break;
			
		case 0x15:	// lwi rd, ra, imm
			inst->ADDR = reg[ inst->RA ] + inst->IMM;			
			wait(2);
			break;
			
		case 0x16:	// mul rd, ra, rb
			inst->RESULT = reg[ inst->RA ] * reg[ inst->RB ];
			wait(3);
			break;
			
		case 0x17:	// muli rd, ra, imm
			inst->RESULT = reg[ inst->RA ] * inst->IMM;
			wait(3);
			break;
			
		case 0x18:	// nop
			wait(1);
			break;
			
		case 0x19:	// or rd, ra, rb
			inst->RESULT = reg[ inst->RA ] | reg[ inst->RB ];
			wait(1);
			break;
			
		case 0x1A:	// ori rd, ra, imm
			inst->RESULT = reg[ inst->RA ] | inst->IMM;
			wait(1);
			break;
			
		case 0x1B:	// ret
			wait(1);
			break;
			
		case 0x1C:	// sub rd, ra, rb
			inst->RESULT = reg[ inst->RA ] - reg[ inst->RB ];
			wait(1);
			break;
			
		case 0x1D:	// subi rd, ra, imm
			inst->RESULT = reg[ inst->RA ] - inst->IMM;
			wait(1);
			break;
			
		case 0x1E:	// sw rd, ra, rb
			inst->ADDR = reg[ inst->RA ] + reg[ inst->RB ];			
			wait(2);
			break;
			
		case 0x1F:	// swi rd, ra, imm
			inst->ADDR = reg[ inst->RA ] + inst->IMM;			
			wait(2);
			break;
			
		case 0x20:	// xor rd, ra, rb
			inst->RESULT = reg[ inst->RA ] ^ reg[ inst->RB ];
			wait(1);
			break;
			
		case 0x21:	// xori rd, ra, imm
			inst->RESULT = reg[ inst->RA ] ^ inst->IMM;
			wait(1);
			break;		
			
		default:
			cout << "PROC: Instruction non supportée" << endl;
			sc_stop();
			break;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	save_inst
//
///////////////////////////////////////////////////////////////////////////////
void processor::save_inst(cInst* inst)
{
	switch ( inst->OPCODE )
	{
		case 0x0:	// add rd, ra, rb
			reg[ inst->RD ] = inst->RESULT;
			break;
			
		case 0x1:	// addi  rd, ra, imm
			reg[ inst->RD ] = inst->RESULT;
			break;
			
		case 0x2:	// and rd, ra, rb
			reg[ inst->RD ] = inst->RESULT;
			break;
			
		case 0x3:	// andi rd, ra, imm
			reg[ inst->RD ] = inst->RESULT;
			break;
			
		case 0x4:	// beq ra, rb
			break;
			
		case 0x5:	// beqi ra, imm
			break;
			
		case 0x6:	// bge ra, rb
			break;
			
		case 0x7:	// bgei ra, imm
			break;
			
		case 0x8:	// bgt ra, rb
			break;
			
		case 0x9:	// bgti ra, imm
			break;
			
		case 0xA:	// ble ra, rb
			break;
			
		case 0xB:	// blei ra, imm
			break;
			
		case 0xC:	// blt ra, rb
			break;
			
		case 0xD:	// blti ra, imm
			break;
			
		case 0xE:	// bne ra, rb
			break;
			
		case 0xF:	// bnei ra, imm
			break;
			
		case 0x10:	// bra rb
			break;
			
		case 0x11:	// brai imm
			break;
			
		case 0x12:	// cmp rd, ra, rb
			reg[ inst->RD ] = inst->RESULT;
			break;
			
		case 0x13:	// idiv rd, ra, rb
			if ( reg[ inst->RA ] == 0 )
				reg[ inst->RD ] = 0;
			else
				reg[ inst->RD ] = reg[ inst->RB ] / reg[ inst->RA ];
			break;
			
		case 0x14:	// lw rd, ra, rb
			if ( inst->ADDR + 4 > m_ulDataRAMSize )
			{
				// On attend que le wrapper soit prêt
				while ( Processor_Ready_InPort.read() == false )
					wait(1);
					
				// On écrit l'adresse
				Processor_Address_OutPort.write( inst->ADDR );
				
				// On indique que la transaction est une lecture
				Processor_RW_OutPort.write(true);
				
				// On indique que la donnée est prête
				Processor_Enable_OutPort.write( true );
				
				// Propagation des signaux
				wait(Processor_Ready_InPort.default_event());
				
				// Attendre que wrapper ait terminé
				while ( Processor_Ready_InPort.read() == false )
					wait(1);
					
				// On lit la donnée
				reg[ inst->RD ] = Processor_Data_OutPort.read();
				
				// On indique qu'il n'y aucune donnée
				Processor_Enable_OutPort.write( false );	
			}
			else
				reg[ inst->RD ] = DataPort->ReadChar( inst->ADDR );	// Lecture de la LMB
			break;
			
		case 0x15:	// lwi rd, ra, imm
			if ( inst->ADDR + 4 > m_ulDataRAMSize )
			{
				// On attend que le wrapper soit prêt
				while ( Processor_Ready_InPort.read() == false )
					wait(1);
					
				// On écrit l'adresse
				Processor_Address_OutPort.write( inst->ADDR );
				
				// On indique que la transaction est une lecture
				Processor_RW_OutPort.write(true);
				
				// On indique que la donnée est prête
				Processor_Enable_OutPort.write( true );
				Processor_Enable_OutPort.write( true );
				
				// Propagation des signaux
				wait(Processor_Ready_InPort.default_event());
				
				// Attendre que wrapper ait terminé
				while ( Processor_Ready_InPort.read() == false )
					wait(1);
					
				// On lit la donnée
				reg[ inst->RD ] = Processor_Data_InPort.read();
				
				// On indique qu'il n'y aucune donnée
				Processor_Enable_OutPort.write( false );
			}
			else
				reg[ inst->RD ] = DataPort->ReadChar( inst->ADDR );	// Lecture de la LMB
			break;
			
		case 0x16:	// mul rd, ra, rb
			reg[ inst->RD ] = inst->RESULT;
			break;
			
		case 0x17:	// muli rd, ra, imm
			reg[ inst->RD ] = inst->RESULT;
			break;
			
		case 0x18:	// nop
			break;
			
		case 0x19:	// or rd, ra, rb
			reg[ inst->RD ] = inst->RESULT;
			break;
			
		case 0x1A:	// ori rd, ra, imm
			reg[ inst->RD ] = inst->RESULT;
			break;
			
		case 0x1B:	// ret
			if ( m_bVerbose )
					cout << "PROC: Fin de l'application" << endl;
			m_bFin = true;
			sc_stop();
			break;
			
		case 0x1C:	// sub rd, ra, rb
			reg[ inst->RD ] = inst->RESULT;
			break;
			
		case 0x1D:	// subi rd, ra, imm
			reg[ inst->RD ] = inst->RESULT;
			break;
			
		case 0x1E:	// sw rd, ra, rb
			if ( inst->ADDR + 4 > m_ulDataRAMSize )
			{
				// On attend que le wrapper soit prêt
				while ( Processor_Ready_InPort.read() == false )
					wait(1);
					
				// On écrit l'adresse
				Processor_Address_OutPort.write( inst->ADDR );
				
				// On indique que la transaction est une écriture
				Processor_RW_OutPort.write(false);

				// On écrit la donnée
				Processor_Data_OutPort.write( reg[ inst->RD ] );
				
				// On indique que la donnée est prête
				Processor_Enable_OutPort.write( true );
								
				// Propagation des signaux
				wait(Processor_Ready_InPort.default_event());
				
				// Attendre que wrapper ait terminé
				while ( Processor_Ready_InPort.read() == false )
					wait(1);
								
				// On indique qu'il n'y aucune donnée
				Processor_Enable_OutPort.write( false );	
			}
			else
				DataPort->Write( inst->ADDR, reg[ inst->RD ] );
			break;
			
		case 0x1F:	// swi rd, ra, imm
			if ( inst->ADDR + 4 > m_ulDataRAMSize )
			{
				// On attend que le wrapper soit prêt
				while ( Processor_Ready_InPort.read() == false )
					wait(1);
					
				// On écrit l'adresse
				Processor_Address_OutPort.write( inst->ADDR );
				
				// On indique que la transaction est une écriture
				Processor_RW_OutPort.write(false);
				
				// On écrit la donnée
				Processor_Data_OutPort.write( reg[ inst->RD ] );
				
				// On indique que la donnée est prête
				Processor_Enable_OutPort.write( true );
			
				// Propagation des signaux
				wait(Processor_Ready_InPort.default_event());
				
				// Attendre que wrapper ait terminé
				while ( Processor_Ready_InPort.read() == false )
					wait(1);

				// On indique qu'il n'y aucune donnée
				Processor_Enable_OutPort.write( false );	
			}
			else
				DataPort->Write( inst->ADDR, reg[ inst->RD ] );			
			break;
			
		case 0x20:	// xor rd, ra, rb
			reg[ inst->RD ] = inst->RESULT;
			break;
			
		case 0x21:	// xori rd, ra, imm
			reg[ inst->RD ] = inst->RESULT;
			break;		
			
		default:
			cout << "PROC: Instruction non supportée" << endl;
			sc_stop();
			break;
	}
}
