//==============================================================
//  Graphics Programming 1 (2026-2027)
//  Authors : Matthieu Delaere
//  Copyright (c) 2026 Matthieu Delaere. All rights reserved.
//==============================================================
#include <software_path_tracer.h>
#include <intersections.h>
#include <scenes.h>
using namespace gfx;

//=============================================================================
//Construction / Destruction
//=============================================================================
SoftwarePathTracer::SoftwarePathTracer(Context* const context) : Renderer(context) {}

SoftwarePathTracer::~SoftwarePathTracer() = default;

//=============================================================================
//Public Functions
//=============================================================================
void SoftwarePathTracer::Render()
{
	assert(context_ && "Context not available!");

	const Scene* activeScene {context_->scene_manager->GetActiveScene()};
	const SurfaceInfo& surface_info = context_->surface_info;

	const float aspectRatio {static_cast<float>(surface_info.width) /
	                         static_cast<float>(surface_info.height)};
	const float fovAngleRadians {activeScene->camera.GetFovAngle() *
	                             (static_cast<float>(std::numbers::pi) / 180.f)};
	const float fovScalar {std::tanf(fovAngleRadians / 2)};

	const Vector3 cameraPosition {activeScene->camera.GetPosition()};

	for (uint32_t py = 0; py < surface_info.height; ++py)
	{
		for (uint32_t px = 0; px < surface_info.width; ++px)
		{
			const float xNdc {(2 * (px + 0.5f) / surface_info.width - 1) * aspectRatio * fovScalar};
			const float yNdc {(1 - 2 * (py + 0.5f) / surface_info.height) * fovScalar};

			RayHitRecord closestHitRecord {};
			const Ray viewRay {cameraPosition, Vector3 {xNdc, yNdc, 1.f}.Normalized()};

			bool didHit {SceneClosestHitTest(viewRay, closestHitRecord)};
			if (didHit)
			{
				const ShadingInput shadingInput {ConstructShadingInput(closestHitRecord)};

				WriteToSurface(px, py, GetColor(shadingInput, closestHitRecord));
			}
			else
			{
				WriteToSurface(px, py, activeScene->background_color);
			}
		}
	}
}

bool SoftwarePathTracer::SceneClosestHitTest(const Ray& ray,
                                             RayHitRecord& closestHit,
                                             bool ignoreRecord) const
{
	const Scene& activeScene {*context_->scene_manager->GetActiveScene()};
	bool didHit {};

	for (uint32_t i {0}; i < activeScene.objects.size(); ++i)
	{
		const SceneObject& object {activeScene.objects.at(i)};
		const auto& currentPrimitive {*activeScene.primitives_factory.Get(object.primitive_index)};

		if (currentPrimitive.HitTest(ray, closestHit, ignoreRecord))
		{
			didHit = true;
			closestHit.object_index = i;
		}
	}
	return didHit;
}

ShadingInput SoftwarePathTracer::ConstructShadingInput(const RayHitRecord& hit) const
{
	ShadingInput shadingInput {};

	shadingInput.view_direction = -hit.ray.direction_.Normalized();
	shadingInput.world_position = hit.ray.origin + hit.t * hit.ray.direction_;

	const auto& hitObject {context_->scene_manager->GetActiveScene()->objects.at(hit.object_index)};
	const auto& hitPrimitive {
	  *context_->scene_manager->GetActiveScene()->primitives_factory.Get(hitObject.primitive_index)};

	assert(hitPrimitive.type == PrimitiveType::kPlane || hitPrimitive.type == PrimitiveType::kSphere);

	switch (hitPrimitive.type)
	{
	case PrimitiveType::kPlane:
		shadingInput.world_normal = static_cast<const Plane&>(hitPrimitive).normal_;
		break;
	case PrimitiveType::kSphere:
		shadingInput.world_normal =
		  static_cast<const Sphere&>(hitPrimitive).GetNormal(shadingInput.world_position);
		break;
	}

	return shadingInput;
}

void SoftwarePathTracer::WriteToSurface(int px, int py, const ColorRgba& color) const
{
	context_->surface_info.pixel_buffer[px + (py * context_->surface_info.width)] =
	  SDL_MapRGB(context_->surface_info.pixel_format_details,
	             nullptr,
	             static_cast<uint8_t>(color.r * 255),
	             static_cast<uint8_t>(color.g * 255),
	             static_cast<uint8_t>(color.b * 255));
}

ColorRgba SoftwarePathTracer::GetColor(const ShadingInput& shadingInput,
                                       const RayHitRecord& rayHitRecord) const
{
	if (context_->debug_params.visualization_mode == VisualizationMode::kDepth)
	{
		constexpr auto maxDepth {100.f};
		const float scaledT {1.f - std::clamp(rayHitRecord.t / maxDepth, 0.f, 1.f)};
		return ColorRgba {scaledT};
	}
	if (context_->debug_params.visualization_mode == VisualizationMode::kNormals)
	{
		const Vector3& n {shadingInput.world_normal};
		return ColorRgba {
		  (n.x + 1.f) * 0.5f,
		  (n.y + 1.f) * 0.5f,
		  (n.z + 1.f) * 0.5f,
		};
	}
	const uint32_t objectIndex {rayHitRecord.object_index};
	const auto r {static_cast<float>(objectIndex & 1)};
	const auto g {static_cast<float>((objectIndex >> 1) & 1)};
	const auto b {static_cast<float>((objectIndex >> 2) & 1)};

	return ColorRgba {r, g, b};
}
