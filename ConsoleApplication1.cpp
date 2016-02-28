// ConsoleApplication1.cpp : Defines the entry point for the console application.
//
//Damian Stañco

#include "stdafx.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
// Funkcja wywo³ywana w celu narysowania sceny
void RenderScene(void) {
	// Wyczyszczenie okna aktualnym kolorem czyszcz¹cym
	glClear(GL_COLOR_BUFFER_BIT);
	/// Przekazanie polecenia czyszczenia do wykonania
	glFlush();
}
// Ustalenie stanu rendrowania  
void SetupRC(void) {
	glClearColor(0.60f, 0.40f, 0.12f, 1.0f);
}
// G³ówny punki wejœcia programu  
void main(int argc, char **argv) {
	//this is init function
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutCreateWindow("Mój pierwszy program w GLUT");
	glutDisplayFunc(RenderScene);
	SetupRC();
	glutMainLoop();
}

