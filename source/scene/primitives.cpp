//==============================================================
//  Graphics Programming 1 (2026-2027)
//  Authors : Matthieu Delaere
//  Copyright (c) 2026 Matthieu Delaere. All rights reserved.
//==============================================================
#include <primitives.h>
#include <complex>
using namespace gfx;

//--- AABB ---
void AABB::ExpandToInclude(const Vector3& point)
{
	min = Vector3::Min(min, point);
	max = Vector3::Max(max, point);
}

void AABB::ExpandToInclude(const AABB& other)
{
	min = Vector3::Min(min, other.min);
	max = Vector3::Max(max, other.max);
}

bool AABB::Contains(const Vector3& point) const
{
	return point.x >= min.x &&
	  point.x <= max.x &&
	  point.y >= min.y &&
	  point.y <= max.y &&
	  point.z >= min.z &&
	  point.z <= max.z;
}

bool AABB::Overlaps(const AABB& other) const
{
	return min.x <= other.max.x &&
	  max.x >= other.min.x &&
	  min.y <= other.max.y &&
	  max.y >= other.min.y &&
	  min.z <= other.max.z &&
	  max.z >= other.min.z;
}

Vector3 AABB::GetCenter() const
{ return {(min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f}; }

Vector3 AABB::GetSize() const { return {max.x - min.x, max.y - min.y, max.z - min.z}; }

bool AABB::IsValid() const { return min.x <= max.x && min.y <= max.y && min.z <= max.z; }

//--- Primitives AABB Updates ---
void Sphere::UpdateAABB()
{
	//TODO
}

bool Sphere::HitTest(const Ray& ray, RayHitRecord& hit_record, const bool ignore_hit_record) const
{
	const Vector3 L {origin - ray.origin};
	const float projection {Vector3::Dot(L, ray.direction_)};
	const float rejectionMagnitude {Vector3::Reject(L, ray.direction_).Magnitude()};

	if (rejectionMagnitude >= radius) return false;

	const float projectionInsideSphere {sqrtf(powf(radius, 2) - powf(rejectionMagnitude, 2))};

	float tMin {projection - projectionInsideSphere};
	float tMax {projection + projectionInsideSphere};
	if (tMin > tMax) std::swap(tMin, tMax);

	if (tMin < 0)
	{
		tMin = tMax;
		if (tMin < 0) return false;
	}
	const float distance {tMin};

	return UpdateIfCloser(ray, hit_record, distance, ignore_hit_record);
}

bool Plane::HitTest(const Ray& ray, RayHitRecord& hit_record, const bool ignore_hit_record) const
{
	const float denominator {Vector3::Dot(ray.direction_, normal_)};

	if (denominator >= 0.f) return false;

	const Vector3 originDistanceVector {origin - ray.origin};
	const float distance {Vector3::Dot(originDistanceVector, normal_) / denominator};

	if (distance < ray.min || distance > ray.max) return false;
	if (distance <= 0) return false; //plane behind camera

	if (half_extent.has_value())
	{
		//check if point in finite plane
		const Vector3 hitPoint {ray.origin + distance * ray.direction_};
		const Vector3 pointPlaneSpace {hitPoint - origin};
		const Vector3 additionalTangent {Vector3::Cross(normal_, tangent)};

		const float pointPlaneDistanceU {Vector3::Dot(pointPlaneSpace, tangent)};
		const float pointPlaneDistanceV {Vector3::Dot(pointPlaneSpace, additionalTangent)};

		if (std::abs(pointPlaneDistanceU) > half_extent.value().x ||
		    std::abs(pointPlaneDistanceV) > half_extent.value().y)
			return false;
	}

	return UpdateIfCloser(ray, hit_record, distance, ignore_hit_record);
}

void Triangle::UpdateAABB()
{
	//TODO
}

void TriangleMesh::UpdateAABB()
{
	//TODO
}
