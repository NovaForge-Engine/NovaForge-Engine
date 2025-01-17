#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/Core.h>

#include "PhysicsUtility.h"
class PhysicsEngine
{
public:
	PhysicsEngine();
	~PhysicsEngine();

	static PhysicsEngine* Get();

	void Init(float gravityScale = 1.0f);
	void Terminate();
	void Update();

	JPH::PhysicsSystem* GetPhysicsSystem();
	JPH::DebugRenderer* GetDebugRenderer() const;

	bool CanCollide(CollisionLayer layer1, CollisionLayer layer2) const;
	void SetCollisionRule(CollisionLayer layer1, CollisionLayer layer2,
	                      bool collisionRule);

	JoltBodyID GetBodyID(NovaBodyID novaBodyId);
	NovaBodyID GetBodyID(JoltBodyID joltBodyId);

	NovaBodyID CreateBody(JPH::BodyCreationSettings& inSettings, float mass);
	NovaBodyID CreateBody(JPH::ShapeSettings* inShapeSettings,
	                      JPH::RVec3Arg inPosition, JPH::QuatArg inRotation,
	                      JPH::EMotionType inMotionType,
	                      JPH::ObjectLayer collisionLayer, float mass);
	NovaBodyID CreateBody(JPH::Shape* inShape, JPH::RVec3Arg inPosition,
	                      JPH::QuatArg inRotation,
	                      JPH::EMotionType inMotionType,
	                      JPH::ObjectLayer collisionLayer, float mass);

	NovaBodyID CreateAndAddBody(const JPH::Shape* shape,
	                            const JPH::Vec3& position,
	                            const JPH::Quat& rotation,
	                            JPH::EMotionType motionType,
	                            JPH::ObjectLayer collisionLayer, float mass);

	void RemoveBody(NovaBodyID id);
	void DestroyBody(NovaBodyID id);
	void RemoveAndDestroyBody(NovaBodyID id);

	std::pair<JPH::Vec3, JPH::Quat> GetBodyTransform(NovaBodyID id);
	void SetBodyTransform(NovaBodyID id, const JPH::Vec3& position,
	                      const JPH::Quat& rotation);

private:
	static PhysicsEngine* instance;

	static constexpr float fpsPhys = 120.0f;

	static constexpr JPH::uint cMaxBodies = 1024;
	static constexpr JPH::uint cNumBodyMutexes = 0;
	static constexpr JPH::uint cMaxBodyPairs = 1024;
	static constexpr JPH::uint cMaxContactConstraints = 1024;
	const int cCollisionSteps = 1;
	const float cDeltaTime = 1.0f / fpsPhys;

	std::vector<std::vector<bool>> collisionMatrix;

	std::unordered_map<NovaBodyID, JoltBodyID> novaToJoltBodyIdTable;
	std::unordered_map<JoltBodyID, NovaBodyID> joltToNovaBodyIdTable;
	NovaBodyID nextBodyID = 0;

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

inline const JoltBodyID MapBodyID(NovaBodyID id)
{
	return PhysicsEngine::Get()->GetBodyID(id);
}

inline const NovaBodyID MapBodyID(JoltBodyID id)
{
	return PhysicsEngine::Get()->GetBodyID(id);
}