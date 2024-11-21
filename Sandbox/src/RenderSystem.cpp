#include "RenderSystem.h"

RenderSystem::RenderSystem()
{
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::Init()
{
	compositor.Init();
}

void RenderSystem::Shutdown()
{
	compositor.Shutdown();
}

void RenderSystem::Update()
{
}

void RenderSystem::FixedUpdate()
{
}

void RenderSystem::Draw()
{
	compositor.Draw();
}

void RenderSystem::PrepareFrame()
{
	compositor.PrepareFrame();
}
