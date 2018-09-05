//gcc ant.c antFunc.c images.c control.c antActions.c -pthread -lGL -lGLU -lglut -lm `sdl-config --cflags --libs` -lSDL_image -o ant
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "SDL.h"
#include "SDL_image.h"

#include <math.h>
#include <time.h>

#include <string.h>
#include <pthread.h>

#define ROWS 100 // alterado de: TAM, para: (ROWS, COLS) da matrix
#define COLS 100 // 100 x 100 = 10000 blocos

#define ANT 10			//Quantidade de formigas vivas
#define DANT 1000		//Quantidade de formigas mortas

#define FPS 60 				//Define o FPS
#define WIDTH 700			//Largura da tela
#define HEIGHT 700			//Altura  da tela
#define QTDPARAM 2


//Terminal Colors
#define GRAY		"\e[38;5;240m"
#define RESET		"\e[0m"
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
struct Matrix;
struct Ant;
struct DeadAnt;
//------------------------------------------------------------
//Linked List
typedef struct LinkedList {
	void * data;
	struct LinkedList * back, * next, * head;
}linkedList;

//------------------------------------------------------------
typedef struct DisplayObj{
	int type;					// tipo da estrutura armazenada: 0 - imagem. 1 - texto.
	struct Image * img;			// caso type == 0, isso deve ser != NULL.
	struct Text * txt;			// caso type == 1, isso deve ser != NULL.


	pthread_mutex_t mutexDisplayObj; // usado para ler ou alterar dados dentro do objeto.


	//Methods:
	void (*setPos)     (struct DisplayObj *, int, int);
	void (*setColor)   (struct DisplayObj *, int, int, int, int);
	void (*setSize)    (struct DisplayObj *, int, int);
	void (*changeText) (struct DisplayObj *, unsigned char *);

	struct DisplayObj * next, * back;// esse tipo de estrutura sera uma lista duplamente encadeada.
}DisplayObj;

typedef struct Textures {
	GLuint * texture;					// boundTextures referenciado por cada imagem carregada na memoria
	char * filename;				// filename da textura
	struct Textures * back, * next;	// isto é uma lista duplamente encadeada
}Textures;

typedef struct Image{
	float x, y,			 // coordenadas
			w, h;		 // width e height
	char * filename;	 // caso seja NULL, cria apenas um retangulo na tela.

	float r, g, b, a;	 // cores (Red, Green, Blue, Alpha). Default é (1, 1, 1, 1). range: [0..1] <= [0% .. 100%]

	float xT1, xT2, 	 // coordenadas de textura. Default é (0, 0) x (1, 1)
			yT1, yT2;	 //									 xT1 yT1  xT2 yT2

	GLuint * boundTexture;

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
	struct Textures * textureBuffer;

	float width, height;		// tamanho base dos blocos da matriz. definido pelo numero (ROWS, COLS) / (WIDTH, HEIGHT)

	pthread_barrier_t barrier;	//Barreira impede que as formigas leiam dados de formigas mortas nao inicializadas ainda.

	int pause;

	struct Matrix * matrix;			//matrix
	struct Ant * arrayAnt;			//vetor de formigas vivas
	struct DeadAnt * arrayDeadAnt;	//vetor de formigas mortas
}Control;

//-----------------------------------------------------------------------------
//Ant Function Structs
typedef struct DeadAnt{
  int i, j, sendoCarregada;
	int grupo;
	float d1,d2;
  DisplayObj *imagem;


  pthread_mutex_t mutexDeadAnt;
}DeadAnt;

typedef struct Ant{
  int i, j;
  int carregando; // 0 ou 1
  struct DeadAnt * corpse;

  void (*randmove) (struct Ant *);
  pthread_t thread;

  DisplayObj *imagem;
}Ant;

typedef struct Matrix{
  int rows, cols;
  int **data;

  //Methods:
  void   (*set) (struct Matrix *, double, int, int);
  double (*get) (struct Matrix *, int, int);

  pthread_mutex_t mutexMatrix;	//usada para escrever ou ler dados referentes à matrix->data
}Matrix;
//-----------------------------------------------------------------------------
// Ant Functions
Matrix 	* newMatrix(Group * g);						//Cria nova matrix
Ant 	* newAnt(Group * g);						//Instancializa n formigas, e retorna vetor com as instancias
DeadAnt * newDeadAnt(Group * g);					//Instancializa n formigas mortas, e retorna vetor com as instancias

void freeMatrix();										//Libera a matrix.
void freeAnt();											//Libera o vetor de formigas vivas
void freeDeadAnt();										//Libera o vetor de formigas mortas

void randMove(); 										//Faz o movimento randomico das formigas (AINDA NÃO ESTÁ EM PARALELO)
void printMatrix(); 									//Printa a matrix no terminal
void localMove(int index, int toI, int toJ);			//Faz o movimento de 1 formiga especifica (requer index).

int hasFreePosition(int freeValue);		//Verifica se as posicoes na matrix possuem 0.
										//usado apenas na criacao de formigas.
int pegar(Ant *a, int x, int y);

void   matrixSet_method(Matrix * m, double v, int i, int j);  //Eu acredito que nao usei estes metodos..
double matrixGet_method(Matrix * m, int i, int j);			  // mas caso sejam necessarios, estao aqui.

void   randMoveMethod(Ant * a);
void * formigaMainLoop(void * p);
//-----------------------------------------------------------------------------
//My OpenGl based API
void draw();											//Desenhar a cada frame (60 FPS - default)
void initOpengl(int * argc, char ** argv, char * name);	//Configuracoes iniciais do opengl (2D)
void drawImages();
void imageManagement();
void insertIntoGroup(Group * g, DisplayObj * d);
void changeText(DisplayObj * disp, unsigned char * newText);
void fpsControl(int v);

Group 		* newGroup();
DisplayObj 	* newImage(Group * group, char * filename, float x, float y);
DisplayObj 	* newText(Group * group, unsigned char * text, float x, float y, void * font);

void printGroup(Group * g);
void removeDisplayObj(DisplayObj * disp);
void removeGroup(Group * group);

void insertDisplayObjMethods(DisplayObj * d);
void setColor(DisplayObj * d, int r, int g, int b, int a);
void setPos(DisplayObj * d, int x, int y);
void setSize(DisplayObj * d, int w, int h);
void setText(DisplayObj * t, unsigned char * newText);

//Texture functions
int * createTexture(Image * img);		//Cria e insere a textura no vetor de texturas, caso ela nao exista.
void  removeTexture();									//Remove todas as texturas alocadas ate o momento.

//Controlers
void keyBoardControl(unsigned char key, int x, int y);
