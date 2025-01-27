#include "ScriptEngine.h"

#include <fstream>
#include <iostream>
#include <cassert>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/threads.h>

#include "../Sandbox/src/imgui.h"

struct ScriptEngineData
{
	MonoDomain* RootDomain = nullptr;
	MonoDomain* AppDomain = nullptr;
	MonoAssembly* CoreAssembly = nullptr;
};

static ScriptEngineData* s_Data = nullptr;

ScriptEngine* ScriptEngine::instance = nullptr;

ScriptEngine* ScriptEngine::Get()
{
	if (!instance)
		instance = new ScriptEngine();
	return instance;
}

void ScriptEngine::Init()
{
	s_Data = new ScriptEngineData();
	InitMono();
}

void ScriptEngine::Shutdown()
{
	ShutdownMono();
	delete s_Data;
}

char* ReadBytes(const std::string& filepath, uint32_t* outSize)
{
	std::ifstream stream(filepath, std::ios::binary | std::ios::ate);
	if (!stream)
	{
		// Failed to open the file
		return nullptr;
	}
	std::streampos end = stream.tellg();
	stream.seekg(0, std::ios::beg);
	uint32_t size = static_cast<uint32_t>(end - stream.tellg());
	if (size == 0)
	{
		// File is empty
		return nullptr;
	}
	char* buffer = new char[size];
	stream.read(buffer, size);
	stream.close();
	*outSize = size;
	return buffer;
}

MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath)
{
	uint32_t fileSize = 0;
	char* fileData = ReadBytes(assemblyPath, &fileSize);
	MonoImageOpenStatus status;
	MonoImage* image =
		mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);
	if (status != MONO_IMAGE_OK)
	{
		const char* errorMessage = mono_image_strerror(status);
		return nullptr;
	}
	MonoAssembly* assembly =
		mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
	mono_image_close(image);
	delete[] fileData;
	return assembly;
}

void PrintAssemblyTypes(MonoAssembly* assembly)
{
	MonoImage* image = mono_assembly_get_image(assembly);
	const MonoTableInfo* typeDefinitionsTable =
		mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
	int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
	for (int32_t i = 0; i < numTypes; i++)
	{
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(typeDefinitionsTable, i, cols,
                           MONO_TYPEDEF_SIZE);
		const char* nameSpace =
			mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
		const char* name =
			mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
		// Replace Nova_CORE_TRACE with std::cout for now
		std::cout << nameSpace << "." << name << std::endl;
	}
}

void ScriptEngine::InitMono()
{
	mono_set_assemblies_path("lib\\mono\\4.5");

	MonoDomain* rootDomain = mono_jit_init("NovaJITRuntime");
	assert(rootDomain != nullptr);
	// Store the root domain pointer
	s_Data->RootDomain = rootDomain;

    s_Data->AppDomain = mono_domain_create_appdomain(const_cast<char*>("NovaScriptRuntime"), nullptr);
	mono_domain_set(s_Data->AppDomain, true);
	s_Data->CoreAssembly = LoadCSharpAssembly("Resources/Scripts/GameObjectBase.dll");
	PrintAssemblyTypes(s_Data->CoreAssembly);
	MonoImage* assemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
	MonoClass* monoRoot = mono_class_from_name(assemblyImage, "GameObjectBase", "Root");
	monoInstance = mono_object_new(s_Data->AppDomain, monoRoot);

	void* cntx = ImGui::GetCurrentContext();
	ImGuiMemAllocFunc allocFunc = nullptr;
	ImGuiMemFreeFunc freeFunc = nullptr;
	void* userData = nullptr;
	ImGui::GetAllocatorFunctions(&allocFunc, &freeFunc, &userData);
	MonoMethod* monoRootInitialize = mono_class_get_method_from_name(monoRoot, "Initialize", 3);
	void* params[3] = {&cntx, &allocFunc, &freeFunc};
	mono_runtime_invoke(monoRootInitialize, monoInstance, params, nullptr);

	methodUpdate = mono_class_get_method_from_name(monoRoot, "Update", 0);
	methodDrawGui = mono_class_get_method_from_name(monoRoot, "DrawGui", 0);


	MonoClass* monoRoot2 = mono_class_from_name(assemblyImage, "GameObjectBase", "InputManager");

	pressKeyboardKey =
		mono_class_get_method_from_name(monoRoot2, "PressKey", 2);
	pressMouseKey =
		mono_class_get_method_from_name(monoRoot2, "PressMouseKey", 2);
	mousePos =
		mono_class_get_method_from_name(monoRoot2, "MousePos", 1);
	releaseKeyBoardKey = mono_class_get_method_from_name(monoRoot2, "ReleaseKey", 2);


	mono_add_internal_call(
		"GameObjectBase.InternalCalls::GameObject_SetPosition",GameObject_SetPosition);

}



void ScriptEngine::OnMonoDrawGui()
{
	mono_runtime_invoke(methodDrawGui, monoInstance, nullptr, nullptr);

}

void ScriptEngine::OnMonoUpdate()
{
	mono_runtime_invoke(methodUpdate, monoInstance, nullptr, nullptr);
}

void ScriptEngine::processPressKey(std::string name, int value)
{
	void* params[2] = {mono_string_new(s_Data->AppDomain, name.c_str()),&value};
	mono_runtime_invoke(pressKeyboardKey, monoInstance, params, nullptr);
}

void ScriptEngine::processReleaseKey(std::string name, int value)
{
	void* params[2] = {mono_string_new(s_Data->AppDomain, name.c_str()),
	                   &value};
	mono_runtime_invoke(releaseKeyBoardKey, monoInstance, params, nullptr);
}

void ScriptEngine::processMouseKey(std::string name, int value)
{
	void* params[2] = {mono_string_new(s_Data->AppDomain, name.c_str()), &value};
	mono_runtime_invoke(pressMouseKey, monoInstance, params, nullptr);
}

void ScriptEngine::processMousePos(glm::vec2 pos)
{
	void* params[1] = {&pos};
	mono_runtime_invoke(mousePos, monoInstance, params, nullptr);
}



void ScriptEngine::ShutdownMono()
{
	s_Data->AppDomain = nullptr;
	s_Data->RootDomain = nullptr;


}

void GameObject_SetPosition(int id, glm::vec3 pos)
{
	spdlog::info("we have call from c# with {} and {} {} {}", id, pos.x, pos.y,
	             pos.z);
}
