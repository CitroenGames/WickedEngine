#pragma once

#include "WickedEngine.h"

class Game : public wi::Application
{
public:
	Game();

	void Initialize() override;

	wi::RenderPath3D renderpath;
private:
};
