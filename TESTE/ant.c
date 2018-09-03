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

/*

	VOU TENTAR FAZER USO DE THREADS.

		O que ja foi feito ate agora:
			ants:
				randmove()
					pegar()
					   ou
					largar()
			imagens:
				funcoes para criar e deletar imagens.
				texture managment.
				criacao de remocao de grupos.
				inserir e deletar textos.

		No momento, randmove() é chamado a cada frame, dentro da funcao
		de desenhar (images.c). Logo, nao ha o uso de threads, pois ela
		simplesmente itera sobre todas as formigas, e move elas ao longo
		da matrix.

	INSERINDO THREADS:

		-> Cada formiga viva sera uma thread. Cada uma delas teria um metodo
		chamado (randmove), que, quando chamado permite mover para uma posicao
		aleatoria, e decidir se deve carregar o item que esta na posicao, ou
		largar um item que esta carregando.

		Aqui entram alguns fatores do problema do produtor - consumidor:
			-> Pegar item:
				Precisa contar todos os itens proximos. Logo no momento desta contagem, NENHUMA formiga
				pode pegar ou largar itens proximos. Neste caso, nao o conceito de executar estas funcoes,
				mas simplesmente nao podem ler, escrever ou alterar a matrix durante este tempo.
			-> Largar item:
				Mesmas condicionais acima.
			
			-> RandMove:
				Como uma formiga pode sobreescrever a outra, ou seja, pode entrar na mesma posicao
				de outra formiga, nao ha problemas aqui.

				ENTRETANTO, como existem 2 threads pelo menos, em que:
					drawImages() - funcao precisa iterar todas as imagens das formigas e desenha-las
					na posicao correta.

					randmove()   - funcao precisa alterar a posicao de uma formiga.

				Entao, cria-se um conflito. No momento em que uma thread esta gravando ou alterando
				uma coordenada de posicao de formiga, ela obrigatoriamente precisa de uma mutex para
				isso.
		O que vamos precisar no momento:
			-> MUTEX para ler/escrever matrix
			-> MUTEX para ler/escrever QUALQUER DADO referente à IMAGEM da FORMIGA.
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

	pthread_barrier_wait(&(gc->barrier));
	//Interface legal: (texto + retangulos amarelos)
	DisplayObj *r1, *r2, *t1, *t2;
	r2 = newImage(over, NULL, WIDTH/2, 60);
	r2->img->w = WIDTH*0.6+10;
	r2->img->h = 60;

	r2->img->r = 0.5;
	r2->img->g = 0.5;
	r2->img->b = 0;
	r2->img->a = 0.7;

	r1 = newImage(over, NULL, WIDTH/2, 60);
	r1->img->w = WIDTH*0.6;
	r1->img->h = 50;

	r1->img->r = 0.8;
	r1->img->g = 0.8;
	r1->img->b = 0.4;
	r1->img->a = 0.4;

	t1 = newText(over, "Pressione 'F' para printar matrix no terminal", 160, 55, GLUT_BITMAP_HELVETICA_18);
	t2 = newText(over, "Pressione 'C' para mostrar estatisticas no terminal", 150, 75, GLUT_BITMAP_HELVETICA_18);

	t1->txt->r = 0;
	t1->txt->g = 0;
	t1->txt->b = 0;

	t2->txt->r = 0;
	t2->txt->g = 0;
	t2->txt->b = 0;

    glutMainLoop();

	return 0;
}
