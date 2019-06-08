#include "target.h"

tlm_sync_enum target::nb_transport_fw(tlm_generic_payload &payload, tlm_phase &phase, sc_time &delay_time)
{
	tlm_command		cmd = payload.get_command();
	unsigned char*	ptr = payload.get_data_ptr();
	unsigned int	len = payload.get_data_length();
	unsigned char*	byt = payload.get_byte_enable_ptr();
    unsigned int	wid = payload.get_streaming_width();

	if(cmd == TLM_WRITE_COMMAND)
	{
		payload.set_respond_status(TLM_OK_RESPOND);
	}
	else if(cmd == TLM_READ_COMMAND)
	{
		payload.set_respond_status(TLM_GENERIC_ERROR_RESPOND);
	}

	delay_time = sc_time(10, SC_PS);

	t_peq.notify(payload, delay_time);
	
	phase = END_REQ;

	return TLM_UPDATED;
}

void target::t_proc()
{
	tlm_generic_payload *trans;
	sc_time delay;
	tlm_phase ph;
	tlm_response resp;
	while(true)
	{
		wait(target_peq.get_event());
		trans = target_peq.get_next_transaction();
		if(cmd == TLM_WRITE_COMMAND)
		{
			memcpy(&mem[adr], ptr, len);
			proc_data(&mem[adr], len, data);
			t_data.write(data);
			//trans->set_response_status(...);
		}
		delay = SC_ZERO_TIME;
		ph = BEGIN_RESP
		resp = tsocket->nb_transport_bw(*trans, ph, delay);
		if(resp != TLM_COMPLETED)
		{

		}
		wait(delay);
	}
}