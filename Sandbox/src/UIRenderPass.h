#pragma once
#include "IRenderPass.h"


class UIRenderPass: public IRenderPass {
public:
	void Init() override;
	void Draw() override;
	void Shutdown() override;
	void PrepareFrame() override;
	UIRenderPass();
	~UIRenderPass();
};

