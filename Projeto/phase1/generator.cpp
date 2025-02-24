#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

/*
    Creates a .3d file of a plane with the required parameters
    @return 0 if sucessufll, 1 if not
*/

int plane_generator(float length, float division, char* filename){
    //Abrir o ficheiro
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
                fprintf(f, "%f %f %f \n", x,				    0.0f, z);
				fprintf(f, "%f %f %f \n", x,				    0.0f, z + eachSquareLength);
				fprintf(f, "%f %f %f \n", x + eachSquareLength,	0.0f, z + eachSquareLength);

				//9º Passo: Criação do complemento do triangulo anterior
				fprintf(f, "%f %f %f \n", x,				    0.0f, z);
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

/*
    Creates a .3d file of a box with the required parameters
*/

void box_generator(float length, float division, char* filename) 
{
    //1º Passo: Abrir o ficheiro
    FILE* f;
    f = fopen(filename, "w+"); // if file already exists erases contents, else 
    //creates new file

    //2º Passo: Temos um plano de tamanho lenght e vamos dividir no numero de 
    //divisões forncido. Em seguida vamos fazer lenght/divison e isto dar-nos-á
    //o tamanho de cada divisão
    float eachSquareLength = length / division;

    //3ºPasso: Determinar o numero de triangulos totais necessários para formar
    //a box
    int totalNumDots = 3 * 2 * pow(division, 2.0f) * 6;
    // Fazemos o divisions^2 para termos o total de quadrados (divisões) em seguida,
    //multiplicamos por 2 por em cada quadrado temos 2 triângulos. Daqui resulta o
    //número total de triangulos. Depois multiplicamos por 3, pois cada triangulo tem
    //3 pontos. Por fim, multiplicamos por 6, pois uma box tem 6 faces, logo 6 lados.

    if(f) // Se o ficheiro for válido
    {
        //Vamos guardar no ficheiro .3d o número total de de pontos
        fprintf(f,"%d \n",totalNumDots);
        //PS: se der erro exprimentar multiplicar por 3

        //Para conseguirmos que a box esteja centrada na origem, vamos
        //começar por construí-la a partir do eixo negativo do x e z, pois a partir
        //dai vai construir para a "frente"
        float a = -length / 2;
        float b = -length / 2;

        //Fazemos um ciclo para começarmos a construir cada coluna
        for (int column = 0; column < division; column++){
            //Fazemos um ciclo para preencher cada coluna com os seus
            //respetivos triângulos
            for (int numtriangle = 0; numtriangle < division; numtriangle++){
                //Bottom
                //Criação do primeiro triangulo
                fprintf(f, "%f %f %f \n", a,                    -length / 2, b);
                fprintf(f, "%f %f %f \n", a                     + eachSquareLength, -length / 2, b + eachSquareLength);
                fprintf(f, "%f %f %f \n", a,                    -length / 2, b + eachSquareLength);
                
    
                //Criação do complemento do triangulo anterior
                fprintf(f, "%f %f %f \n", a,                    -length / 2, b);
                fprintf(f, "%f %f %f \n", a + eachSquareLength, -length / 2, b);
                fprintf(f, "%f %f %f \n", a + eachSquareLength, -length / 2, b + eachSquareLength);
                

                //Top
                //Criação do primeiro triangulo
                fprintf(f, "%f %f %f \n", a,                    length / 2, b);
                fprintf(f, "%f %f %f \n", a,                    length / 2, b + eachSquareLength);
                fprintf(f, "%f %f %f \n", a + eachSquareLength, length / 2, b + eachSquareLength);
                
                //Criação do complemento do triangulo anterior
                fprintf(f, "%f %f %f \n", a,                    length / 2, b);
                fprintf(f, "%f %f %f \n", a + eachSquareLength, length / 2, b + eachSquareLength);
                fprintf(f, "%f %f %f \n", a + eachSquareLength, length / 2, b);

                //Front
                //Criação do primeiro triangulo
                fprintf(f, "%f %f %f \n", a,                    b + eachSquareLength, length / 2);
                fprintf(f, "%f %f %f \n", a,                    b, length / 2);
                fprintf(f, "%f %f %f \n", a + eachSquareLength, b, length / 2);

                //Criação do complemento do triangulo anterior
                fprintf(f, "%f %f %f \n", a,                    b + eachSquareLength, length / 2);
                fprintf(f, "%f %f %f \n", a + eachSquareLength, b, length / 2);
                fprintf(f, "%f %f %f \n", a + eachSquareLength, b + eachSquareLength, length / 2);

                //Back
                //Criação do primeiro triangulo
                fprintf(f, "%f %f %f \n", a, b + eachSquareLength, -length / 2);
                fprintf(f, "%f %f %f \n", a + eachSquareLength, b, -length / 2);
                fprintf(f, "%f %f %f \n", a, b, -length / 2);

                //Criação do complemento do triangulo anterior
                fprintf(f, "%f %f %f \n", a, b + eachSquareLength, -length / 2);
                fprintf(f, "%f %f %f \n", a + eachSquareLength, b + eachSquareLength, -length / 2);
                fprintf(f, "%f %f %f \n", a + eachSquareLength, b, -length / 2);

                //Left
                //Criação do primeiro triangulo
                fprintf(f, "%f %f %f \n", -length / 2, b + eachSquareLength, a + eachSquareLength);
                fprintf(f, "%f %f %f \n", -length / 2, b + eachSquareLength, a);
                fprintf(f, "%f %f %f \n", -length / 2, b, a);

                //Criação do complemento do triangulo anterior
                fprintf(f, "%f %f %f \n", -length / 2, b + eachSquareLength, a + eachSquareLength);
                fprintf(f, "%f %f %f \n", -length / 2, b, a);
                fprintf(f, "%f %f %f \n", -length / 2, b, a + eachSquareLength);

                //Right
                //Criação do primeiro triangulo
                fprintf(f, "%f %f %f \n", length / 2, b + eachSquareLength, a + eachSquareLength);
                fprintf(f, "%f %f %f \n", length / 2, b, a);
                fprintf(f, "%f %f %f \n", length / 2, b + eachSquareLength, a);

                //Criação do complemento do triangulo anterior
                fprintf(f, "%f %f %f \n", length / 2, b + eachSquareLength, a + eachSquareLength);
                fprintf(f, "%f %f %f \n", length / 2, b, a + eachSquareLength);
                fprintf(f, "%f %f %f \n", length / 2, b, a);

                //Avançar no eixo do x para construir o próximo triângulo
                a += eachSquareLength;
            }
            //Incrementamos o z para avançarmos no eixo do z e criarmos
            // o novo quadrado da coluna
            b += eachSquareLength;

            //Resetamos o x para voltar à sua posição inicial
            a = -length / 2;

        }
        //Mandar para o terminal que o ficheiro filename.3d foi criado 
        //com sucesso
        std::cout << filename << " Created Sucessfully!\n";
    }
    else // Se o ficheiro for inválido
    {
        printf("ERROR: Creating .3d file -> Box\n");
    }
    fclose(f); //Fechamos o ficheiro que abrimos em cima
}

void cone(float radius, float height, int slices, int stacks, char* filename) 
{
	FILE* f;
	f = fopen(filename, "w+");

    // Consoante o número de slices, vamos determinar o angulo de cada slice
    float angleBase = 2 * M_PI / slices;
    // Consoante o número de stacks, vamos determinar a altura de cada stack
    float angleSides = height / stacks;

    // Determinar o número total de pontos necessários para formar o cone
    int totalNumDots = 3 * 3 * slices + 3 * 6 * stacks * slices;
    //3*3*slices -> base
    //3*6*stacks*slices -> sides

    if (f){

        fprintf(f,"%d \n",totalNumDots);

        //base
        for (int i = 0; i < slices; i++) {
            float angleBaseTemp = i * angleBase;
            fprintf(f, "%f %f %f \n", radius * sin(angleBaseTemp), 0.0f, radius * cos(angleBaseTemp));
            fprintf(f, "%f %f %f \n", 0.0f, 0.0, 0.0f);
            fprintf(f, "%f %f %f \n", radius * sin(angleBaseTemp + angleBase), 0.0f, radius * cos(angleBaseTemp + angleBase));
        }

        //sides
        for (int i = 0; i < stacks; i++) {
            float sides1 = i * angleSides;
			float sides2 = (i + 1) * angleSides;
			float rTemp1 = radius - ((radius / stacks) * i);
			float rTemp2 = radius - ((radius / stacks) * (i + 1));
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

				fprintf(f, "%f %f %f \n", x1, y1, z1);
				fprintf(f, "%f %f %f \n", x2, y2, z2);
				fprintf(f, "%f %f %f \n", x3, y2, z3);

				fprintf(f, "%f %f %f \n", x1, y1, z1);
				fprintf(f, "%f %f %f \n", x4, y1, z4);
				fprintf(f, "%f %f %f \n", x2, y2, z2);
            }
        }
        std::cout << filename << " Created Sucessfully!\n";
    }
    else 
    {
        printf("ERROR: Creating .3d file -> Cone\n");
    }

}

int main(int argc, char* argv[]) {

	if (argc < 1)
	{
		printf("ERROR:Not enough arguments\n");
		return 1; //error
	}
	else if (strcmp(argv[1], "plane") == 0)
	{

		plane_generator(atof(argv[2]), atof(argv[3]), argv[4]);// length, divisions , filename
		return 0;
	}
	else if (strcmp(argv[1], "box") == 0)
	{

		box_generator(atof(argv[2]), atof(argv[3]), argv[4]);// length, division, filename 
		return 0;
	}
	//else if (strcmp(argv[1], "sphere") == 0)
	//{

	//	sphere(atof(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5]);// radius, slices, stacks, filename 
	//	return 0;
	//}
	//else if (strcmp(argv[1], "cone") == 0)
	//{

	//	cone(atof(argv[2]), atof(argv[3]), atoi(argv[4]), atoi(argv[5]), argv[6]); //radius, height, slices, stacks, filename
	//	return 0;

	//}
	else if (strcmp(argv[1], "all") == 0)
	{
		plane_generator((float)1, (float)3, "plane.3d");	//Guiao
		box_generator((float)2, (float)3, "box.3d");		//Guiao
		//sphere((float)1, 10, 10, "sphere.3d");	//Guiao
		//cone((float)1, (float)2, 4, 3, "cone.3d");		//Guiao
		//cone((float)1, (float)2, 4, 3, "cone_1_2_4_3.3d");		//test_1_1.xml && test_1_2.xml
		//sphere((float)1, 10, 10, "sphere_1_10_10.3d");	//test_1_3.xml && test_1_5.xml
		box_generator((float)2, (float)3, "box_2_3.3d");			//test_1_4.xml
		plane_generator((float)1, (float)3, "plane_2_3.3d");		//test_1_5.xml

		printf("testes criados\n");
		return 0;
	}
	else 
	{
		printf("ERROR: Invalid Format!\nFormat is <Primitive> <Values> <Filename>\nExiting Generator\n");
		return 1; //error
	}


	return 0;
}