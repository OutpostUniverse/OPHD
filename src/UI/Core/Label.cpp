#include "Label.h"

#include "../../Constants.h"
#include "../../FontManager.h"

#include "NAS2D/Renderer/Renderer.h"
#include "NAS2D/Resources/Font.h"

#include "NAS2D/MathUtils.h"
#include "NAS2D/Utility.h"

#include <locale>

static const int FIELD_PADDING = 4;
static NAS2D::Font* TXT_FONT = nullptr;

Label::Label() {
    TXT_FONT = NAS2D::Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
    height(static_cast<float>(TXT_FONT->height() + FIELD_PADDING * 2));
}

Label::~Label() {

}

void Label::font(NAS2D::Font* font) {
    TXT_FONT = font;
    height(static_cast<float>(TXT_FONT->height() + FIELD_PADDING * 2));
}

void Label::update() {
    if(!visible()) { return; }

    NAS2D::Renderer& r = NAS2D::Utility<NAS2D::Renderer>::get();

    r.drawText(*TXT_FONT, text(), positionX() + FIELD_PADDING, positionY() + FIELD_PADDING, textColor.red(), textColor.green(), textColor.blue(), textColor.alpha());
}

void Label::color(const NAS2D::Color& color) {
    textColor = color;
}
