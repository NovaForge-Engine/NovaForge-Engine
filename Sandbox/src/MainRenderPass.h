#pragma once
#include "IRenderPass.h"

class MainRenderPass: public IRenderPass{
public:
    void Init() override;
    void Draw() override;
    void Shutdown() override;
    void PrepareFrame() override;
    MainRenderPass();
    ~MainRenderPass();
};
