#include "info_register.h"
#include "general_register.h"

class SCI_Reg: public sc_module
{
public:
	Info_Reg<8> SCiCR_Info;
	General_Reg<8> SCiCR;

	Info_Reg<8> SCiMD_Info;		
	General_Reg<8> SCiMD;

	Info_Reg<8> SCiEMD_Info;
	General_Reg<8> SCiEMD;

	Info_Reg<8> SCiBR_Info;
	General_Reg<8> SCiBR;

	Info_Reg<8> SCiTB_Info;
	General_Reg<8> SCiTB;

	Info_Reg<8> SCiRB_Info;
	General_Reg<8> SCiRB;

	Info_Reg<8> SCiSR_Info;
	General_Reg<8> SCiSR;

public:
	SCI_Reg(sc_module_name name): sc_module(name),
		SCiCR_Info("SCiCR_Info", "SCiCR.txt"), SCiCR("SCiCR", SCiCR_Info.reg_Default, SCiCR_Info.bit_RangeNum, SCiCR_Info.bit_Name, SCiCR_Info.bit_Start, SCiCR_Info.bit_End, SCiCR_Info.bit_Option),
		SCiMD_Info("SCiMD_Info", "SCiMD.txt"), SCiMD("SCiMD", SCiMD_Info.reg_Default, SCiMD_Info.bit_RangeNum, SCiMD_Info.bit_Name, SCiMD_Info.bit_Start, SCiMD_Info.bit_End, SCiMD_Info.bit_Option),
		SCiEMD_Info("SCiEMD_Info", "SCiEMD.txt"), SCiEMD("SCiEMD", SCiEMD_Info.reg_Default, SCiEMD_Info.bit_RangeNum, SCiEMD_Info.bit_Name, SCiEMD_Info.bit_Start, SCiEMD_Info.bit_End, SCiEMD_Info.bit_Option),
		SCiBR_Info("SCiBR_Info", "SCiBR.txt"), SCiBR("SCiBR", SCiBR_Info.reg_Default, SCiBR_Info.bit_RangeNum, SCiBR_Info.bit_Name, SCiBR_Info.bit_Start, SCiBR_Info.bit_End, SCiBR_Info.bit_Option),
		SCiTB_Info("SCiTB_Info", "SCiTB.txt"), SCiTB("SCiTB", SCiTB_Info.reg_Default, SCiTB_Info.bit_RangeNum, SCiTB_Info.bit_Name, SCiTB_Info.bit_Start, SCiTB_Info.bit_End, SCiTB_Info.bit_Option),
		SCiRB_Info("SCiRB_Info", "SCiRB.txt"), SCiRB("SCiRB", SCiRB_Info.reg_Default, SCiRB_Info.bit_RangeNum, SCiRB_Info.bit_Name, SCiRB_Info.bit_Start, SCiRB_Info.bit_End, SCiRB_Info.bit_Option),
		SCiSR_Info("SCiSR_Info", "SCiSR.txt"), SCiSR("SCiSR", SCiSR_Info.reg_Default, SCiSR_Info.bit_RangeNum, SCiSR_Info.bit_Name, SCiSR_Info.bit_Start, SCiSR_Info.bit_End, SCiSR_Info.bit_Option)
	{
		
	}
};