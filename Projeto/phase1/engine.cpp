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

//Movimento de desenho
float angle = 0;
float tx = 0;
float ty = 0;
float tz = 0;

float mode = GL_LINE;

int numPrimitives;// O número de primitivas a serem desenhadas (quantos ficheiros .3d foram carregados)
float** all_vertices;
float* vertices;
int N;// número de vértices
int num;

//Variáveis globais para interação com o rato
bool mouseLeftDown = false;
bool mouseRightDown = false;
bool mouseMiddleDown = false;
int mouseX = 0, mouseY = 0;

/*
    Tamanho da janela
    @param 0 - largura
    @param 1 - altura
*/
int w_size[2];
// Valores da câmara
float c_pos[3];
float c_lAt[3];	
float c_lUp[3];
// Valores da perspetiva
float c_proj[3]; 

float radius, dist;//dist é o mesmo que radius mas a coordenada y é 0
float angleAlpha, angleBeta;


void changeSize(int w, int h) {

	// Prevenir uma divisão por zero, quando a janela é muito curta (não se pode fazer uma janela com largura zero).
	if (h == 0)
		h = 1;

	// Calcular o aspect ratio da janela
	float ratio = w * 1.0 / h;

	// Definir a matriz de projeção como atual
	glMatrixMode(GL_PROJECTION);
	// Carregar a Matriz Identidade
	glLoadIdentity();

	// Definir o viewport para ser a janela inteira
	glViewport(0, 0, w, h);

	// Definir a perspetiva
	gluPerspective(c_proj[0], ratio, c_proj[1], c_proj[2]);

	// Voltar ao modo de matriz de visualização do modelo
	glMatrixMode(GL_MODELVIEW);
}

void Axis() {
	glBegin(GL_LINES);
	// Eixo X em vermelho
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 0.0f, 0.0f);
	// Eixo Y em verde
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 100.0f, 0.0f);
	// Eixo Z em azul
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();
}

void renderScene(void) {

	// limpar buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// definir a câmara
	glLoadIdentity();
	gluLookAt(	c_pos[0], c_pos[1], c_pos[2],
				c_lAt[0], c_lAt[1], c_lAt[2],
				c_lUp[0], c_lUp[1], c_lUp[2]);
	Axis();

	// colocar as transformações geométricas aqui
		
	glTranslated(tx, ty, tz);
	glRotated(angle, 0, 1, 0);
	glPolygonMode(GL_FRONT_AND_BACK, mode);

	for (int i = 0; i < numPrimitives; i++) {

		for (int j = 1; j < (int)all_vertices[i][0]; j += 9) { // Começa em 1, porque all_vertices[i][0] contém o número de vértices
			glColor3f(0.729f, 0.333f, 0.827f); // Fúcsia Escuro
			glBegin(GL_TRIANGLES);
			glVertex3f(all_vertices[i][j], all_vertices[i][j + 1], all_vertices[i][j + 2]);
			glVertex3f(all_vertices[i][j + 3], all_vertices[i][j + 4], all_vertices[i][j + 5]);
			glVertex3f(all_vertices[i][j + 6], all_vertices[i][j + 7], all_vertices[i][j + 8]);
			glEnd();

			j += 9;
			glColor3f(1.000f, 0.271f, 0.000f); // Laranja Vermelho
			glBegin(GL_TRIANGLES);
			glVertex3f(all_vertices[i][j], all_vertices[i][j + 1], all_vertices[i][j + 2]);
			glVertex3f(all_vertices[i][j + 3], all_vertices[i][j + 4], all_vertices[i][j + 5]);
			glVertex3f(all_vertices[i][j + 6], all_vertices[i][j + 7], all_vertices[i][j + 8]);
			glEnd();
		}
	}

	


	// Fim do frame
	glutSwapBuffers();
}



int readFile(const char* filename) {
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
		// Verifica o número de .3d a carregar
		for (XMLElement* mod = doc.FirstChildElement("world")->FirstChildElement("group")->FirstChildElement("models")->FirstChildElement("model"); mod; mod = mod->NextSiblingElement()) {
			if (j = k) {
				k = k * 2;
				all_vertices = (float**)malloc(sizeof(float) * k);
			}
			j++;

		}

		int i = 0;
		// Carrega todos os ficheiros .3d
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


// escrever função para processar eventos de teclado
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
    case '=': // Permitir tecla = (mesma tecla física que +)
        c_pos[2] -= 1.0f; // Zoom in - mover câmara para mais perto
        break;
    case '-': 
    case '_': // Permitir tecla _ (mesma tecla física que -)
        c_pos[2] += 1.0f; // Zoom out - mover câmara para mais longe
        break;
    }
	glutPostRedisplay();
}

// escrever função para processar eventos de teclado especiais
void specialKeysFunc(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_RIGHT: angle += 5; break;
	case GLUT_KEY_LEFT: angle -= 5; break;
	case GLUT_KEY_UP: tx += 0.5; break;
	case GLUT_KEY_DOWN: tx -= 0.5; break;
	}
	glutPostRedisplay();
}

// Função de callback para botões do rato
void mouseFunc(int button, int state, int x, int y) {
    // Atualizar a posição atual do rato
    mouseX = x;
    mouseY = y;
    
    // Lidar com o pressionar/libertar do botão
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

// Função de callback para movimento do rato (quando os botões estão pressionados)
void mouseMotionFunc(int x, int y) {
    // Calcular a diferença em relação à posição anterior
    int deltaX = x - mouseX;
    int deltaY = y - mouseY;
    
    // Atualizar a posição armazenada
    mouseX = x;
    mouseY = y;
    
    // Botão esquerdo: rotação em torno dos eixos Y e X
    if (mouseLeftDown) {
        angle += (float)deltaX * 0.2f;
        glutPostRedisplay();
    }
    // Botão direito: zoom in/out
    else if (mouseRightDown) {
        // Mover a posição da câmara para efeito de zoom
        c_pos[2] += (float)deltaY * 0.1f;
        glutPostRedisplay();
    }
    // Botão do meio: pan
    else if (mouseMiddleDown) {
        tx += (float)deltaX * 0.05f;
        ty -= (float)deltaY * 0.05f;  // Inverter Y para pan intuitivo
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
	// iniciar GLUT e a janela
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(w_size[0], w_size[1]);
	glutCreateWindow("Phase 1");

	// Registo de callbacks necessários 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);


	// colocar aqui o registo dos callbacks do teclado
	glutKeyboardFunc(keysFunc);
	glutSpecialFunc(specialKeysFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(mouseMotionFunc);


	// configurações do OpenGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// entrar no ciclo principal do GLUT
	glutMainLoop();

	return 0;
}
