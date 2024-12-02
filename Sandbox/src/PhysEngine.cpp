#include "PhysEngine.h"

PhysicsEngine::PhysicsEngine()
	: tempAllocator(nullptr), jobSystem(nullptr), debugRenderer(nullptr)
{
}

PhysicsEngine::~PhysicsEngine()
{
	delete tempAllocator;
	delete jobSystem;
}

void PhysicsEngine::Initialize(float gravityScale)
{
	JPH::RegisterDefaultAllocator();
	JPH::Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)

	JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();

	tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
	jobSystem = new JPH::JobSystemThreadPool(
		JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers,
		JPH::thread::hardware_concurrency() - 1);

	physicsSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs,
	                    cMaxContactConstraints, broadPhaseLayerInterface,
	                    objectVsBroadphaseLayerFilter,
	                    objectVsObjectLayerFilter);

	physicsSystem.SetBodyActivationListener(&bodyActivationListener);
	physicsSystem.SetContactListener(&contactListener);
	
	physicsSystem.SetGravity(JPH::Vec3(0.0f, -9.81f / 100 * gravityScale, 0.0f));

	debugRenderer = new DebugRenderer();
}

void PhysicsEngine::UpdatePhysics()
{
	physicsSystem.Update(cDeltaTime, cCollisionSteps, tempAllocator, jobSystem);
}

JPH::PhysicsSystem* PhysicsEngine::GetPhysicsSystem()
{
	return &physicsSystem;
}

JPH::DebugRenderer* PhysicsEngine::GetDebugRenderer() const
{
	return debugRenderer;
}

void PhysicsEngine::Cleanup()
{
	JPH::UnregisterTypes();
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;
}