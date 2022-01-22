/****************************************************
 * 2016-2021 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted.
 ****************************************************/

#include <ctime>
#include <vector>
#include "defs.h"
#include "utilities.h"
#include "framebuffer.h"
#include "colorandmaterials.h"
#include "rasterization.h"

FrameBuffer frameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

const int SZ = 51;
const int SZ2 = SZ / 2;

void closedSquare(int x, int y, color C) {
}

void closedSquare(const ivec2 &centerPt, color C) {
}

void openSquare(const ivec2 &centerPt, color C) {
}

void render() {
	frameBuffer.clearColorAndDepthBuffers();

	drawLine(frameBuffer, 0, 0, 100, 100, red);
	drawLine(frameBuffer, 100, 100, 200, 100, green);

	closedSquare(100, 150, red);
	closedSquare(ivec2(200, 150), green);
	openSquare(ivec2(300, 150), blue);

	frameBuffer.showColorBuffer();
}

void resize(int width, int height) {
	frameBuffer.setFrameBufferSize(width, height);
	glutPostRedisplay();
}
int main(int argc, char *argv[]) {
    /*graphicsInit(argc, argv, __FILE__);
        
	glutDisplayFunc(render);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboardUtility);
	glutMouseFunc(mouseUtility);

	frameBuffer.setClearColor(white);

	glutMainLoop();*/

	double x = 45;
	double y = 99;
	cout << x << ' ' << y << endl;
	swap(x, y);
	cout << x << ' ' << y << endl;

	cout << approximatelyEqual(0.000, 0.00000001) << endl;

	cout << approximatelyZero(-9) << endl;
	return 0;
}