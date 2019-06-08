#include "SCI_Function.h"

SCI_Func::SCI_Func(sc_module_name name): SCI_Reg(name)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------
// write 0 to TE
//----------------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::SCI_disTrans()
{
	SCiCR.write_bit<1>("TE", 0);
}

//----------------------------------------------------------------------------------------------------------------------------------------
// write 1 to TE
//----------------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::SCI_enTrans()
{
	SCiCR.write_bit<1>("TE", 1);
}

//----------------------------------------------------------------------------------------------------------------------------------------
// write transmit buffer register
//----------------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::SCI_WBuffTrans(sc_bv<8> data)
{
	SCiTB.write(data);
	//cout << "[" << SCiTB.reg_Name << "] " << SCiTB.read() << " [" << sc_time_stamp() << "]" << endl;
}

//----------------------------------------------------------------------------------------------------------------------------------------
// write receive buffer register
//----------------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::SCI_WBuffRecei(sc_bv<8> data)
{
	SCiRB.write(data);
	//cout << "[" << SCiRB.reg_Name << "] " << SCiRB.read() << " [" << sc_time_stamp() << "]" << endl;
}

//----------------------------------------------------------------------------------------------------------------------------------------
// setting bit rate
//----------------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::SCI_BitRate(int bit_rate)
{
	SCiCR.write_bit<1>("TE", "0");
	SCiCR.write_bit<1>("RE", "0");
	SCiBR.write(bit_rate);
}

//----------------------------------------------------------------------------------------------------------------------------------------
// setting for asynchronous mode
//----------------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::SCI_setting(string mode, string clock, string datlen, string enpar, string par, string stobilen, string countsrc, string datdir)
{
	sel_Mode(mode);
	sel_Clock(clock);
	sel_DatLen(datlen);
	en_Parity(enpar);
	sel_Parity(par);
	sel_StopBitLen(stobilen);
	sel_CountSource(countsrc);
	sel_DatDir(datdir);
}

//----------------------------------------------------------------------------------------------------------------------------------------
// setting for clock synchronous mode
//----------------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::SCI_setting(string mode, string clock, string countsrc, string datdir, string polarity, string phase)
{
	sel_Mode(mode);
	sel_Clock(clock);
	sel_CountSource(countsrc);
	sel_DatDir(datdir);
	sel_ClkPolarity(polarity);
	sel_ClkPhase(phase);
}

//----------------------------------------------------------------------------------------------------------------------------------------
//select clock
//   for asynchronous:
//       00: on-chip baud rate generator (the SCK pin can be used as an input/output port)
//       01: on-chip baud rate generator (the SCK pin generator clock signal with the bit rate frequency)
//       1X: external clock (a clock signal of frequency 16 times that of the bit rate should be injected from the SCK pin)
//   for clock synchronous:
//       0X: internal clock (the SCK pin is clock output pin)
//       1X: external clock (the SCK pin is clock input pin)
//-----------------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::sel_Clock(string clock)
{
	SCiCR.write_bit<1>("TE", "0");
	SCiCR.write_bit<1>("RE", "0");
	if(SCiMD.read_bit<1>("SMS") == "0")
	{
		if(clock == "on-chip_SCK_I/O")
		{
			SCiCR.write_bit<2>("CKS", "00");
		}
		else if(clock == "on-chip_SCK_generate_clock")
		{
			SCiCR.write_bit<2>("CKS", "01");
		}
		else if(clock == "external_clock")
		{
			SCiCR.write_bit<2>("CKS", "11");
		}
		else
		{
			cout << "ERROR select clock!" << endl;
		}
	}
	if(SCiMD.read_bit<1>("SMS") == "1")
	{
		if(clock == "internal_clock")
		{
			SCiCR.write_bit<2>("CSK", "00");
		}
		else if(clock == "external_clock")
		{
			SCiCR.write_bit<2>("CSK", "11");
		}
		else
		{
			cout << "ERROR select clock!" << endl;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------
//select mode SCI
//   0: Asynchronous mode
//   1: Clock synchronous mode
//-----------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::sel_Mode(string mode)
{
	SCiCR.write_bit<1>("TE", "0");
	SCiCR.write_bit<1>("RE", "0");
	if(mode == "asynchronous")
	{
		SCiMD.write_bit<1>("SMS", "0");
	}
	else if(mode == "synchronous")
	{
		SCiMD.write_bit<1>("SMS", "1");
		SCiMD.write_bit<1>("SDLS", "0");

	}
	else
	{
		cout << "ERROR select mode!" << endl;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------
//select data length: valid only for asynchronous (SMS bit in SCiMD = 0), data length 8-bit for clock synchronous
//     0: Transmit/receive 8-bit data
//     1: Transmit/receive 7-bit data
//-------------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::sel_DatLen(string len)
{
	SCiCR.write_bit<1>("TE", "0");
	SCiCR.write_bit<1>("RE", "0");
	if((SCiMD.read_bit<1>("SMS") == "1") && (len == "7-bit"))
	{
		SCiMD.write_bit<1>("SDLS", "0");
		cout << "Data length in synchronous mode is only 8-bit" << endl;
	}
	else
	{
		if(len == "8-bit")
		{
			SCiMD.write_bit<1>("SDLS", "0");
		}
		else if(len == "7-bit")
		{
			SCiMD.write_bit<1>("SDLS", "1");
		}
		else
		{
			cout << "ERROR select data length!" << endl;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------
//enable parity: valid only for asynchronous (SMS bit in SCiMD = 0)
//     0: not parity
//     1: parity
//------------------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::en_Parity(string en)
{
	SCiCR.write_bit<1>("TE", "0");
	SCiCR.write_bit<1>("RE", "0");
	if(en == "parity")
	{
		SCiMD.write_bit<1>("PE", "1");
	}
	else if(en == "not_parity")
	{
		SCiMD.write_bit<1>("PE", "0");
	}
	else
	{
		cout << "ERROR enable parity!" << endl;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------
//select parity: valid only for asynchronous (SMS bit in SCiMD = 0)
//     0: even parity
//     1: odd parity
//----------------------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::sel_Parity(string parity)
{
	SCiCR.write_bit<1>("TE", "0");
	SCiCR.write_bit<1>("RE", "0");
	if(parity == "even")
	{
		SCiMD.write_bit<1>("OES", "0");
	}
	else if(parity == "odd")
	{
		SCiMD.write_bit<1>("OES", "1");
	}
	else
	{
		cout << "ERROR select parity!" << endl;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//select stop bit length: valid only for asynchronous (SMS bit in SCiMD = 0)
//     0: single stop bit
//     1: two stop bit
//------------------------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::sel_StopBitLen(string len)
{
	SCiCR.write_bit<1>("TE", "0");
	SCiCR.write_bit<1>("RE", "0");
	if(len == "single_stopbit")
	{
		SCiMD.write_bit<1>("TSTLS", "0");
	}
	else if(len == "two_stopbit")
	{
		SCiMD.write_bit<1>("TSTLS", "1");
	}
	else
	{
		cout << "ERROR select stop bit length!" << endl;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//select count source
//     00: 1/1 clock cycle of peripheral function clock A
//     01: 1/4 clock cycle of peripheral function clock A
//     10: 1/16 clock cycle of peripheral function clock A
//     11: 1/64 clock cycle of peripheral function clock A
//---------------------------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::sel_CountSource(string source)
{
	SCiCR.write_bit<1>("TE", "0");
	SCiCR.write_bit<1>("RE", "0");
	if(source == "1/1")
	{
		SCiMD.write_bit<2>("SCSS", "00");
	}
	else if(source == "1/4")
	{
		SCiMD.write_bit<2>("SCSS", "01");
	}
	else if(source == "1/16")
	{
		SCiMD.write_bit<2>("SCSS", "10");
	}
	else if(source == "1/64")
	{
		SCiMD.write_bit<2>("SCSS", "11");
	}
	else
	{
		cout << "ERROR select count source!" << endl;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
//select data direction
//     0: LSB-first transmission/reception
//     1: MSB-first transmission/reception
//--------------------------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::sel_DatDir(string dir)
{
	SCiCR.write_bit<1>("TE", "0");
	SCiCR.write_bit<1>("RE", "0");
	if((SCiMD.read_bit<1>("SDLS") == "1") & (dir == "MSB"))
	{
		SCiMD.write_bit<1>("SIDR", "0");
		cout << "Data direct in the case of 7-bit data is LSB-first" << endl;
	}
	else
	{
		if(dir == "LSB")
		{
		SCiEMD.write_bit<1>("SDIR", "0");
		}
		else if(dir == "MSB")
		{
			SCiEMD.write_bit<1>("SDIR", "1");
		}
		else
		{
			cout << "ERROR select data direction!" << endl;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//select clock polarity: valid only for clock synchronous mode
//     0: without polarity inversion
//		  data is output and input in synchronization wwith the falling and rising edges of the transmit/receive clock, respectively
//     1: without polarity inversion
//        data is output and input in synchronization wwith the rising and falling edges of the transmit/receive clock, respectively
//-------------------------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::sel_ClkPolarity(string polarity)
{
	SCiCR.write_bit<1>("TE", "0");
	SCiCR.write_bit<1>("RE", "0");
	if(polarity == "not_polarity")
	{
		SCiEMD.write_bit<1>("CKPOS", "0");
	}
	else if(polarity == "polarity")
	{
		SCiEMD.write_bit<1>("CKPOS", "1");
	}
	else
	{
		cout << "ERROR select clock polarity!" << endl;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------
//select clock phase: valid only for clock synchronous mode
//     0: the clock phase is not delayed
//     1: the clock phase is delayed by half a phase
//----------------------------------------------------------------------------------------------------------------------------------------------
void SCI_Func::sel_ClkPhase(string phase)
{
	SCiCR.write_bit<1>("TE", "0");
	SCiCR.write_bit<1>("RE", "0");
	if(phase == "not_delay_clock")
	{
		SCiEMD.write_bit<1>("CKPHS", "0");
	}
	else if(phase == "delay_clock")
	{
		SCiEMD.write_bit<1>("CKPHS", "1");
	}
	else
	{
		cout << "ERROR select clock phase" << endl;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
// function trans data for asynchronous mode
//----------------------------------------------------------------------------------------------------------------------------------------------------
int SCI_Func::SCI_check_num1(sc_bv<8> data)
{
	int result;
	int count = 0;
	for(int i = 0; i < 8; i++)
	{
		if(data[i] == SC_LOGIC_1) count = count++;
	}
	if(count % 2 == 0)
	{
		result = 0;
	}
	if(count % 2 != 0)
	{
		result = 1;
	}
	return result;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
// process data trans in asynchronous mode
//----------------------------------------------------------------------------------------------------------------------------------------------------
sc_bv<12> SCI_Func::SCI_ProcDatAsynTrans(sc_bv<8> data_buffTrans, sc_bv<12> &data)
{
	bool parity = true;
	//sc_bv<12> data_temp;
	int check_parity; // check parity is even or odd   ; check_parity = 0 even    ;   check_parity = 1   odd

	check_parity = SCI_check_num1(data_buffTrans);
	//cout << "[check_parity] " << check_parity << endl;
	data[11] = '0';

	switch(SCiMD.read_bit_uint<1>("SDLS"))
	{
	//-----------------------------------------------------------------------------------------------------------------------
	case 0: // 8-bit data

		//----------------------------------------------------------------------------------------
		switch(SCiEMD.read_bit_uint<1>("SDIR"))
		{
		case 0: // LSB-first
			data.range(10,3) = data_buffTrans;
			break;
		case 1: // MSB-first
			data.range(3,10) = data_buffTrans;
			break;
		}
		//----------------------------------------------------------------------------------------

		//----------------------------------------------------------------------------------------
		switch(SCiMD.read_bit_uint<1>("PE"))
		{
		case 0: // not parity
			parity = false;
			break;
		case 1: // parity
			if(check_parity == 0) // the number bit_1 is even
			{
				switch(SCiMD.read_bit_uint<1>("OES"))
				{
				case 0: // even parity
					data[2] = 0;
					break;
				case 1: // odd parity
					data[2] = 1;
					break;
				}
			}

			if(check_parity == 1) // the number bit_1 is odd
			{
				switch(SCiMD.read_bit_uint<1>("OES"))
				{
				case 0: // even parity
					data[2] = 1;
					break;
				case 1: // odd parity
					data[2] = 0;
					break;
				}
			}
			break;
		}
		//----------------------------------------------------------------------------------------

		switch(SCiMD.read_bit_uint<1>("TSTLS"))
		{
		case 0: // single stop bit
			if(parity == false) data[2] = "1";
			else data[1] = "1";
			break;
		case 1: // two stop bit
			if(parity == false) data.range(2,1) = "11";
			else data.range(1,0) = "11";
			break;
		}
		break;

	//-------------------------------------------------------------------------------------------------------------------------
	case 1: // 7-bit data
		data.range(10,4) = data_buffTrans.range(6,0); // bit b7 in SCiTB is not transmitted

		//----------------------------------------------------------------------------------------
		switch(SCiMD.read_bit_uint<1>("PE"))
		{
		case 0: // not parity
			parity = false;
			break;
		case 1: // parity
			if(check_parity == 0) // the number bit_1 is even
			{
				switch(SCiMD.read_bit_uint<1>("OES"))
				{
				case 0: // even parity
					data[3] = 0;
					break;
				case 1: // odd parity
					data[3] = 1;
					break;
				}
			}

			if(check_parity == 1) // the number bit_1 is odd
			{
				switch(SCiMD.read_bit_uint<1>("OES"))
				{
				case 0: // even parity
					data[3] = 1;
					break;
				case 1: // odd parity
					data[3] = 0;
					break;
				}
			}
			break;
		}
		break;

		//-------------------------------------------------------------------------------------------------
		switch(SCiMD.read_bit_uint<1>("TSTLS"))
		{
		case 0: // single stop bit
			if(parity == false) data[3] = "1";
			else data[2] = "1";
			break;
		case 1: // two stop bit
			if(parity == false) data.range(3,2) = "11";
			else data.range(2,1) = "11";
			break;
		}
		break;
	}
	return data;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
// process data recei in asynchronous mode
//----------------------------------------------------------------------------------------------------------------------------------------------------
sc_bv<8> SCI_Func::SCI_ProcDatAsynRecei(sc_bv<12> data_recei, sc_bv<8> &data)
{
	int check_parity;

	SCiSR.write_bit<1>("PERF", "0");
	SCiSR.write_bit<1>("FREF", "0");

	switch(SCiMD.read_bit_uint<1>("SDLS"))
	{
	case 0: // 8-bit
		switch(SCiEMD.read_bit_uint<1>("SDIR"))
		{
		case 0: // LSB-first
			data = data_recei.range(10,3);
			break;
		case 1: // MSB-first
			data = data_recei.range(3,10);
			break;
		}

		//------------------------------------check error in case 8-bit----------------------------
		if(data_recei[11] == SC_LOGIC_1) 
		{
			SCiSR.write_bit<1>("FREF", "1");
			//cout << "[frame_error] " << SCiSR.read_bit<1>("FREF") << endl;
		}
		else
		{
			switch(SCiMD.read_bit_uint<1>("PE"))
			{
			case 0: // parity not enabled
				if(data_recei[2] == SC_LOGIC_0) SCiSR.write_bit<1>("FREF", "1");
				break;
			case 1: // parity enabled
				if(data_recei[1] == SC_LOGIC_0) SCiSR.write_bit<1>("FREF", "1");
				else
				{
					check_parity = SCI_check_num1(data_recei.range(10,3));
					//cout << "[check_parity recei] " << check_parity << endl;
					if(check_parity == 0) // the number bit_1 is even
					{
						switch(SCiMD.read_bit_uint<1>("OES"))
						{
						case 0: // even parity
							if(data_recei[2] == SC_LOGIC_1) 
							{
								SCiSR.write_bit<1>("PERF", "1");
							}
							break;
						case 1: // odd parity
							if(data_recei[2] == SC_LOGIC_0) 
							{
								SCiSR.write_bit<1>("PERF", "1");
							}
							break;
						}
					}

					if(check_parity == 1) // the number bit_1 is odd
					{
						switch(SCiMD.read_bit_uint<1>("OES"))
						{
						case 0: // even parity
							if(data_recei[2] == SC_LOGIC_0) 
							{
								SCiSR.write_bit<1>("PERF", "1");
							}
							break;
						case 1: // odd parity
							if(data_recei[2] == SC_LOGIC_1) 
							{
								SCiSR.write_bit<1>("PERF", "1");
							}
							break;
						}
					}
					//cout << "[parity_error] " << SCiSR.read_bit<1>("PERF") << endl;
				}
				break;
			}
		}
		break;

	case 1: // 7-bit LSB-first
		data.range(6,0) = data_recei.range(10,4);

		//------------------------------------check error in case 7-bit----------------------------
		if(data_recei[11] == SC_LOGIC_1) SCiSR.write_bit<1>("FREF", "1");
		else
		{
			switch(SCiMD.read_bit_uint<1>("PE"))
			{
			case 0: // parity not enabled
				if(data_recei[3] == SC_LOGIC_0) SCiSR.write_bit<1>("FREF", "1");
				break;
			case 1: // parity enabled
				if(data_recei[2] == SC_LOGIC_0) SCiSR.write_bit<1>("FREF", "1");
				else
				{
					check_parity = SCI_check_num1(data_recei.range(11,4));
					//cout << "[check_parity recei] " << check_parity << endl;
					if(check_parity == 0) // the number bit_1 is even
					{
						switch(SCiMD.read_bit_uint<1>("OES"))
						{
						case 0: // even parity
							if(data_recei[3] == SC_LOGIC_1)
							{
								SCiSR.write_bit<1>("PREF", "1");
							}
							break;
						case 1: // odd parity
							if(data_recei[3] == SC_LOGIC_0) 
							{
								SCiSR.write_bit<1>("PREF", "1");
							}
							break;
						}
					}

					if(check_parity == 1) // the number bit_1 is odd
					{
						switch(SCiMD.read_bit_uint<1>("OES"))
						{
						case 0: // even parity
							if(data_recei[3] == SC_LOGIC_0) 
							{
								SCiSR.write_bit<1>("PREF", "1");
							}
							break;
						case 1: // odd parity
							if(data_recei[3] == SC_LOGIC_1)
							{
								SCiSR.write_bit<1>("PREF", "1");
							}
							break;
						}
					}
					//cout << "[parity_error] " << SCiSR.read_bit<1>("PREF") << endl;
				}
				break;
			}
		}
		break;
	}

	return data;
}