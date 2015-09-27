#include "NAS2D/NAS2D.h"

#include "EditState.h"

using namespace std;


int main(int argc, char *argv[])
{

	Game game("Thingy", argv[0]);
	game.go(new EditState());

	return 0;
}
