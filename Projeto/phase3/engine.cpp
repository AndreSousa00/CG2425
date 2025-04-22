#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <functional>

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
float* vertices = nullptr;
int N = 0; // Número de vértices
int num = 0;

// Tamanho da janela
int w_size[2] = {800, 600}; // Valores padrão: largura = 800, altura = 600

// Valores da câmara
float c_pos[3] = {0, 0, 5}; // Posição da câmara (padrão)
float c_lAt[3] = {0, 0, 0}; // Ponto para onde a câmara olha (padrão)
float c_lUp[3] = {0, 1, 0}; // Vetor "up" da câmara (padrão)

// Valores de perspetiva
float c_proj[3] = {60, 1, 1000}; // Campo de visão (fov), plano próximo (near), plano distante (far) (padrão)

// Variáveis para controlo da câmara
float radius = 5.0f, dist = 5.0f; // Distância da câmara ao ponto de interesse
float angleAlpha = 0.0f, angleBeta = 0.0f; // Ângulos para controlo da câmara
int cord_x = -1, cord_y = -1; // Coordenadas do rato para controlo da câmara

// Variável para controlar o tempo
int lastTime = 0;

// Estrutura para armazenar ponto 3D para as curvas
typedef struct point3d {
    float x, y, z;
} Point3D;

// Estrutura para armazenar buffers de vértices (VBO)
typedef struct buffer {
    GLuint buffers[1]; // Identificador do buffer
    int verticesCount; // Número de vértices
    struct buffer* next; // Próximo buffer 
    int pos; // Posição
}*Buffer;

// Estrutura para armazenar transformação de translação com curva
typedef struct translate_curve {
    float time;        // Tempo total para percorrer a curva
    bool align;        // Se o objeto deve ser alinhado com a curva
    std::vector<Point3D> points; // Pontos da curva
} TranslateCurve;

// Estrutura para armazenar transformação de rotação com tempo
typedef struct rotate_time {
    float time;        // Tempo para completar uma rotação de 360 graus
    float x, y, z;     // Eixo de rotação
} RotateTime;

// Estrutura para armazenar grupos de transformações e subgrupos
typedef struct group {
    float* translate;             // Transformação de translação estática
    TranslateCurve* translate_curve; // Transformação de translação animada
    float* rotate;                // Transformação de rotação estática
    RotateTime* rotate_time;      // Transformação de rotação animada
    float* scale;                 // Transformação de escala
    int* order;                   // Ordem das transformações
    Buffer b;                     // Buffer associado ao grupo
    struct group** sub;           // Subgrupos
    int nSub;                     // Número de subgrupos
}*Group;

Group* group = nullptr; // Array de grupos principais
int nGroup = 0; // Número de grupos principais

// Constante para valores pequenos
const float EPSILON = 0.0001f;

// Função para interpolar um ponto em uma curva de Catmull-Rom
void getCatmullRomPoint(float t, Point3D p0, Point3D p1, Point3D p2, Point3D p3, float* pos, float* deriv) {
    // Cálculo otimizado para Catmull-Rom
    // Coeficientes para posição
    float t2 = t * t;
    float t3 = t2 * t;
    
    // Posição
    // X
    pos[0] = 0.5f * ((-t3 + 2*t2 - t) * p0.x + 
                     (3*t3 - 5*t2 + 2) * p1.x + 
                     (-3*t3 + 4*t2 + t) * p2.x + 
                     (t3 - t2) * p3.x);
    
    // Y
    pos[1] = 0.5f * ((-t3 + 2*t2 - t) * p0.y + 
                     (3*t3 - 5*t2 + 2) * p1.y + 
                     (-3*t3 + 4*t2 + t) * p2.y + 
                     (t3 - t2) * p3.y);
    
    // Z
    pos[2] = 0.5f * ((-t3 + 2*t2 - t) * p0.z + 
                     (3*t3 - 5*t2 + 2) * p1.z + 
                     (-3*t3 + 4*t2 + t) * p2.z + 
                     (t3 - t2) * p3.z);
    
    // Derivada
    // X
    deriv[0] = 0.5f * ((-3*t2 + 4*t - 1) * p0.x + 
                       (9*t2 - 10*t) * p1.x + 
                       (-9*t2 + 8*t + 1) * p2.x + 
                       (3*t2 - 2*t) * p3.x);
    
    // Y
    deriv[1] = 0.5f * ((-3*t2 + 4*t - 1) * p0.y + 
                       (9*t2 - 10*t) * p1.y + 
                       (-9*t2 + 8*t + 1) * p2.y + 
                       (3*t2 - 2*t) * p3.y);
    
    // Z
    deriv[2] = 0.5f * ((-3*t2 + 4*t - 1) * p0.z + 
                       (9*t2 - 10*t) * p1.z + 
                       (-9*t2 + 8*t + 1) * p2.z + 
                       (3*t2 - 2*t) * p3.z);
}

// Função para determinar o ponto global na curva
void getGlobalCatmullRomPoint(float gt, const std::vector<Point3D>& points, float* pos, float* deriv) {
    if (points.size() < 4) {
        // Precisamos de pelo menos 4 pontos para Catmull-Rom
        std::cerr << "ERROR: Catmull-Rom curve requires at least 4 points!" << std::endl;
        
        // Coloca valores padrão nos vetores de saída
        pos[0] = pos[1] = pos[2] = 0.0f;
        deriv[0] = deriv[1] = 0.0f;
        deriv[2] = 1.0f; // Direção Z por padrão
        
        return;
    }
    
    int point_count = points.size();
    float t = gt * point_count; // valor de t global [0, point_count]
    int index = floor(t);  // índice do segmento
    t = t - index; // valor de t local [0, 1]
    
    // Índices dos pontos de controle
    int indices[4];
    indices[0] = (index + point_count - 1) % point_count;
    indices[1] = (indices[0] + 1) % point_count;
    indices[2] = (indices[1] + 1) % point_count;
    indices[3] = (indices[2] + 1) % point_count;
    
    // Obter ponto pela função de Catmull-Rom
    getCatmullRomPoint(t, points[indices[0]], points[indices[1]], 
                      points[indices[2]], points[indices[3]], pos, deriv);
}

// Função para normalizar um vetor 3D
void normalizeVector(float* v) {
    float len = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (len > EPSILON) {
        v[0] /= len;
        v[1] /= len;
        v[2] /= len;
    }
}

// Função para calcular o produto vetorial
void crossProduct(float* a, float* b, float* result) {
    result[0] = a[1]*b[2] - a[2]*b[1];
    result[1] = a[2]*b[0] - a[0]*b[2];
    result[2] = a[0]*b[1] - a[1]*b[0];
}

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
    if (!g) return;
    
    // Obtém o tempo atual em milissegundos
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    
    // Para o OpenGL, que usa ordem de multiplicação à direita, aplicamos as transformações
    // na ordem inversa para o sistema de coordenadas
    
    // 1. Escala (se a flag estiver ativa e existir escala)
    if (g->scale != NULL && flag) {
        glScalef(g->scale[0], g->scale[1], g->scale[2]);
    }
    
    // 2. Rotação do planeta (rotação sobre si mesmo)
    if (g->rotate != NULL) {
        glRotatef(g->rotate[0], g->rotate[1], g->rotate[2], g->rotate[3]);
    }
    
    // 3. Rotação baseada em tempo (rotação em torno de si próprio)
    if (g->rotate_time != NULL) {
        // Reduzindo a velocidade de rotação aplicando um fator de desaceleração
        // Multiplique o tempo atual por um fator menor para rotação mais lenta
        float timeSlowFactor = 0.1f; // Ajuste este valor para controlar a velocidade (menor = mais lento)
        float adjustedTime = (currentTime / 1000.0f) * timeSlowFactor;
        
        // Calcular o ângulo com base no tempo ajustado
        float angle_time = fmod(adjustedTime, g->rotate_time->time) / g->rotate_time->time * 360.0f;
        
        glRotatef(angle_time, g->rotate_time->x, g->rotate_time->y, g->rotate_time->z);
    }
    
    // 4. Translação estática (posição do planeta)
    if (g->translate != NULL) {
        glTranslatef(g->translate[0], g->translate[1], g->translate[2]);
    }
    
    // 5. Translação baseada em curva (se existir)
    if (g->translate_curve != NULL && g->translate_curve->points.size() >= 4) {
        // Também desaceleramos o movimento em curva
        float timeSlowFactor = 0.1f; // Use o mesmo fator da rotação para consistência
        float adjustedTime = (currentTime / 1000.0f) * timeSlowFactor;
        
        float pos[3], deriv[3];
        float gt = fmod(adjustedTime, g->translate_curve->time) / g->translate_curve->time;
        
        getGlobalCatmullRomPoint(gt, g->translate_curve->points, pos, deriv);
        
        glTranslatef(pos[0], pos[1], pos[2]);
        
        // Se alinhamento estiver ativado, alinhar objeto com a tangente da curva
        if (g->translate_curve->align) {
            float x[3], y[3], z[3];
            
            // Normalizar vetor tangente para ter direção Z
            z[0] = deriv[0];
            z[1] = deriv[1];
            z[2] = deriv[2];
            
            float len = sqrt(z[0]*z[0] + z[1]*z[1] + z[2]*z[2]);
            if (len > EPSILON) {
                z[0] /= len;
                z[1] /= len;
                z[2] /= len;
            } else {
                // Caso degenerado, usar um vetor Z padrão
                z[0] = 0.0f;
                z[1] = 0.0f;
                z[2] = 1.0f;
            }
            
            // Escolha de um vetor up alternativo se o Z for quase vertical
            float up[3] = {0.0f, 1.0f, 0.0f}; // vetor up padrão
            if (fabs(z[1]) > 0.99f) {
                // Se Z estiver muito próximo do eixo Y, usar X como up temporário
                up[0] = 1.0f; up[1] = 0.0f; up[2] = 0.0f;
            }
            
            // Produto vetorial entre vetor up e Z para obter X
            crossProduct(up, z, x);
            normalizeVector(x);
            
            // Produto vetorial entre Z e X para obter Y
            crossProduct(z, x, y);
            normalizeVector(y);
            
            // Matriz de rotação para alinhar com a curva
            float m[16] = {
                x[0], x[1], x[2], 0,
                y[0], y[1], y[2], 0,
                z[0], z[1], z[2], 0,
                0,    0,    0,    1
            };
            
            glMultMatrixf(m);
        }
    }
}

// Função auxiliar para renderizar um grupo e os seus subgrupos
void renderSceneAux(Group g, Group first) {
    if (g == nullptr) return;
    
    glPushMatrix();  // Salva o estado atual da matriz

    // Aplica transformações ao grupo atual
    transform(g, 1);

    // Desenha o buffer associado ao grupo
    Buffer b = g->b;
    if (b != nullptr) {
        glBindBuffer(GL_ARRAY_BUFFER, b->buffers[0]);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, b->verticesCount);
    }

    // Renderiza subgrupos recursivamente
    if (g->sub && g->nSub > 0) {
        std::cout << "Rendering " << g->nSub << " subgroups" << std::endl;
        for (int i = 0; i < g->nSub; i++) {
            if (g->sub[i] != nullptr) {
                std::cout << "Rendering subgroup " << i << std::endl;
                renderSceneAux(g->sub[i], g);
            }
        }
    }

    glPopMatrix();  // Restaura o estado da matriz antes das transformações
}


// Função principal de renderização
void renderScene(void) {
    // Limpa os buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configura a câmara
    glLoadIdentity();
    gluLookAt(cos(angleBeta) * sin(angleAlpha) * radius, sin(angleBeta) * radius, cos(angleBeta) * cos(angleAlpha) * radius, 
             c_lAt[0], c_lAt[1], c_lAt[2], 
             c_lUp[0], c_lUp[1], c_lUp[2]);

    // Aplica transformações globais
    glTranslatef(tx, ty, tz);
    glRotatef(angle, 0.0f, 1.0f, 0.0f);

    // Define o modo de desenho
    glPolygonMode(GL_FRONT_AND_BACK, mode);

    // Renderiza todos os grupos
    for (int i = 0; i < nGroup; i++) {
        if (group[i] != nullptr) {
            renderSceneAux(group[i], NULL);
        }
    }

    // Desenha os eixos
    Axis();

    // Atualiza o tempo
    lastTime = glutGet(GLUT_ELAPSED_TIME);

    // Troca os buffers
    glutSwapBuffers();
    
    // Força o redesenho para animação contínua
    glutPostRedisplay();
}

// Função para ler ficheiros .3d
void readFile(const char* filename) {
    if (!filename) {
        printf("ERROR: Invalid filename (null)\n");
        return;
    }
    
    FILE* f;
    std::cout << "readFile filename:" << filename << '\n';
    f = fopen(filename, "r");
    
    if (!f) {
        printf("ERROR: cannot find .3d file: %s\n", filename);
        // Tenta criar o diretório se não existir
        #ifdef _WIN32
        std::string cmd = "mkdir solarSystem 2>nul";
        #else
        std::string cmd = "mkdir -p solarSystem";
        #endif
        system(cmd.c_str());
        
        // Tenta novamente após criar o diretório
        f = fopen(filename, "r");
        if (!f) {
            return;
        }
    }
    
    // Liberta memória anterior se existir
    if (vertices != nullptr) {
        free(vertices);
        vertices = nullptr;
    }
    
    char* n = (char*)malloc(sizeof(char) * 20);
    if (!n) {
        printf("ERROR: Memory allocation failed\n");
        fclose(f);
        return;
    }
    n[0] = 0;
    
    char* currElem = (char*)malloc(sizeof(char) * 20);
    if (!currElem) {
        printf("ERROR: Memory allocation failed\n");
        free(n);
        fclose(f);
        return;
    }
    currElem[0] = 0;
    
    char c;
    int i = 0;
    int flag = 1;

    while ((c = fgetc(f)) != EOF) {
        if (flag) {
            if (c == '\n') {
                num = atoi(n);
                if (num <= 0) {
                    printf("ERROR: Invalid number of vertices in file: %s\n", filename);
                    free(n);
                    free(currElem);
                    fclose(f);
                    return;
                }
                vertices = (float*)malloc(sizeof(float) * num);
                if (!vertices) {
                    printf("ERROR: Memory allocation failed for vertices\n");
                    free(n);
                    free(currElem);
                    fclose(f);
                    return;
                }
                flag = 0;
            }
            else strncat(n, &c, 1);
        }
        else {
            if (c == ' ' || c == '\n') {
                if (strlen(currElem) > 0) {
                    float elem = atof(currElem);
                    if (i < num) {
                        vertices[i++] = elem;
                    }
                    currElem[0] = 0;
                }
            }
            else strncat(currElem, &c, 1);
        }
    }
    
    // Adiciona o último número se não terminar com espaço ou nova linha
    if (strlen(currElem) > 0 && i < num) {
        vertices[i++] = atof(currElem);
    }
    
    N = i;
    
    free(n);
    free(currElem);
    fclose(f);
    
    if (N != num) {
        printf("WARNING: Expected %d vertices, but read %d in file: %s\n", num, N, filename);
    }
}

// Função auxiliar para ler o XML e preencher a estrutura de grupos
void readXMLaux(Group g, XMLElement* elem) {
    if (!g || !elem) {
        std::cerr << "ERROR: Invalid group or element in readXMLaux" << std::endl;
        return;
    }
    
    // Inicialização explícita de todos os campos
    g->translate = nullptr;
    g->translate_curve = nullptr;
    g->rotate = nullptr;
    g->rotate_time = nullptr;
    g->scale = nullptr;
    g->order = nullptr;
    g->b = nullptr;
    g->sub = nullptr;
    g->nSub = 0;
    
    // Processar os models (arquivos 3D)
    XMLElement* modelsElem = elem->FirstChildElement("models");
    if (modelsElem) {
        XMLElement* modelElem = modelsElem->FirstChildElement("model");
        while (modelElem) {
            const char* file3d = modelElem->Attribute("file");
            if (file3d) {
                std::cout << "Loading model: " << file3d << std::endl;
                readFile(file3d);
                
                if (vertices != nullptr && N > 0) {
                    float* vertexB = (float*)malloc(sizeof(float) * N);
                    if (!vertexB) {
                        std::cerr << "ERROR: Memory allocation failed for vertex buffer" << std::endl;
                    } else {
                        memcpy(vertexB, vertices, sizeof(float) * N);
                        
                        Buffer buffer = (Buffer)malloc(sizeof(struct buffer));
                        if (!buffer) {
                            std::cerr << "ERROR: Memory allocation failed for buffer" << std::endl;
                            free(vertexB);
                        } else {
                            buffer->verticesCount = N;
                            buffer->next = nullptr;
                            buffer->pos = 0;
                            
                            glGenBuffers(1, buffer->buffers);
                            glBindBuffer(GL_ARRAY_BUFFER, buffer->buffers[0]);
                            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * N, vertexB, GL_STATIC_DRAW);
                            
                            // Assign buffer to group
                            g->b = buffer;
                            
                            free(vertexB);
                            std::cout << "Buffer created for model: " << file3d << " with " << N << " vertices" << std::endl;
                        }
                    }
                    
                    free(vertices);
                    vertices = nullptr;
                }
            } else {
                std::cerr << "WARNING: Model without 'file' attribute" << std::endl;
            }
            
            modelElem = modelElem->NextSiblingElement("model");
        }
    }
    
    // Processar transformações
    XMLElement* transformElem = elem->FirstChildElement("transform");
    if (transformElem) {
        // Processar translação
        XMLElement* translateElem = transformElem->FirstChildElement("translate");
        while (translateElem) {
            if (translateElem->Attribute("time")) {
                // Translação baseada em curva Catmull-Rom
                float time = translateElem->FloatAttribute("time");
                bool align = false;
                if (translateElem->Attribute("align")) {
                    const char* alignAttr = translateElem->Attribute("align");
                    align = (strcmp(alignAttr, "true") == 0 || strcmp(alignAttr, "True") == 0);
                }
                
                g->translate_curve = new TranslateCurve();
                if (!g->translate_curve) {
                    std::cerr << "ERROR: Memory allocation failed for translate_curve" << std::endl;
                } else {
                    g->translate_curve->time = time;
                    g->translate_curve->align = align;
                    
                    // Ler os pontos
                    XMLElement* pointElem = translateElem->FirstChildElement("point");
                    while (pointElem) {
                        Point3D point;
                        point.x = pointElem->FloatAttribute("x");
                        point.y = pointElem->FloatAttribute("y");
                        point.z = pointElem->FloatAttribute("z");
                        
                        g->translate_curve->points.push_back(point);
                        pointElem = pointElem->NextSiblingElement("point");
                    }
                    
                    if (g->translate_curve->points.size() < 4) {
                        std::cerr << "ERROR: Catmull-Rom curve requires at least 4 points!" << std::endl;
                        delete g->translate_curve;
                        g->translate_curve = nullptr;
                    } else {
                        std::cout << "Created Catmull-Rom curve with " << g->translate_curve->points.size() << " points" << std::endl;
                    }
                }
            } else {
                // Translação estática
                g->translate = (float*)malloc(sizeof(float) * 3);
                if (!g->translate) {
                    std::cerr << "ERROR: Memory allocation failed for translate" << std::endl;
                } else {
                    g->translate[0] = translateElem->FloatAttribute("x");
                    g->translate[1] = translateElem->FloatAttribute("y");
                    g->translate[2] = translateElem->FloatAttribute("z");
                    std::cout << "Static translation: [" << g->translate[0] << ", " << g->translate[1] << ", " << g->translate[2] << "]" << std::endl;
                }
            }
            
            translateElem = translateElem->NextSiblingElement("translate");
        }
        
        // Processar rotação
        XMLElement* rotateElem = transformElem->FirstChildElement("rotate");
        while (rotateElem) {
            if (rotateElem->Attribute("time")) {
                // Rotação baseada em tempo
                g->rotate_time = new RotateTime();
                if (!g->rotate_time) {
                    std::cerr << "ERROR: Memory allocation failed for rotate_time" << std::endl;
                } else {
                    g->rotate_time->time = rotateElem->FloatAttribute("time");
                    g->rotate_time->x = rotateElem->FloatAttribute("x");
                    g->rotate_time->y = rotateElem->FloatAttribute("y");
                    g->rotate_time->z = rotateElem->FloatAttribute("z");
                    std::cout << "Time-based rotation: time=" << g->rotate_time->time << ", axis=[" 
                              << g->rotate_time->x << ", " << g->rotate_time->y << ", " << g->rotate_time->z << "]" << std::endl;
                }
            } else {
                // Rotação estática
                g->rotate = (float*)malloc(sizeof(float) * 4);
                if (!g->rotate) {
                    std::cerr << "ERROR: Memory allocation failed for rotate" << std::endl;
                } else {
                    g->rotate[0] = rotateElem->FloatAttribute("angle");
                    g->rotate[1] = rotateElem->FloatAttribute("x");
                    g->rotate[2] = rotateElem->FloatAttribute("y");
                    g->rotate[3] = rotateElem->FloatAttribute("z");
                    std::cout << "Static rotation: angle=" << g->rotate[0] << ", axis=[" 
                              << g->rotate[1] << ", " << g->rotate[2] << ", " << g->rotate[3] << "]" << std::endl;
                }
            }
            
            rotateElem = rotateElem->NextSiblingElement("rotate");
        }
        
        // Processar escala
        XMLElement* scaleElem = transformElem->FirstChildElement("scale");
        if (scaleElem) {
            g->scale = (float*)malloc(sizeof(float) * 3);
            if (!g->scale) {
                std::cerr << "ERROR: Memory allocation failed for scale" << std::endl;
            } else {
                g->scale[0] = scaleElem->FloatAttribute("x");
                g->scale[1] = scaleElem->FloatAttribute("y");
                g->scale[2] = scaleElem->FloatAttribute("z");
                std::cout << "Scale: [" << g->scale[0] << ", " << g->scale[1] << ", " << g->scale[2] << "]" << std::endl;
            }
        }
    }
    
    // Processar subgrupos - primeiro contar quantos existem
    int subgroupCount = 0;
    for (XMLElement* subgroupElem = elem->FirstChildElement("group"); 
         subgroupElem != nullptr; 
         subgroupElem = subgroupElem->NextSiblingElement("group")) {
        subgroupCount++;
    }
    
    // Se há subgrupos, alocar memória e processá-los
    if (subgroupCount > 0) {
        std::cout << "Found " << subgroupCount << " subgroups" << std::endl;
        
        g->sub = (Group*)malloc(sizeof(Group) * subgroupCount);
        if (!g->sub) {
            std::cerr << "ERROR: Memory allocation failed for subgroups array" << std::endl;
        } else {
            g->nSub = subgroupCount;
            
            int index = 0;
            XMLElement* subgroupElem = elem->FirstChildElement("group");
            
            while (subgroupElem && index < subgroupCount) {
                g->sub[index] = (Group)malloc(sizeof(struct group));
                if (!g->sub[index]) {
                    std::cerr << "ERROR: Memory allocation failed for subgroup " << index << std::endl;
                } else {
                    std::cout << "Processing subgroup " << index << std::endl;
                    // Chamada recursiva para processar o subgrupo
                    readXMLaux(g->sub[index], subgroupElem);
                }
                
                subgroupElem = subgroupElem->NextSiblingElement("group");
                index++;
            }
            
            if (index != subgroupCount) {
                std::cerr << "WARNING: Expected " << subgroupCount << " subgroups but processed " << index << std::endl;
            }
        }
    }
}

// Função principal para ler o XML
void readXML(char* filename) {
    if (!filename) {
        std::cerr << "ERROR: Invalid XML filename (null)" << std::endl;
        return;
    }
    
    std::cout << "Reading XML file: " << filename << std::endl;
    
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.LoadFile(filename);
    
    if (err != tinyxml2::XML_SUCCESS) {
        std::cerr << "ERROR: Failed to load XML file: " << filename << std::endl;
        std::cerr << "Error code: " << err << std::endl;
        return;
    }
    
    tinyxml2::XMLElement* worldElem = doc.FirstChildElement("world");
    if (!worldElem) {
        std::cerr << "ERROR: Invalid XML format - missing 'world' element" << std::endl;
        return;
    }
    
    // Processar configurações da janela
    tinyxml2::XMLElement* windowElem = worldElem->FirstChildElement("window");
    if (windowElem) {
        w_size[0] = windowElem->IntAttribute("width", 800);
        w_size[1] = windowElem->IntAttribute("height", 600);
        std::cout << "Window size: " << w_size[0] << "x" << w_size[1] << std::endl;
    } else {
        w_size[0] = 800;
        w_size[1] = 600;
        std::cout << "Using default window size: 800x600" << std::endl;
    }
    
    // Processar configurações da câmera
    tinyxml2::XMLElement* cameraElem = worldElem->FirstChildElement("camera");
    if (cameraElem) {
        tinyxml2::XMLElement* posElem = cameraElem->FirstChildElement("position");
        if (posElem) {
            c_pos[0] = posElem->FloatAttribute("x", 0.0f);
            c_pos[1] = posElem->FloatAttribute("y", 0.0f);
            c_pos[2] = posElem->FloatAttribute("z", 5.0f);
            std::cout << "Camera position: [" << c_pos[0] << ", " << c_pos[1] << ", " << c_pos[2] << "]" << std::endl;
        }
        
        tinyxml2::XMLElement* lookAtElem = cameraElem->FirstChildElement("lookAt");
        if (lookAtElem) {
            c_lAt[0] = lookAtElem->FloatAttribute("x", 0.0f);
            c_lAt[1] = lookAtElem->FloatAttribute("y", 0.0f);
            c_lAt[2] = lookAtElem->FloatAttribute("z", 0.0f);
            std::cout << "Camera lookAt: [" << c_lAt[0] << ", " << c_lAt[1] << ", " << c_lAt[2] << "]" << std::endl;
        }
        
        tinyxml2::XMLElement* upElem = cameraElem->FirstChildElement("up");
        if (upElem) {
            c_lUp[0] = upElem->FloatAttribute("x", 0.0f);
            c_lUp[1] = upElem->FloatAttribute("y", 1.0f);
            c_lUp[2] = upElem->FloatAttribute("z", 0.0f);
            std::cout << "Camera up: [" << c_lUp[0] << ", " << c_lUp[1] << ", " << c_lUp[2] << "]" << std::endl;
        }
        
        tinyxml2::XMLElement* projElem = cameraElem->FirstChildElement("projection");
        if (projElem) {
            c_proj[0] = projElem->FloatAttribute("fov", 60.0f);
            c_proj[1] = projElem->FloatAttribute("near", 1.0f);
            c_proj[2] = projElem->FloatAttribute("far", 1000.0f);
            std::cout << "Projection: fov=" << c_proj[0] << ", near=" << c_proj[1] << ", far=" << c_proj[2] << std::endl;
        }
    } else {
        // Valores padrão
        c_pos[0] = 0.0f; c_pos[1] = 0.0f; c_pos[2] = 5.0f;
        c_lAt[0] = 0.0f; c_lAt[1] = 0.0f; c_lAt[2] = 0.0f;
        c_lUp[0] = 0.0f; c_lUp[1] = 1.0f; c_lUp[2] = 0.0f;
        c_proj[0] = 60.0f; c_proj[1] = 1.0f; c_proj[2] = 1000.0f;
        std::cout << "Using default camera settings" << std::endl;
    }
    
    // Configuração da câmera
    radius = sqrt(pow(c_pos[0], 2) + pow(c_pos[1], 2) + pow(c_pos[2], 2));
    dist = sqrt(pow(c_pos[0], 2) + pow(c_pos[2], 2));
    
    if (dist > EPSILON) {
        angleAlpha = acos(c_pos[2] / dist);
        if (c_pos[0] < 0) angleAlpha = -angleAlpha;
    } else {
        angleAlpha = 0;
    }
    
    if (radius > EPSILON) {
        angleBeta = asin(c_pos[1] / radius);
    } else {
        angleBeta = 0;
    }
    
    // Liberar memória anterior se existir
    if (group != nullptr) {
        // Aqui deveria ter uma função para liberar recursivamente os grupos
        // mas vamos simplificar
        free(group);
        group = nullptr;
    }
    
    // Contar grupos principais
    int groupCount = 0;
    for (tinyxml2::XMLElement* groupElem = worldElem->FirstChildElement("group");
         groupElem != nullptr;
         groupElem = groupElem->NextSiblingElement("group")) {
        groupCount++;
    }
    
    if (groupCount == 0) {
        std::cerr << "ERROR: No groups found in XML" << std::endl;
        return;
    }
    
    std::cout << "Found " << groupCount << " main groups" << std::endl;
    
    // Alocar memória para os grupos principais
    group = (Group*)malloc(sizeof(Group) * groupCount);
    if (!group) {
        std::cerr << "ERROR: Memory allocation failed for main groups" << std::endl;
        return;
    }
    
    nGroup = groupCount;
    
    // Processar grupos principais
    int index = 0;
    tinyxml2::XMLElement* groupElem = worldElem->FirstChildElement("group");
    
    while (groupElem && index < groupCount) {
        group[index] = (Group)malloc(sizeof(struct group));
        if (!group[index]) {
            std::cerr << "ERROR: Memory allocation failed for main group " << index << std::endl;
        } else {
            std::cout << "Processing main group " << index << std::endl;
            readXMLaux(group[index], groupElem);
        }
        
        groupElem = groupElem->NextSiblingElement("group");
        index++;
    }
    
    std::cout << "XML parsing complete" << std::endl;
}

// Função para processar eventos do teclado
void keys(unsigned char key, int x, int y) {
    switch (key)
    {
    case 's': //"Tras"
        tz += 2.5;
        break;
    case 'w': //"Frente"
        tz -= 2.5;
        break;
    case 'r': //subir
        ty += 2.5;
        break;
    case 'f': //descer
        ty -= 2.5;
        break;
    case 'd': //direita
        tx += 2.5;
        break;
    case 'a': //esquerda
        tx -= 2.5;
        break;
    case 'q': //rodar esquerda
        angle += 5;
        break;
    case 'e': //rodar direita
        angle -= 5;
        break;
    case 'i':
        radius -= 2.0f;
        if (radius < 1.0f) radius = 1.0f;
        break;
    case 'k':
        radius += 2.0f;
        break;
    case 'o':
        radius -= 0.1f;
        if (radius < 1.0f) radius = 1.0f;
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
    glutPostRedisplay();
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
        if (angleBeta < M_PI / 2 - 0.1f)
            angleBeta += 0.07f;
        break;
    case GLUT_KEY_DOWN:
        if (angleBeta > -M_PI / 2 + 0.1f)
            angleBeta -= 0.07f;
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
        angleAlpha = (x - cord_x) * 0.01f;
        if (angleBeta < M_PI / 2) {
            angleBeta = (cord_y - y) * 0.01f;
        }
        if (angleBeta > -M_PI / 2) {
            angleBeta = (cord_y - y) * 0.01f;
        }
    }

    glutPostRedisplay();
}

// Função para limpar a memória alocada
void cleanup() {
    // Libera a memória dos vértices
    if (vertices != NULL) {
        free(vertices);
    }
    
    // Função recursiva para liberar a memória dos grupos
    std::function<void(Group)> freeGroup = [&freeGroup](Group g) {
        if (g == NULL) return;
        
        // Libera buffers
        if (g->b != NULL) {
            if (g->b->buffers[0] != 0) {
                glDeleteBuffers(1, g->b->buffers);
            }
            free(g->b);
        }
        
        // Libera transformações
        if (g->translate != NULL) free(g->translate);
        if (g->translate_curve != NULL) delete g->translate_curve;
        if (g->rotate != NULL) free(g->rotate);
        if (g->rotate_time != NULL) delete g->rotate_time;
        if (g->scale != NULL) free(g->scale);
        if (g->order != NULL) free(g->order);
        
        // Libera grupos filhos recursivamente
        if (g->sub != NULL) {
            for (int i = 0; i < g->nSub; i++) {
                freeGroup(g->sub[i]);
            }
            free(g->sub);
        }
        
        free(g);
    };
    
    // Libera todos os grupos principais
    if (group != NULL) {
        for (int i = 0; i < nGroup; i++) {
            freeGroup(group[i]);
        }
        free(group);
    }
}

// Função principal
int main(int argc, char** argv) {
    printf("boot\n");
    // Inicializa GLUT e a janela
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow("Grupo 16 - Phase 3");
    
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
    
    // Cria o diretório para os modelos do sistema solar se não existir
    #ifdef _WIN32
    system("mkdir solarSystem 2>nul");
    #else
    system("mkdir -p solarSystem");
    #endif
    
    if (argc == 2) {
        std::cout << "pre-readXML\n";
        readXML(argv[1]);
    }
    else {
        printf("Incorrect number of arguments\n");
        printf("Usage: engine <xml_file>\n");
        return 0;
    }
    
    // Entra no ciclo principal do GLUT
    glutMainLoop();
    
    // Limpa a memória antes de sair
    cleanup();
    
    return 1;
}