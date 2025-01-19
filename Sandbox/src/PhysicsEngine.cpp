#include <spdlog/spdlog.h>
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

void PhysicsEngine::Init(float gravityScale)
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

	spdlog::info("PhysicsEngine initialized");
}

void PhysicsEngine::Terminate()
{
	JPH::UnregisterTypes();
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;
	delete instance;
}

void PhysicsEngine::Update()
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


JoltBodyID PhysicsEngine::GetBodyID(NovaBodyID novaBodyId)
{
	JPH_ASSERT(novaToJoltBodyIdTable.contains(novaBodyId), "Trying get JoltBodyID with invalid NovaBodyID");
	return novaToJoltBodyIdTable.at(novaBodyId);
}


NovaBodyID PhysicsEngine::GetBodyID(JoltBodyID joltBodyId)
{
	JPH_ASSERT(joltToNovaBodyIdTable.contains(joltBodyId), "Trying get NovaBodyID with invalid JoltBodyID");
	return joltToNovaBodyIdTable.at(joltBodyId);
}


NovaBodyID PhysicsEngine::CreateBody(JPH::BodyCreationSettings& inSettings, float mass)
{
	JPH::MassProperties msp;
	msp.ScaleToMass(mass);
	inSettings.mMassPropertiesOverride = msp;
	inSettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;

	NovaBodyID bodyID = nextBodyID++;
	JPH::Body* body = bodyInterface.CreateBody(inSettings);

	novaToJoltBodyIdTable[bodyID] = body->GetID();
	joltToNovaBodyIdTable[body->GetID()] = bodyID;
	return bodyID;
}

//(@Tenzy21 | 04.01.2025) TODO: Implement/Fix later
// 
//ID PhysicsEngine::CreateBody(JPH::ShapeSettings* inShapeSettings, JPH::RVec3Arg inPosition, JPH::QuatArg inRotation, JPH::EMotionType inMotionType, JPH::ObjectLayer collisionLayer, float mass)
//{
//	return CreateBody(JPH::BodyCreationSettings(inShapeSettings, inPosition, inRotation, inMotionType, collisionLayer), mass);
//}
//
//
//ID PhysicsEngine::CreateBody(JPH::Shape* inShape, JPH::RVec3Arg inPosition, JPH::QuatArg inRotation, JPH::EMotionType inMotionType, JPH::ObjectLayer collisionLayer, float mass)
//{
//	return CreateBody(JPH::BodyCreationSettings(inShape, inPosition, inRotation, inMotionType, collisionLayer), mass);
//}


//(@Tenzy21 | 04.01.2025) TODO: Separate adding and creating logic + Implement other signatures later
NovaBodyID PhysicsEngine::CreateAndAddBody(const JPH::Shape* shape, const JPH::Vec3& position, const JPH::Quat& rotation, JPH::EMotionType motionType, JPH::ObjectLayer collisionLayer, float mass)
{
	NovaBodyID bodyID = nextBodyID++;

	JPH::BodyCreationSettings bodySettings(shape, position, rotation, motionType, collisionLayer);
	JPH::MassProperties msp;
	msp.ScaleToMass(mass);
	bodySettings.mMassPropertiesOverride = msp;
	bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;

	JPH::Body* body = bodyInterface.CreateBody(bodySettings);
	novaToJoltBodyIdTable[bodyID] = body->GetID();
	joltToNovaBodyIdTable[body->GetID()] = bodyID;
	bodyInterface.AddBody(body->GetID(), JPH::EActivation::Activate);

	return bodyID;
}

void PhysicsEngine::RemoveBody(NovaBodyID id)
{
	if (novaToJoltBodyIdTable.find(id) != novaToJoltBodyIdTable.end()) {
		bodyInterface.RemoveBody(novaToJoltBodyIdTable[id]);
	}
	else {
		spdlog::warn("Jolt | RemoveBody: Trying to remove non-existed body (ID: {})", id);
	}
}
void PhysicsEngine::DestroyBody(NovaBodyID id)
{
	if (novaToJoltBodyIdTable.find(id) != novaToJoltBodyIdTable.end())
	{
		JoltBodyID bodyID = novaToJoltBodyIdTable[id];
		novaToJoltBodyIdTable.erase(id);
		joltToNovaBodyIdTable.erase(bodyID);
		bodyInterface.DestroyBody(bodyID);
	}
	else {
		spdlog::warn("Jolt | DestroyBody: Trying to destroy non-existed body (ID: {})", id);
	}
}

void PhysicsEngine::RemoveAndDestroyBody(NovaBodyID id)
{
	RemoveBody(id);
	DestroyBody(id);
}

std::pair<JPH::Vec3, JPH::Quat> PhysicsEngine::GetBodyTransform(NovaBodyID id)
{
	JoltBodyID bodyID = GetBodyID(id);
	JPH::Vec3 position;
	JPH::Quat rotation;
	bodyInterface.GetPositionAndRotation(bodyID, position, rotation);

	return {position, rotation};
}

void PhysicsEngine::SetBodyTransform(NovaBodyID id, const JPH::Vec3& position, const JPH::Quat& rotation)
{
	JoltBodyID bodyID = GetBodyID(id);
	bodyInterface.SetPositionAndRotation(bodyID, position, rotation, JPH::EActivation::Activate);
}
