//gcc ant.c antFunc.c images.c -lGL -lGLU -lglut -lm `sdl-config --cflags --libs` -lSDL_image -o ant
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "SDL.h"
#include "SDL_image.h"

#include <math.h>

#include <string.h>

#define TAM 10
#define ANT 10
#define DANT 30

#define FPS 60
#define WIDTH 600
#define HEIGHT 600

//-----------------------------------------------------------------------------
//My OpenGl base API structs:
/*
		Basicamente isso é uma copia da API de display do Corona SDK, uma framework em Lua
		com alvo para Android que eu uso.

		Foi feita a partir de OpenGL, logo é bem facil traduzir alguns conceitos pra ca.

		Fiz somente o basico, problema que vc nao pode criar imagens explicitamente em
		threads.


			Group:
				-- Pode armazenar varias display objects.
				-- Alterar a posicao de um grupo altera a posicao global das imagens,
				logo image.x e image.y nao serao alterados.

			Display Object:
				-- Como vc pode instancializar Textos e Imagens no programa, e ambos
				devem estar dentro de grupos em uma certa ordem de renderizacao,
				basicamente Imagens e Textos sao Display Objects.

			Image:
				-- Armazena informacoes locais da imagem.
				-- Pode pertencer a um grupo ou nao. Caso nao pertença, existe um grupo
				'global' que armazena todas as imagens sem grupo. Este grupo sempre esta
				no topo.

		OBS: -- AINDA NAO ESTA IMPLEMENTADO!
			Praticamente o problema do Consumidor x Produtor que a gente vê em SOP:
				-- Vou tentar depois fazer um pequeno gerenciador em algum local, para
				que quando quiser criar alguma imagem em uma thread, nada mais do que
				encha algum buffer nessa thread, e na outra (em que o opengl esta
				executando) ele esvazie o buffer e coloque a imagem na tela.

				-- Assim, deve ter um jeito de implementar threads e criar imagens ao mesmo tempo.


				-- Na verdade nao precisaria disso tudo, mas como funcao de timer do glut
				recebe uma funcao do tipo:
					int func(int v){

					}
				Eu nao sei se é seguro passar um ponteiro de qqr tipo pra essa funcao:
					struct ABC * p;
					func(&p);
				Eu acho.. que pode bugar, mas nao tenho certeza mesmo..
				testei antes com algumas funcoes e ate funciona.. mas vai saber.
				O gcc reclama disso ao compilar, dando um Warning.
*/
struct Group;
struct Image;
struct Text;

typedef struct DisplayObj{
	int type;					// tipo da estrutura armazenada: 0 - imagem. 1 - texto.
	struct Image * img;			// caso type == 0, isso deve ser != NULL.
	struct Text * txt;			// caso type == 1, isso deve ser != NULL.

	struct DisplayObj * next, * back;// esse tipo de estrutura sera uma lista duplamente encadeada.
}DisplayObj;

typedef struct Image{
	float x, y,			 // coordenadas
			w, h;		 // width e height
	char * filename;	 // caso seja NULL, cria apenas um retangulo na tela.
	float r, g, b, a;	 // cores (Red, Green, Blue, Alpha). Default é (1, 1, 1, 1). range: [0..1] <= [0% .. 100%]

	float xT1, xT2, 	 // coordenadas de textura. Default é (0, 0) x (1, 1)
			yT1, yT2;	 //									 xT1 yT1  xT2 yT2

	int boundTexture;

	struct Group * group;// referencia ao grupo atual. Caso foi criado sem grupo, aponta para o grupo global.
}Image;

typedef struct Text{
	float x, y;			 // Coordenadas (Left, Bot). Porque o proprio glut implementa assim, nao é culpa minha.
	unsigned char * text;// texto que deve ser mostrado. Alterar isso a qqr momento, altera o texto na tela.
	float r, g, b, a;	 // cores (Red, Green, Blue, Alpha). Default é (1, 1, 1, 1). range: [0..1] <= [0% .. 100%]

	void * font;		 // fonte de texto. Eu uso normalmente: GLUT_BITMAP_8_BY_13
						 // nesse site tem mais: https://www.opengl.org/resources/libraries/glut/spec3/node76.html

	struct Group * group;// referencia ao grupo atual. Caso foi criado sem grupo, aponta para o grupo global.
}Text;

typedef struct Group{
	float x, y;
	int isGlobalGroup;		//Alterar esta opcao para 1 impede vc de apagar um grupo!
	struct DisplayObj * imageBuffer;

	struct Group * next, * back; // esse tipo de estrutura sera uma lista duplamente simples.
}Group;

// struct de controle geral, declarada em initOpengl()
// nao ha como inserir imagens sem que esta variavel esteja disponivel.
typedef struct Control{
	struct Group * globalGroup;	// globalGroup->next deve apontar para NULL sempre. Alterar isso nao resulta em nada no momento.
	struct Group * groupBuffer;	// groupBuffer possui todos os grupos criados.
}Control;

//-----------------------------------------------------------------------------
//Ant Function Structs
typedef struct DeadAnt{
  int i, j;
	DisplayObj *imagem;
}DeadAnt;

typedef struct Ant{
  int i, j;
  int carregando; // 0 ou 1
  struct DeadAnt * corpse;
	DisplayObj *imagem;
}Ant;

typedef struct Matrix{
  int rows, cols;
  int **data;
}Matrix;
//-----------------------------------------------------------------------------
// Ant Functions
Matrix * newMatrix(int r, int c);					//Cria nova matrix
Ant * newAnt(Matrix *m, int nAnts);					//Instancializa n formigas, e retorna vetor com as instancias
DeadAnt * newDeadAnt(Matrix *m, int nDeadAnts);		//Instancializa n formigas mortas, e retorna vetor com as instancias
void freeMatrix(Matrix *m);							//Libera a matrix.
void randMove(Matrix *m,Ant *a, int n); //Faz o movimento randomico das formigas (AINDA NÃO ESTÁ EM PARALELO)
void printMatrix(Matrix *m); //Printa a matrix no terminal
void move(int i, int j, Ant *a, Matrix *m);
													//Vetor de formigas (mortas ou nao) podem ser
													//	liberados diretamente usando free()
int hasFreePosition(Matrix * m, int freeValue);

//-----------------------------------------------------------------------------
//My OpenGl based API
void draw();											//Desenhar a cada frame (60 FPS - default)
void initOpengl(int * argc, char ** argv, char * name);	//Configuracoes iniciais do opengl (2D)
void drawImages();
void imageManagement();
void insertIntoGroup(Group * g, DisplayObj * d);
void changeText(DisplayObj * disp, unsigned char * newText);

Group * newGroup();
DisplayObj * newImage(Group * group, char * filename, float x, float y);
DisplayObj * newText(Group * group, unsigned char * text, float x, float y, void * font);

void removeDisplayObj(DisplayObj * disp);
void removeGroup(Group * group);
