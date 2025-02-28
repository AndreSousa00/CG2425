#include <iostream>
#include <fstream>
#include <vector>
#include "tinyxml2.h"
#include "tinyxml2.cpp"
#include <stdio.h>
#include <string.h>


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
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

int numPrimitives; // The number of primitives to be drawn ( how many .3d files where loaded)
float** all_vertices;
float* vertices;
int N; //vertices number 
int num;

// Global variables for mouse interaction
bool mouseLeftDown = false;
bool mouseRightDown = false;
bool mouseMiddleDown = false;
int mouseX = 0, mouseY = 0;

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
	// Y Axis in Green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 100.0f, 0.0f);
	// Z Axis in Blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();
}

void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(	c_pos[0], c_pos[1], c_pos[2],
				c_lAt[0], c_lAt[1], c_lAt[2],
				c_lUp[0], c_lUp[1], c_lUp[2]);
	Axis();

	// put the geometric transformations here
		
	glTranslated(tx, ty, tz);
	glRotated(angle, 0, 1, 0);
	glPolygonMode(GL_FRONT_AND_BACK, mode);

	for (int i = 0; i < numPrimitives; i++) {

		for (int j = 1; j < (int)all_vertices[i][0]; j += 9) { //Starts at 1, because all_vertices[i][0] contains the number of vertices
			glColor3f(0.729f, 0.333f, 0.827f); // Dark Fuschia
			glBegin(GL_TRIANGLES);
			glVertex3f(all_vertices[i][j], all_vertices[i][j + 1], all_vertices[i][j + 2]);
			glVertex3f(all_vertices[i][j + 3], all_vertices[i][j + 4], all_vertices[i][j + 5]);
			glVertex3f(all_vertices[i][j + 6], all_vertices[i][j + 7], all_vertices[i][j + 8]);
			glEnd();

			j += 9;
			glColor3f(1.000f, 0.271f, 0.000f); // Red Orange
			glBegin(GL_TRIANGLES);
			glVertex3f(all_vertices[i][j], all_vertices[i][j + 1], all_vertices[i][j + 2]);
			glVertex3f(all_vertices[i][j + 3], all_vertices[i][j + 4], all_vertices[i][j + 5]);
			glVertex3f(all_vertices[i][j + 6], all_vertices[i][j + 7], all_vertices[i][j + 8]);
			glEnd();
		}
	}

	


	// End of frame
	glutSwapBuffers();
}



int readFile(const char* filename) { //TODO: usar ifstreams de C++
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
		return 1;
	}
	fclose(f);
	return 0;
}

int readXML(char* filename) 
{
	std::cout <<"readXML filename:" << filename << '\n';
	XMLDocument doc;
	const char* file3d;

	if (!(doc.LoadFile(filename)))
	{
		XMLElement * win = doc.FirstChildElement("world")->FirstChildElement("window");
		std::cout << "readXML->win\n";
		w_size[0] = win->FloatAttribute("width");
		w_size[1] = win->FloatAttribute("height");
		

		XMLElement* pos = doc.FirstChildElement("world")->FirstChildElement("camera")->FirstChildElement("position");
		std::cout << "readXML->pos\n";
		c_pos[0] = atof(pos->Attribute("x"));
		c_pos[1] = atof(pos->Attribute("y"));
		c_pos[2] = atof(pos->Attribute("z"));

		XMLElement* lAt = doc.FirstChildElement("world")->FirstChildElement("camera")->FirstChildElement("lookAt");
		std::cout << "readXML->lAt\n";
		c_lAt[0] = atof(lAt->Attribute("x"));
		c_lAt[1] = atof(lAt->Attribute("y"));
		c_lAt[2] = atof(lAt->Attribute("z"));

		XMLElement* lUp = doc.FirstChildElement("world")->FirstChildElement("camera")->FirstChildElement("up");
		std::cout << "readXML->lUp\n";
		c_lUp[0] = atof(lUp->Attribute("x"));
		c_lUp[1] = atof(lUp->Attribute("y"));
		c_lUp[2] = atof(lUp->Attribute("z"));

		XMLElement* proj = doc.FirstChildElement("world")->FirstChildElement("camera")->FirstChildElement("projection");
		std::cout << "readXML->proj\n";
		c_proj[0] = atof(proj->Attribute("fov"));
		c_proj[1] = atof(proj->Attribute("near"));
		c_proj[2] = atof(proj->Attribute("far"));

		int j = 1, k = 1;
		//Checks the number of .3d to load
		for (XMLElement* mod = doc.FirstChildElement("world")->FirstChildElement("group")->FirstChildElement("models")->FirstChildElement("model"); mod; mod = mod->NextSiblingElement()) {
			if (j = k) {
				k = k * 2;
				all_vertices = (float**)malloc(sizeof(float) * k);
			}
			j++;

		}

		int i = 0;
		//Loads all .3d files
		for (XMLElement* mod = doc.FirstChildElement("world")->FirstChildElement("group")->FirstChildElement("models")->FirstChildElement("model"); mod; mod = mod->NextSiblingElement()) {

			file3d = mod->Attribute("file");

			readFile(file3d);
			all_vertices[i] = (float*)malloc(sizeof(float) * (N + 1));
			all_vertices[i][0] = (float)N;
			for (int w = 1; w <= N; w++) {
				all_vertices[i][w] = vertices[w - 1];
			}
			i++;
		}

		numPrimitives = i;


	}
	else {
		printf("ERROR: XML not found\n");
		return 1;
	}

	return 0;
}


// write function to process keyboard events
void keysFunc(unsigned char key, int x, int y) {
	switch (key) {
	case 'd': angle += 5; break;
	case 'a': angle -= 5; break;
	case 'w': tx += 0.5; break;
	case 's': tx -= 0.5; break;
	case 'q': ty += 0.5; break;
	case 'e': ty -= 0.5; break;
	case 'z': tz += 0.5; break;
	case 'x': tz -= 0.5; break;
	case 'm': mode = GL_FILL; break;
	case 'n': mode = GL_LINE; break;
    case '+': 
    case '=': // Allow = key (same physical key as +)
        c_pos[2] -= 1.0f; // Zoom in - move camera closer
        break;
    case '-': 
    case '_': // Allow _ key (same physical key as -)
        c_pos[2] += 1.0f; // Zoom out - move camera further
        break;
    }
	glutPostRedisplay();
}

// write function to process special keyboard events
void specialKeysFunc(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_RIGHT: angle += 5; break;
	case GLUT_KEY_LEFT: angle -= 5; break;
	case GLUT_KEY_UP: tx += 0.5; break;
	case GLUT_KEY_DOWN: tx -= 0.5; break;
	}
	glutPostRedisplay();
}

// Mouse button callback function
void mouseFunc(int button, int state, int x, int y) {
    // Update current mouse position
    mouseX = x;
    mouseY = y;
    
    // Handle button press/release
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseLeftDown = true;
        }
        else if (state == GLUT_UP) {
            mouseLeftDown = false;
        }
    }
    else if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseRightDown = true;
        }
        else if (state == GLUT_UP) {
            mouseRightDown = false;
        }
    }
    else if (button == GLUT_MIDDLE_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseMiddleDown = true;
        }
        else if (state == GLUT_UP) {
            mouseMiddleDown = false;
        }
    }
}

// Mouse motion callback function (when buttons are pressed)
void mouseMotionFunc(int x, int y) {
    // Calculate the difference from the previous position
    int deltaX = x - mouseX;
    int deltaY = y - mouseY;
    
    // Update the stored position
    mouseX = x;
    mouseY = y;
    
    // Left button: rotation around Y and X axes
    if (mouseLeftDown) {
        angle += (float)deltaX * 0.2f;
        glutPostRedisplay();
    }
    // Right button: zoom in/out
    else if (mouseRightDown) {
        // Move camera position for zoom effect
        c_pos[2] += (float)deltaY * 0.1f;
        glutPostRedisplay();
    }
    // Middle button: panning
    else if (mouseMiddleDown) {
        tx += (float)deltaX * 0.05f;
        ty -= (float)deltaY * 0.05f;  // Invert Y for intuitive panning
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {

	printf("boot\n");
	if (argc == 2) 
	{
		std::cout << "pre-readXML\n";
		if (readXML(argv[1]) != 0) return 1;
	}
	else {
		printf("Incorrect number of arguments\n");
		return 1;
	}
	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(w_size[0], w_size[1]);
	glutCreateWindow("Phase 1");

	// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);


	// put here the registration of the keyboard callbacks
	glutKeyboardFunc(keysFunc);
	glutSpecialFunc(specialKeysFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(mouseMotionFunc);


	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// enter GLUT's main cycle
	glutMainLoop();

	return 0;
}
