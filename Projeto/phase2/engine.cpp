#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <string.h>

#include "tinyxml2.h"
#include "tinyxml2.cpp"


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

using namespace tinyxml2;
/*
	Drawing movement
*/
float angle = 0;
float tx = 0;
float ty = 0;
float tz = 0;

float mode = GL_LINE;

float* init_translate;
float** all_vertices;
float* vertices;
int N; //vertices number 
int num;

/*
	Window size
	@param 0 - width
	@param 1 - height
*/
int w_size[2];
//Camera Values
float c_pos[3];
float c_lAt[3];
float c_lUp[3];
//Perspective values
float c_proj[3];

float radius, dist; //dist is the same as radius but y coord is 0
float angleAlpha, angleBeta;
int cord_x = -1, cord_y = -1;

//Vertex Buffer Object -> VBO
typedef struct buffer {
	GLuint buffers[1];
	int verticesCount;
	struct buffer* next;
	int pos;
}*Buffer;

typedef struct group {

	float* translate;
	float* rotate;
	float* scale;

	Buffer b;
	struct group** sub;
	int nSub;
}*Group;

Group* group; //array with all the groups info
int nGroup;



void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(c_proj[0], ratio, c_proj[1], c_proj[2]);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void Axis() {
	glBegin(GL_LINES);
	// X axis in red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 0.0f, 0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-100.0f, 0.0f, 0.0f);
	// Y Axis in Green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 100.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, -100.0f, 0.0f);
	// Z Axis in Blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, -100.0f);
	glEnd();
}

void transform(Group g, int flag) {

	glTranslatef(init_translate[0], init_translate[1], init_translate[2]);

	//std::cout << "initTranslate: " << init_translate[0] <<", " << init_translate[1]<< ", " << init_translate[2] << '\n';
	if (g->translate != NULL)
		glTranslatef(g->translate[0], g->translate[1], g->translate[2]);

	if (g->rotate != NULL)
		glRotatef(g->rotate[0], g->rotate[1], g->rotate[2], g->rotate[3]);


	if (g->scale != NULL && flag ) //flag serve para escalas relativas as sub
		glScalef(g->scale[0], g->scale[1], g->scale[2]);

	

}


void renderSceneAux(Group g, Group first) {
	Buffer b = g->b;
	glPushMatrix();

	if (first) transform(first, 0);
	 transform(g, 1);
	 


	glBindBuffer(GL_ARRAY_BUFFER, b->buffers[0]);

	glVertexPointer(3, GL_FLOAT, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, b->verticesCount);

	glPopMatrix();

	if (g->sub) {
		for (int i = 0; i < g->nSub; i++) {
			renderSceneAux(g->sub[i], g);
			
		}
	}




}

void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();

	gluLookAt(cos(angleBeta) * sin(angleAlpha) * radius, sin(angleBeta) * radius, cos(angleBeta) * cos(angleAlpha) * radius,
			  c_lAt[0], c_lAt[1], c_lAt[2],
			  c_lUp[0], c_lUp[1], c_lUp[2]);


	for (int i = 0; i < nGroup; i++) {
		renderSceneAux(group[i], NULL); //NULL, pois nao � subgrupo
	}

	glTranslated(tx, ty, tz);
	glRotated(angle, 0, 1, 0);
	glPolygonMode(GL_FRONT_AND_BACK, mode);

	Axis();

	// End of frame
	glutSwapBuffers();
}





void readFile(const char* filename) {
	FILE* f;
	std::cout << "readFile filename:" << filename << '\n';
	f = fopen(filename, "r");
	char* n = (char*)malloc(sizeof(char) * 10);
	n[0] = 0;
	char* currElem = (char*)malloc(sizeof(char) * 9);
	currElem[0] = 0;
	char c;
	int i = 0;
	int flag = 1;

	if (f) {
		while ((c = fgetc(f)) != EOF) {
			if (flag) {
				if (c == '\n') {
					num = atoi(n);
					vertices = (float*)malloc(sizeof(float) * num);
					flag = 0;
				}
				else strncat(n, &c, 1);
			}
			else {
				if (c == ' ') {
					float elem = atof(currElem);
					vertices[i++] = elem;
					currElem[0] = 0;
				}
				else strncat(currElem, &c, 1);
			}
		}
		N = i;
	}
	else {
		printf("ERROR:cannot find .3d file\n");
		
	}
	fclose(f);
	
}

void readXMLaux(Group g, XMLElement* elem) {

	const char* file3d;
	if (elem->FirstChildElement("models") != NULL) {
		file3d = elem->FirstChildElement("models")->FirstChildElement("model")->Attribute("file");
		readFile(file3d);
	}

	float* vertexB = (float*)malloc(sizeof(float) * N);
	vertexB = vertices;
	g->b = (Buffer)malloc(sizeof(buffer));

	g->b->verticesCount = N;

	glGenBuffers(1, g->b->buffers); //criar o VB0
	glBindBuffer(GL_ARRAY_BUFFER, g->b->buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * g->b->verticesCount, vertexB, GL_STATIC_DRAW);


	XMLElement* TL = elem->FirstChildElement("transform")->FirstChildElement("translate");
	XMLElement* RT = elem->FirstChildElement("transform")->FirstChildElement("rotate");
	XMLElement* SC = elem->FirstChildElement("transform")->FirstChildElement("scale");


	if (TL) {
		g->translate = (float*)malloc(sizeof(float) * 3);
		g->translate[0] = atof(TL->Attribute("x"));
		g->translate[1] = atof(TL->Attribute("y"));
		g->translate[2] = atof(TL->Attribute("z"));
	}

	else {
		g->translate = NULL;
	}

	if (RT) {
		g->rotate = (float*)malloc(sizeof(float) * 4);
		g->rotate[0] = atof(RT->Attribute("angle"));
		g->rotate[1] = atof(RT->Attribute("x"));
		g->rotate[2] = atof(RT->Attribute("y"));
		g->rotate[3] = atof(RT->Attribute("z"));
	}

	else {
		g->rotate = NULL;
	}


	if (SC) {
		g->scale = (float*)malloc(sizeof(float) * 3);

		g->scale[0] = atof(SC->Attribute("x"));
		g->scale[1] = atof(SC->Attribute("y"));
		g->scale[2] = atof(SC->Attribute("z"));

	}

	else {
		g->scale = NULL;

	}


	if (elem->FirstChildElement("group")) {

		int i = 0;
		for (XMLElement* elem2 = elem->FirstChildElement("group"); elem2; elem2 = elem2->NextSiblingElement()) i++;
		g->sub = (Group*)malloc(sizeof(struct group) * i);
		g->nSub = i;


		XMLElement* elem3 = elem->FirstChildElement("group");
		for (int k = 0; k < g->nSub; k++) {
			g->sub[k] = (Group)malloc(sizeof(struct group));
			readXMLaux(g->sub[k], elem3);
			elem3 = elem3->NextSiblingElement();
		}
	}
	else { g->sub = NULL; g->nSub = 0; }
}

int readXML(char* filename)
{
	std::cout <<"readXML filename:" << filename << '\n';
	XMLDocument doc;
	const char* file3d;

	if (!(doc.LoadFile(filename)))
	{
		XMLElement* win = doc.FirstChildElement("world")->FirstChildElement("window");
		//std::cout << "readXML->win\n";
		w_size[0] = win->FloatAttribute("width");
		w_size[1] = win->FloatAttribute("height");


		XMLElement* pos = doc.FirstChildElement("world")->FirstChildElement("camera")->FirstChildElement("position");
		//std::cout << "readXML->pos\n";
		c_pos[0] = atof(pos->Attribute("x"));
		c_pos[1] = atof(pos->Attribute("y"));
		c_pos[2] = atof(pos->Attribute("z"));

		XMLElement* lAt = doc.FirstChildElement("world")->FirstChildElement("camera")->FirstChildElement("lookAt");
		//std::cout << "readXML->lAt\n";
		c_lAt[0] = atof(lAt->Attribute("x"));
		c_lAt[1] = atof(lAt->Attribute("y"));
		c_lAt[2] = atof(lAt->Attribute("z"));

		XMLElement* lUp = doc.FirstChildElement("world")->FirstChildElement("camera")->FirstChildElement("up");
		//std::cout << "readXML->lUp\n";
		c_lUp[0] = atof(lUp->Attribute("x"));
		c_lUp[1] = atof(lUp->Attribute("y"));
		c_lUp[2] = atof(lUp->Attribute("z"));

		XMLElement* proj = doc.FirstChildElement("world")->FirstChildElement("camera")->FirstChildElement("projection");
		//std::cout << "readXML->proj\n";
		c_proj[0] = atof(proj->Attribute("fov"));
		c_proj[1] = atof(proj->Attribute("near"));
		c_proj[2] = atof(proj->Attribute("far"));



		XMLElement* TR = doc.FirstChildElement("world")->FirstChildElement("group")->FirstChildElement("transform");
		if (TR) {
			XMLElement* IT = TR->FirstChildElement("translate");
			if (IT) {
				init_translate = (float*)malloc(sizeof(float) * 3);
				init_translate[0] = atof(IT->Attribute("x"));
				init_translate[1] = atof(IT->Attribute("y"));
				init_translate[2] = atof(IT->Attribute("z"));
			}
			else {
				init_translate = NULL;
			}
		}

		int i = 0;
		//Checks the number of groups
		XMLElement* mod = doc.FirstChildElement("world")->FirstChildElement("group")->FirstChildElement("group");
		for (i = 0; mod; mod = mod->NextSiblingElement()) i++;
		if (i != 0) { // caso seja apenas 1 elemento
			group = (Group*)malloc(sizeof(struct group) * i);
			nGroup = i;




			int k = 0;
			XMLElement* elem = doc.FirstChildElement("world")->FirstChildElement("group")->FirstChildElement("group");

			for (k = 0; k < nGroup; k++) {

				group[k] = (Group)malloc(sizeof(struct group));
				readXMLaux(group[k], elem);
				elem = elem->NextSiblingElement();


			}
		}
		else {
			group = (Group*)malloc(sizeof(struct group) * 1);
			nGroup = 1;
			XMLElement* elem = doc.FirstChildElement("world")->FirstChildElement("group");
			group[0] = (Group)malloc(sizeof(struct group));
			readXMLaux(group[0], elem);
		}


		//camera stuff
		radius = sqrt(pow(c_pos[0], 2) + pow(c_pos[1], 2) + pow(c_pos[2], 2));
		dist = sqrt(pow(c_pos[0], 2) + pow(c_pos[2], 2)); //dist is the same as radius but y coord is 0
		angleAlpha = acos(c_pos[2] / dist);
		angleBeta = acos(dist / radius);


	}
	else {
		printf("ERROR: XML not found\n");
		return 1;
	}

	return 0;
}


// write function to process keyboard events
void keys(unsigned char key, int x, int y) {
	switch (key)
	{
	case 's': //"Tras"
		tz += 0.1;
		break;
	case 'w': //"Frente"
		tz -= 0.1;
		break;
	case 'r': //subir
		ty += 0.1;
		break;
	case 'f': //descer
		ty -= 0.1;
		break;
	case 'd': //direita
		tx += 0.1;
		break;
	case 'a': //esquerda
		tx -= 0.1;
		break;
	case 'q': //rodar esquerda
		angle += 5;
		break;
	case 'e': //rodar direita
		angle -= 5;
		break;
	case 'i':
		radius -= 0.3f;
		break;
	case 'k':
		radius += 0.3f;
		break;
	case 'o':
		radius -= 0.1f;
		break;
	case 'l':
		radius += 0.1f;
		break;

	case '1':
		mode = GL_FILL;
		break;
	case '2':
		mode = GL_LINE;
		break;
	case '3':
		mode = GL_POINT;
		break;
	default:
		glutPostRedisplay();
		break;
	}
}

void sKeys(int key_code, int x, int y) {
	switch (key_code) {
	case GLUT_KEY_LEFT:
		angleAlpha -= 0.07f;
		break;
	case GLUT_KEY_RIGHT:
		angleAlpha += 0.07f;
		break;
	case GLUT_KEY_UP:
		if (angleBeta < M_PI / 2)
			angleBeta += 0.1f;
		break;
	case GLUT_KEY_DOWN:
		if (angleBeta > -M_PI / 2)
			angleBeta -= 0.1f;
		break;
	}

	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			cord_x = x;
			cord_y = y;
		}
		else {
			cord_x = -1.0f;
			cord_y = -1.0f;
		}
	}
}

void motion_mouse(int x, int y) {
	if (cord_x >= 0) {
		angleAlpha = (x + cord_x) * 0.01f;
		if (angleBeta < M_PI / 2) {
			angleBeta = (y - cord_y) * 0.01f;
		}
		if (angleBeta > -M_PI / 2) {
			angleBeta = (y - cord_y) * 0.01f;
		}
	}

	glutPostRedisplay();
}




int main(int argc, char** argv) {

	
	printf("boot\n");
	
	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(512, 512); //TODO: load do w size antes de ser necessario glew
	glutCreateWindow("Grupo 17");

	// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);


	// put here the registration of the keyboard callbacks
	glutKeyboardFunc(keys);
	glutSpecialFunc(sKeys);
	glutMouseFunc(mouse);
	glutMotionFunc(motion_mouse);

	// init GLEW
	#ifndef __APPLE__
		glewInit();
	#endif

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnableClientState(GL_VERTEX_ARRAY);

	

	if (argc == 2)
	{
		//std::cout << "pre-readXML\n";
		if (readXML(argv[1]) != 0) return 1;
	}
	else {
		printf("Incorrect number of arguments\n");
		return 1;
	}


	// enter GLUT's main cycle
	glutMainLoop();
	free(vertices);

	return 0;
}
