#include "LoadRectangleSkin.h"

#include "Control.h"

#include <NAS2D/Renderer/RectangleSkin.h>


NAS2D::RectangleSkin loadRectangleSkin(const std::string& pathPrefix)
{
	return {
		Control::getImage(pathPrefix + "_top_left.png"),
		Control::getImage(pathPrefix + "_top_middle.png"),
		Control::getImage(pathPrefix + "_top_right.png"),
		Control::getImage(pathPrefix + "_middle_left.png"),
		Control::getImage(pathPrefix + "_middle_middle.png"),
		Control::getImage(pathPrefix + "_middle_right.png"),
		Control::getImage(pathPrefix + "_bottom_left.png"),
		Control::getImage(pathPrefix + "_bottom_middle.png"),
		Control::getImage(pathPrefix + "_bottom_right.png")
	};
}
