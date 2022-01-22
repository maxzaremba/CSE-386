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

vector<PositionalLightPtr> lights = {
	new PositionalLight(dvec3(10, 10, 10), pureWhiteLight),
	new SpotLight(dvec3(2, 5, -2), dvec3(0,-1,0), glm::radians(45.0), pureWhiteLight)
};

PositionalLightPtr posLight = lights[0];
SpotLightPtr spotLight = (SpotLightPtr)lights[1];

FrameBuffer frameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
RayTracer rayTrace(lightGray);

dvec3 cameraPos(0, 5, 10);
dvec3 cameraFocus(0, 5, 0);
dvec3 cameraUp = Y_AXIS;
double cameraFOV = PI_2;

PerspectiveCamera pCamera(cameraPos, cameraFocus, cameraUp, cameraFOV, WINDOW_WIDTH, WINDOW_HEIGHT);
IScene scene(&pCamera);

void render() {
	int frameStartTime = glutGet(GLUT_ELAPSED_TIME);
	int width = frameBuffer.getWindowWidth();
	int height = frameBuffer.getWindowHeight();

	double N = 10.0;
	pCamera = PerspectiveCamera(cameraPos, cameraFocus, cameraUp, cameraFOV, width, height);
	rayTrace.raytraceScene(frameBuffer, 0, scene);

	int frameEndTime = glutGet(GLUT_ELAPSED_TIME); // Get end time
	double totalTimeSec = (frameEndTime - frameStartTime) / 1000.0;
	cout << "Render time: " << totalTimeSec << " sec." << endl;
}

void resize(int width, int height) {
	frameBuffer.setFrameBufferSize(width, height);
	glutPostRedisplay();
}

void buildScene() {
	IShape *plane = new IPlane(dvec3(0.0, -2.0, 0.0), dvec3(0.0, 1.0, 0.0));
	ISphere *sphere1 = new ISphere(dvec3(0.0, 0.0, 0.0), 2.0);
	ISphere *sphere2 = new ISphere(dvec3(-2.0, 0.0, -8.0), 2.0);
	IEllipsoid *ellipsoid = new IEllipsoid(dvec3(4.0, 0.0, 3.0), dvec3(2.0, 1.0, 2.0));
	IDisk* disk = new IDisk(dvec3(15.0, 0.0, 0.0), dvec3(0.0, 0.0, 1.0), 5.0);
	IDisk* disk2 = new IDisk(dvec3(-5.0, 0.0, 0.0), dvec3(1.0, 0.0, 1.0), 2.0);

	scene.addOpaqueObject(new VisibleIShape(plane, tin));
	scene.addOpaqueObject(new VisibleIShape(sphere1, silver));
	scene.addOpaqueObject(new VisibleIShape(sphere2, bronze));
	scene.addOpaqueObject(new VisibleIShape(ellipsoid, redPlastic));
	scene.addOpaqueObject(new VisibleIShape(disk, cyanPlastic));
	scene.addOpaqueObject(new VisibleIShape(disk2, gold));

	scene.addLight(lights[0]);
}

int main(int argc, char *argv[]) {
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
