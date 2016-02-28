// PUNKTYZAD5.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <GL/glut.h>
#include <vector>
using namespace std;

struct myVECTOR
{
	myVECTOR() = default;
	myVECTOR(int x, int y) : x(x), y(y) {}
	int x;
	int y;
};

vector<myVECTOR> myPOINTS;

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.10f, 0.10f, 0.10f);
	glBegin(GL_POINTS);
	for (int n = 0; n < myPOINTS.size(); n++)
		glVertex2f(myPOINTS[n].x, myPOINTS[n].y);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	for (int n = 0; n < myPOINTS.size(); n++)
		glVertex2f(myPOINTS[n].x, myPOINTS[n].y);
	glEnd();

	glFlush();
}

void SetupRC(void)
{
	glPointSize(2.0f);
	glClearColor(0.60f, 0.40f, 0.12f, 1.0f);
}

void ChangeSize(int w, int h) {

	GLfloat aspectRatio;

	if (h == 0)   h = 1;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	aspectRatio = (GLfloat)w / (GLfloat)h;

	if (w <= h)
	{
		glOrtho(0.0f, w, 0.0f / aspectRatio, h / aspectRatio, 1.0f, -1.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glScalef(1.0f, (GLfloat)h / (GLfloat)w, 1.0f);
	}
	else
	{
		glOrtho(0.0 * aspectRatio, w * aspectRatio, 0.0f, h, 1.0f, -1.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glScalef((GLfloat)w / (GLfloat)h, 1.0f, 1.0f);
	}

}

void onMouseDown(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		myPOINTS.push_back(myVECTOR(x, glutGet(GLUT_WINDOW_HEIGHT) - y));
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(400, 400);
	glutCreateWindow("PUNKTY");

	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutMouseFunc(onMouseDown);

	SetupRC();

	glutMainLoop();

	return 0;

}

