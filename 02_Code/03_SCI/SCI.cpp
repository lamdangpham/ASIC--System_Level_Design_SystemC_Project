#include "SCI.h"

SCI::SCI(sc_module_name name, sc_time period_): SCKi("SCKi"), RXDi("RXDi"), TXDi("TXDi"), SCI_Func(name), i_socketSCI("i_socketSCI"), t_socketSCI("t_socketSCI"), period(period_)
{
	data_trans_external = "000000000000";
	data_recei_external = "00000000";
	data_recei_CPU = "00000000";

	SCI_setting("asynchronous", "external_clock", "8-bit", "parity", "even", "two_stopbit", "1/1", "LSB");
	//SCI_setting("synchronous", "external_clock", "1/1", "MSB", "not_polarity", "not_delay_clock");
	SCI_BitRate(32);

	t_socketSCI.register_b_transport(this, &SCI::b_transport);

	//--------------------------asynchronous mode-----------------------------------------------------------------------------
	if(SCiMD.read_bit<1>("SMS") == 0)
	{
		//----------------------------------------trans------------------------
		SC_THREAD(SCI_data_trans_asyn);
		sensitive << has_data;

		//----------------------------------recei------------------------------
		SC_THREAD(SCI_data_recei_asyn);
		sensitive << RXDi;
	}

	//----------------------------synchronous mode------------------------------------------------------------------------------------
	if(SCiMD.read_bit<1>("SMS") == 1 )
	{
		//----------------------------------------trans------------------------
		SC_THREAD(SCI_data_trans_syn);
		sensitive << has_data;

		//----------------------------------recei------------------------------
		SC_THREAD(SCI_data_recei_syn);
		sensitive << RXDi;
	}
}

//---------------------------------------------------trans data in asynchronous mode-----------------------------------------------------
void SCI::SCI_data_trans_asyn()
{
	while(true)
	{
		wait();
		SCiCR.write_bit<1>("TE", 1);
		if(SCiSR.read_bit_uint<1>("TBEF") == 1)
		{
			SCI_WBuffTrans(data_recei_CPU);
			SCiSR.write_bit<1>("TBEF", "0");
			SCI_ProcDatAsynTrans(SCiTB.read(), data_trans_external);
			SCiSR.write_bit<1>("TBEF", "1");
			TXDi.write(data_trans_external);
			SCiCR.write_bit<1>("TE", "0");
		}
	}
}

//---------------------------------------------------recei data in asynchronous mode-----------------------------------------------------
void SCI::SCI_data_recei_asyn()
{
	while(true)
	{
		wait();
		SCiCR.write_bit<1>("RE", 1);
		SCI_ProcDatAsynRecei(RXDi.read(), data_recei_external);

		if(SCiSR.read_bit_uint<1>("FREF") == 0 && SCiSR.read_bit_uint<1>("PERF") == 0)
		{
			SCI_WBuffRecei(data_recei_external);
			SCiSR.write_bit<1>("RBFF", "1");
			SCiCR.write_bit<1>("RE", "0");
			
			tlm_write<8>(SCiRB.read());

			SCiSR.write_bit<1>("RBFF", "0");
		}

		if(SCiSR.read_bit_uint<1>("FREF") == 1 || SCiSR.read_bit_uint<1>("PERF") == 1)
		{
			SCI_WBuffRecei(data_recei_external);
			SCiSR.write_bit<1>("RBFF", "0");

			tlm_write_0();
		}
	}
}

//---------------------------------------------------trans data in synchronous mode-----------------------------------------------------
void SCI::SCI_data_trans_syn()
{
	while(true)
	{
		wait();
		wait(4*period);
		SCiCR.write_bit<1>("TE", 1);
		if(SCiSR.read_bit_uint<1>("TBEF") == 1)
		{
			SCI_WBuffTrans(data_recei_CPU);
			SCiSR.write_bit<1>("TBEF", "0");
			data_trans_external = SCiTB.read();
			SCiSR.write_bit<1>("TBEF", "1");
			TXDi.write(data_trans_external);
			SCiCR.write_bit<1>("TE", "0");
		}
	}
}

//---------------------------------------------------recei data in synchronous mode-----------------------------------------------------
void SCI::SCI_data_recei_syn()
{
	while(true)
	{
		wait();
		SCiCR.write_bit<1>("RE", 1);
		data_recei_external = RXDi.read();

		if(SCiSR.read_bit_uint<1>("FREF") == 0 && SCiSR.read_bit_uint<1>("PERF") == 0)
		{
			SCI_WBuffRecei(data_recei_external);
			SCiSR.write_bit<1>("RBFF", "1");

			tlm_write<8>(SCiRB.read());

			SCiSR.write_bit<1>("RBFF", "0");
			SCiCR.write_bit<1>("RE", "0");
		}

		if(SCiSR.read_bit_uint<1>("FREF") == 1 || SCiSR.read_bit_uint<1>("PERF") == 1)
		{
			if(SCiSR.read_bit_uint<1>("FREF") == 1) cout << "[ERROR FRAME]" << endl;
			if(SCiSR.read_bit_uint<1>("PERF") == 1) cout << "[ERROR PARITY]" << endl;
			SCI_WBuffRecei(data_recei_external);

			tlm_write_0();

			SCiSR.write_bit<1>("RBFF", "0");
		}
	}
}

void SCI::tlm_write_0()
{
	tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
	sc_time delay = sc_time(10, SC_NS);

	tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;

	unsigned char send[8];
	for(int i = 0; i < 8; i++)
	{
		send[i] = '0';
	}

	trans->set_command( cmd );
	trans->set_address(0);
	trans->set_data_ptr(send);
	trans->set_data_length( 8 );
	trans->set_streaming_width( 24 ); // = data_length to indicate no streaming
	trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
	trans->set_dmi_allowed( false ); // Mandatory initial value
	trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

	i_socketSCI->b_transport( *trans, delay );  // Blocking transport call
      
	//Initiator obliged to check response status and delay
	if ( trans->is_response_error() )
	SC_REPORT_ERROR("TLM-2", "Response error from b_transport");
}

void SCI::b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command	cmd = trans.get_command();
    sc_dt::uint64		adr = trans.get_address() / 4;
    unsigned char*		ptr = trans.get_data_ptr();
    unsigned int		len = trans.get_data_length();
    unsigned char*		byt = trans.get_byte_enable_ptr();
    unsigned int		wid = trans.get_streaming_width();

    if (adr >= sc_dt::uint64(256) || byt != 0 || len > 4 || wid < len)
      SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");

	trans.set_response_status( tlm::TLM_OK_RESPONSE );

	for(int i = 0; i < 8; i++)
	{
		if(ptr[i] == '1') data_recei_CPU[i] = SC_LOGIC_1;
		if(ptr[i] == '0') data_recei_CPU[i] = SC_LOGIC_0;
	}

	has_data.notify();	
}