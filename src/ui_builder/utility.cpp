#include "utility.h"

#include "NAS2D/NAS2D.h"

#include "NAS2D/Mixer/Mixer_SDL.h"
#include "NAS2D/Renderer/OGL_Renderer.h"

using namespace NAS2D;


void createMixer()
{
	Utility<Mixer>::instantiateDerived(new Mixer_SDL());
}


void createRenderer()
{
	try
	{
		Utility<Renderer>::instantiateDerived(new OGL_Renderer("UI Builder"));
	}
	catch (std::exception e)
	{
		throw;
	}
	catch (...)
	{
		throw std::runtime_error("Unhandled exception occured while creating a Renderer.");
	}
}


void initNas2d(const std::string& argv_0, const std::string& startPath, const std::string& config_path)
{
	std::cout << "Starting NAS2D:" << std::endl << "==============================" << std::endl << std::endl;

	Utility<Filesystem>::get().init(argv_0, startPath);

	Configuration& cf = Utility<Configuration>::get();
	cf.load(config_path);
	validateVideoResolution();
	cf.save();

	createMixer();
	createRenderer();

	std::cout << std::endl << "==============================" << std::endl << std::endl;
}



void updateHandlePositions(Control* w, NAS2D::Rectangle_2d& top_left, NAS2D::Rectangle_2d& top_right, NAS2D::Rectangle_2d& bottom_left, NAS2D::Rectangle_2d& bottom_right)
{
	top_left(w->positionX() - (HANDLE_SIZE + HANDLE_MARGIN), w->positionY() - (HANDLE_SIZE + HANDLE_MARGIN), HANDLE_SIZE, HANDLE_SIZE);
	top_right(w->positionX() + w->width() + HANDLE_MARGIN, w->rect().y() - (HANDLE_SIZE + HANDLE_MARGIN), HANDLE_SIZE, HANDLE_SIZE);

	bottom_left(w->positionX() - (HANDLE_SIZE + HANDLE_MARGIN), w->positionY() + w->height() + HANDLE_MARGIN, HANDLE_SIZE, HANDLE_SIZE);
	bottom_right(w->positionX() + w->width() + HANDLE_MARGIN, w->positionY() + w->height() + HANDLE_MARGIN, HANDLE_SIZE, HANDLE_SIZE);
}


void updateRectPosition(Rectangle_2d& p, int dX, int dY)
{
	p.x(p.x() + dX);
	p.y(p.y() + dY);
}


// Makes sure video resolution is never less than 800x600
void validateVideoResolution()
{
	Configuration& cf = Utility<Configuration>::get();

	if (cf.graphicsWidth() < 800 || cf.graphicsHeight() < 600)
	{
		cf.graphicsWidth(800);
		cf.graphicsHeight(600);
	}
}
