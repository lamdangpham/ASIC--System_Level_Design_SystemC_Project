#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

using namespace sc_core;
using namespace tlm_utils;

class target: public sc_module
{
	simple_target_socket<target> t_socket;

	sc_inout<sc_bv<8>> t_data;

	peq_with_get<tlm_generic_payload> t_peq;

	SC_CTOR(target): t_socket("t_socket"), t_data("t_data")
	{
		SC_THREAD(t_proc);
		t_socket.register_nb_transport_fw(this, &target::nb_transport_fw);
	}

	void t_proc();
	tlm_sync_enum nb_transport_fw(tlm_generic_payload &payload, tlm_phase &phase, sc_time &delay_time);
};