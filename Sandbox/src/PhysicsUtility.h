#pragma once

#define JPH_ENABLE_ASSERTS

#include <Jolt/Jolt.h>
#include <Jolt/Math/Real.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include "Jolt/RegisterTypes.h"
#include <Jolt/Renderer/DebugRenderer.h>
#include <magic_enum/magic_enum.hpp>

#include <iostream>
#include <cstdarg>

using namespace JPH::literals;
using NovaBodyID = JPH::uint32;
using JoltBodyID = JPH::BodyID;

void TraceImpl(const char* inFMT, ...);

#ifdef JPH_ENABLE_ASSERTS

bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine);

#endif

enum class CollisionLayer : JPH::ObjectLayer 
{
	NO_COLLISION = 0,
	NON_MOVING = 1,
	MOVING = 2,
	PLAYER = 3
};

constexpr JPH::ObjectLayer collisionLayersCount = magic_enum::enum_count<CollisionLayer>();

enum class BroadPhaseLayer : JPH::BroadPhaseLayer::Type
{
	NON_MOVING = 0,
	MOVING = 1,
	PLAYER = 2,
};

constexpr JPH::BroadPhaseLayer::Type broadPhaseLayersCount = magic_enum::enum_count<BroadPhaseLayer>();

inline const JPH::ObjectLayer ToJoltLayer(CollisionLayer layer)
{
	return static_cast<JPH::ObjectLayer>(layer);
}

inline const CollisionLayer FromJoltLayer(JPH::ObjectLayer layer)
{
	return static_cast<CollisionLayer>(layer);
}

inline const JPH::BroadPhaseLayer ToJoltLayer(BroadPhaseLayer layer)
{
	return JPH::BroadPhaseLayer(static_cast<JPH::BroadPhaseLayer::Type>(layer));
}

inline const BroadPhaseLayer FromJoltLayer(JPH::BroadPhaseLayer layer)
{
	return static_cast<BroadPhaseLayer>(layer.GetValue());
}

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
public:
	bool ShouldCollide(CollisionLayer inObject1, CollisionLayer inObject2) const;
	virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override;
};

class BroadPhaseLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
public:
	virtual JPH::uint GetNumBroadPhaseLayers() const override;
	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)

	virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override;
	const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const;

#endif
};

class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
	bool ShouldCollide(CollisionLayer inLayer1, BroadPhaseLayer inLayer2) const;
	virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override;
};

class ContactListener : public JPH::ContactListener
{
public:
	virtual JPH::ValidateResult OnContactValidate(const JPH::Body& inBody1, const JPH::Body& inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult) override;
	virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;
	virtual void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;
	virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override;
};

class BodyActivationListener : public JPH::BodyActivationListener
{
public:
	virtual void OnBodyActivated(const JoltBodyID& inBodyID, JPH::uint64 inBodyUserData) override;
	virtual void OnBodyDeactivated(const JoltBodyID& inBodyID, JPH::uint64 inBodyUserData) override;
};

class DebugRenderer : public JPH::DebugRenderer
{
public:
	virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
	virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override;
	virtual Batch CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount) override;
	virtual Batch CreateTriangleBatch(const Vertex* inVertices, int inVertexCount, const JPH::uint32* inIndices, int inIndexCount) override;
	virtual void DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox& inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode) override;
	virtual void DrawText3D(JPH::RVec3Arg inPosition, const std::string_view& inString, JPH::ColorArg inColor, float inHeight) override;
};