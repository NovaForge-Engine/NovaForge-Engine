#pragma once
#include "ISystem.h"

class RenderSystem : public ISystem
{

private:
public:
	RenderSystem();
	~RenderSystem();
	void Init() override;
	void Shutdown() override;
	void Update() override;
	void FixedUpdate() override;
	void PrepareFrame();
	void Draw();
};
