#include "Memory.h"
#include "RegFile.h"
#include "math.h"
#include <iostream>
#include "info_register.h"
#include "general_register.h"
#include "tlm.h"
#include "tlm_utils\simple_initiator_socket.h"
#include "tlm_utils\simple_target_socket.h"

#define ADD			1
#define AND			5
#define NOT			9
#define BR			0
#define JMP_RET		12
#define JSR_JSRR	4
#define LD			2
#define LDI			10
#define LDR			6
#define LEA			14
#define ST			3
#define STI			11
#define STR			7
#define TRAP		15

#define PORT_SCI	31

void StringHexToDecMem(char s[], int &datHex);
void DecToBinaryMem(int datDec, DatWidMem &dat);
void BinaryToDecMem(AddWidMem add, int &add_int);

class CPU_Function: public sc_module
{
public:
	tlm_utils::simple_target_socket<CPU_Function> t_socketCPU;
	tlm_utils::simple_initiator_socket<CPU_Function> i_socketCPU_to_SCI;
	sc_bv<16> IRDat;
	sc_bv<4> Opcode;
	sc_bv<1> Mode;
	sc_bv<16> ALU_out;
	bool N;
	bool Z;
	bool P;
	bool n;
	bool z;
	bool p;
	sc_bv<3> DR, SR1, SR2;
	sc_bv<5> Imm5;
	sc_bv<6> Offset6;
	sc_bv<9> PCOffset9;
	sc_bv<11> PCOffset11;
	sc_bv<8> trapvect8;
	sc_bv<16> data_recei;

	sc_process_handle Fe;
	sc_process_handle De;
	sc_process_handle Ev;
	sc_process_handle FeO;
	sc_process_handle Ex;
	sc_process_handle St;

	sc_time period;

	Memory mem;
	RegFile regfile;

	Info_Reg<16> IR_Info;
	General_Reg<16> IR;

	Info_Reg<16> MAR_Info;
	General_Reg<16> MAR;

	Info_Reg<16> MDR_Info;
	General_Reg<16> MDR;

	Info_Reg<16> PC_Info;
	General_Reg<16> PC;

	CPU_Function(sc_module_name name, sc_time _period);

	template <int N> void ALU(sc_bv<16> &DR, sc_bv<16> SR1, sc_bv<N> Operand2)
	{
		/*if(Binary_4ToDec(Opcode) == AND)
		{
			if((SR1[15] == SC_LOGIC_1) && (Operand2[N-1] != SC_LOGIC_1))
			{
				DR = (-BinaryToDec<16>(TwoCom(SR1))) *  BinaryToDec<N>(Operand2);
				cout << "\t\t\t\t\t\t\t\t\tALU: In1 = " << -BinaryToDec<16>(TwoCom(SR1)) << " , In2 = " << BinaryToDec<N>(Operand2) << endl;
				if(DR[15] == SC_LOGIC_1)
				{
					cout << "\t\t\t\t\t\t\t\t\t     Out = " << -BinaryToDec<16>(TwoCom(DR)) << endl;
				}
				else
				{
					cout << "\t\t\t\t\t\t\t\t\t     Out = " << BinaryToDec<16>(DR) << endl;
				}
			}
			if((SR1[15] != SC_LOGIC_1) && (Operand2[N-1] == SC_LOGIC_1))
			{
				DR = BinaryToDec<16>(SR1) * (-BinaryToDec<N>(TwoCom(Operand2)));
				if(DR[15] == SC_LOGIC_1)
				{
					cout << "\t\t\t\t\t\t\t\t\t     Out = " << -BinaryToDec<16>(TwoCom(DR)) << endl;
				}
				else
				{
					cout << "\t\t\t\t\t\t\t\t\t     Out = " << BinaryToDec<16>(DR) << endl;
				}
			}
			if((SR1[15] == SC_LOGIC_1) && (Operand2[N-1] == SC_LOGIC_1))
			{
				DR = (-BinaryToDec<16>(TwoCom(SR1))) * (-BinaryToDec<N>(TwoCom(Operand2)));
				cout << "\t\t\t\t\t\t\t\t\tALU: In1 = " << -BinaryToDec<16>(TwoCom(SR1)) << " , In2 = " << -BinaryToDec<N>(TwoCom(Operand2)) << endl;
				cout << "\t\t\t\t\t\t\t\t\t     Out = " << -BinaryToDec<16>(DR) << endl;
			}
			if((SR1[15] != SC_LOGIC_1) && (Operand2[N-1] != SC_LOGIC_1))
			{
				DR = BinaryToDec<16>(SR1) *  BinaryToDec<N>(Operand2);
				cout << "\t\t\t\t\t\t\t\t\tALU: In1 = " << BinaryToDec<16>(SR1) << " , In2 = " << BinaryToDec<N>(Operand2) << endl;
				cout << "\t\t\t\t\t\t\t\t\t     Out = " << BinaryToDec<16>(DR) << endl;
			}
		}
		else
		{
			if((SR1[15] == SC_LOGIC_1) && (Operand2[N-1] != SC_LOGIC_1))
			{
				DR = -BinaryToDec<16>(TwoCom(SR1)) +  BinaryToDec<N>(Operand2);
				cout << "\t\t\t\t\t\t\t\t\tALU: In1 = " << -BinaryToDec<16>(TwoCom(SR1)) << " , In2 = " << BinaryToDec<N>(Operand2) << endl;
				if(DR[15] == SC_LOGIC_1)
				{
					cout << "\t\t\t\t\t\t\t\t\t     Out = " << -BinaryToDec<16>(TwoCom(DR)) << endl;
				}
				else
				{
					cout << "\t\t\t\t\t\t\t\t\t     Out = " << BinaryToDec<16>(DR) << endl;
				}
			}
			if((SR1[15] != SC_LOGIC_1) && (Operand2[N-1] == SC_LOGIC_1))
			{
				DR = BinaryToDec<16>(SR1) -  BinaryToDec<N>(TwoCom(Operand2));
				cout << "\t\t\t\t\t\t\t\t\tALU: In1 = " << BinaryToDec<16>(SR1) << " , In2 = " << -BinaryToDec<N>(TwoCom(Operand2)) << endl;
				if(DR[15] == SC_LOGIC_1)
				{
					cout << "\t\t\t\t\t\t\t\t\t     Out = " << -BinaryToDec<16>(TwoCom(DR)) << endl;
				}
				else
				{
					cout << "\t\t\t\t\t\t\t\t\t     Out = " << BinaryToDec<16>(DR) << endl;
				}
			}
			if((SR1[15] == SC_LOGIC_1) && (Operand2[N-1] == SC_LOGIC_1))
			{
				DR = -BinaryToDec<16>(TwoCom(SR1)) -  BinaryToDec<N>(TwoCom(Operand2));
				cout << "\t\t\t\t\t\t\t\t\tALU: In1 = " << -BinaryToDec<16>(TwoCom(SR1)) << " , In2 = " << -BinaryToDec<N>(TwoCom(Operand2)) << endl;
				cout << "\t\t\t\t\t\t\t\t\t     Out = " << -BinaryToDec<16>(DR) << endl;
			}
			if((SR1[15] != SC_LOGIC_1) && (Operand2[N-1] != SC_LOGIC_1))
			{
				DR = BinaryToDec<16>(SR1) +  BinaryToDec<N>(Operand2);
				cout << "\t\t\t\t\t\t\t\t\tALU: In1 = " << BinaryToDec<16>(SR1) << " , In2 = " << BinaryToDec<N>(Operand2) << endl;
				cout << "\t\t\t\t\t\t\t\t\t     Out = " << BinaryToDec<16>(DR) << endl;
			}
		}*/
		if(Binary_4ToDec(Opcode) == AND)
		{
			DR = BinaryToDec<16>(SR1) *  BinaryToDec<N>(Operand2);
			cout << "\t\t\t\t\t\t\t\t\tALU: In1 = " << BinaryToDec<16>(SR1) << " , In2 = " << BinaryToDec<N>(Operand2) << endl;
			cout << "\t\t\t\t\t\t\t\t\t     Out = " << BinaryToDec<16>(DR) << " (" << DR << ")" << endl;
		}
		else
		{
			DR = BinaryToDec<16>(SR1) +  BinaryToDec<N>(Operand2);
			cout << "\t\t\t\t\t\t\t\t\tALU: In1 = " << BinaryToDec<16>(SR1) << " , In2 = " << BinaryToDec<N>(Operand2) << endl;
			cout << "\t\t\t\t\t\t\t\t\t     Out = " << BinaryToDec<16>(DR) << " (" << DR << ")" << endl;
		}
	}

	void ALU(sc_bv<16> &DR, sc_bv<16> SR1);
	void LoadFile(char *fileName);
	void Fetch();
	void Decode();
	void EvaluateAddress();
	void FetchOperands();
	void Execute();
	void Store();
	void Display_Memory();
	void Display_RegFile();

	template <int N>
	void tlm_write(sc_bv<N> data)
	{
		tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
		sc_time delay = sc_time(50, SC_PS);

		tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;
		unsigned char send[N];
		for(int i = 0; i < N; i++)
		{
			if(data[i] == SC_LOGIC_1) send[i] = '1';
			if(data[i] == SC_LOGIC_0) send[i] = '0';
		}

		trans->set_command( cmd );
		trans->set_address(0);
		trans->set_data_ptr(send);
		trans->set_data_length( N );
		trans->set_streaming_width( 24 ); // = data_length to indicate no streaming
		trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
		trans->set_dmi_allowed( false ); // Mandatory initial value
		trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

		i_socketCPU_to_SCI->b_transport( *trans, delay );  // Blocking transport call
      
		//Initiator obliged to check response status and delay
		if ( trans->is_response_error() )
		SC_REPORT_ERROR("TLM-2", "Response error from b_transport");
	}

	void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay );
};