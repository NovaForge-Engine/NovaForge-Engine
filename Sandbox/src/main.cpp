
#include "Application.h"

#include <NRI.h>

int main(int argc, char** argv) {

	Application app;
	app.shouldClose = app.Init(argc, argv);

	spdlog::info("Application started with startup {}", app.shouldClose);
	while (app.shouldClose)
	{
		app.Update();
		app.Draw();
	}
	return 0;
}
