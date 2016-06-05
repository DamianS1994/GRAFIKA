#include "stdafx.h"
#include <GL/glut.h>
#include <stdlib.h>
#include "colors.h"

// stałe do obsługi menu podręcznego
enum
{
	WARIANT1,
	WARIANT2,
	WARIANT3,
	FULL_WINDOW, // aspekt obrazu - całe okno
	ASPECT_1_1, // aspekt obrazu 1:1
	EXIT // wyjście
};

// aspekt obrazu

int aspect = FULL_WINDOW;

// rozmiary bryły obcinania

const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near_ = 3.0;
const GLdouble far_ = 7.0;

// kąty obrotu sceny

GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

// wskaźnik naciśnięcia lewego przycisku myszki

int button_state = GLUT_UP;

// położenie kursora myszki

int button_x, button_y;

// identyfikatory list wyświetlania

GLint Kula, Kula2, Torus, Ostroslup, Czworoscian, Dwudziestoscian;

// rodzaj operacji CSG

int csg_op = WARIANT1;

// ustawienie bufora szablonowego tak, aby wydzielić i wyświetlić
// te elementy obiektu A, które znajdują się we wnętrzu obiektu B;
// stronę (przednią lub tylną) wyszukiwanych elementów obiektu A
// określa parametr cull_face

void Inside(GLint A, GLint B, GLenum cull_face, GLenum stencil_func)
{
	// początkowo rysujemy obiekt A w buforze głębokości przy
	// wyłączonym zapisie składowych RGBA do bufora kolorów

	// włączenie testu bufora głębokości
	glEnable(GL_DEPTH_TEST);

	// wyłączenie zapisu składowych RGBA do bufora kolorów
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// rysowanie wybranej strony wielokątów
	glCullFace(cull_face);

	// wyświetlenie obiektu A
	glCallList(A);

	// następnie przy użyciu bufora szablonowego wykrywamy te elementy
	// obiektu A, które znajdują się wewnątrz obiektu B; w tym celu
	// zawartość bufora szablonowego jest zwiększana o 1, wszędzie gdzie
	// będą przednie strony wielokątów składających się na obiekt B

	// wyłączenie zapisu do bufora głębokości
	glDepthMask(GL_FALSE);

	// włączenie bufora szablonowego
	glEnable(GL_STENCIL_TEST);

	// test bufora szablonowego
	glStencilFunc(GL_ALWAYS, 0, 0);

	// określenie operacji na buforze szablonowym
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

	// rysowanie tylko przedniej strony wielokątów
	glCullFace(GL_BACK);

	// wyświetlenie obiektu B
	glCallList(B);

	// w kolejnym etapie zmniejszamy zawartość bufora szablonowego o 1
	// wszędzie tam, gdzie są tylne strony wielokątów obiektu B

	// określenie operacji na buforze szablonowym
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);

	// rysowanie tylko tylnej strony wielokątów
	glCullFace(GL_FRONT);

	// wyświetlenie obiektu B
	glCallList(B);

	// dalej wyświetlamy te elementy obiektu A, które
	// znajdują się we wnętrzu obiektu B

	// włączenie zapisu do bufora głębokości
	glDepthMask(GL_TRUE);

	// włączenie zapisu składowych RGBA do bufora kolorów
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// test bufora szablonowego
	glStencilFunc(stencil_func, 0, 1);

	// wyłączenie testu bufora głębokości
	glDisable(GL_DEPTH_TEST);

	// rysowanie wybranej strony wielokątów
	glCullFace(cull_face);

	// wyświetlenie obiektu A
	glCallList(A);

	// wyłączenie bufora szablonowego
	glDisable(GL_STENCIL_TEST);
}


void AND(GLint o1, GLint o2)
{
	// elementy obiektu A znajdujące się we wnętrzu B
	Inside(o1, o2, GL_BACK, GL_NOTEQUAL);

	// wyłączenie zapisu składowych RGBA do bufora kolorów
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// włączenie testu bufora głębokości
	glEnable(GL_DEPTH_TEST);

	// wyłączenie bufora szablonowego
	glDisable(GL_STENCIL_TEST);

	// wybór funkcji do testu bufora głębokości
	glDepthFunc(GL_ALWAYS);

	// wyświetlenie obiektu B
	glCallList(o2);

	// wybór funkcji do testu bufora głębokości
	glDepthFunc(GL_LESS);

	// elementy obiektu B znajdujące się we wnętrzu A
	Inside(o2, o1, GL_BACK, GL_NOTEQUAL);

	glDisable(GL_DEPTH_TEST);
}

void SUB(GLint o1, GLint o2)
{
	// elementy obiektu A znajdujące się we wnętrzu B
	Inside(o1, o2, GL_FRONT, GL_NOTEQUAL);

	// wyłączenie zapisu składowych RGBA do bufora kolorów
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// włączenie testu bufora głębokości
	glEnable(GL_DEPTH_TEST);

	// wyłączenie bufora szablonowego
	glDisable(GL_STENCIL_TEST);

	// wybór funkcji do testu bufora głębokości
	glDepthFunc(GL_ALWAYS);

	// wyświetlenie obiektu B
	glCallList(o2);

	// wybór funkcji do testu bufora głębokości
	glDepthFunc(GL_LESS);

	// elementy obiektu B znajdujące się we wnętrzu A
	Inside(o2, o1, GL_BACK, GL_EQUAL);

	glDisable(GL_DEPTH_TEST);
}

// funkcja generująca scenę 3D

void DisplayScene()
{
	// kolor tła - zawartość bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru, bufora głębokości i bufora szablonowego
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// wybór macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// przesunięcie układu współrzędnych obiektów do środka bryły odcinania
	glTranslatef(0, 0, -(near_ + far_) / 2);

	// obroty całej sceny
	glRotatef(rotatex, 1.0, 0.0, 0.0);
	glRotatef(rotatey, 0.0, 1.0, 0.0);

	// włączenie oświetlenia
	glEnable(GL_LIGHTING);

	// włączenie światła GL_LIGHT0
	glEnable(GL_LIGHT0);

	// włączenie automatycznej normalizacji wektorów normalnych
	glEnable(GL_NORMALIZE);

	// włączenie obsługi właściwości materiałów
	glEnable(GL_COLOR_MATERIAL);

	// właściwości materiału określone przez kolor wierzchołków
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// włączenie rysowania wybranej strony wielokątów
	glEnable(GL_CULL_FACE);

	// v1
	if (csg_op == WARIANT1)
	{
		// włączenie testu bufora głębokości
		glEnable(GL_DEPTH_TEST);

		SUB(Ostroslup, Kula);
		glClear(GL_STENCIL_BUFFER_BIT);
		AND(Torus, Kula);

		// wyłączenie testu bufora głębokości
		glDisable(GL_DEPTH_TEST);
	}
	// v2
	if (csg_op == WARIANT2)
	{
		glEnable(GL_DEPTH_TEST);

		SUB(Ostroslup, Czworoscian);
		glClear(GL_STENCIL_BUFFER_BIT);
		AND(Kula, Torus);

		glDisable(GL_DEPTH_TEST);
	}

	// v3
	if (csg_op == WARIANT3)
	{
		glEnable(GL_DEPTH_TEST);

		SUB(Ostroslup, Kula);
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		AND(Dwudziestoscian, Kula2);

		glDisable(GL_DEPTH_TEST);
	}

	// skierowanie poleceń do wykonania
	glFlush();

	// zamiana buforów koloru
	glutSwapBuffers();
}

// zmiana wielkości okna

void Reshape(int width, int height)
{
	// obszar renderingu - całe okno
	glViewport(0, 0, width, height);

	// wybór macierzy rzutowania
	glMatrixMode(GL_PROJECTION);

	// macierz rzutowania = macierz jednostkowa
	glLoadIdentity();

	// parametry bryły obcinania
	if (aspect == ASPECT_1_1)
	{
		// wysokość okna większa od wysokości okna
		if (width < height && width > 0)
			glFrustum(left, right, bottom * height / width, top * height / width, near_, far_);
		else

			// szerokość okna większa lub równa wysokości okna
			if (width >= height && height > 0)
				glFrustum(left * width / height, right * width / height, bottom, top, near_, far_);

	}
	else
		glFrustum(left, right, bottom, top, near_, far_);

	// generowanie sceny 3D
	DisplayScene();
}

// obsługa przycisków myszki

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		// zapamiętanie stanu lewego przycisku myszki
		button_state = state;

		// zapamiętanie położenia kursora myszki
		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

// obsługa ruchu kursora myszki

void MouseMotion(int x, int y)
{
	if (button_state == GLUT_DOWN)
	{
		rotatey += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) *(x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) *(button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

// obsługa menu podręcznego

void Menu(int value)
{
	switch (value)
	{
		// operacja CSG
	case WARIANT1:
	case WARIANT2:
	case WARIANT3:
		csg_op = value;
		DisplayScene();
		break;

		// obszar renderingu - całe okno
	case FULL_WINDOW:
		aspect = FULL_WINDOW;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// obszar renderingu - aspekt 1:1
	case ASPECT_1_1:
		aspect = ASPECT_1_1;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// wyjście
	case EXIT:
		exit(0);
	}
}

// utworzenie list wyświetlania

void GenerateDisplayLists()
{
	Kula = glGenLists(1);

	glNewList(Kula, GL_COMPILE);

	glColor4fv(Green);
	glutSolidSphere(0.6, 30, 30);

	glEndList();


	Kula2 = glGenLists(1);

	glNewList(Kula2, GL_COMPILE);

	glColor4fv(Blue);
	glutSolidSphere(0.9, 30, 30);

	glEndList();


	Torus = glGenLists(1);

	glNewList(Torus, GL_COMPILE);

	glColor4fv(Blue);
	glutSolidTorus(0.1, 0.6, 10, 20);

	glEndList();


	Ostroslup = glGenLists(1);

	glNewList(Ostroslup, GL_COMPILE);

	glBegin(GL_TRIANGLES);
	glColor3fv(Red);
	glVertex3f(1.0, -1.0, -1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(0.0, 1.0, 0.0);

	glColor3fv(Blue);
	glVertex3f(0.0, -1.0, 1.0);
	glVertex3f(1.0, -1.0, -1.0);
	glVertex3f(0.0, 1.0, 0.0);

	glColor3fv(Green);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(0.0, -1.0, 1.0);
	glVertex3f(0.0, 1.0, 0.0);

	glColor3fv(Yellow);
	glVertex3f(1.0, -1.0, -1.0);
	glVertex3f(0.0, -1.0, 1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glEnd();

	glEndList();

	Czworoscian = glGenLists(1);
	glNewList(Czworoscian, GL_COMPILE);

	glColor4fv(Brown);
	glutSolidTetrahedron();
	glEndList();

	Dwudziestoscian = glGenLists(1);
	glNewList(Dwudziestoscian, GL_COMPILE);

	glColor4fv(Purple);
	glutSolidIcosahedron();
	glEndList();
}

int main(int argc, char * argv[])
{
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);

	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);

	// rozmiary głównego okna programu
	glutInitWindowSize(500, 500);

	// utworzenie głównego okna programu
	glutCreateWindow("CSG");

	// dołączenie funkcji generującej scenę 3D
	glutDisplayFunc(DisplayScene);

	// dołączenie funkcji wywoływanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// obsługa przycisków myszki
	glutMouseFunc(MouseButton);

	// obsługa ruchu kursora myszki
	glutMotionFunc(MouseMotion);

	// utworzenie podmenu - Operacja CSG
	int MenuCSGOp = glutCreateMenu(Menu);
	glutAddMenuEntry("Wariant 1", WARIANT1);
	glutAddMenuEntry("Wariant 2", WARIANT2);
	glutAddMenuEntry("Wariant 3", WARIANT3);

	// utworzenie podmenu - Aspekt obrazu
	int MenuAspect = glutCreateMenu(Menu);
	glutAddMenuEntry("Aspekt obrazu - całe okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);

	// menu główne
	glutCreateMenu(Menu);
	glutAddSubMenu("Operacja CSG", MenuCSGOp);

	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjście", EXIT);


	// określenie przycisku myszki obsługującego menu podręczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// utworzenie list wyświetlania
	GenerateDisplayLists();

	// wprowadzenie programu do obsługi pętli komunikatów
	glutMainLoop();
	return 0;
}