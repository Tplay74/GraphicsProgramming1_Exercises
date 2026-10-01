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

			Vector3 rayDirection{(xNdc), (yNdc), 1.f};
			rayDirection.Normalize();

			// Convert gradient value to color.
			ColorRgba final_color{rayDirection.x, rayDirection.y, rayDirection.z};
			final_color.MaxToOne();

			// Write to surface
			surface_info.pixel_buffer[px + (py * surface_info.width)] = SDL_MapRGB(
				surface_info.pixel_format_details, nullptr,
				static_cast<uint8_t>(final_color.r * 255),
				static_cast<uint8_t>(final_color.g * 255),
				static_cast<uint8_t>(final_color.b * 255));
		}
	}
}