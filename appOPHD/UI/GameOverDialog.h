#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>

#include <NAS2D/Signal/Delegate.h>


class GameOverDialog : public Window
{
public:
	using ClickDelegate = NAS2D::Delegate<void()>;

public:
	GameOverDialog(ClickDelegate clickHandler);

	void update() override;

private:
	void onClose();

	const NAS2D::Image& mHeader;

	Button btnClose;

	ClickDelegate mClickHandler;
};
