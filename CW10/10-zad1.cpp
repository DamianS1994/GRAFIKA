#include "stdafx.h"
#include <GL/glut.h>
#include <GL/glu.h>
#include "glext.h"
#include <stdlib.h>
#include <stdio.h>
#include "colors.h"
#include "materials.h"


// stałe do obsługi menu podręcznego
enum
{

	// materiały
	BRASS, // mosiądz
	BRONZE, // brąz
	POLISHED_BRONZE, // polerowany brąz
	CHROME, // chrom
	COPPER, // miedź
	POLISHED_COPPER, // polerowana miedź
	GOLD, // złoto
	POLISHED_GOLD, // polerowane złoto
	PEWTER, // grafit (cyna z ołowiem)
	SILVER, // srebro
	POLISHED_SILVER, // polerowane srebro
	EMERALD, // szmaragd
	JADE, // jadeit
	OBSIDIAN, // obsydian
	PEARL, // perła
	RUBY, // rubin
	TURQUOISE, // turkus
	BLACK_PLASTIC, // czarny plastik
	BLACK_RUBBER, // czarna guma

				  // obszar renderingu
	FULL_WINDOW, // aspekt obrazu - całe okno
	ASPECT_1_1, // aspekt obrazu 1:1
	EXIT, // wyjście
};

// aspekt obrazu

int aspect = FULL_WINDOW;

// usunięcie definicji makr near i far

#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif

// rozmiary bryły obcinania

const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near = 3.0;
const GLdouble far = 7.0;

// kąty obrotu obiektu

GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

// wskaźnik naciśnięcia lewego przycisku myszki

int button_state = GLUT_UP;

// położenie kursora myszki

int button_x, button_y;

// współczynnik skalowania

GLfloat scale = 1.0;

// właściwości materiału - domyślnie mosiądz

const GLfloat * ambient = BrassAmbient;
const GLfloat * diffuse = BrassDiffuse;
const GLfloat * specular = BrassSpecular;
GLfloat shininess = BrassShininess;

// metoda podziału powierzchni NURBS na wielokąty

int sampling_method = GLU_PATH_LENGTH;

// sposób renderowania powierzchni NURBS

int display_mode = GLU_FILL;

const int K = 4;

// współrzędne punktów kontrolnych powierzchni

GLfloat points[4 * K * 3] =
{
	-0.8, 0.0, 0.0,			-0.5, 1.6, 0.0,			0.5, -0.5, 0.0,			-0.8, 0.0, 0.5,
	-0.8, 0.0, 0.5,			-0.5, 0.3, 0.5,			-0.7, -0.3, 0.5,	    -0.8, 0.0, 0.5,
	-0.8, 0.0, 1.0,			-0.5, 0.6, 1.0,			0.5, 0.6, 1.0,			-0.8, 0.0, 0.5,
	-0.8, 0.0, 1.5,			-0.4, 1.5, 1.5,			0.5, -0.6, 1.5,			-0.8, 0.0, 0.5,

};

// węzły

GLfloat knots[4 * 2] =
{
	0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0
};

// funkcja generująca scenę 3D

void DisplayScene()
{
	// kolor tła - zawartość bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru i bufora głębokości
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// wybór macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// włączenie testu bufora głębokości
	glEnable(GL_DEPTH_TEST);

	// przesunięcie układu współrzędnych obiektu do środka bryły odcinania
	glTranslatef(0, 0, -(near + far) / 2);

	// obroty obiektu
	glRotatef(rotatex, 1.0, 0, 0);
	glRotatef(rotatey, 0, 1.0, 0);

	// skalowanie obiektu - klawisze "+" i "-"
	glScalef(scale, scale, scale);

	// włączenie efektów oświetlenia, gry renderowana jest wypełniona powierzchnia
	if (display_mode == GLU_FILL)
	{
		// włączenie oświetlenia
		glEnable(GL_LIGHTING);

		// włączenie światła GL_LIGHT0 z parametrami domyślnymi
		glEnable(GL_LIGHT0);

		// włączenie automatycznej normalizacji wektorów normalnych
		glEnable(GL_NORMALIZE);

		// właściwości materiału
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

		// włączenie automatycznego generowania wektorów normalnych
		glEnable(GL_AUTO_NORMAL);
	}

	// kolor krawędzi
	glColor3fv(Black);

	// utworzenie obiektu NURBS
	GLUnurbsObj * nurbs = gluNewNurbsRenderer();

	// początek definicji powierzchni NURBS
	gluBeginSurface(nurbs);

	// sposób renderowania powierzchni NURBS
	gluNurbsProperty(nurbs, GLU_DISPLAY_MODE, display_mode);

	// metoda podziału powierzchni NURBS na wielokąty
	gluNurbsProperty(nurbs, GLU_SAMPLING_METHOD, sampling_method);

	// narysowanie powierzchni
	gluNurbsSurface(nurbs, 8, knots, 8, knots, 4 * 3, 3, points, 4, 4, GL_MAP2_VERTEX_3);

	// koniec definicji powierzchni
	gluEndSurface(nurbs);

	// usunięcie obiektu NURBS
	gluDeleteNurbsRenderer(nurbs);

	// wyłączenie automatycznego generowania wektorów normalnych
	glDisable(GL_AUTO_NORMAL);

	// wyłączenie automatycznej normalizacji wektorów normalnych
	glDisable(GL_NORMALIZE);

	// wyłączenie światła GL_LIGHT0
	glDisable(GL_LIGHT0);

	// wyłaczenie oświetlenia
	glDisable(GL_LIGHTING);

	// kolor punktów
	glColor3fv(Blue);

	// rozxmiar punktów
	glPointSize(6.0);

	// narysowanie punktów kontrolnych
	glBegin(GL_POINTS);
	for (int i = 0; i < 4 * K; i++)
		glVertex3fv(points + i * 3);

	glEnd();

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
			glFrustum(left, right, bottom * height / width, top * height / width, near, far);
		else

			// szerokość okna większa lub równa wysokości okna
			if (width >= height && height > 0)
				glFrustum(left * width / height, right * width / height, bottom, top, near, far);

	}
	else
		glFrustum(left, right, bottom, top, near, far);

	// generowanie sceny 3D
	DisplayScene();
}

// obsługa klawiatury

void Keyboard(unsigned char key, int x, int y)
{
	// klawisz +
	if (key == '+')
		scale += 0.05;
	else

		// klawisz -
		if (key == '-' && scale > 0.05)
			scale -= 0.05;

	// narysowanie sceny
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
		// GLU_DISPLAY_MODE
	case GLU_FILL:
	case GLU_OUTLINE_PATCH:
	case GLU_OUTLINE_POLYGON:
		display_mode = value;
		DisplayScene();
		break;

		// GLU_SAMPLING_METHOD
	case GLU_PATH_LENGTH:
	case GLU_PARAMETRIC_ERROR:
	case GLU_DOMAIN_DISTANCE:

		sampling_method = value;
		DisplayScene();
		break;


		// materiał - mosiądz
	case BRASS:
		ambient = BrassAmbient;
		diffuse = BrassDiffuse;
		specular = BrassSpecular;
		shininess = BrassShininess;
		DisplayScene();
		break;

		// materiał - brąz
	case BRONZE:
		ambient = BronzeAmbient;
		diffuse = BronzeDiffuse;
		specular = BronzeSpecular;
		shininess = BronzeShininess;
		DisplayScene();
		break;

		// materiał - polerowany brąz
	case POLISHED_BRONZE:
		ambient = PolishedBronzeAmbient;
		diffuse = PolishedBronzeDiffuse;
		specular = PolishedBronzeSpecular;
		shininess = PolishedBronzeShininess;
		DisplayScene();
		break;

		// materiał - chrom
	case CHROME:
		ambient = ChromeAmbient;
		diffuse = ChromeDiffuse;
		specular = ChromeSpecular;
		shininess = ChromeShininess;
		DisplayScene();
		break;

		// materiał - miedź
	case COPPER:
		ambient = CopperAmbient;
		diffuse = CopperDiffuse;
		specular = CopperSpecular;
		shininess = CopperShininess;
		DisplayScene();
		break;

		// materiał - polerowana miedź
	case POLISHED_COPPER:
		ambient = PolishedCopperAmbient;
		diffuse = PolishedCopperDiffuse;
		specular = PolishedCopperSpecular;
		shininess = PolishedCopperShininess;
		DisplayScene();
		break;

		// materiał - złoto
	case GOLD:
		ambient = GoldAmbient;
		diffuse = GoldDiffuse;
		specular = GoldSpecular;
		shininess = GoldShininess;
		DisplayScene();
		break;

		// materiał - polerowane złoto
	case POLISHED_GOLD:
		ambient = PolishedGoldAmbient;
		diffuse = PolishedGoldDiffuse;
		specular = PolishedGoldSpecular;
		shininess = PolishedGoldShininess;
		DisplayScene();
		break;

		// materiał - grafit (cyna z ołowiem)
	case PEWTER:
		ambient = PewterAmbient;
		diffuse = PewterDiffuse;
		specular = PewterSpecular;
		shininess = PewterShininess;
		DisplayScene();
		break;

		// materiał - srebro
	case SILVER:
		ambient = SilverAmbient;
		diffuse = SilverDiffuse;
		specular = SilverSpecular;
		shininess = SilverShininess;
		DisplayScene();
		break;

		// materiał - polerowane srebro
	case POLISHED_SILVER:
		ambient = PolishedSilverAmbient;
		diffuse = PolishedSilverDiffuse;
		specular = PolishedSilverSpecular;
		shininess = PolishedSilverShininess;
		DisplayScene();
		break;

		// materiał - szmaragd
	case EMERALD:
		ambient = EmeraldAmbient;
		diffuse = EmeraldDiffuse;
		specular = EmeraldSpecular;
		shininess = EmeraldShininess;
		DisplayScene();
		break;

		// materiał - jadeit
	case JADE:
		ambient = JadeAmbient;
		diffuse = JadeDiffuse;
		specular = JadeSpecular;
		shininess = JadeShininess;
		DisplayScene();
		break;

		// materiał - obsydian
	case OBSIDIAN:
		ambient = ObsidianAmbient;
		diffuse = ObsidianDiffuse;
		specular = ObsidianSpecular;
		shininess = ObsidianShininess;
		DisplayScene();
		break;

		// materiał - perła
	case PEARL:
		ambient = PearlAmbient;
		diffuse = PearlDiffuse;
		specular = PearlSpecular;
		shininess = PearlShininess;
		DisplayScene();
		break;

		// metariał - rubin
	case RUBY:
		ambient = RubyAmbient;
		diffuse = RubyDiffuse;
		specular = RubySpecular;
		shininess = RubyShininess;
		DisplayScene();
		break;

		// materiał - turkus
	case TURQUOISE:
		ambient = TurquoiseAmbient;
		diffuse = TurquoiseDiffuse;
		specular = TurquoiseSpecular;
		shininess = TurquoiseShininess;
		DisplayScene();
		break;

		// materiał - czarny plastik
	case BLACK_PLASTIC:
		ambient = BlackPlasticAmbient;
		diffuse = BlackPlasticDiffuse;
		specular = BlackPlasticSpecular;
		shininess = BlackPlasticShininess;
		DisplayScene();
		break;

		// materiał - czarna guma
	case BLACK_RUBBER:
		ambient = BlackRubberAmbient;
		diffuse = BlackRubberDiffuse;
		specular = BlackRubberSpecular;
		shininess = BlackRubberShininess;
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


int main(int argc, char * argv[])
{
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);

	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// rozmiary głównego okna programu
	glutInitWindowSize(500, 500);

	// utworzenie głównego okna programu
	glutCreateWindow("Powierzchnia NURBS");

	// dołączenie funkcji generującej scenę 3D
	glutDisplayFunc(DisplayScene);

	// dołączenie funkcji wywoływanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// dołączenie funkcji obsługi klawiatury
	glutKeyboardFunc(Keyboard);

	// obsługa przycisków myszki
	glutMouseFunc(MouseButton);

	// obsługa ruchu kursora myszki
	glutMotionFunc(MouseMotion);

	// utworzenie menu podręcznego
	glutCreateMenu(Menu);

	// utworzenie podmenu - GLU_DISPLAY_MODE
	int MenuDisplayMode = glutCreateMenu(Menu);
	glutAddMenuEntry("GLU_FILL", GLU_FILL);
	glutAddMenuEntry("GLU_OUTLINE_PATCH", GLU_OUTLINE_PATCH);
	glutAddMenuEntry("GLU_OUTLINE_POLYGON", GLU_OUTLINE_POLYGON);

	// utworzenie podmenu - GLU_SAMPLING_METHOD
	int MenuSamplingMethod = glutCreateMenu(Menu);
	glutAddMenuEntry("GLU_PATH_LENGTH", GLU_PATH_LENGTH);
	glutAddMenuEntry("GLU_PARAMETRIC_ERROR", GLU_PARAMETRIC_ERROR);
	glutAddMenuEntry("GLU_DOMAIN_DISTANCE", GLU_DOMAIN_DISTANCE);

	// utworzenie podmenu - Materiał
	int MenuMaterial = glutCreateMenu(Menu);

	glutAddMenuEntry("Mosiądz", BRASS);
	glutAddMenuEntry("Brąz", BRONZE);
	glutAddMenuEntry("Polerowany brąz", POLISHED_BRONZE);
	glutAddMenuEntry("Chrom", CHROME);
	glutAddMenuEntry("Miedź", COPPER);
	glutAddMenuEntry("Polerowana miedź", POLISHED_COPPER);
	glutAddMenuEntry("Złoto", GOLD);
	glutAddMenuEntry("Polerowane złoto", POLISHED_GOLD);
	glutAddMenuEntry("Grafit (cyna z ołowiem)", PEWTER);
	glutAddMenuEntry("Srebro", SILVER);
	glutAddMenuEntry("Polerowane srebro", POLISHED_SILVER);
	glutAddMenuEntry("Szmaragd", EMERALD);
	glutAddMenuEntry("Jadeit", JADE);
	glutAddMenuEntry("Obsydian", OBSIDIAN);
	glutAddMenuEntry("Perła", PEARL);
	glutAddMenuEntry("Rubin", RUBY);
	glutAddMenuEntry("Turkus", TURQUOISE);
	glutAddMenuEntry("Czarny plastik", BLACK_PLASTIC);
	glutAddMenuEntry("Czarna guma", BLACK_RUBBER);

	// utworzenie podmenu - Aspekt obrazu
	int MenuAspect = glutCreateMenu(Menu);

	glutAddMenuEntry("Aspekt obrazu - całe okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);

	// menu główne
	glutCreateMenu(Menu);
	glutAddSubMenu("GLU_DISPLAY_MODE", MenuDisplayMode);
	glutAddSubMenu("GLU_SAMPLING_METHOD", MenuSamplingMethod);

	glutAddSubMenu("Materiał", MenuMaterial);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjście", EXIT);


	// określenie przycisku myszki obsługującej menu podręczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// wprowadzenie programu do obsługi pętli komunikatów
	glutMainLoop();
	return 0;
}