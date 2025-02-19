#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

int plane_genarator(float length, float division, char* filename){
    //1º Passo: Abrir o ficheiro
    FILE* f;
    f = fopen(filename, "w+");
    //In 'w+' mode, the file is opened for both reading and writing, existing 
    //content is cleared, a new empty file is created if it doesn't exist, and 
    //the file pointer is positioned at the beginning.

    //2º Passo: Temos um plano de tamanho lenght e vamos dividir no numero de 
    //divisões forncido. Em seguida vamos fazer lenght/divison e isto dar-nos-á
    //o tamanho de cada divisão
    float eachSquareLength = length / division;

    //3ºPasso: Determinar o numero de triangulos totais necessários para formar
    //o plano total
    int totalNumDots = 3 * 2 * pow(division, 2.0f);
    // Fazemos o divisions^2 para termos o total de quadrados (divisões) em seguida,
    //multiplicamos por 2 por em cada quadrado temos 2 triângulos. Daqui resulta o
    //número total de triangulos. Depois multiplicamos por 3, pois cada triangulo tem
    //3 pontos, resultando assim no total de pontos necessários para se construir o 
    //plano 

    if(f) // Se o ficheiro for válido
    {
        //4ºPasso: Vamos guardar no ficheiro .3d o número total de de pontos
        fprintf(f,"%d \n",totalNumDots);
        //PS: se der erro exprimentar multiplicar por 3

        //5ºPasso: Para conseguirmos que o plano esteja centrado na origem, vamos
        //começar por construí-lo a partir do eixo negativo do x e z, pois a partir
        //dai vai construir para a "frente"
        float x = -length / 2;
		float z = -length / 2;

        //6ºPasso: Fazemos um ciclo para começarmos a construir cada coluna
        for (int column = 0; column < division; column++){
            //7º Passo: Fazemos um ciclo para preencher cada coluna com os seus
            //respetivos triângulos
            for (int numtriangle = 0; numtriangle < division; numtriangle++){

                //8º Passo: Criação do primeiro triangulo
                fprintf(f, "%f %f %f \n", x,				0.0f, z);
				fprintf(f, "%f %f %f \n", x,				0.0f, z + eachSquareLength);
				fprintf(f, "%f %f %f \n", x + eachSquareLength,	0.0f, z + eachSquareLength);

				//9º Passo: Criação do complemento do triangulo anterior
				fprintf(f, "%f %f %f \n", x,				0.0f, z);
				fprintf(f, "%f %f %f \n", x + eachSquareLength,	0.0f, z + eachSquareLength);
				fprintf(f, "%f %f %f \n", x + eachSquareLength,	0.0f, z);

                //10ºPasso: Avançar no eixo do x para construir o próximo triângulo
				x += eachSquareLength;
            }
            //11º Passo: Incrementamos o z para avançarmos no eixo do z e criarmos
            // o novo quadrado da coluna
            z += eachSquareLength;

            //12º Passo: Resetamos o x para voltar à sua posição inicial
            x = -length / 2;
        }
        //13º Passo: Mandar para o terminal que o ficheiro filename.3d foi criado 
        //com sucesso
        std::cout << filename << " Created Sucessfully!\n";
    }
    else // Se o ficheiro for inválido
    {
        printf("ERROR: Creating .3d file -> Plane\n"); 
		return 1;
    }
    fclose(f); //Fechamos o ficheiro que abrimos em cima
    return 0;
}