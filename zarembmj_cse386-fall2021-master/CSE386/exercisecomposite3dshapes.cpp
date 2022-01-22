/****************************************************
 * 2016-2021 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted..
 ****************************************************/

#include <ctime>
#include "defs.h"
#include "io.h"
#include "ishape.h"
#include "framebuffer.h"
#include "raytracer.h"
#include "iscene.h"
#include "light.h"
#include "image.h"
#include "camera.h"
#include "rasterization.h"

double z = 0.0;
double inc = 0.2;

dvec3 cameraPos(10, 10, 10);
dvec3 cameraFocus(0, 0, 0);
dvec3 cameraUp = Y_AXIS;
double cameraFOV = PI_2;
PositionalLight posLight(dvec3(10, 20, -5), pureWhiteLight);

SpotLight spotLight(dvec3(-10, 10, 10), glm::normalize(dvec3(1,-1,-1)), 15*PI/180, pureWhiteLight);

FrameBuffer frameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
RayTracer rayTrace(lightGray);

PerspectiveCamera pCamera(cameraPos, cameraFocus, cameraUp, cameraFOV, WINDOW_WIDTH, WINDOW_HEIGHT);

IScene scene(&pCamera);
LightATParams ATparam(1,0,0);

void render() {
	int frameStartTime = glutGet(GLUT_ELAPSED_TIME);
	int width = frameBuffer.getWindowWidth();
	int height = frameBuffer.getWindowHeight();

	pCamera = PerspectiveCamera(cameraPos, cameraFocus, cameraUp, cameraFOV, width, height);
	rayTrace.raytraceScene(frameBuffer, scene.numReflections, scene);

//	IPlane* pl = (IPlane*)scene.transparentObjs[0]->shape;
//	pl->a.x = pl->a.x + 1.0;

	int frameEndTime = glutGet(GLUT_ELAPSED_TIME); // Get end time
	double totalTimeSec = (frameEndTime - frameStartTime) / 1000.0;
	cout << "Render time: " << totalTimeSec << " sec." << endl;
}

void resize(int width, int height) {
	frameBuffer.setFrameBufferSize(width, height);
	glutPostRedisplay();
}

IPlane* plane = new IPlane(dvec3(0, -4, 0), Y_AXIS);
ICylinderY* cylinderY = new ICylinderY(dvec3(0.0, 0.0, 6.5), 2, 4.0);
ICylinderZ* cylinderZ = new ICylinderZ(dvec3(8.0, -2.0, 0.5), 1.5, 5.0);
ISphere* sphere = new ISphere(dvec3(0.0, 0.0, 0.0), 4.0);
IEllipsoid* ellipsoide = new IEllipsoid(dvec3(0.0, 0.0, 0.0), dvec3(3.0, 2.0, 5.0));
IClosedCylinderY* closedCylinder = new IClosedCylinderY(dvec3(4.0, 1.0, -8.0), 3.0, 5.0);
IConeY* coneY = new IConeY(dvec3(13.0, -4.0, -4.0), 3, 8);

IPlane* Tplane = new IPlane(dvec3(0, 0, 0), X_AXIS);

Image* imageCy = new Image("usflag.ppm");

void buildScene() {

	scene.addOpaqueObject(new VisibleIShape(plane, tin));

	scene.addOpaqueObject(new VisibleIShape(cylinderY, gold));
	scene.addOpaqueObject(new VisibleIShape(cylinderZ, polishedBronze));
	scene.addOpaqueObject(new VisibleIShape(sphere, copper));
	scene.addOpaqueObject(new VisibleIShape(coneY, polishedSilver));

	scene.addOpaqueObject(new VisibleIShape(closedCylinder->diskBottom, chrome));
	scene.addOpaqueObject(new VisibleIShape(closedCylinder->diskTop, chrome));
	scene.addOpaqueObject(new VisibleIShape(closedCylinder->cylinder, chrome, imageCy));


	scene.addTransparentObject(new VisibleIShape(Tplane, polishedGold), 0.4);

	posLight.setAttenuationParams(LightATParams(1.0, 0.017, 0.0007));
	spotLight.setAttenuationParams(LightATParams(1.0, 0.017, 0.0007));

	scene.addLight(&posLight);
	scene.addLight(&spotLight);
}

void keyboardUtility(unsigned char key, int x, int y) {
#ifndef CONSOLE_ONLY
	switch (key) {
	case ESCAPE:		glutLeaveMainLoop();
		break;
	case '-':
		scene.antiAliasing = 1;
		break;
	case '+':
		scene.antiAliasing = 3;
		break;
	case '0':
		scene.numReflections = 0;
		break;
	case '1':
		scene.numReflections = 1;
		break;
	case '2':
		scene.numReflections = 2;
		break;
	case 'A':
	case 'a':
		scene.currentLight = 0;
		break;
	case 'B':
	case 'b':
		scene.currentLight = 1;
		break;
	case 'q':
		if (scene.currentLight) {
			scene.Slights[0]->setAttenuation(true);
		}
		else {
			scene.Plights[0]->setAttenuation(true);
		}
		break;
	case 'Q':
		if (scene.currentLight) {
			scene.Slights[0]->setAttenuation(false);
		}
		else {
			scene.Plights[0]->setAttenuation(false);
		}
		break;
	case 'o':
		if (scene.currentLight) {
			scene.Slights[0]->isOn = true;
		}
		else {
			scene.Plights[0]->isOn = true;
		}
		break;
	case 'O':
		if (scene.currentLight) {
			scene.Slights[0]->isOn = false;
		}
		else {
			scene.Plights[0]->isOn = false;
		}
		break;
	case 'w':
		if (scene.currentLight) {
			ATparam = scene.Slights[0]->atParams;
			ATparam.constant = ATparam.constant - 0.1;
			scene.Slights[0]->setAttenuationParams(ATparam);
		}
		else {
			ATparam = scene.Plights[0]->atParams;
			ATparam.constant = ATparam.constant - 0.1;
			scene.Plights[0]->setAttenuationParams(ATparam);
		}
		break;
	case 'W':
		if (scene.currentLight) {
			ATparam = scene.Slights[0]->atParams;
			ATparam.constant = ATparam.constant + 0.1;
			scene.Slights[0]->setAttenuationParams(ATparam);
		}
		else {
			ATparam = scene.Plights[0]->atParams;
			ATparam.constant = ATparam.constant + 0.1;
			scene.Plights[0]->setAttenuationParams(ATparam);
		}
		break;
	case 'e':
		if (scene.currentLight) {
			ATparam = scene.Slights[0]->atParams;
			ATparam.linear = ATparam.linear - 0.001;
			scene.Slights[0]->setAttenuationParams(ATparam);
		}
		else {
			ATparam = scene.Plights[0]->atParams;
			ATparam.linear = ATparam.linear - 0.001;
			scene.Plights[0]->setAttenuationParams(ATparam);
		}
		break;
	case 'E':
		if (scene.currentLight) {
			ATparam = scene.Slights[0]->atParams;
			ATparam.linear = ATparam.linear + 0.001;
			scene.Slights[0]->setAttenuationParams(ATparam);
		}
		else {
			ATparam = scene.Plights[0]->atParams;
			ATparam.linear = ATparam.linear + 0.001;
			scene.Plights[0]->setAttenuationParams(ATparam);
		}
		break;
	case 'r':
		if (scene.currentLight) {
			ATparam = scene.Slights[0]->atParams;
			ATparam.quadratic = ATparam.quadratic - 0.0001;
			scene.Slights[0]->setAttenuationParams(ATparam);
		}
		else {
			ATparam = scene.Plights[0]->atParams;
			ATparam.quadratic = ATparam.quadratic - 0.0001;
			scene.Plights[0]->setAttenuationParams(ATparam);
		}
		break;
	case 'R':
		if (scene.currentLight) {
			ATparam = scene.Slights[0]->atParams;
			ATparam.quadratic = ATparam.quadratic + 0.0001;
			scene.Slights[0]->setAttenuationParams(ATparam);
		}
		else {
			ATparam = scene.Plights[0]->atParams;
			ATparam.quadratic = ATparam.quadratic + 0.0001;
			scene.Plights[0]->setAttenuationParams(ATparam);
		}
		break;
	case 'x':
		if (scene.currentLight) {
			scene.Slights[0]->pos.x = scene.Slights[0]->pos.x - 0.5;
		}
		else {
			scene.Plights[0]->pos.x = scene.Plights[0]->pos.x - 0.5;
		}
		break;
	case 'X':
		if (scene.currentLight) {
			scene.Slights[0]->pos.x = scene.Slights[0]->pos.x + 0.5;
		}
		else {
			scene.Plights[0]->pos.x = scene.Plights[0]->pos.x + 0.5;
		}
		break;
	case 'y':
		if (scene.currentLight) {
			scene.Slights[0]->pos.y = scene.Slights[0]->pos.y - 0.5;
		}
		else {
			scene.Plights[0]->pos.y = scene.Plights[0]->pos.y - 0.5;
		}
		break;
	case 'Y':
		if (scene.currentLight) {
			scene.Slights[0]->pos.y = scene.Slights[0]->pos.y + 0.5;
		}
		else {
			scene.Plights[0]->pos.y = scene.Plights[0]->pos.y + 0.5;
		}
		break;
	case 'z':
		if (scene.currentLight) {
			scene.Slights[0]->pos.z = scene.Slights[0]->pos.z - 0.5;
		}
		else {
			scene.Plights[0]->pos.z = scene.Plights[0]->pos.z - 0.5;
		}
		break;
	case 'Z':
		if (scene.currentLight) {
			scene.Slights[0]->pos.z = scene.Slights[0]->pos.z + 0.5;
		}
		else {
			scene.Plights[0]->pos.z = scene.Plights[0]->pos.z + 0.5;
		}
		break;
	case 'v':
		if (scene.currentLight) {
			scene.Slights[0]->isTiedToWorld = false;
		}
		else {
			scene.Plights[0]->isTiedToWorld = false;
		}
		break;
	case 'V':
		if (scene.currentLight) {
			scene.Slights[0]->isTiedToWorld = true;
		}
		else {
			scene.Plights[0]->isTiedToWorld = true;
		}
		break;
	case 'f':
		scene.Slights[0]->fov = scene.Slights[0]->fov - PI / 180;
		break;
	case 'F':
		scene.Slights[0]->fov = scene.Slights[0]->fov + PI / 180;
		break;
	case 'j':
		scene.Slights[0]->spotDir.x = scene.Slights[0]->spotDir.x - 0.02;
		scene.Slights[0]->spotDir = glm::normalize(scene.Slights[0]->spotDir);
		break;
	case 'J':
		scene.Slights[0]->spotDir.x = scene.Slights[0]->spotDir.x + 0.02;
		scene.Slights[0]->spotDir = glm::normalize(scene.Slights[0]->spotDir);
		break;
	case 'k':
		scene.Slights[0]->spotDir.y = scene.Slights[0]->spotDir.y - 0.02;
		scene.Slights[0]->spotDir = glm::normalize(scene.Slights[0]->spotDir);
		break;
	case 'K':
		scene.Slights[0]->spotDir.y = scene.Slights[0]->spotDir.y + 0.02;
		scene.Slights[0]->spotDir = glm::normalize(scene.Slights[0]->spotDir);
		break;
	case 'l':
		scene.Slights[0]->spotDir.z = scene.Slights[0]->spotDir.z - 0.02;
		scene.Slights[0]->spotDir = glm::normalize(scene.Slights[0]->spotDir);
		break;
	case 'L':
		scene.Slights[0]->spotDir.z = scene.Slights[0]->spotDir.z + 0.02;
		scene.Slights[0]->spotDir = glm::normalize(scene.Slights[0]->spotDir);
		break;
	case 'p':
		scene.animation = false;
		break;
	case 'P':
		scene.animation = true;
		break;
	default:	cout << (int)key << "unmapped key pressed." << endl;
	}

	glutPostRedisplay();
#endif
}

int main(int argc, char* argv[]) {
	graphicsInit(argc, argv, __FILE__);

	glutDisplayFunc(render);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboardUtility);
	glutMouseFunc(mouseUtility);
	buildScene();

	rayTrace.defaultColor = gray;
	glutMainLoop();

	return 0;
}
