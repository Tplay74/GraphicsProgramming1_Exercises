//==============================================================
//  Graphics Programming 1 (2026-2027)
//  Authors : Matthieu Delaere
//  Copyright (c) 2026 Matthieu Delaere. All rights reserved.
//==============================================================
#ifndef SOFTWARE_PATH_TRACER_HEADER
#define SOFTWARE_PATH_TRACER_HEADER

//--- Standard Includes ---

//--- Framework Includes ---
#include <renderer.h>

namespace gfx
{
	//--- Software Path Tracer ---
	class SoftwarePathTracer final : public Renderer
	{
	public:
		//--- Construction / Destruction ---
		SoftwarePathTracer(Context* context);
		~SoftwarePathTracer() override;

		SoftwarePathTracer(const SoftwarePathTracer&) = delete;
		SoftwarePathTracer& operator=(const SoftwarePathTracer&) = delete;
		SoftwarePathTracer(SoftwarePathTracer&&) = delete;
		SoftwarePathTracer& operator=(SoftwarePathTracer&&) = delete;

		//--- Public Functions ---
		void Render() override;

	private:
		bool
		SceneClosestHitTest(const Ray& ray, RayHitRecord& closestHit, bool ignoreRecord = false) const;
		ShadingInput ConstructShadingInput(const RayHitRecord& hit) const;
		void WriteToSurface(int px, int py, const ColorRgba& color) const;
		[[nodiscard]] ColorRgba GetColor(const ShadingInput& shadingInput,
		                                 const RayHitRecord& rayHitRecord) const;
	};
} //namespace gfx
#endif //SOFTWARE_PATH_TRACER_HEADER
