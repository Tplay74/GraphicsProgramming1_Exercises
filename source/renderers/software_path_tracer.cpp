//==============================================================
//  Graphics Programming 1 (2026-2027)
//  Authors : Matthieu Delaere
//  Copyright (c) 2026 Matthieu Delaere. All rights reserved.
//==============================================================
#include <software_path_tracer.h>
#include <scenes.h>
#include <intersections.h>
using namespace gfx;

// =============================================================================
// Construction / Destruction
// =============================================================================
SoftwarePathTracer::SoftwarePathTracer(Context* const context)
	: Renderer(context)
{}

SoftwarePathTracer::~SoftwarePathTracer() = default;

// =============================================================================
// Public Functions
// =============================================================================
void SoftwarePathTracer::Render()
{
	assert(context_ && "Context not available!");

	// DEMO CODE - TODO: remove!
	const SurfaceInfo& surface_info = context_->surface_info;
	const float aspectRatio{static_cast<float>(surface_info.width) / static_cast<float>(surface_info.height)};
	for (uint32_t py = 0; py < surface_info.height; ++py)
	{
		for (uint32_t px = 0; px < surface_info.width; ++px)
		{
			const float xNdc{(2 * (px + 0.5f) / surface_info.width - 1) * aspectRatio};
			const float yNdc{1 - 2 * (py + 0.5f) / surface_info.height};

			const Vector3 rayDirection{Vector3{(xNdc), (yNdc), 1.f}.Normalized()};
			const Ray viewRay{Vector3{0.f, 0.f, 0.f}, rayDirection};

			RayHitRecord closestHitRecord{};
			const Sphere sphere {Vector3{0.f, 0.f, 100.f}, 50.f};
			const bool didHit{ HitTestSphere(sphere, viewRay, closestHitRecord)};
			if (!didHit) continue;

			ShadingInput shadingInput{};
			shadingInput.world_position = viewRay.origin + closestHitRecord.t * rayDirection;
			shadingInput.world_normal = Vector3{shadingInput.world_position - sphere.origin}.Normalized();

			ColorRgba finalColor{};

			if (context_->debug_params.visualization_mode == VisualizationMode::kDepth)
			{
				const float maxDepth{100.f};
				const float scaledT
				{
					1.f - std::clamp(closestHitRecord.t / maxDepth, 0.f, 1.f)
				};
				finalColor = ColorRgba{scaledT};
			}
			else if (context_->debug_params.visualization_mode == VisualizationMode::kNormals)
			{
				const Vector3& n{shadingInput.world_normal};
				finalColor = ColorRgba
				{
					(n.x + 1.f) * 0.5f,
					(n.y + 1.f) * 0.5f,
					(n.z + 1.f) * 0.5f,
				};
			}
			else
			{
				finalColor = ColorRgba{1.f, 0.f, 0.f};
			}

			// Write to surface
			surface_info.pixel_buffer[px + (py * surface_info.width)] = SDL_MapRGB(
				surface_info.pixel_format_details, nullptr,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}
}