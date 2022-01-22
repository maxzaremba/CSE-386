/****************************************************
 * 2016-2020 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted..
 ****************************************************/

#pragma once
#include <vector>
#include <map>
#include "defs.h"
#include "light.h"
#include "camera.h"
#include "light.h"
#include "eshape.h"
#include "ishape.h"

/**
 * @struct	IScene
 * @brief	Represents an scene of implicitly represented objects. Used mostly in ray tracing.
 */

struct IScene {
	vector<PositionalLightPtr> Plights;				//!< All the positional lights in the scene
	vector<SpotLightPtr> Slights;				//!< All the spot lights in the scene
	vector<VisibleIShapePtr> opaqueObjs;			//!< All the visible objects in the scene
	vector<VisibleIShapePtr> transparentObjs;		//!< All the transparent objects in the scene
	RaytracingCamera *camera;						//!< The one camera in the scene
	IScene(RaytracingCamera *theCamera);
	void addOpaqueObject(const VisibleIShapePtr obj);
	void addTransparentObject(const VisibleIShapePtr obj, double alpha);
	
	void addLight(const PositionalLightPtr light);
	void addLight(const SpotLightPtr light);

	int antiAliasing = 1;
	int numReflections = 0;
	int currentLight = 0;
	bool animation = true;
	bool animationForward = true;
};
