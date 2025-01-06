
#include "Application.h"
#include <NRI.h>


int main(int argc, char** argv) {
  constexpr auto api = nri::GraphicsAPI::D3D12;


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

