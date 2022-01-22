/****************************************************
 * 2016-2021 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted.
 ****************************************************/
#include "raytracer.h"
#include "ishape.h"
#include "io.h"

 /**
  * @fn	RayTracer::RayTracer(const color &defa)
  * @brief	Constructs a raytracers.
  * @param	defa	The clear color.
  */

RayTracer::RayTracer(const color& defa)
	: defaultColor(defa) {
}

color RayTracer::traceRay(const IScene& theScene, const PositionalLight& Light,
	const Ray& ray, int depth) const {

	HitRecord hit;

	HitRecord hitO;
	VisibleIShape::findIntersection(ray, theScene.opaqueObjs, hitO);

	HitRecord hitT;
	VisibleIShape::findIntersection(ray, theScene.transparentObjs, hitT);

	if (hitT.t < hitO.t) {
		hit = hitT;
	}
	else {
		hit = hitO;
	}

	if (hit.t != FLT_MAX) {

		// add a small offset to point of intersection for numerical stability
		dvec3 Po = IShape::movePointOffSurface(hit.interceptPt, hit.normal);

		Frame frm = theScene.camera->getFrame();
		double sha = inShadow(Light.actualPosition(frm), Po, hit.normal, theScene.opaqueObjs, theScene.transparentObjs);
		color finalColor = Light.illuminate(hit.interceptPt, hit.normal, hit.material, frm, sha);

		if ((hit.texture != nullptr) &&
			(hit.texture->H != 0) &&
			(hit.texture->W != 0)) {
			color texColor = hit.texture->getPixelUV(hit.u, hit.v);
			finalColor = (finalColor + texColor) * 0.5;
		}

		if (depth > 0) {
			// Viewing direction for specular
			dvec3 inci = glm::normalize(ray.dir);
			dvec3 R = inci - 2.0f * glm::dot(hit.normal, inci) * hit.normal;
			dvec3 RR = glm::normalize(R);

			color colReflection = traceRay(theScene, Light, Ray(Po, RR), depth - 1);

			dvec3 Pr = IShape::movePointOffSurface(hit.interceptPt, -hit.normal);
			color colRefraction = traceRay(theScene, Light, Ray(Pr, ray.dir), depth - 1);

			double refractionFactor = 0.1;
			finalColor = hit.material.alpha *
				((1.0f - refractionFactor) * finalColor +
					refractionFactor * colReflection) +
					(1.0f - hit.material.alpha) * colRefraction;
		}

		return finalColor;
	}

	return this->defaultColor;
}

color RayTracer::traceRay(const IScene& theScene, const SpotLight& Light,
	const Ray& ray, int depth) const {

	HitRecord hit;

	HitRecord hitO;
	VisibleIShape::findIntersection(ray, theScene.opaqueObjs, hitO);

	HitRecord hitT;
	VisibleIShape::findIntersection(ray, theScene.transparentObjs, hitT);

	if (hitT.t < hitO.t) {
		hit = hitT;
	}
	else {
		hit = hitO;
	}

	if (hit.t != FLT_MAX) {

		// add a small offset to point of intersection for numerical stability
		dvec3 Po = IShape::movePointOffSurface(hit.interceptPt, hit.normal);

		Frame frm = theScene.camera->getFrame();
		double sha = inShadow(Light.actualPosition(frm), Po, hit.normal, theScene.opaqueObjs, theScene.transparentObjs);
		color finalColor = Light.illuminate(hit.interceptPt, hit.normal, hit.material, frm, sha);

		if ( (hit.texture != nullptr) && 
			(hit.texture->H != 0) &&
			(hit.texture->W != 0)) {
			color texColor = hit.texture->getPixelUV(hit.u, hit.v);
			finalColor = (finalColor + texColor) * 0.5;
		}

		if (depth > 0) {
			// View direction for specular
			dvec3 inci = glm::normalize(ray.dir);
			dvec3 R = inci - 2.0f * glm::dot(hit.normal, inci) * hit.normal;
			dvec3 RR = glm::normalize(R);

			color colReflection = traceRay(theScene, Light, Ray(Po, RR), depth - 1);

			dvec3 Pr = IShape::movePointOffSurface(hit.interceptPt, -hit.normal);
			color colRefraction = traceRay(theScene, Light, Ray(Pr, ray.dir), depth - 1);

			double refractionFactor = 0.1;
			finalColor = hit.material.alpha *
				((1.0f - refractionFactor) * finalColor +
					refractionFactor * colReflection) +
					(1.0f - hit.material.alpha) * colRefraction;
		}

		return finalColor;
	}

	return this->defaultColor;
}

/**
 * @fn	void RayTracer::raytraceScene(FrameBuffer &frameBuffer, int depth, const IScene &theScene) const
 * @brief	Raytrace scene
 * @param [in,out]	frameBuffer	Framebuffer.
 * @param 		  	depth	   	The current depth of recursion.
 * @param 		  	theScene   	The scene.
 */

void RayTracer::raytraceScene(FrameBuffer & frameBuffer, int depth,
	const IScene & theScene) const {

	const RaytracingCamera& camera = *theScene.camera;
	const vector<VisibleIShapePtr>& objs = theScene.opaqueObjs;

	const vector<PositionalLightPtr>& pLights = theScene.Plights;
	const vector<SpotLightPtr>& sLights = theScene.Slights;

	for (int y = 0; y < frameBuffer.getWindowHeight(); ++y) {

		cout << "Progress " << (y * 100.0) / frameBuffer.getWindowHeight() << "%. \n";

		for (int x = 0; x < frameBuffer.getWindowWidth(); ++x) {

			color finalColor = dvec3(0, 0, 0);

			int antiAliasing = theScene.antiAliasing;

			// Calculate positional light
			for (int k = 0; k < pLights.size(); k++) {
				if (pLights[k]->isOn) {
					for (int i = 0; i < antiAliasing; i++) {
						for (int j = 0; j < antiAliasing; j++) {

							Ray ray = camera.getRay(x + (i / (antiAliasing * 1.0)), y + (j / (antiAliasing * 1.0)));
							color col = traceRay(theScene, *pLights[k], ray, depth);
							finalColor = finalColor + col;
						}
					}
				}
			}

			// Calculate spotligt
			for (int k = 0; k < sLights.size(); k++) {
				if (sLights[k]->isOn) {
					for (int i = 0; i < antiAliasing; i++) {
						for (int j = 0; j < antiAliasing; j++) {

							Ray ray = camera.getRay(x + (i / (antiAliasing * 1.0)), y + (j / (antiAliasing * 1.0)));

							color col = traceRay(theScene, *sLights[k], ray, depth);
							finalColor = finalColor + col;
						}
					}
				}
			}

            // Set alias scale and compute final pixel color.
			double aliasScale = antiAliasing * antiAliasing;
			finalColor = finalColor / aliasScale;
			frameBuffer.setColor(x, y, finalColor);

			//			frameBuffer.showAxes(x, y, camera.getRay(x,y), 0.25);			// Displays R/x, G/y, B/z axes
		}
	}

	frameBuffer.showColorBuffer();
}

/**
 * @fn	color RayTracer::traceIndividualRay(const Ray &ray,
 *											const IScene &theScene,
 *											int recursionLevel) const
 * @brief	Trace an individual ray.
 * @param	ray			  	The ray.
 * @param	theScene	  	The scene.
 * @param	recursionLevel	The recursion level.
 * @return	The color to be displayed as a result of this ray.
 */

color RayTracer::traceIndividualRay(const Ray & ray, const IScene & theScene, int recursionLevel) const {
	/* CSE 386 - todo  */
	// This might be a useful helper function.
	return black;
}
