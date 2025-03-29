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

// Movimento de desenho
float angle = 0; // Ângulo de rotação
float tx = 0, ty = 0, tz = 0; // Translações nos eixos X, Y e Z
float mode = GL_LINE; // Modo de desenho (GL_LINE para linhas, GL_FILL para polígonos preenchidos)

// Variáveis para armazenar vértices e transformações
float* init_translate;
float* vertices;
int N; // Número de vértices
int num;

// Tamanho da janela
int w_size[2]; // Índice 0: largura, Índice 1: altura

// Valores da câmara
float c_pos[3]; // Posição da câmara
float c_lAt[3]; // Ponto para onde a câmara olha
float c_lUp[3]; // Vetor "up" da câmara

// Valores de perspetiva
float c_proj[3]; // Campo de visão (fov), plano próximo (near), plano distante (far)

// Variáveis para controlo da câmara
float radius, dist; // Distância da câmara ao ponto de interesse
float angleAlpha, angleBeta; // Ângulos para controlo da câmara
int cord_x = -1, cord_y = -1; // Coordenadas do rato para controlo da câmara

// Estrutura para armazenar buffers de vértices (VBO)
typedef struct buffer {
	GLuint buffers[1]; // Identificador do buffer
	int verticesCount; // Número de vértices
	struct buffer* next; // Próximo buffer 
	int pos; // Posição
}*Buffer;

// Estrutura para armazenar grupos de transformações e subgrupos
typedef struct group {
	float* translate; // Transformação de translação
	float* rotate; // Transformação de rotação
	float* scale; // Transformação de escala
	int* order; // Ordem das transformações
	Buffer b; // Buffer associado ao grupo
	struct group** sub; // Subgrupos
	int nSub; // Número de subgrupos
}*Group;

Group* group; // Array de grupos principais
int nGroup; // Número de grupos principais

// Função para ajustar o tamanho da janela
void changeSize(int w, int h) {
	// Previne divisão por zero
	if (h == 0) h = 1;

	// Calcula a proporção da janela
	float ratio = w * 1.0 / h;

	// Define a matriz de projeção como atual
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Define a viewport para a janela inteira
	glViewport(0, 0, w, h);

	// Define a perspetiva
	gluPerspective(c_proj[0], ratio, c_proj[1], c_proj[2]);

	// Retorna ao modo de matriz de vista de modelo
	glMatrixMode(GL_MODELVIEW);
}

// Função para desenhar os eixos X, Y e Z
void Axis() {
	glBegin(GL_LINES);
	// Eixo X (vermelho)
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(250000.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-250000.0f, 0.0f, 0.0f);
	// Eixo Y (verde)
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 250000.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, -250000.0f, 0.0f);
	// Eixo Z (azul)
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 250000.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, -250000.0f);
	glEnd();
}

// Função para aplicar transformações a um grupo
void transform(Group g, int flag) {
	// Aplica rotação
	if (g->rotate != NULL)
		glRotatef(g->rotate[0], g->rotate[1], g->rotate[2], g->rotate[3]);
	// Aplica escala (se a flag estiver ativa)
	if (g->scale != NULL && flag)
		glScalef(g->scale[0], g->scale[1], g->scale[2]);
	// Aplica translação
	if (g->translate != NULL)
		glTranslatef(g->translate[0], g->translate[1], g->translate[2]);
}

// Função auxiliar para renderizar um grupo e os seus subgrupos
void renderSceneAux(Group g, Group first) {
	Buffer b = g->b;
	glPushMatrix();

	// Aplica transformações ao grupo atual
	transform(g, 1);

	// Desenha o buffer associado ao grupo
	if (b != NULL) {
		glBindBuffer(GL_ARRAY_BUFFER, b->buffers[0]);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, b->verticesCount);
	}

	// Renderiza subgrupos recursivamente
	if (g->sub) {
		for (int i = 0; i < g->nSub; i++) {
			renderSceneAux(g->sub[i], g);
		}
	}

	glPopMatrix();
}

// Função principal de renderização
void renderScene(void) {
	// Limpa os buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Configura a câmara
	glLoadIdentity();
	gluLookAt(cos(angleBeta) * sin(angleAlpha) * radius, sin(angleBeta) * radius, cos(angleBeta) * cos(angleAlpha) * radius, c_lAt[0], c_lAt[1], c_lAt[2], c_lUp[0], c_lUp[1], c_lUp[2]);

	// Aplica transformações globais
	glTranslatef(tx, ty, tz);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	// Define o modo de desenho
	glPolygonMode(GL_FRONT_AND_BACK, mode);

	// Renderiza todos os grupos
	for (int i = 0; i < nGroup; i++) {
		renderSceneAux(group[i], NULL);
	}

	// Desenha os eixos
	Axis();

	// Troca os buffers
	glutSwapBuffers();
}

// Função para ler ficheiros .3d
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

// Função auxiliar para ler o XML e preencher a estrutura de grupos
void readXMLaux(Group g, XMLElement* elem) {

	const char* file3d;
	if (elem->FirstChildElement("models") != NULL) {
		XMLElement* model = elem->FirstChildElement("models")->FirstChildElement("model");
		while (model) {
			const char* file3d = model->Attribute("file");
			readFile(file3d);

			float* vertexB = (float*)malloc(sizeof(float) * N);
			memcpy(vertexB, vertices, sizeof(float) * N);

			g->b = (Buffer)malloc(sizeof(buffer));
			g->b->verticesCount = N;

			glGenBuffers(1, g->b->buffers);
			glBindBuffer(GL_ARRAY_BUFFER, g->b->buffers[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * g->b->verticesCount, vertexB, GL_STATIC_DRAW);

			model = model->NextSiblingElement("model");
		}
	} else {
		g->b = NULL;
	}


	XMLElement* TL = elem->FirstChildElement("transform")->FirstChildElement("translate");
	XMLElement* RT = elem->FirstChildElement("transform")->FirstChildElement("rotate");
	XMLElement* SC = elem->FirstChildElement("transform")->FirstChildElement("scale");


	if (TL) {
		std::cout << "readXML->trans\n";
		g->translate = (float*)malloc(sizeof(float) * 3);
		g->translate[0] = atof(TL->Attribute("x"));
		g->translate[1] = atof(TL->Attribute("y"));
		g->translate[2] = atof(TL->Attribute("z"));
	}

	else {
		g->translate = NULL;
	}

	if (RT) {
		std::cout << "readXML->rot\n";
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
		std::cout << "readXML->scale\n";
		g->scale = (float*)malloc(sizeof(float) * 3);
		g->scale[0] = atof(SC->Attribute("x"));
		g->scale[1] = atof(SC->Attribute("y"));
		g->scale[2] = atof(SC->Attribute("z"));
	}

	else {
		g->scale = NULL;
	}


	int i = 0;
	if (elem->FirstChildElement("group")) {

		//int i = 0;
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

// Função principal para ler o XML
void readXML(char* filename)
{
	std::cout <<"readXML filename:" << filename << '\n';
	XMLDocument doc;
	const char* file3d;
	int i = 0, k;

	if (!(doc.LoadFile(filename)))
	{
		XMLElement* win = doc.FirstChildElement("world")->FirstChildElement("window");
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
		//Verifica o numero de grupos
		for (XMLElement* mod = doc.FirstChildElement("world")->FirstChildElement("group"); mod; mod = mod->NextSiblingElement()) i++;
		group = (Group*)malloc(sizeof(struct group) * i);
		nGroup = i;
		XMLElement* elem = doc.FirstChildElement("world")->FirstChildElement("group");
		for (k = 0; k < nGroup; k++) {
			group[k] = (Group)malloc(sizeof(struct group));
			readXMLaux(group[k], elem);
			elem = elem->NextSiblingElement();
			}

		//camera 
		radius = sqrt(pow(c_pos[0], 2) + pow(c_pos[1], 2) + pow(c_pos[2], 2));
		dist = sqrt(pow(c_pos[0], 2) + pow(c_pos[2], 2)); //dist é o mesmo que radius mas a coordenada y é 0
		angleAlpha = acos(c_pos[2] / dist);
		angleBeta = acos(dist / radius);
	}
	else {
		printf("ERROR: XML not found\n");
	}
}

// Função para processar eventos do teclado
void keys(unsigned char key, int x, int y) {
	switch (key)
	{
	case 's': //"Tras"
		tz += 250;
		break;
	case 'w': //"Frente"
		tz -= 250;
		break;
	case 'r': //subir
		ty += 250;
		break;
	case 'f': //descer
		ty -= 250;
		break;
	case 'd': //direita
		tx += 250;
		break;
	case 'a': //esquerda
		tx -= 250;
		break;
	case 'q': //rodar esquerda
		angle += 5;
		break;
	case 'e': //rodar direita
		angle -= 5;
		break;
	case 'i':
		radius -= 10.0f;
		break;
	case 'k':
		radius += 10.0f;
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
		break;
	}
	glutPostRedisplay(); // Adiciona isto para garantir que a cena é redesenhada após qualquer tecla ser pressionada
}

// Função para processar eventos do teclado especial
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

// Função para processar eventos do rato
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

// Função para processar movimento do rato
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

// Função principal
int main(int argc, char** argv) {
	printf("boot\n");
	// Inicializa GLUT e a janela
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1024, 1024);
	glutCreateWindow("Grupo 16");
	// Registo de callbacks necessários
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
	// Registo de callbacks do teclado
	glutKeyboardFunc(keys);
	glutSpecialFunc(sKeys);
	glutMouseFunc(mouse);
	glutMotionFunc(motion_mouse);
	// Inicializa GLEW
	#ifndef __APPLE__
		glewInit();
	#endif
	// Configurações do OpenGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	if (argc == 2){
		std::cout << "pre-readXML\n";
		readXML(argv[1]);
	}
	else {
		printf("Incorrect number of arguments\n");
		return 0;
	}
	// Entra no ciclo principal do GLUT
	glutMainLoop();
	free(vertices);
	return 1;
}
