#include "ant.h"
#include <time.h>
#include <stdlib.h>

Matrix * newMatrix(int r, int c){
  Matrix* matrix = NULL;
  matrix = (Matrix *) malloc(sizeof(Matrix));
  if (matrix == NULL)
    return NULL;

  matrix->rows = r;   //Esquecemos de fazer isso
  matrix->cols = c;

  int* bloco = (int *) malloc(sizeof(int)*r*c);
  if (bloco == NULL)
    return NULL;

  matrix->data = (int **) malloc(sizeof(int*)*r);
  if (matrix->data == NULL)
    return NULL;

  for(int i = 0; i < r; i++) {
      matrix->data[i] = bloco+i*c;
      for (int j = 0; j < c; j++){
        matrix->data[i][j] = 0;     //E isso. Por isso causava erro. Nada a ver com o srand e rand.
      }
  }

  return matrix;
}

Ant * newAnt(Matrix *m , int nAnts){
  srand(time(NULL));
  Ant * arrayAnt = (Ant *) malloc(sizeof(Ant) * nAnts);
  if (arrayAnt == NULL)
    return NULL;

  int i = 0, j = 0;
  for(int n = 0; n<nAnts; n++){
    do{
      i = rand() % m->rows;
      j = rand() % m->cols;
      // garante que nao vai sobrescrever alguma formiga viva ou morta.
      // caso nao hajam mais posicoes != 0, deve sobrescrever para nao causar loop.
      if (!hasFreePosition(m, 0))
        break;
    }while(m->data[i][j] != 0);
    
    arrayAnt[n].i = i;
    arrayAnt[n].j = j;
    arrayAnt[n].corpse = NULL;    //Isso era um ponteiro, nao a variavel de baixo :/
    arrayAnt[n].carregando = 0;
    m->data[i][j] = 1;
  }

  return arrayAnt;
}

DeadAnt * newDeadAnt(Matrix *m, int nDeadAnts){
  srand(time(NULL));
  DeadAnt * arrayDeadAnt = (DeadAnt *) malloc(sizeof(DeadAnt) * nDeadAnts);
  if (arrayDeadAnt == NULL)
    return NULL;

  int i = 0, j = 0;
  for(int n = 0; n<nDeadAnts; n++){
    do{
      i = rand() % m->rows;
      j = rand() % m->cols;
      // garante que nao vai sobrescrever alguma formiga viva ou morta.
      // caso nao hajam mais posicoes != 0, deve sobrescrever para nao causar loop.
      if (!hasFreePosition(m, 0))
        break;
    }while(m->data[i][j]!=0);
    arrayDeadAnt[n].i = i;
    arrayDeadAnt[n].j = j;
    m->data[i][j] = 2;
  }

  return arrayDeadAnt;
}

//Verifica se ha alguma posicao 'freeValue' na matriz.
//Se sim, return 1. Caso contrario, return 0.
//Usado para impedir loops na criacao de formigas.
int hasFreePosition(Matrix * m, int freeValue){
  for (int i = 0; i < m->rows; i++){
    for (int j = 0; j < m->cols; j++){
      if (m->data[i][j]!=freeValue){
        return 1;
      }
    }
  }
  return 0;
}

void freeMatrix(Matrix *m){
  free(m->data[0]);
  free(m->data);
  free(m);
}
