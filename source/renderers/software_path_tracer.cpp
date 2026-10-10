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
SoftwarePathTracer::SoftwarePathTracer(Context* const context)
  : Renderer(context)
{
}

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
	const Vector3 cameraPosition {activeScene->camera.GetPosition()};

	for (uint32_t py = 0; py < surface_info.height; ++py)
	{
		for (uint32_t px = 0; px < surface_info.width; ++px)
		{
			const float xNdc {(2 * (px + 0.5f) / surface_info.width - 1) *
			                  aspectRatio};
			const float yNdc {1 - 2 * (py + 0.5f) / surface_info.height};

			RayHitRecord closestHitRecord {};
			const Ray viewRay {cameraPosition,
			                   Vector3 {xNdc, yNdc, 1.f}.Normalized()};

			bool didHit {SceneClosestHitTest(viewRay, closestHitRecord)};
			if (didHit)
			{
				const ShadingInput shadingInput {
				  ConstructShadingInput(closestHitRecord)};

				//Choose color based on visualization mode
				ColorRgba finalColor {};
				if (context_->debug_params.visualization_mode ==
				    VisualizationMode::kDepth)
				{
					constexpr auto maxDepth {100.f};
					const float scaledT {
					  1.f - std::clamp(closestHitRecord.t / maxDepth, 0.f, 1.f)};
					finalColor = ColorRgba {scaledT};
				}
				else if (context_->debug_params.visualization_mode ==
				         VisualizationMode::kNormals)
				{
					const Vector3& n {shadingInput.world_normal};
					finalColor = ColorRgba {
					  (n.x + 1.f) * 0.5f,
					  (n.y + 1.f) * 0.5f,
					  (n.z + 1.f) * 0.5f,
					};
				}
				else
				{
					const uint32_t objectIndex {closestHitRecord.object_index};
					const auto r {static_cast<float>(objectIndex & 1)};
					const auto g {static_cast<float>((objectIndex >> 1) & 1)};
					const auto b {static_cast<float>((objectIndex >> 2) & 1)};

					finalColor = ColorRgba {r, g, b};
				}

				//Write to surface
				surface_info.pixel_buffer[px + (py * surface_info.width)] =
				  SDL_MapRGB(surface_info.pixel_format_details,
				             nullptr,
				             static_cast<uint8_t>(finalColor.r * 255),
				             static_cast<uint8_t>(finalColor.g * 255),
				             static_cast<uint8_t>(finalColor.b * 255));
			}
			else
			{
				const auto& color {activeScene->background_color};

				//Write to surface
				surface_info.pixel_buffer[px + (py * surface_info.width)] =
				  SDL_MapRGB(surface_info.pixel_format_details,
				             nullptr,
				             static_cast<uint8_t>(color.r * 255),
				             static_cast<uint8_t>(color.g * 255),
				             static_cast<uint8_t>(color.b * 255));
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
		const auto& currentPrimitive {
		  *activeScene.primitives_factory.Get(object.primitive_index)};

		if (currentPrimitive.HitTest(ray, closestHit, ignoreRecord))
		{
			didHit = true;
			closestHit.object_index = i;
		}
	}
	return didHit;
}

ShadingInput
SoftwarePathTracer::ConstructShadingInput(const RayHitRecord& hit) const
{
	ShadingInput shadingInput {};

	shadingInput.view_direction = hit.ray.direction.Normalized();
	shadingInput.world_position = hit.ray.origin + hit.t * hit.ray.direction;

	const auto& hitObject {
	  context_->scene_manager->GetActiveScene()->objects.at(hit.object_index)};
	const auto& hitPrimitive {
	  *context_->scene_manager->GetActiveScene()->primitives_factory.Get(
	    hitObject.primitive_index)};

	assert(hitPrimitive.type == PrimitiveType::kPlane ||
	       hitPrimitive.type == PrimitiveType::kSphere);

	switch (hitPrimitive.type)
	{
	case PrimitiveType::kPlane:
		shadingInput.world_normal = static_cast<const Plane&>(hitPrimitive).normal;
		break;
	case PrimitiveType::kSphere:
		shadingInput.world_normal = static_cast<const Sphere&>(hitPrimitive)
		                              .GetNormal(shadingInput.world_position);
		break;
	}

	return shadingInput;
}
