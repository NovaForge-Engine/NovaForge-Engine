
#include "Application.h"
#include <NRI.h>


int main(int argc, char** argv) {
	constexpr auto api = nri::GraphicsAPI::VK;

	Application app;

	bool startup = app.Init(api);
	spdlog::info("Application started with startup {}", startup);
	while (startup) {
		app.Update();

		app.Draw();	
		glfwPollEvents();
	}



	return 0;
}

