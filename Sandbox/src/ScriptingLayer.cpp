#include <stdexcept>

#include "ScriptingLayer.hpp"

namespace NovaEngine {

void ScriptingLayer::init() {
    initMono();
}

void ScriptingLayer::initMono() {
    mono_set_assemblies_path("mono/lib");

    _rootDomain = mono_jit_init("NovaScriptingRuntime");
    if (nullptr == _rootDomain) {
        throw std::runtime_error("Unable to initialzie Mono runtime!");
    }

    _appDomain = mono_domain_create_appdomain("NovaGameDomain", nullptr);
    mono_domain_set(_appDomain, true);
}

} // namespace NovaEngine