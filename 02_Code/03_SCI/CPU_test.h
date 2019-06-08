//#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

class CPU: public sc_module
{
public:
	tlm_utils::simple_target_socket<CPU> t_socketCPU;
	tlm_utils::simple_initiator_socket<CPU> i_socketCPU;

	sc_in<sc_bv<8>> cpu_trans;
	sc_out<sc_bv<8>> cpu_recei;

public:
	SC_HAS_PROCESS(CPU);
	CPU(sc_module_name name): sc_module(name), t_socketCPU("t_socketCPU"), i_socketCPU("i_socketCPU"), cpu_trans("cpu_trans"), cpu_recei("cpu_recei")
	{
		t_socketCPU.register_b_transport(this, &CPU::b_transport);
		SC_THREAD(test);
		sensitive << cpu_trans;
	}

	void test()
	{
		while(true)
		{
			wait();
			tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
			sc_time delay = sc_time(10, SC_PS);

			tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;

			sc_bv<8> data_temp;

			data_temp = cpu_trans.read();

			unsigned char send[8];

			for(int i = 0; i < 8; i++)
			{
				if(data_temp[i] == SC_LOGIC_1) send[i] = '1';
				if(data_temp[i] == SC_LOGIC_0) send[i] = '0';
			}

			trans->set_command( cmd );
			trans->set_address(0);
			trans->set_data_ptr( send );
			trans->set_data_length( 4 );
			trans->set_streaming_width( 24 ); // = data_length to indicate no streaming
			trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
			trans->set_dmi_allowed( false ); // Mandatory initial value
			trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

			i_socketCPU->b_transport( *trans, delay );  // Blocking transport call
      
			//Initiator obliged to check response status and delay
			if ( trans->is_response_error() )
				SC_REPORT_ERROR("TLM-2", "Response error from b_transport");
		}
	}

public:
	// TLM-2 blocking transport method
	void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay );
};