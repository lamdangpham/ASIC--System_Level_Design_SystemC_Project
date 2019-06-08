#include "CPU_test.h"

// TLM-2 blocking transport method
void CPU::b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command	cmd = trans.get_command();
	sc_dt::uint64		adr = trans.get_address() / 4;
	unsigned char*		ptr = trans.get_data_ptr();
	unsigned int		len = trans.get_data_length();
	unsigned char*		byt = trans.get_byte_enable_ptr();
	unsigned int		wid = trans.get_streaming_width();

	// Obliged to check address range and check for unsupported features,
	// i.e. byte enables, streaming, and bursts
	// Can ignore DMI hint and extensions
	// Using the SystemC report handler is an acceptable way of signalling an error

	if (adr >= sc_dt::uint64(256) || byt != 0 || len > 8 || wid < len)
		SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");

	trans.set_response_status( tlm::TLM_OK_RESPONSE );
	
	sc_bv<8> data_temp;

	for(int i = 0; i < 8; i++)
	{
		if(ptr[i] == '1') data_temp[i] = SC_LOGIC_1;
		if(ptr[i] == '0') data_temp[i] = SC_LOGIC_0;
	}

	cpu_recei.write(data_temp);
	//wait(SC_ZERO_TIME);
	//cout << "[cpu_recei] " << cpu_recei.read() << " [" << sc_time_stamp() << "]" << endl;
}