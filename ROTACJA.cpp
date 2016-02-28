#include "stdafx.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

///////////////////////////////////////////////////////////
// Wywo³ywana w celu przerysowania sceny
void RenderScene(void) {
	for (int i = 0; i < 1000000; i++)
	{
		// Wyczyszczenie okna aktualnym kolorem czyszczcym   
		glClear(GL_COLOR_BUFFER_BIT);
		// Aktualny kolor rysujcy - czerwony  
		//           R     G     B  
		glColor3f(1.0f, 0.0f, 0.0f);
		//   
		glBegin(GL_POLYGON);
		glVertex3f(10.0f, 70.0f, 0.0f);//1
		glVertex3f(20.0f, 60.0f, 0.0f);//2
		glVertex3f(30.0f, 50.0f, 0.0f);//3
		glVertex3f(40.0f, 40.0f, 0.0f);//4
		glVertex3f(50.0f, 40.0f, 0.0f);//5
		glVertex3f(60.0f, 20.0f, 0.0f);//6
		glVertex3f(70.0f, 10.0f, 0.0f);//7
		glVertex3f(-70.0f, -10.0f, 0.0f);//8
		glVertex3f(-60.0f, -20.0f, 0.0f);//9
		glVertex3f(-50.0f, -30.0f, 0.0f);//10
		glVertex3f(-40.0f, -40.0f, 0.0f);//11
		glVertex3f(-30.0f, -50.0f, 0.0f);//12
		glVertex3f(-20.0f, -60.0f, 0.0f);//13
		glVertex3f(-10.0f, -70.0f, 0.0f);//14
		glEnd();

		//OBRACANIE
		glRotatef(i, 0.1f, 0.f, 0.0f);
		Sleep(1000);


		// Wys³anie polece do wykonania     
		glFlush();
	}
}
///////////////////////////////////////////////////////////
// Konfiguracja stanu renderowania  
void SetupRC(void) {
	// Ustalenie niebieskiego koloru czyszczcego     
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
}
///////////////////////////////////////////////////////////
// Wywo³ywana przez bibliotek GLUT w przypadku zmiany rozmiaru okna
void ChangeSize(int w, int h) {
	GLfloat aspectRatio;
	// Zabezpieczenie przed dzieleniem przez zero  
	if (h == 0)   h = 1;
	// Ustawienie wielkoœci widoku na równ¹ wielkoœci okna     
	glViewport(0, 0, w, h);
	// Ustalenie uk³adu wspó³rzêdnych  
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Wyznaczenie przestrzeni ograniczaj¹cej (lewy, prawy, dolny, górny, bliski, odleg³y)  
	aspectRatio = (GLfloat)w / (GLfloat)h;
	if (w <= h)    glOrtho(-100.0, 100.0, -100 / aspectRatio, 100.0 / aspectRatio, 1.0, -1.0);
	else    glOrtho(-100.0 * aspectRatio, 100.0 * aspectRatio, -100.0, 100.0, 1.0, -1.0);
	glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
}
///////////////////////////////////////////////////////////
// G³ówny punkt wejcia programu
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutCreateWindow("GLRect");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	SetupRC();
	glutMainLoop();
	return 0;
}