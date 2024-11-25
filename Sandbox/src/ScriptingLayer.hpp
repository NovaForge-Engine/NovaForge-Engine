#pragma once

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"

#include "include/EngineLayer.h"

namespace NovaEngine {

class ScriptingLayer: public EngineLayer {
    public: 
    void init() override;
    void shutdown() override;
    void update() override;
    void fixedUpdate() override;

    private:
    void initMono();
    MonoDomain *_rootDomain, *_appDomain;
};

} // namespace NovaEngine
