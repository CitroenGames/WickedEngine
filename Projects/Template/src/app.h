#pragma once

#include "WickedEngine.h"

class SampleRenderPath : public wi::RenderPath3D
{
public:
	void Load() override;
	void Update(float dt) override;
};

class SampleApp : public wi::Application
{
public:
    SampleRenderPath render;
	void Initialize() override;
};
