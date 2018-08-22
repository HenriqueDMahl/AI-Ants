#include "ant.h"
#include <time.h>
#include <stdlib.h>

Matrix * newMatrix(int r, int c){
  Matrix* matrix = NULL;
  matrix = malloc(sizeof(Matrix));
  int* bloco = malloc(sizeof(int)*r*c);
  matrix->data = malloc(sizeof(int*)*r);
  for(int i = 0; i <= r; i++) {
      matrix->data[i] = bloco+i*c;
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
      i = random() % m->rows;
      j = random() % m->cols;
    }while(m->data[i][j] != 2);
    arrayAnt[n].i = i;
    arrayAnt[n].j = j;
    arrayAnt[n].corpse = 0;
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
    }while(m->data[i][j] != 1);
    arrayDeadAnt[n].i = i;
    arrayDeadAnt[n].j = j;
    m->data[i][j] = 2;
  }

  return arrayDeadAnt;
}

void freeMatrix(Matrix *m){
  free(m->data[0]);
  free(m->data);
  free(m);
}
