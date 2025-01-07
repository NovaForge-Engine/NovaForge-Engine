
#include "Application.h"
#include <NRI.h>


int main(int argc, char** argv) {
	Application app;

	bool startup = app.Init(argc,argv);
	spdlog::info("Application started with startup {}", startup);
	while (startup) {
		app.Update();

		app.Draw();	
		glfwPollEvents();
	}



	return 0;
}

