#include "PhysicsUtility.h"


void TraceImpl(const char* message, ...)
{
	va_list list;
	va_start(list, message);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), message, list);
	va_end(list);

	//TODO: Probably we need some macros for Logging
	printf_s("Jolt | TraceImpl: %s\n", buffer);
}


#ifdef JPH_ENABLE_ASSERTS

	bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine)
	{
		// TODO: We also need some macros for Asserts
		printf_s("Jolt | AssertFailedImpl: %s:%d: (%s) %s\n", inFile, inLine, inExpression, (inMessage != nullptr ? inMessage : ""));
		JPH_ASSERT(false);

		return true;
	}

#endif // JPH_ENABLE_ASSERTS

bool ObjectLayerPairFilterImpl::ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const
{
	return false;
}


BroadPhaseLayerInterfaceImpl::BroadPhaseLayerInterfaceImpl()
{
	// Create a mapping table from object to broad phase layer
	mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
	mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
	mObjectToBroadPhase[Layers::PLAYER] = BroadPhaseLayers::PLAYER;
}


JPH::uint BroadPhaseLayerInterfaceImpl::GetNumBroadPhaseLayers() const
{
	return BroadPhaseLayers::NUM_LAYERS;
}


JPH::BroadPhaseLayer BroadPhaseLayerInterfaceImpl::GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const
{
	JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
	return mObjectToBroadPhase[inLayer];
}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)

	const char* BroadPhaseLayerInterfaceImpl::GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const
	{
		switch ((JPH::BroadPhaseLayer::Type)inLayer)
		{
			case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
				return "NON_MOVING";
			case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
				return "MOVING";
			case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::PLAYER:
				return "PLAYER";
			default:
				JPH_ASSERT(false);
				return "INVALID";
		}
	}

#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

bool ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(
	JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const
{
	switch (inLayer1)
	{
		case Layers::PLAYER:
			return true;
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			JPH_ASSERT(false);
			return false;
	}
}

JPH::ValidateResult ContactListener::OnContactValidate(
	const JPH::Body& inBody1, const JPH::Body& inBody2,
	JPH::RVec3Arg inBaseOffset,
	const JPH::CollideShapeResult& inCollisionResult)
{
	// LOG:
	// std::cout << "Contact validate callback" << std::endl;
	return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

void ContactListener::OnContactAdded(const JPH::Body& inBody1,
                                       const JPH::Body& inBody2,
                                       const JPH::ContactManifold& inManifold,
                                       JPH::ContactSettings& ioSettings)
{
	// Tenzy21: 02.12.2024
	// TODO: Use our collider implementation here
	//auto* collider1 = reinterpret_cast<ColliderComponent*>(inBody1.GetUserData());
	//auto* collider2 = reinterpret_cast<ColliderComponent*>(inBody2.GetUserData());

	//if (collider1 && collider2)
	//{
	//	collider1->OnCollision().Execute(collider2);
	//	collider2->OnCollision().Execute(collider1);
	//}

	// LOG:
	// std::cout << "A contact was added" << std::endl;
}

void ContactListener::OnContactPersisted(
	const JPH::Body& inBody1, const JPH::Body& inBody2,
	const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
	// LOG:
	// std::cout << "A contact was persisted" << std::endl;
}

void ContactListener::OnContactRemoved(
	const JPH::SubShapeIDPair& inSubShapePair)
{
	// LOG:
	// std::cout << "A contact was removed" << std::endl;
}

void BodyActivationListener::OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData)
{
	// LOG:
	// std::cout << "A body got activated" << std::endl;
}

void BodyActivationListener::OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData)
{
	// LOG:
	// std::cout << "A body went to sleep" << std::endl;
}

void DebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
{
}

void DebugRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2,
                                   JPH::RVec3Arg inV3, JPH::ColorArg inColor,
                                   ECastShadow inCastShadow)
{
}

JPH::DebugRenderer::Batch DebugRenderer::CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount)
{
	return nullptr;
}

JPH::DebugRenderer::Batch DebugRenderer::CreateTriangleBatch( const Vertex* inVertices, int inVertexCount, const JPH::uint32* inIndices, int inIndexCount)
{
	return nullptr;
}

void DebugRenderer::DrawGeometry(
	JPH::RMat44Arg inModelMatrix, const JPH::AABox& inWorldSpaceBounds,
	float inLODScaleSq, JPH::ColorArg inModelColor,
	const GeometryRef& inGeometry, ECullMode inCullMode,
	ECastShadow inCastShadow, EDrawMode inDrawMode)
{
}

void DebugRenderer::DrawText3D(JPH::RVec3Arg inPosition,
                                 const std::string_view& inString,
                                 JPH::ColorArg inColor, float inHeight)
{
}