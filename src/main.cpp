#include "NAS2D/NAS2D.h"
#include "GameState.h"

#include <iostream>
#include <fstream>

using namespace NAS2D;
using namespace std;


int main(int argc, char *argv[])
{

	//Crude way of redirecting stream buffer when building in release (no console)
#ifdef NDEBUG
	std::streambuf *backup;
	std::ofstream filestr;
	filestr.open("ophd.log");

	backup = std::cout.rdbuf();
	std::cout.rdbuf(filestr.rdbuf());
#endif

	try
	{
		Game game("OutpostHD", argv[0]);
		game.go(new GameState("maps/mars_04"));
	}
	catch(Exception& e)
	{
		cout << "EXCEPTION (" << e.getBriefDescription() << "): " << e.getDescription() << endl;
	}

#ifdef NDEBUG
	filestr.close();
#endif

	return 0;
}
