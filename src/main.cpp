#include "NAS2D/NAS2D.h"

#include "States/GameState.h"
#include "States/PlanetSelectState.h"

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

		game.mount("fonts.dat");
		game.mount("maps.dat");
		game.mount("planets.dat");
		game.mount("robots.dat");
		game.mount("sfx.dat");
		game.mount("structures.dat");
		game.mount("ui.dat");

		Filesystem& f = Utility<Filesystem>::get();
		
		if (!f.exists(constants::SAVE_GAME_PATH))
			f.makeDirectory(constants::SAVE_GAME_PATH);

		game.go(new PlanetSelectState());
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
