#include "antFunc.h"

//Esse cara aqui (gc) possui referencia para:
//  matrix        : matrix inicializada na main
//  arrayAnt      : vetor de formigas vivas
//  arrayDeadAnt  : vetor de formigas mortas

//  width         : tamanho da celula da matrix (comprimento)
//  height        : tamanho da celula da matrix (altura)
extern Control * gc;

Matrix * newMatrix(Group * g){
  Matrix* matrix = NULL;
  Image * tmp = NULL;

  matrix = (Matrix *) malloc(sizeof(Matrix));
  if (matrix == NULL)
    return NULL;

  matrix->rows = ROWS;   //Esquecemos de fazer isso
  matrix->cols = COLS;

  int* bloco = (int *) malloc(sizeof(int)*ROWS*COLS);
  if (bloco == NULL)
    return NULL;

  matrix->data = (int **) malloc(sizeof(int*)*ROWS);
  if (matrix->data == NULL)
    return NULL;

  for(int i = 0; i < ROWS; i++) {
      matrix->data[i] = bloco+i*COLS;
      for (int j = 0; j < COLS; j++){
        matrix->data[i][j] = 0;     //E isso. Por isso causava erro. Nada a ver com o srand e rand.
        tmp = newImage(g, "matrixBlock.png", i*(gc->width) + gc->width/2, j*(gc->height) + gc->height/2)->img;
        //printf("%f, %f\n", gc->width, gc->height);
        tmp->w = gc->width;
        tmp->h = gc->height;
      }
  }

  tmp = NULL;

  return matrix;
}

Ant * newAnt(Group * g){
  srand(time(NULL));

  Matrix * m = gc->matrix;
  Ant * arrayAnt = (Ant *) malloc(sizeof(Ant) * ANT);
  DisplayObj * im = NULL;


  float width=WIDTH/m->rows, height=HEIGHT/m->cols;
  if (arrayAnt == NULL)
    return NULL;

  int i = 0, j = 0;
  for(int n = 0; n<ANT; n++){
    do{
      i = rand() % m->rows;
      j = rand() % m->cols;
      // garante que nao vai sobrescrever alguma formiga viva ou morta.
      // caso nao hajam mais posicoes != 0, deve sobrescrever para nao causar loop.
      if (!hasFreePosition(0))
        break;
    }while(m->data[i][j] != 0);

    arrayAnt[n].i = i;
    arrayAnt[n].j = j;
    arrayAnt[n].corpse = NULL;    //Isso era um ponteiro, nao a variavel de baixo :/
    arrayAnt[n].carregando = 0;
    im = newImage(g, "ant.png", i, j);
    im->img->w = width*0.8;
    im->img->h = height*0.8;

    im->img->x = floor(i*width) + width/2;
    im->img->y = floor(j*height) + height/2;

    arrayAnt[n].imagem = im;
    m->data[i][j] = 1;
  }

  m = NULL;
  im = NULL;

  return arrayAnt;
}

DeadAnt * newDeadAnt(Group * g){
  srand(time(NULL));
  Matrix * m = gc->matrix;
  DeadAnt * arrayDeadAnt = (DeadAnt *) malloc(sizeof(DeadAnt) * DANT);
  DisplayObj * im = NULL;
  float width=WIDTH/m->rows, height=HEIGHT/m->cols;
  if (arrayDeadAnt == NULL)
    return NULL;

  int i = 0, j = 0;
  for(int n = 0; n<DANT; n++){
    do{
      i = rand() % m->rows;
      j = rand() % m->cols;
      // garante que nao vai sobrescrever alguma formiga viva ou morta.
      // caso nao hajam mais posicoes != 0, deve sobrescrever para nao causar loop.
      if (!hasFreePosition(0) && m->data[i][j] != 2)
        break;
    }while(m->data[i][j]!=0);
    arrayDeadAnt[n].i = i;
    arrayDeadAnt[n].j = j;
    im = newImage(g, "ghost.png", i, j);
    im->img->w = width*0.6;
    im->img->h = height*0.6;

    im->img->x = floor(i*width) + width/2;
    im->img->y = floor(j*height) + height/2;


    arrayDeadAnt[n].imagem = im;
    m->data[i][j] = 2;
  }
  m = NULL;
  return arrayDeadAnt;
}

//Verifica se ha alguma posicao 'freeValue' na matriz.
//Se sim, return 1. Caso contrario, return 0.
//Usado para impedir loops na criacao de formigas.
int hasFreePosition(int freeValue){
  Matrix * m = gc->matrix;
  for (int i = 0; i < m->rows; i++){
    for (int j = 0; j < m->cols; j++){
      if (m->data[i][j]!=freeValue){
        return 1;
      }
    }
  }
  m = NULL;
  return 0;
}

void freeMatrix(){
  free(gc->matrix->data[0]);
  gc->matrix->data[0] = NULL;
  free(gc->matrix->data);
  gc->matrix->data = NULL;
  free(gc->matrix);
  gc->matrix = NULL;
}

void pegar(Ant *a, int x, int y){
  Matrix  * m = gc->matrix;
  DeadAnt * d = gc->arrayDeadAnt, *n=NULL;
  int cont = 0;
  float chance = 0.0;
  int radius = 2;
  int max = (radius*2 + 1)*(radius*2 + 1);

    if(a->carregando != 1){
      for (int i = x-radius; i <= x+radius; i++){
        for (int j = y-radius; j <= y+radius; j++){
          if (i >= 0 && j >= 0 && i < m->rows && j < m->cols){

            for (int k = 0; k<DANT; k++){
              if (d[k].i == i && d[k].j == j){
                cont++;
              }
              if (d[k].i == x && d[k].j == y){
                  n = &d[k];
              }
            }


          }
        }
      }
      chance = 1 - (cont/max) + 0,01;
      if(rand()%100 <= chance){
        a->carregando = 1;
        a->corpse = n;
      }
  }


}


void randMove(){
  srand(time(NULL));
  Matrix  * m = gc->matrix;
  Ant     * a = gc->arrayAnt;

  int i = 0, j = 0;
  for(int k = 0; k<ANT; k++){
    do{
      i = 2*(rand() % 2) - 1;
      j = 2*(rand() % 2) - 1;
    }while(!(a[k].i + i >= 0 && a[k].i + i <= m->rows-1 && a[k].j + j >= 0 && a[k].j + j <= m->cols-1));
    if(m->data[a[k].i + i][a[k].j + j] = 2){
      pegar(&a[k],a[k].i+i,a[k].j+j);
    }
    m->data[a[k].i][a[k].j] = 0;
    a[k].i = a[k].i + i;
    a[k].j = a[k].j + j;
    if(a[k].carregando != 1)
      m->data[a[k].i][a[k].j] = 1;

    localMove(k, a[k].i, a[k].j);
  }

  m = NULL;
  a = NULL;
}

void localMove(int index, int toI, int toJ){
    Ant * a = &(gc->arrayAnt[index]);

    a->imagem->img->x = toI*gc->width  + gc->width/2;
    a->imagem->img->y = toJ*gc->height + gc->height/2;

    if (a->carregando){
      if (a->corpse != NULL){
        a->corpse->imagem->img->x = toI*gc->width + gc->width/2;
        a->corpse->imagem->img->y = toI*gc->width + gc->width/2;
      }
    }

    a = NULL;
}

void printMatrix(){
    Matrix * m = gc->matrix;
  	for(int i = 0; i<m->rows; i++){
  		for(int j = 0; j<m->cols; j++){
  			printf("%d ",m->data[i][j]);
  		}
  		printf("\n");
  	}
    m = NULL;
}
