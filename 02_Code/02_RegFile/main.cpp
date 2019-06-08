#include "RegFile.h"

int sc_main(int argc,char* argv[])
{
	RegFile regfile("RegFile", 8);

	regfile.Display();

	getch();
	return 0;
}