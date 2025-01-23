#include "PhysicsUtility.h"

#include "PhysicsEngine.h"

#include <spdlog/spdlog.h>

void TraceImpl(const char* message, ...)
{
	va_list list;
	va_start(list, message);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), message, list);
	va_end(list);

	spdlog::info("Jolt | TraceImpl: {}", buffer);
}

#ifdef JPH_ENABLE_ASSERTS

bool AssertFailedImpl(const char* inExpression, const char* inMessage,
                      const char* inFile, JPH::uint inLine)
{
	spdlog::critical("Jolt | Assert: {}:{}: ({}) {}", inFile, inLine,
	                 inExpression, (inMessage != nullptr ? inMessage : ""));
	return false;
}

#endif

bool ObjectLayerPairFilterImpl::ShouldCollide(JPH::ObjectLayer inObject1,
                                              JPH::ObjectLayer inObject2) const
{
	return ShouldCollide(FromJoltLayer(inObject1), FromJoltLayer(inObject2));
}

bool ObjectLayerPairFilterImpl::ShouldCollide(CollisionLayer inObject1,
                                              CollisionLayer inObject2) const
{
	return PhysicsEngine::Get()->CanCollide(inObject1, inObject2);
}

JPH::uint BroadPhaseLayerInterfaceImpl::GetNumBroadPhaseLayers() const
{
	return static_cast<JPH::uint>(broadPhaseLayersCount);
}

JPH::BroadPhaseLayer
BroadPhaseLayerInterfaceImpl::GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const
{
	JPH_ASSERT(inLayer < collisionLayersCount);
	return JPH::BroadPhaseLayer(inLayer);
}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)

const char* BroadPhaseLayerInterfaceImpl::GetBroadPhaseLayerName(
	JPH::BroadPhaseLayer inLayer) const
{
	return GetBroadPhaseLayerName(FromJoltLayer(inLayer));
}

const char* BroadPhaseLayerInterfaceImpl::GetBroadPhaseLayerName(
	BroadPhaseLayer inLayer) const
{
	return magic_enum::enum_name(inLayer).data();
}

#endif

bool ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(
	CollisionLayer inLayer1, BroadPhaseLayer inLayer2) const
{
	return PhysicsEngine::Get()->CanCollide(inLayer1, CollisionLayer(inLayer2));
}

bool ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(
	JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const
{
	return PhysicsEngine::Get()->CanCollide(
		FromJoltLayer(inLayer1), CollisionLayer(FromJoltLayer(inLayer2)));
}

JPH::ValidateResult ContactListener::OnContactValidate(
	const JPH::Body& inBody1, const JPH::Body& inBody2,
	JPH::RVec3Arg inBaseOffset,
	const JPH::CollideShapeResult& inCollisionResult)
{
	spdlog::debug(
		"Jolt | Contact validate callback: BodyID-1: {}, BodyID-2: {}",
		MapBodyID(inBody1.GetID()), MapBodyID(inBody2.GetID()));
	return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

void ContactListener::OnContactAdded(const JPH::Body& inBody1,
                                     const JPH::Body& inBody2,
                                     const JPH::ContactManifold& inManifold,
                                     JPH::ContactSettings& ioSettings)
{
	// Tenzy21: 02.12.2024
	// TODO: Use our collider implementation here
	// auto* collider1 =
	// reinterpret_cast<ColliderComponent*>(inBody1.GetUserData()); auto*
	// collider2 = reinterpret_cast<ColliderComponent*>(inBody2.GetUserData());

	// if (collider1 && collider2)
	//{
	//	collider1->OnCollision().Execute(collider2);
	//	collider2->OnCollision().Execute(collider1);
	// }
	spdlog::debug("Jolt | A contact was added: BodyID-1: {}, BodyID-2: {}",
	              MapBodyID(inBody1.GetID()), MapBodyID(inBody2.GetID()));
}

void ContactListener::OnContactPersisted(const JPH::Body& inBody1,
                                         const JPH::Body& inBody2,
                                         const JPH::ContactManifold& inManifold,
                                         JPH::ContactSettings& ioSettings)
{
	spdlog::debug("Jolt | A contact was persisted: BodyID-1: {}, BodyID-2: {}",
	              MapBodyID(inBody1.GetID()), MapBodyID(inBody2.GetID()));
}

void ContactListener::OnContactRemoved(
	const JPH::SubShapeIDPair& inSubShapePair)
{
	spdlog::debug("Jolt | A contact was removed: BodyID-1: {}, BodyID-2: {}",
	              MapBodyID(inSubShapePair.GetBody1ID()),
	              MapBodyID(inSubShapePair.GetBody2ID()));
}

void BodyActivationListener::OnBodyActivated(const JPH::BodyID& inBodyID,
                                             JPH::uint64 inBodyUserData)
{
	spdlog::debug("Jolt | A body got activated: BodyID: {}",
	              MapBodyID(inBodyID));
}

void BodyActivationListener::OnBodyDeactivated(const JPH::BodyID& inBodyID,
                                               JPH::uint64 inBodyUserData)
{
	spdlog::debug("Jolt | A body went to sleep: BodyID: {}",
	              MapBodyID(inBodyID));
}

void DebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo,
                             JPH::ColorArg inColor)
{
}

void DebugRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2,
                                 JPH::RVec3Arg inV3, JPH::ColorArg inColor,
                                 ECastShadow inCastShadow)
{
}

JPH::DebugRenderer::Batch
DebugRenderer::CreateTriangleBatch(const Triangle* inTriangles,
                                   int inTriangleCount)
{
	return nullptr;
}

JPH::DebugRenderer::Batch
DebugRenderer::CreateTriangleBatch(const Vertex* inVertices, int inVertexCount,
                                   const JPH::uint32* inIndices,
                                   int inIndexCount)
{
	return nullptr;
}

void DebugRenderer::DrawGeometry(JPH::RMat44Arg inModelMatrix,
                                 const JPH::AABox& inWorldSpaceBounds,
                                 float inLODScaleSq, JPH::ColorArg inModelColor,
                                 const GeometryRef& inGeometry,
                                 ECullMode inCullMode, ECastShadow inCastShadow,
                                 EDrawMode inDrawMode)
{

}

void DebugRenderer::DrawText3D(JPH::RVec3Arg inPosition,
                               const std::string_view& inString,
                               JPH::ColorArg inColor, float inHeight)
{
}