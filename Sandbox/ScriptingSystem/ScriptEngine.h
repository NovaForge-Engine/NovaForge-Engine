#pragma once

#include <filesystem>

extern "C"
{
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
}

class ScriptEngine
{
public:
	static ScriptEngine* Get();

	void Init();
	void Shutdown();
	void OnMonoDrawGui();
	void OnMonoUpdate();

	void ProcessInput(std::string name, int value);


private:
	static ScriptEngine* instance;

	void InitMono();
	void ShutdownMono();

	MonoObject* monoInstance = nullptr;
	MonoMethod* methodDrawGui = nullptr;
	MonoMethod* methodUpdate = nullptr;
	MonoMethod* processUnput = nullptr;
};