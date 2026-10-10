//==============================================================
//  Graphics Programming 1 (2026-2027)
//  Authors : Matthieu Delaere
//  Copyright (c) 2026 Matthieu Delaere. All rights reserved.
//==============================================================
#ifndef INTERSECTIONS_HEADER
#define INTERSECTIONS_HEADER

//--- Standard Includes ---
#include <cassert>

//--- Framework Includes ---
#include <matrix.h>
#include <primitives.h>
#include <valarray>
#include <assimp/MathFunctions.h>

namespace gfx
{
	//--- Intersection Tests ---
	[[maybe_unused]] static bool HitTestSphere(const Sphere& sphere,
	                                           const Ray& ray,
	                                           RayHitRecord& hit_record,
	                                           const bool ignore_hit_record = false)
	{
		const Vector3 L {sphere.origin - ray.origin};
		const float projection {Vector3::Dot(L, ray.direction_)};
		const float rejectionMagnitude {Vector3::Reject(L, ray.direction_).Magnitude()};

		if (rejectionMagnitude >= sphere.radius) return false;
		if (ignore_hit_record) return true;

		const float projectionInsideSphere {
		  sqrtf(powf(sphere.radius, 2) - powf(rejectionMagnitude, 2))};
		const float distance {projection - projectionInsideSphere};
		hit_record.ray = ray;
		hit_record.t = distance;

		return true;
	}

	[[maybe_unused]] static bool HitTestPlane(const Plane& plane,
	                                          const Ray& ray,
	                                          RayHitRecord& hit_record,
	                                          const bool ignore_hit_record = false)
	{
		const Vector3 originDistanceVector {plane.origin - ray.origin};
		const float dn {Vector3::Dot(ray.direction_, plane.normal_)};

		//backface culling
		if (dn >= 0) return false;

		const float t {Vector3::Dot(originDistanceVector, plane.normal_) / dn};
		if (t <= 0) return false; //plane behind camera

		//check if point in finite plane
		const Vector3 hitPoint {ray.origin + t * ray.direction_};
		const float ONB_u {Vector3::Dot(hitPoint, plane.tangent)};
		const float ONB_v {Vector3::Dot(hitPoint, Vector3::Cross(plane.normal_, plane.tangent))};

		if (abs(ONB_u) > plane.half_extent->x || abs(ONB_v) > plane.half_extent->y) return false;

		if (ignore_hit_record) return true;

		hit_record.t = t;
		return true;
	}

	[[maybe_unused]] static bool HitTestTriangle(const Triangle& triangle,
	                                             const Ray& ray,
	                                             RayHitRecord& hit_record,
	                                             const bool ignore_hit_record = false)
	{
		//TODO
		assert(false && "Not Implemented");
		(void)triangle;
		(void)ray;
		(void)hit_record;
		(void)ignore_hit_record;
		return false;
	}

	[[maybe_unused]] static bool HitTestAABB(const AABB& aabb, const Ray& ray)
	{
		//TODO
		assert(false && "Not Implemented");
		(void)aabb;
		(void)ray;
		return false;
	}
} //namespace gfx
#endif //INTERSECTIONS_HEADER
