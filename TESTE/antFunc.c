#include "ant.h"

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

  //Inserindo constantes
  matrix->rows = ROWS;
  matrix->cols = COLS;

  pthread_mutex_init(&(matrix->mutexMatrix), NULL);

  //Inserindo metodos:
  matrix->get = matrixGet_method;
  matrix->set = matrixSet_method;

  //Inserindo campo 'data':
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
    arrayAnt[n].corpse = NULL;
    arrayAnt[n].carregando = 0;
    arrayAnt[n].grupo = 0;

    im = newImage(g, NULL, i, j);
    im->img->w = width*0.8;
    im->img->h = height*0.8;
    im->img->g = 0;
    im->img->b = 0;

    im->img->x = floor(i*width) + width/2;
    im->img->y = floor(j*height) + height/2;

    arrayAnt[n].imagem = im;
    m->data[i][j] = 1;

    arrayAnt[n].randmove = randMoveMethod;

    //é aqui, que o programa vai comecar a bugar :(
    pthread_create(&(arrayAnt[n].thread), NULL, formigaMainLoop, (void *) &(arrayAnt[n]));
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
  //POS = le do arquivo
  //for(int n = 0; n<POS; n++)
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
    im = newImage(g, NULL, i, j);
    im->img->w = width*0.6;
    im->img->h = height*0.6;
    //arrayDeadAnt[n].d1 = le do arquivo
    //arrayDeadAnt[n].d2 = le do arquivo
    //arrayDeadAnt[n].grupo = le do arquivo
    //float cor = 1/grupo;
    //im->setColor(im,cor,0,1,1);
    im->img->x = floor(i*width) + width/2;
    im->img->y = floor(j*height) + height/2;

    arrayDeadAnt[n].imagem = im;
    arrayDeadAnt[n].sendoCarregada = 0;
    m->data[i][j] = 2;

    pthread_mutex_init(&(arrayDeadAnt[n].mutexDeadAnt), NULL);
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

void matrixSet_method(Matrix * m, double v, int i, int j){
  pthread_mutex_lock(&(m->mutexMatrix));

  if (i >= 0 && i <= m->rows-1 &&
        j >= 0 && j <= m->cols-1)
    m->data[i][j] = v;

  pthread_mutex_unlock(&(m->mutexMatrix));
}

double matrixGet_method(Matrix * m, int i, int j){
  double v;
  pthread_mutex_lock(&(m->mutexMatrix));

  if (i >= 0 && i <= m->rows-1 &&
      j >= 0 && j <= m->cols-1)
    v = m->data[i][j];

  pthread_mutex_unlock(&(m->mutexMatrix));

  return v;
}

void freeMatrix(){
  free(gc->matrix->data[0]);
  gc->matrix->data[0] = NULL;
  free(gc->matrix->data);
  gc->matrix->data = NULL;
  free(gc->matrix);
  gc->matrix = NULL;
}

void printMatrix(){
    Matrix * m = gc->matrix;
  	for(int j = 0; j<m->cols; j++){
  		for(int i = 0; i<m->rows; i++){
  			printf("%d ", m->data[i][j]);
  		}
  		printf("\n");
  	}
    m = NULL;
}
