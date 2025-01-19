#pragma once
class IRenderPass
{
public:
	virtual void Draw() = 0;
	virtual void PrepareFrame() = 0;
	virtual void Init() = 0;
	virtual void Shutdown() = 0;
};
