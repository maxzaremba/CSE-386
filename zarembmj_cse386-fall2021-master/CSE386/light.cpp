/****************************************************
 * 2016-2021 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted.
 ****************************************************/

#include "light.h"
#include "io.h"
#include <cmath>

/**
 * @fn	color ambientColor(const color &mat, const color &light)
 * @brief	Computes the ambient color produced by a single light at a single point.
 * @param	mat  	Ambient material property.
 * @param	lightAmbient	Light's ambient color.
 * @return	Ambient color.
  */

color ambientColor(const color &mat, const color &lightAmbient) {
	/* CSE 386 - todo  */
	return glm::clamp(mat * lightAmbient, 0.0, 1.0);
}

/**
 * @fn	color diffuseColor(const color &mat, const color &light, const dvec3 &l, const dvec3 &n)
 * @brief	Computes diffuse color produce by a single light at a single point.
 * @param	mat		 	Material.
 * @param	lightDiffuse	 	The light color.
 * @param	l		 	Light vector.
 * @param	n		 	Normal vector.
 * @return	Diffuse color.
 */

color diffuseColor(const color &mat, const color &lightDiffuse,
					const dvec3 &l, const dvec3 &n, double sha) {
	/* CSE 386 - todo  */
	return glm::clamp((1.0f - sha) * mat * lightDiffuse * glm::dot(l, n), 0.0, 1.0);
}

/**
 * @fn	color specularColor(const color &mat, const color &light, double shininess, 
 *							const dvec3 &r, const dvec3 &v)
 * @brief	Computes specular color produce by a single light at a single point.
 * @param	mat		 	Material.
 * @param	lightSpecular	 	The light's color.
 * @param	shininess	Material shininess.
 * @param	r		 	Reflection vector.
 * @param	v		 	Viewing vector.
 * @return	Specular color.
 */

color specularColor(const color &mat, const color &lightSpecular,
					double shininess,
					const dvec3 &r, const dvec3 &v, double sha) {
	/* CSE 386 - todo  */
	return glm::clamp((1.0f - sha) * mat * lightSpecular * glm::pow(std::fmax(glm::dot(r, v), 0.0), shininess), 0.0, 1.0);
}

/**
 * @fn	color totalColor(const Material &mat, const LightColor &lightColor, 
 *						const dvec3 &viewingDir, const dvec3 &normal, 
 *						const dvec3 &lightPos, const dvec3 &intersectionPt, 
 *						bool attenuationOn, const LightAttenuationParameters &ATparams)
 * @brief	Color produced by a single light at a single point.
 * @param	mat			  	Material.
 * @param	lightColor	  	The light's color.
 * @param	v	  			The v vector.
 * @param	n   		  	Normal vector.
 * @param	lightPos	  	Light position.
 * @param	intersectionPt	(x,y,z) of intersection point.
 * @param	attenuationOn 	true if attenuation is on.
 * @param	ATparams	  	Attenuation parameters.
 * @return	Color produced by a single light at a single point.
 */
 
color totalColor(const Material &mat, const LightColor &lightColor,
				const dvec3 &v, const dvec3 &n,
				const dvec3 &lightPos, const dvec3 &intersectionPt,
				bool attenuationOn, 
				const LightATParams &ATparams, double sha) {
	/* CSE 386 - todo  */
	dvec3 l = glm::normalize(lightPos - intersectionPt);
	double distance = glm::distance(lightPos, intersectionPt);
	dvec3 r = ((2.0 * glm::dot(l, n) * n) - l);
	color a = ambientColor(mat.ambient, lightColor.ambient);
	color b = diffuseColor(mat.diffuse, lightColor.diffuse, l, n, sha);
	color c = specularColor(mat.specular, lightColor.specular, mat.shininess, r, v, sha);

	double at = ATparams.factor(distance);

	if (!attenuationOn) {
		return glm::clamp(a + b + c, 0.0, 1.0);
	} else {
		return glm::clamp(a + (at * (b)) + c, 0.0, 1.0);
	}
}

/**
 * @fn	color PositionalLight::illuminate(const dvec3 &interceptWorldCoords, 
 *										const dvec3 &normal, const Material &material, 
 *										const Frame &eyeFrame, bool inShadow) const
 * @brief	Computes the color this light produces in RAYTRACING applications.
 * @param	interceptWorldCoords	(x, y, z) at the intercept point.
 * @param	normal				The normal vector.
 * @param	material			The object's material properties.
 * @param	eyeFrame			The coordinate frame of the camera.
 * @param	inShadow			true if the point is in a shadow.
 * @return	The color produced at the intercept point, given this light.
 */

color PositionalLight::illuminate(const dvec3& interceptWorldCoords,
									const dvec3& normal,
									const Material& material,
									const Frame& eyeFrame, double inShadow) const {
	/* CSE 386 - todo  */
	if (!isOn) {
		return black;
	}
	else {
		dvec3 v = glm::normalize(eyeFrame.origin - interceptWorldCoords);
		return(totalColor(material, lightColor, v, normal, actualPosition(eyeFrame), interceptWorldCoords,
			attenuationIsTurnedOn, atParams, inShadow));
	}
}

dvec3 PositionalLight::actualPosition(const Frame& eyeFrame) const {	
	if (isTiedToWorld) {
		return pos;
	}
	else {
		return eyeFrame.toFrameCoords(pos);
	}
}

/**
 * @fn	color SpotLight::illuminate(const dvec3 &interceptWorldCoords, 
 *									const dvec3 &normal, const Material &material, 
 *									const Frame &eyeFrame, bool inShadow) const
 * @brief	Computes the color this light produces in raytracing applications.
 * @param	interceptWorldCoords				The surface properties of the intercept point.
 * @param	normal					The normal vector.
 * @param	material			The object's material properties.
 * @param	eyeFrame			The coordinate frame of the camera.
 * @param	inShadow			true if the point is in a shadow.
 * @return	The color produced at the intercept point, given this light.
 */

color SpotLight::illuminate(const dvec3 &interceptWorldCoords,
							const dvec3 &normal,
							const Material &material,
							const Frame &eyeFrame, bool inShadow) const {
	/* CSE 386 - todo  */
	if (!isOn) {
		return black;
	}
	else if (!inCone(actualPosition(eyeFrame), this->spotDir, this->fov, interceptWorldCoords)) {
		return black;
	}
	else {
		dvec3 v = glm::normalize(eyeFrame.origin - interceptWorldCoords);
		return(totalColor(material, lightColor, v, normal, actualPosition(eyeFrame), interceptWorldCoords,
			attenuationIsTurnedOn, atParams, inShadow));
	}
}

/**
* @fn	void setDir (double dx, double dy, double dz)
* @brief	Sets the direction of the spotlight.
* @param	dx		x component of the direction
* @param	dy		y component of the direction
* @param	dz		z component of the direction
*/

void SpotLight::setDir(double dx, double dy, double dz) {
	spotDir = glm::normalize(dvec3(dx, dy, dz));
}

/**
* @fn	bool inCone(const dvec3& spotPos, const dvec3& spotDir, double spotFOV, const dvec3& intercept)
* @brief	Determines if an intercept point falls within a spotlight's cone.
* @param	spotPos		where the spotlight is positioned
* @param	spotDir		normalized direction of spotlight's pointing direction
* @param	spotFOV		spotlight's field of view, which is 2X of the angle from the viewing axis
* @param	intercept	the position of the intercept.
*/

bool inCone(const dvec3& spotPos, const dvec3& spotDir, double spotFOV, const dvec3& intercept) {
	/* CSE 386 - todo  */
	dvec3 lightDir = glm::normalize(intercept - spotPos);

	if (glm::dot(lightDir, spotDir) < cos(spotFOV)) {
		return false;
	}
	else {
		return true;
	}
}

/**
* @fn	bool inShadow(const dvec3& lightPos, const dvec3& intercept, const dvec3& normal, const vector<VisibleIShapePtr> objects)
* @brief	Determines if an intercept point falls in a shadow.
* @param	lightPos		where the spotlight is positioned
* @param	intercept	the position of the intercept.
* @param	normal		the normal vector at the intercept point
* @param	objects		the collection of opaque objects in the scene
*/

double inShadow(const dvec3& lightPos, const dvec3& intercept, const dvec3& normal, const vector<VisibleIShapePtr>& Oobjects, const vector<VisibleIShapePtr>& Tobjects) {
	/* CSE 386 - todo  */

	// Direction of light w.r.t point of intersection
	dvec3 Lu = glm::normalize(lightPos - intercept);

	Ray ray(intercept, Lu);

	double sha = 0;
	sha = 0;

	// Calculate shadow contribution from all the Opaque objects
	for (int i = 0; i < Oobjects.size(); i++)
	{
		VisibleIShape& thisShape = *Oobjects[i];
		HitRecord hit;
		thisShape.findClosestIntersection(ray, hit);

		// check if ray intersects the object
		if (hit.t != FLT_MAX) {

			// apply a small threshold for numerical stability
			if (abs(hit.t) >= 0.0001) {

				// update shadow co-efficient by adding the transparency value of the node
				sha = sha + hit.material.alpha;

				// Clip shadow to value 1.0
				if (sha > 1.0f) {
					return 1.0;
				}
			}
		}
	}

	// Calculate shadow contribution from all the Transparent objects
	for (int i = 0; i < Tobjects.size(); i++)
	{
		VisibleIShape& thisShape = *Tobjects[i];
		HitRecord hit;
		thisShape.findClosestIntersection(ray, hit);

		// check if ray intersects the object
		if (hit.t != FLT_MAX) {

			// apply a small threshold for numerical stability
			if (abs(hit.t) >= 0.0001) {

				// update shadow co-efficient by adding the transparency value of the node
				sha = sha + hit.material.alpha;

				// Clip shadow to value 1.0
				if (sha > 1.0f) {
					return 1.0;
				}
			}
		}
	}

	// Clip shadow to value 1.0
	if (sha > 1.0f) {
		return 1.0;
	}

	return sha;

}
