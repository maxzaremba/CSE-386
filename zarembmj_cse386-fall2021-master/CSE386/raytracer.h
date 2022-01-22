/****************************************************
 * 2016-2020 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted..
 ****************************************************/

#pragma once

#include "utilities.h"
#include "framebuffer.h"
#include "camera.h"
#include "iscene.h"

/**
 * @struct	RayTracer
 * @brief	Encapsulates the functionality of a ray tracer.
 */

struct RayTracer {
	color defaultColor;
	RayTracer(const color &defaultColor);
	void raytraceScene(FrameBuffer &frameBuffer, int depth,
						const IScene &theScene) const;

	color traceRay(const IScene& theScene, const PositionalLight& Light,
		const Ray& ray, int depth) const;

	color traceRay(const IScene& theScene, const SpotLight& Light,
		const Ray& ray, int depth) const;

protected:
	color traceIndividualRay(const Ray &ray, const IScene &theScene, int recursionLevel) const;
};