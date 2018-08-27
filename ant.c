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


int main(int argc, char ** argv){

	initOpengl(&argc, argv, "Ants");

	Matrix *m = newMatrix(TAM,TAM);
	Ant *a = newAnt(m,ANT);
	DeadAnt *d = newDeadAnt(m,DANT);

	printf("Ant: 1\nDead Ant: 2\nFree Position: 0\n");
	printMatrix(m);
	printf("\n\n\n");
	randMove(m,a,ANT);
	printMatrix(m);

	move(0, 0, &a[0], m);





	//Init images here:

	//Deve criar quadrado 100x100 (se tirar as instrucoes abaixo)
	//DisplayObj * d1 = newImage(NULL, NULL, 0, 0); // default (w=100, h=100)

	//Pode alterar dimensoes da imagem (rect)
	//d1->img->w = 300;

	//Pode alterar a cor da imagem (vermelho)
	//d1->img->g=0;
	//d1->img->b=0;

	//Deve colocar texto na tela.
	DisplayObj * d2 = newText(NULL, "OLA, ANTS!", 0, -100, NULL);

	//Texto pode ser alterado a qualquer momento.
	changeText(d2, "abcdef");

	//Pode alterar cor do texto (amarelo)
	d2->txt->b=0;

	//E finalmente, colocar uma imagem na tela, possuindo as propriedades do retangulo 'd1' acima.
	//DisplayObj * d3 = newImage(NULL, "ant.png", 50, 50);

	/*
		//Remove todas as imagens
		removeGroup(d1->img->group);
		d1 = NULL;
		d2 = NULL; // Se for apagar tudo, nao esqueça de fazer isso! Se nao da erro! (na linha de baixo.)
		d3 = NULL;
																							 |
																							 |
		//Ou remove as imagens separadamente												 |
		removeDisplayObj(d2);								<---------------------------------
	*/






    glutMainLoop(); // Botei isso separado, senao nao tem como inicializar imagens...


    //Tudo o que estiver aqui em baixo NUNCA sera executado.
    //A nao ser que o glut nao funcione :D
	return 0;
}
