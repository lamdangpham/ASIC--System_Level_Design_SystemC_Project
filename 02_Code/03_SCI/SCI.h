#include "SCI_function.h"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

class SCI: public SCI_Func
{
public:
	sc_in_clk SCKi;
	sc_in<sc_bv<12>> RXDi;
	sc_out<sc_bv<12>> TXDi;

	sc_event has_data;

	sc_bv<12> data_trans_external;
	sc_bv<8> data_recei_external;
	sc_bv<8> data_recei_CPU;

	tlm_utils::simple_initiator_socket<SCI> i_socketSCI;
	tlm_utils::simple_target_socket<SCI> t_socketSCI;

	sc_time period;

	SC_HAS_PROCESS(SCI);
	SCI(sc_module_name name, sc_time period_);

	void SCI_data_trans_asyn();
	void SCI_data_recei_asyn();
	void SCI_data_trans_syn();
	void SCI_data_recei_syn();

	template <int N>
	void tlm_write(sc_bv<N> data)
	{
		tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
		sc_time delay = sc_time(10, SC_PS);

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

		i_socketSCI->b_transport( *trans, delay );  // Blocking transport call
      
		//Initiator obliged to check response status and delay
		if ( trans->is_response_error() )
		SC_REPORT_ERROR("TLM-2", "Response error from b_transport");
	}

	void tlm_write_0();

	void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay );
};

