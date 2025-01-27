#pragma once

#include <filesystem>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
extern "C"
{
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
}



static void GameObject_SetPosition(int id, glm::vec3 pos);

class ScriptEngine
{
public:
	static ScriptEngine* Get();

	void Init();
	void Shutdown();
	void OnMonoDrawGui();
	void OnMonoUpdate();

	void processPressKey(std::string name, int value);
	void processReleaseKey(std::string name, int value);
	void processMouseKey(std::string name, int value);
	void processMousePos(glm::vec2 pos);



private:
	static ScriptEngine* instance;

	void InitMono();
	void ShutdownMono();

	MonoObject* monoInstance = nullptr;
	MonoMethod* methodDrawGui = nullptr;
	MonoMethod* methodUpdate = nullptr;

	MonoMethod* pressKeyboardKey = nullptr;
	MonoMethod* releaseKeyBoardKey = nullptr;
	MonoMethod* pressMouseKey = nullptr;

    MonoMethod* resetInput = nullptr;

	MonoMethod* mousePos = nullptr;
};