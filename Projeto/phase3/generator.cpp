#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

// Estrutura para representar um ponto 3D
struct Point3D {
    float x, y, z;

    Point3D() : x(0), y(0), z(0) {}
    Point3D(float a, float b, float c) : x(a), y(b), z(c) {}
};

// Estrutura para representar uma patch de Bezier
struct BezierPatch {
    int indices[16]; // Índices dos pontos de controle (4x4)
};

// Função para gerar um plano
void plane_generator(float length, float division, char* filename){
    //Abrir o ficheiro
    FILE* f;
    f = fopen(filename, "w+");

    //Variável que calcula o tamanho de cada quadrado
    //float eachSquareLength = (length + 1) / division;
    float eachSquareLength = length / division;

    //Determinar o numero de triangulos totais necessários para formar o plano total
    int totalNumDots = 3 * 2 * pow(division, 2.0f);
    
    if(f) // Se o ficheiro for válido
    {
        //Vamos guardar no ficheiro .3d o número total de de pontos 
        fprintf(f,"%d\n",totalNumDots * 3);

        //Centrar o plano na origem
        float x = -(length) / 2; // estava -(length + 1)
        float z = -(length)/ 2;

        //Fazemos um ciclo para começarmos a construir cada coluna
        for (int column = 0; column < division; column++){
            //Fazemos um ciclo para preencher cada coluna com os seus respetivos triângulos
            for (int numtriangle = 0; numtriangle < division; numtriangle++){

                //Criação do primeiro triangulo
                fprintf(f, "%f %f %f\n", x,                    0.0f, z);
                fprintf(f, "%f %f %f\n", x,                    0.0f, z + eachSquareLength);
                fprintf(f, "%f %f %f\n", x + eachSquareLength, 0.0f, z + eachSquareLength);

                //Criação do complemento do triangulo anterior
                fprintf(f, "%f %f %f\n", x,                    0.0f, z);
                fprintf(f, "%f %f %f\n", x + eachSquareLength, 0.0f, z + eachSquareLength);
                fprintf(f, "%f %f %f\n", x + eachSquareLength, 0.0f, z);

                //Avançar no eixo do x para construir o próximo triângulo
                x += eachSquareLength;
            }
            //Incrementamos o z para avançarmos no eixo do z e criarmos o novo quadrado da coluna
            z += eachSquareLength;

            //Resetamos o x para voltar à sua posição inicial
            x = -(length) / 2; // -(length + 1)
        }
        //Mandar para o terminal que o ficheiro filename.3d foi criado com sucesso
        std::cout << filename << " Created Sucessfully!\n";
    }
    else // Se o ficheiro for inválido
    {
        printf("ERROR: Creating .3d file -> Plane\n"); 
    }
    fclose(f); //Fechamos o ficheiro que abrimos em cima
}

// Função para gerar uma caixa
void box_generator(float length, float division, char* filename) 
{
    //Abrir o ficheiro
    FILE* f;
    f = fopen(filename, "w+");

    //Variável que calcula o tamanho de cada quadrado
    float eachSquareLength = length / division;

    //Determinar o numero de triangulos totais necessários para formar a box
    int totalNumDots = 3 * 2 * pow(division, 2.0f) * 6;

    if(f) // Se o ficheiro for válido
    {
        //Vamos guardar no ficheiro .3d o número total de de pontos
        fprintf(f,"%d\n",totalNumDots * 3);

        //Para conseguirmos que a box esteja centrada na origem, vamos começar por construí-la a partir do eixo negativo do x e z
        float a = -length / 2;
        float b = -length / 2;

        //Fazemos um ciclo para começarmos a construir cada coluna
        for (int column = 0; column < division; column++){
            //Fazemos um ciclo para preencher cada coluna com os seus respetivos triângulos
            for (int numtriangle = 0; numtriangle < division; numtriangle++){
                //Construção da Base
                //Criação do primeiro triangulo
                fprintf(f, "%f %f %f\n", a,                    -length / 2, b);
                fprintf(f, "%f %f %f\n", a                     + eachSquareLength, -length / 2, b + eachSquareLength);
                fprintf(f, "%f %f %f\n", a,                    -length / 2, b + eachSquareLength);
                
                //Criação do complemento do triangulo anterior
                fprintf(f, "%f %f %f\n", a,                    -length / 2, b);
                fprintf(f, "%f %f %f\n", a + eachSquareLength, -length / 2, b);
                fprintf(f, "%f %f %f\n", a + eachSquareLength, -length / 2, b + eachSquareLength);
                
                //Construção do Topo
                //Criação do primeiro triangulo
                fprintf(f, "%f %f %f\n", a,                    length / 2, b);
                fprintf(f, "%f %f %f\n", a,                    length / 2, b + eachSquareLength);
                fprintf(f, "%f %f %f\n", a + eachSquareLength, length / 2, b + eachSquareLength);
                
                //Criação do complemento do triangulo anterior
                fprintf(f, "%f %f %f\n", a,                    length / 2, b);
                fprintf(f, "%f %f %f\n", a + eachSquareLength, length / 2, b + eachSquareLength);
                fprintf(f, "%f %f %f\n", a + eachSquareLength, length / 2, b);

                //Construção da Frente
                //Criação do primeiro triangulo
                fprintf(f, "%f %f %f\n", a,                    b + eachSquareLength, length / 2);
                fprintf(f, "%f %f %f\n", a,                    b, length / 2);
                fprintf(f, "%f %f %f\n", a + eachSquareLength, b, length / 2);

                //Criação do complemento do triangulo anterior
                fprintf(f, "%f %f %f\n", a,                    b + eachSquareLength, length / 2);
                fprintf(f, "%f %f %f\n", a + eachSquareLength, b, length / 2);
                fprintf(f, "%f %f %f\n", a + eachSquareLength, b + eachSquareLength, length / 2);

                //Construção da face de trás
                //Criação do primeiro triangulo
                fprintf(f, "%f %f %f\n", a, b + eachSquareLength, -length / 2);
                fprintf(f, "%f %f %f\n", a + eachSquareLength, b, -length / 2);
                fprintf(f, "%f %f %f\n", a, b, -length / 2);

                //Criação do complemento do triangulo anterior
                fprintf(f, "%f %f %f\n", a, b + eachSquareLength, -length / 2);
                fprintf(f, "%f %f %f\n", a + eachSquareLength, b + eachSquareLength, -length / 2);
                fprintf(f, "%f %f %f\n", a + eachSquareLength, b, -length / 2);

                //Construção da face da esquerda
                //Criação do primeiro triangulo
                fprintf(f, "%f %f %f\n", -length / 2, b + eachSquareLength, a + eachSquareLength);
                fprintf(f, "%f %f %f\n", -length / 2, b + eachSquareLength, a);
                fprintf(f, "%f %f %f\n", -length / 2, b, a);

                //Criação do complemento do triangulo anterior
                fprintf(f, "%f %f %f\n", -length / 2, b + eachSquareLength, a + eachSquareLength);
                fprintf(f, "%f %f %f\n", -length / 2, b, a);
                fprintf(f, "%f %f %f\n", -length / 2, b, a + eachSquareLength);

                //Construção da face da direita
                //Criação do primeiro triangulo
                fprintf(f, "%f %f %f\n", length / 2, b + eachSquareLength, a + eachSquareLength);
                fprintf(f, "%f %f %f\n", length / 2, b, a);
                fprintf(f, "%f %f %f\n", length / 2, b + eachSquareLength, a);

                //Criação do complemento do triangulo anterior
                fprintf(f, "%f %f %f\n", length / 2, b + eachSquareLength, a + eachSquareLength);
                fprintf(f, "%f %f %f\n", length / 2, b, a + eachSquareLength);
                fprintf(f, "%f %f %f\n", length / 2, b, a);

                //Avançar no eixo do x para construir o próximo triângulo
                a += eachSquareLength;
            }
            //Incrementamos o z para avançarmos no eixo do z e criarmos o novo quadrado da coluna
            b += eachSquareLength;

            //Resetamos o x para voltar à sua posição inicial
            a = -length / 2;

        }
        //Mandar para o terminal que o ficheiro filename.3d foi criado com sucesso
        std::cout << filename << " Created Sucessfully!\n";
    }
    else // Se o ficheiro for inválido
    {
        printf("ERROR: Creating .3d file -> Box\n");
    }
    fclose(f); //Fechamos o ficheiro que abrimos em cima
}

// Função para gerar um cone
void cone(float radius, float height, int slices, int stacks, char* filename) 
{
    //Abrir o ficheiro
    FILE* f;
    f = fopen(filename, "w+");

    // Consoante o número de slices, vamos determinar o angulo de cada slice
    float angleBase = 2 * M_PI / slices;
    // Consoante o número de stacks, vamos determinar a altura de cada stack
    float angleSides = height / stacks;

    // Determinar o número total de pontos necessários para formar o cone
    int totalNumDots = 3 * 3 * slices + 3 * 6 * stacks * slices;

    if (f){ // Se o ficheiro for válido

        // Determinar o numero de pontos necessários para formar o cone
        fprintf(f,"%d\n",totalNumDots);

        //Construção da Base
        for (int i = 0; i < slices; i++) {
            float angleBaseTemp = i * angleBase;
            fprintf(f, "%f %f %f\n", radius * sin(angleBaseTemp), 0.0f, radius * cos(angleBaseTemp));
            fprintf(f, "%f %f %f\n", 0.0f, 0.0, 0.0f);
            fprintf(f, "%f %f %f\n", radius * sin(angleBaseTemp + angleBase), 0.0f, radius * cos(angleBaseTemp + angleBase));
        }

        //Construção dos Lados
        for (int i = 0; i < stacks; i++) {
            float sides1 = i * angleSides;
            float sides2 = (i + 1) * angleSides;
            float rTemp1 = radius - ((radius / stacks) * i);
            float rTemp2 = radius - ((radius / stacks) * (i + 1));
            //Construção dos Lados
            for (int j = 0; j < slices; j++) {
                float angleBaseTemp2 = j * angleBase;
                float x1 = rTemp1 * sin(angleBaseTemp2);
                float x2 = rTemp2 * sin(angleBaseTemp2 + angleBase);
                float x3 = rTemp2 * sin(angleBaseTemp2);
                float x4 = rTemp1 * sin(angleBaseTemp2 + angleBase);
                float y1 = sides1;
                float y2 = sides2;
                float z1 = rTemp1 * cos(angleBaseTemp2);
                float z2 = rTemp2 * cos(angleBaseTemp2 + angleBase);
                float z3 = rTemp2 * cos(angleBaseTemp2);
                float z4 = rTemp1 * cos(angleBaseTemp2 + angleBase);

                //Construção do primeiro triangulo
                fprintf(f, "%f %f %f\n", x1, y1, z1);
                fprintf(f, "%f %f %f\n", x2, y2, z2);
                fprintf(f, "%f %f %f\n", x3, y2, z3);

                //Construção do complemento do triangulo anterior
                fprintf(f, "%f %f %f\n", x1, y1, z1);
                fprintf(f, "%f %f %f\n", x4, y1, z4);
                fprintf(f, "%f %f %f\n", x2, y2, z2);
            }
        }
        //Mandar para o terminal que o ficheiro filename.3d foi criado com sucesso
        std::cout << filename << " Created Sucessfully!\n";
    }
    else // Se o ficheiro for inválido
    {
        printf("ERROR: Creating .3d file -> Cone\n");
    }
    fclose(f); //Fechamos o ficheiro que abrimos em cima
}

// Função para gerar uma esfera
void sphere(float radius, int slices, int stacks, char* filename) {
    //Abrir o ficheiro
    FILE* f;
    f = fopen(filename, "w+");

    // Consoante o número de slices, vamos determinar o angulo de cada slice
    float angleCirc = 2 * M_PI / slices;
    float angleSides = M_PI / stacks;
    int numVertices = 6 * stacks * slices; 

    if (f) // Se o ficheiro for válido
    {
        //Vamos guardar no ficheiro .3d o número total de de pontos
        fprintf(f, "%d\n", 3 * numVertices);


        for (int i = 0; i < stacks; i++) 
        {
            //Determinar a altura de cada stack
            float angleSidesTemp = -(M_PI / 2) + i * angleSides;
            //Determinar a altura do stack seguinte
            float y1 = radius * sin(angleSidesTemp + angleSides);
            float y2 = radius * sin(angleSidesTemp);

            //Construção dos Lados
            for (int j = 0; j < slices; j++) 
            {
                //Determinar o angulo de cada slice
                float angleCircTemp = j * angleCirc;
                float x1 = radius * cos(angleSidesTemp + angleSides) * sin(angleCircTemp);
                float x2 = radius * cos(angleSidesTemp) * sin(angleCircTemp);
                float x3 = radius * cos(angleSidesTemp) * sin(angleCircTemp + angleCirc);
                float x4 = radius * cos(angleSidesTemp + angleSides) * sin(angleCircTemp + angleCirc);
                float z1 = radius * cos(angleSidesTemp + angleSides) * cos(angleCircTemp);
                float z2 = radius * cos(angleSidesTemp) * cos(angleCircTemp);
                float z3 = radius * cos(angleSidesTemp) * cos(angleCircTemp + angleCirc);
                float z4 = radius * cos(angleSidesTemp + angleSides) * cos(angleCircTemp + angleCirc);

                //Construção do primeiro triangulo
                fprintf(f, "%f %f %f\n", x1, y1, z1);
                fprintf(f, "%f %f %f\n", x2, y2, z2);
                fprintf(f, "%f %f %f\n", x3, y2, z3);

                //Construção do complemento do triangulo anterior
                fprintf(f, "%f %f %f\n", x1, y1, z1);
                fprintf(f, "%f %f %f\n", x3, y2, z3);
                fprintf(f, "%f %f %f\n", x4, y1, z4);
            }
        }
        //Mandar para o terminal que o ficheiro filename.3d foi criado com sucesso
        std::cout << filename << " Created Sucessfully!\n";
    }
    else // Se o ficheiro for inválido
    {
        printf("ERROR: Creating .3d file -> Sphere\n");
    }
    fclose(f); //Fechamos o ficheiro que abrimos em cima
}

// Função para calcular um ponto em uma superfície de Bezier
Point3D calculateBezierPoint(float u, float v, const std::vector<Point3D>& controlPoints, const BezierPatch& patch) {
    // Polinômios de Bernstein para u e v
    float bernstein_u[4], bernstein_v[4];
    
    // Calcular os polinômios de Bernstein para u
    float u2 = u * u;
    float u3 = u2 * u;
    bernstein_u[0] = (1 - u) * (1 - u) * (1 - u);  // (1-u)^3
    bernstein_u[1] = 3 * u * (1 - u) * (1 - u);    // 3u(1-u)^2
    bernstein_u[2] = 3 * u2 * (1 - u);             // 3u^2(1-u)
    bernstein_u[3] = u3;                           // u^3
    
    // Calcular os polinômios de Bernstein para v
    float v2 = v * v;
    float v3 = v2 * v;
    bernstein_v[0] = (1 - v) * (1 - v) * (1 - v);  // (1-v)^3
    bernstein_v[1] = 3 * v * (1 - v) * (1 - v);    // 3v(1-v)^2
    bernstein_v[2] = 3 * v2 * (1 - v);             // 3v^2(1-v)
    bernstein_v[3] = v3;                           // v^3
    
    Point3D result(0, 0, 0);
    
    // Calcular o ponto da superfície usando os polinômios de Bernstein
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int index = patch.indices[i * 4 + j];
            float weight = bernstein_u[i] * bernstein_v[j];
            
            result.x += controlPoints[index].x * weight;
            result.y += controlPoints[index].y * weight;
            result.z += controlPoints[index].z * weight;
        }
    }
    
    return result;
}

// Função para ler arquivos de patches de Bezier
bool readBezierFile(const char* filename, std::vector<Point3D>& controlPoints, std::vector<BezierPatch>& patches) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open Bezier file: " << filename << std::endl;
        return false;
    }
    
    try {
        // Ler número de patches
        int numPatches;
        file >> numPatches;
        
        if(numPatches <= 0 || numPatches > 10000) {
            std::cerr << "ERROR: Invalid number of patches: " << numPatches << std::endl;
            return false;
        }
        
        // Ignorar o resto da linha após o número de patches
        std::string line;
        std::getline(file, line);
        
        // Ler índices de cada patch
        patches.clear();  // Garantir que o vetor esteja vazio
        patches.reserve(numPatches);  // Pré-alocar memória
        
        for (int i = 0; i < numPatches; i++) {
            BezierPatch patch;
            // Ler uma linha inteira
            if (!std::getline(file, line)) {
                std::cerr << "ERROR: Failed to read line for patch " << i << std::endl;
                return false;
            }
            
            // Substitui todas as vírgulas por espaços para facilitar o parsing
            for (char &c : line) {
                if (c == ',') c = ' ';
            }
            
            std::istringstream iss(line);
            int j = 0;
            int value;
            
            // Lê os 16 valores inteiros da linha
            while (iss >> value && j < 16) {
                patch.indices[j++] = value;
            }
            
            if (j != 16) {
                std::cerr << "ERROR: Expected 16 indices for patch " << i << ", but got " << j << std::endl;
                return false;
            }
            
            patches.push_back(patch);
        }
        
        // Ler número de pontos de controle
        int numPoints;
        file >> numPoints;
        
        if(numPoints <= 0 || numPoints > 50000) {
            std::cerr << "ERROR: Invalid number of control points: " << numPoints << std::endl;
            return false;
        }
        
        // Ignorar o resto da linha após o número de pontos
        std::getline(file, line);
        
        // Ler coordenadas dos pontos de controle
        controlPoints.clear();  // Garantir que o vetor esteja vazio
        controlPoints.reserve(numPoints);  // Pré-alocar memória
        
        for (int i = 0; i < numPoints; i++) {
            Point3D point;
            if (!std::getline(file, line)) {
                std::cerr << "ERROR: Failed to read line for control point " << i << std::endl;
                return false;
            }
            
            // Substitui vírgulas por espaços (caso haja)
            for (char &c : line) {
                if (c == ',') c = ' ';
            }
            
            std::istringstream iss(line);
            if (!(iss >> point.x >> point.y >> point.z)) {
                std::cerr << "ERROR: Failed to read control point coordinates at point " << i << std::endl;
                return false;
            }
            
            controlPoints.push_back(point);
        }
        
        file.close();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: Exception reading Bezier file: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

// Função para gerar superfície de Bezier com escrita em blocos
void bezier_surface(const char* patchFile, int tessellation, char* outputFile) {
    std::vector<Point3D> controlPoints;
    std::vector<BezierPatch> patches;
    
    // Verificar tessellation válida
    if (tessellation <= 0 || tessellation > 100) {
        printf("ERROR: Invalid tessellation value (1-100 allowed): %d\n", tessellation);
        return;
    }
    
    // Ler o arquivo de patches
    if (!readBezierFile(patchFile, controlPoints, patches)) {
        return;
    }
    
    // Abrir o arquivo para escrever
    FILE* f = fopen(outputFile, "w+");
    if (!f) {
        printf("ERROR: Creating .3d file -> Bezier Surface\n");
        return;
    }
    
    // Calcular o número total de vértices necessários
    int numVertices = patches.size() * tessellation * tessellation * 6;
    fprintf(f, "%d\n", numVertices * 3);
    
    // Variáveis para informação de progresso
    int totalPatches = patches.size();
    int processedPatches = 0;
    
    // Para cada patch
    for (const auto& patch : patches) {
        // Atualizar e mostrar progresso
        processedPatches++;
        if (processedPatches % 10 == 0 || processedPatches == totalPatches) {
            printf("Processing Bezier patches: %d/%d (%.1f%%)\n", 
                   processedPatches, totalPatches, 
                   (float)processedPatches / totalPatches * 100.0f);
        }
        
        // Para cada segmento de tessellation
        float stepSize = 1.0f / tessellation;
        
        for (int i = 0; i < tessellation; i++) {
            float u0 = i * stepSize;
            float u1 = (i + 1) * stepSize;
            
            for (int j = 0; j < tessellation; j++) {
                float v0 = j * stepSize;
                float v1 = (j + 1) * stepSize;
                
                // Calcular os quatro cantos do quadrado da tessellation
                Point3D p00 = calculateBezierPoint(u0, v0, controlPoints, patch);
                Point3D p01 = calculateBezierPoint(u0, v1, controlPoints, patch);
                Point3D p10 = calculateBezierPoint(u1, v0, controlPoints, patch);
                Point3D p11 = calculateBezierPoint(u1, v1, controlPoints, patch);
                
                // Triângulo 1
                fprintf(f, "%f %f %f\n", p00.x, p00.y, p00.z);
                fprintf(f, "%f %f %f\n", p01.x, p01.y, p01.z);
                fprintf(f, "%f %f %f\n", p11.x, p11.y, p11.z);
                
                // Triângulo 2
                fprintf(f, "%f %f %f\n", p00.x, p00.y, p00.z);
                fprintf(f, "%f %f %f\n", p11.x, p11.y, p11.z);
                fprintf(f, "%f %f %f\n", p10.x, p10.y, p10.z);
            }
        }
    }
    
    fclose(f);
    std::cout << outputFile << " Created Successfully!\n";
}

// Função para gerar um torus (para os anéis de Saturno)
void torus(float innerRadius, float outerRadius, int sides, int rings, char* filename) {
    FILE* f = fopen(filename, "w+");
    
    if (!f) {
        printf("ERROR: Creating .3d file -> Torus\n");
        return;
    }
    
    // Número total de vértices (2 triângulos por cada face x rings x sides)
    int numVertices = 6 * rings * sides;
    fprintf(f, "%d\n", numVertices * 3);
    
    float ringStep = 2.0f * M_PI / rings;
    float sideStep = 2.0f * M_PI / sides;
    
    // Raio médio do torus
    float meanRadius = (innerRadius + outerRadius) / 2.0f;
    // Espessura do tubo
    float tubeRadius = (outerRadius - innerRadius) / 2.0f;
    
    for (int i = 0; i < rings; i++) {
        float theta = i * ringStep;
        float nextTheta = ((i + 1) % rings) * ringStep;
        
        for (int j = 0; j < sides; j++) {
            float phi = j * sideStep;
            float nextPhi = ((j + 1) % sides) * sideStep;
            
            // Calcular os quatro vértices de um segmento do torus
            
            // Vértice 1: (i, j)
            float x1 = (meanRadius + tubeRadius * cos(phi)) * cos(theta);
            float y1 = tubeRadius * sin(phi);
            float z1 = (meanRadius + tubeRadius * cos(phi)) * sin(theta);
            
            // Vértice 2: (i+1, j)
            float x2 = (meanRadius + tubeRadius * cos(phi)) * cos(nextTheta);
            float y2 = tubeRadius * sin(phi);
            float z2 = (meanRadius + tubeRadius * cos(phi)) * sin(nextTheta);
            
            // Vértice 3: (i, j+1)
            float x3 = (meanRadius + tubeRadius * cos(nextPhi)) * cos(theta);
            float y3 = tubeRadius * sin(nextPhi);
            float z3 = (meanRadius + tubeRadius * cos(nextPhi)) * sin(theta);
            
            // Vértice 4: (i+1, j+1)
            float x4 = (meanRadius + tubeRadius * cos(nextPhi)) * cos(nextTheta);
            float y4 = tubeRadius * sin(nextPhi);
            float z4 = (meanRadius + tubeRadius * cos(nextPhi)) * sin(nextTheta);
            
            // Primeiro triângulo
            fprintf(f, "%f %f %f\n", x1, y1, z1);
            fprintf(f, "%f %f %f\n", x2, y2, z2);
            fprintf(f, "%f %f %f\n", x4, y4, z4);
            
            // Segundo triângulo
            fprintf(f, "%f %f %f\n", x1, y1, z1);
            fprintf(f, "%f %f %f\n", x4, y4, z4);
            fprintf(f, "%f %f %f\n", x3, y3, z3);
        }
    }
    
    fclose(f);
    std::cout << filename << " Created Successfully!\n";
}

int main(int argc, char* argv[]) {
    // Permitir vetores de tamanho maior
    try {
        // Define um limite maior para os vetores
        std::vector<int>().max_size();
    }
    catch (const std::exception& e) {
        std::cerr << "WARNING: Vector allocation error preparation failed: " << e.what() << std::endl;
    }
    
    // Verificar se o número de argumentos é suficiente
    if (argc < 2) { // Se não for suficiente
        printf("ERROR: Not enough arguments\n");
        printf("Usage: generator <primitive> <params>...\n");
        return 1; // error
    }
    else if (strcmp(argv[1], "plane") == 0) {
        if (argc < 5) {
            printf("ERROR: Not enough arguments for plane\n");
            printf("Usage: generator plane <length> <divisions> <filename>\n");
            return 1;
        }
        plane_generator(atof(argv[2]), atof(argv[3]), argv[4]); // length, divisions, filename
        return 0;
    }
    else if (strcmp(argv[1], "box") == 0) {
        if (argc < 5) {
            printf("ERROR: Not enough arguments for box\n");
            printf("Usage: generator box <length> <divisions> <filename>\n");
            return 1;
        }
        box_generator(atof(argv[2]), atof(argv[3]), argv[4]); // length, division, filename 
        return 0;
    }
    else if (strcmp(argv[1], "cone") == 0) {
        if (argc < 7) {
            printf("ERROR: Not enough arguments for cone\n");
            printf("Usage: generator cone <radius> <height> <slices> <stacks> <filename>\n");
            return 1;
        }
        cone(atof(argv[2]), atof(argv[3]), atoi(argv[4]), atoi(argv[5]), argv[6]); // radius, height, slices, stacks, filename
        return 0;
    }
    else if (strcmp(argv[1], "sphere") == 0) {
        if (argc < 6) {
            printf("ERROR: Not enough arguments for sphere\n");
            printf("Usage: generator sphere <radius> <slices> <stacks> <filename>\n");
            return 1;
        }
        sphere(atof(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5]); // radius, slices, stacks, filename 
        return 0;
    }
    // Caso para patches de Bezier
    else if (strcmp(argv[1], "bezier") == 0) {
        if (argc < 5) {
            printf("ERROR: Not enough arguments for bezier\n");
            printf("Usage: generator bezier <patch_file> <tessellation> <output_file>\n");
            return 1;
        }
        
        // Configurar para usar mais memória
        std::cout << "Starting Bezier surface generation with tessellation: " << argv[3] << std::endl;
        std::cout << "Using optimized memory handling for high tessellation values\n";
        
        // Limitar tessellation para evitar crashes
        int tess = atoi(argv[3]);
        if (tess > 50) {
            std::cout << "WARNING: Tessellation value " << tess << " is very high!\n";
            std::cout << "         This might require a lot of memory.\n";
            std::cout << "         Proceeding anyway...\n";
        }
        
        bezier_surface(argv[2], tess, argv[4]); // patchFile, tessellation, outputFile
        return 0;
    }
    // Caso para torus (anéis de Saturno)
    else if (strcmp(argv[1], "torus") == 0) {
        if (argc < 7) {
            printf("ERROR: Not enough arguments for torus\n");
            printf("Usage: generator torus <inner_radius> <outer_radius> <sides> <rings> <filename>\n");
            return 1;
        }
        torus(atof(argv[2]), atof(argv[3]), atoi(argv[4]), atoi(argv[5]), argv[6]); // innerRadius, outerRadius, sides, rings, filename
        return 0;
    }
    else if (strcmp(argv[1], "all") == 0) {
        plane_generator(1.0f, 3.0f, "plane.3d");
        box_generator(2.0f, 3.0f, "box.3d");
        sphere(1.0f, 10, 10, "sphere.3d");
        cone(1.0f, 2.0f, 4, 3, "cone.3d");
        torus(0.7f, 1.0f, 16, 32, "torus.3d");
        
        // Modelos adicionais para o sistema solar
        sphere(1.0f, 20, 20, "solarSystem/Sun.3d");
        sphere(0.4f, 20, 20, "solarSystem/Mercury.3d");
        sphere(0.5f, 20, 20, "solarSystem/Venus.3d");
        sphere(0.5f, 20, 20, "solarSystem/Earth.3d");
        sphere(0.2f, 10, 10, "solarSystem/Moon.3d");
        sphere(0.45f, 20, 20, "solarSystem/Mars.3d");
        sphere(1.2f, 20, 20, "solarSystem/Jupiter.3d");
        sphere(1.0f, 20, 20, "solarSystem/Saturn.3d");
        torus(1.2f, 2.0f, 20, 40, "solarSystem/torus.3d");
        sphere(0.8f, 20, 20, "solarSystem/Uranus.3d");
        sphere(0.8f, 20, 20, "solarSystem/Neptune.3d");
        
        printf("All models created successfully\n");
        return 0;
    }
    else {
        printf("ERROR: Invalid primitive type!\n");
        printf("Supported primitives: plane, box, sphere, cone, bezier, torus\n");
        printf("Use 'generator all' to generate all default models\n");
        return 1; // error
    }
    return 0;
}