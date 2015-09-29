#include "NAS2D/NAS2D.h"

using namespace NAS2D;

#include "GameState.h"

using namespace std;


int main(int argc, char *argv[])
{

	try
	{
		Game game("OutpostHD", argv[0]);
		game.go(new GameState("maps/02"));
	}
	catch(Exception& e)
	{
		cout << "EXCEPTION (" << e.getBriefDescription() << "): " << e.getDescription() << endl;
	}

	return 0;
}
