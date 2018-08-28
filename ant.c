#include "ant.h"

/*
	Para compilar isso, computador precisa de:
		Opengl + SDL
				  \-----> Nao é a versao 2.

				  Eu usei a extensao SDL_image. Se Apenas instalar o SDL
				  nao deu certo, tente instalar alguma extensao que possui
				  "SDL_image.h". (so usar apt-get install)

		Nao esqueça de ter o Glut instalado!

	Para compilar: primeira linha do ant.h
*/

//Variavel compartilhada entre os arquivos.c
extern Control * gc;

int main(int argc, char ** argv){
	initOpengl(&argc, argv, "Ants");

	//Alocando grupos de imagens
	Group * background = newGroup(),
		  * middle     = newGroup(),
		  * over       = newGroup();

	//Alocando variaveis locais temporarias
	DisplayObj * tmp = NULL;

	//Alocando: MAPA, FORMIGAS (vivas e mortas):
	gc->matrix       = newMatrix(background);
	gc->arrayAnt     = newAnt(middle);
	gc->arrayDeadAnt = newDeadAnt(middle);

	printf("Ant: 1\nDead Ant: 2\nFree Position: 0\n");
	printMatrix();
	printf("\n\n\n");
	

	//randMove(m,a,ANT);
	
	//Mostrando no terminal
	printMatrix();

	newText(over, "Pressione 'F' ou para avancar de quadros", 200, 50, NULL);

    glutMainLoop();
    //Tudo o que estiver aqui em baixo NUNCA sera executado.
    //A nao ser que o glut nao funcione :D
	return 0;
}
