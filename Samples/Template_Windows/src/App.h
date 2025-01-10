#pragma once

#include "wiApplication.h"
#include "Character/Player.h"

class App : public wi::Application
{
public:
    void Initialize() override;
	void Update(float dt) override;
private:
	Player player;
};
