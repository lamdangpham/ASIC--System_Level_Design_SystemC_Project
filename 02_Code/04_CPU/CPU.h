#include "CPU_Function.h"

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

class CPU: public CPU_Function
{
public:
	sc_in_clk clk;
	//tlm_utils::simple_target_socket<CPU> t_socketCPU;
	//tlm_utils::simple_initiator_socket<CPU> i_socketCPU;
	
	sc_time period;

	SC_HAS_PROCESS(CPU);
	CPU(sc_module_name name, sc_time _period);
};