
#include "Application.h"
#include <NRI.h>


int main(int argc, char** argv) {
  constexpr auto api = nri::GraphicsAPI::D3D12;

	Sample sample;
	Test test;
	auto collisions = collisionLayersCount;
	auto bpLayers = broadPhaseLayersCount;
	ID testBodyId = test.TryCreateBody();

	Application app;

	bool startup = app.Init(api);
	spdlog::info("Application started with startup {}", startup);
	while (startup) {
		sample.RenderFrame();
		test.Update();

		JPH::Vec3 location = test.GetPhys()->GetBodyTransform(testBodyId).first;
		std::cout << location.GetX() << " " << location.GetY() << " " << location.GetZ() << std::endl;
		app.Update();

		app.Draw();	
		glfwPollEvents();
	}
	return 0;
}

