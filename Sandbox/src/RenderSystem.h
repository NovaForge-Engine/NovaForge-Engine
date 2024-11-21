#pragma once
#include "ISystem.h"
#include "PassCompositor.h"
class RenderSystem : public ISystem{

    private:
	PassCompositor compositor;
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

