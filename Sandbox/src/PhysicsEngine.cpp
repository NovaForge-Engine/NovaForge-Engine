#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine()
	: tempAllocator(nullptr), jobSystem(nullptr), debugRenderer(nullptr),
	  collisionMatrix(collisionLayersCount, std::vector<bool>(collisionLayersCount, false))
{
}

PhysicsEngine::~PhysicsEngine()
{
	delete tempAllocator;
	delete jobSystem;
}

PhysicsEngine* PhysicsEngine::instance = nullptr;

PhysicsEngine* PhysicsEngine::Get()
{
	if (!instance)
		instance = new PhysicsEngine();
	return instance;
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

bool PhysicsEngine::CanCollide(CollisionLayer layer1, CollisionLayer layer2) const
{
	auto l1 = ToJoltLayer(layer1);
	auto l2 = ToJoltLayer(layer2);
	JPH_ASSERT(l1 < collisionMatrix.size() && l2 < collisionMatrix[l1].size(), "Trying to compare wrong collision layers");

	return collisionMatrix[l1][l2];
}

void PhysicsEngine::SetCollisionRule(CollisionLayer layer1, CollisionLayer layer2, bool collisionRule)
{
	auto l1 = ToJoltLayer(layer1);
	auto l2 = ToJoltLayer(layer2);
	JPH_ASSERT(l1 < collisionMatrix.size() && l2 < collisionMatrix[l1].size(), "Trying to set collision rule for wrong collision layers");

	collisionMatrix[l1][l2] = collisionRule;
	collisionMatrix[l2][l1] = collisionRule;
}