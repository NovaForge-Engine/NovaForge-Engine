#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include "PhysicsUtility.h"


class PhysicsEngine
{
public:
	PhysicsEngine();
	~PhysicsEngine();
	void Initialize(float gravityScale = 1.0f);
	void UpdatePhysics();
	JPH::PhysicsSystem* GetPhysicsSystem();
	JPH::DebugRenderer* GetDebugRenderer() const;
	void Cleanup();

private:
	static constexpr float fpsPhys = 120.0f;

	static constexpr JPH::uint cMaxBodies = 1024;
	static constexpr JPH::uint cNumBodyMutexes = 0;
	static constexpr JPH::uint cMaxBodyPairs = 1024;
	static constexpr JPH::uint cMaxContactConstraints = 1024;
	const int cCollisionSteps = 1;
	const float cDeltaTime = 1.0f / fpsPhys;

	JPH::TempAllocatorImpl* tempAllocator;
	JPH::JobSystemThreadPool* jobSystem;
	JPH::PhysicsSystem physicsSystem;
	BodyActivationListener bodyActivationListener;
	ContactListener contactListener;
	BroadPhaseLayerInterfaceImpl broadPhaseLayerInterface;
	ObjectVsBroadPhaseLayerFilterImpl objectVsBroadphaseLayerFilter;
	ObjectLayerPairFilterImpl objectVsObjectLayerFilter;
	JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
	JPH::DebugRenderer* debugRenderer;
};