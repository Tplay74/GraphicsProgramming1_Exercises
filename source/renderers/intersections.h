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
#include <primitives.h>
#include <matrix.h>
#include <valarray>

namespace gfx
{
	//--- Intersection Tests ---
	[[maybe_unused]]
	static bool HitTestSphere(const Sphere& sphere, const Ray& ray,
		RayHitRecord& hit_record, const bool ignore_hit_record = false)
	{
		const Vector3 L {sphere.origin - ray.origin};
		const float projection{Vector3::Dot(L, ray.direction)};
		const float rejectionMagnitude{Vector3::Reject(L, ray.direction).Magnitude()};

		if (rejectionMagnitude >= sphere.radius) return false;
		if (ignore_hit_record) return true;

		const float projectionInsideSphere{sqrtf(powf(sphere.radius, 2) - powf(rejectionMagnitude, 2))};
		const float distance {projection - projectionInsideSphere};
		hit_record.ray = ray;
		hit_record.t = distance;

		return true;
	}

	[[maybe_unused]]
	static bool HitTestPlane(const Plane& plane, const Ray& ray, 
		RayHitRecord& hit_record, const bool ignore_hit_record = false)
	{
		//TODO
		assert(false && "Not Implemented");
		(void)plane; (void)ray; (void)hit_record; (void)ignore_hit_record;
		return false;
	}

	[[maybe_unused]]
	static bool HitTestTriangle(const Triangle& triangle, const Ray& ray, 
		RayHitRecord& hit_record, const bool ignore_hit_record = false)
	{
		//TODO
		assert(false && "Not Implemented");
		(void)triangle; (void)ray; (void)hit_record; (void)ignore_hit_record;
		return false;
	}

	[[maybe_unused]]
	static bool HitTestAABB(const AABB& aabb, const Ray& ray)
	{
		//TODO
		assert(false && "Not Implemented");
		(void)aabb; (void)ray;
		return false;
	}
}
#endif //INTERSECTIONS_HEADER