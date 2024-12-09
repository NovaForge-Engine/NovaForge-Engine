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
	jobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, JPH::thread::hardware_concurrency() - 1);

	physicsSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, broadPhaseLayerInterface, objectVsBroadphaseLayerFilter, objectVsObjectLayerFilter);

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


JPH::BodyID PhysicsEngine::GetBodyID(ID id)
{
	JPH_ASSERT(id < bodyIdTable.size(), "Trying get BodyID with invalid id");
	return bodyIdTable.at(id);
}


ID PhysicsEngine::AddBody(const JPH::Vec3& position, const JPH::Quat& rotation, const JPH::Shape* shape, JPH::EMotionType motionType, JPH::ObjectLayer collisionLayer, float mass)
{
	static ID nextBodyID = 1;
	ID bodyID = nextBodyID++;

	JPH::BodyCreationSettings bodySettings(shape, position, rotation, motionType, collisionLayer);
	JPH::MassProperties msp;
	msp.ScaleToMass(mass);
	bodySettings.mMassPropertiesOverride = msp;
	bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;

	JPH::Body* body = bodyInterface.CreateBody(bodySettings);
	bodyIdTable[bodyID] = body->GetID();
	bodyInterface.AddBody(body->GetID(), JPH::EActivation::Activate);

	return bodyID;
}


void PhysicsEngine::RemoveBody(ID id)
{
	if (bodyIdTable.find(id) != bodyIdTable.end())
	{
		JPH::BodyID bodyID = bodyIdTable[id];
		bodyInterface.RemoveBody(bodyID);
		bodyIdTable.erase(id);
	}
}


std::pair<JPH::Vec3, JPH::Quat> PhysicsEngine::GetBodyTransform(ID id)
{
	JPH::BodyID bodyID = GetBodyID(id);
	JPH::Vec3 position;
	JPH::Quat rotation;
	bodyInterface.GetPositionAndRotation(bodyID, position, rotation);

	return {position, rotation};
}


void PhysicsEngine::SetBodyTransform(ID id, const JPH::Vec3& position, const JPH::Quat& rotation)
{
	JPH::BodyID bodyID = GetBodyID(id);
	bodyInterface.SetPositionAndRotation(bodyID, position, rotation, JPH::EActivation::Activate);
}
