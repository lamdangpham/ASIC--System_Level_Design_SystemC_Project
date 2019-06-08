#include "Memory.h"

int sc_main(int argc,char* argv[])
{
	int size = 32;

	Memory Mem("Memory", size);

	Mem.Display();

	Mem.Put_Data("0000000000000000", "11001010");
	Mem.Put_Data("0000000000000001", "01011010");
	Mem.Put_Data("0000000000000010", "00011100");
	Mem.Put_Data("0000000000000011", "11111000");
	Mem.Put_Data("0000000000000100", "00101101");
	Mem.Put_Data("0000000000000101", "10101010");
	Mem.Display();

	DatWidth dat;
	dat = Mem.Get_Data("0000000000000100");
	cout << dat;

	getch();
	return 0;
}