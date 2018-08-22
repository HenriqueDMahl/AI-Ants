//gcc ant.c antFunc.c -lGL -lGLU -lglut -lm
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <math.h>

#include <string.h>

#define TAM 10
#define ANT 10
#define DANT 30

typedef struct DeadAnt{
  int i, j;
}DeadAnt;

typedef struct Ant{
  int i, j;
  int carregando; // 0 ou 1
  struct DeadAnt * corpse;
}Ant;

typedef struct Matrix{
  int rows, cols;
  int **data;
}Matrix;

Matrix * newMatrix(int r, int c);
Ant * newAnt(Matrix *m, int nAnts);
DeadAnt * newDeadAnt(Matrix *m, int nDeadAnts);

void freeMatrix(Matrix *m);
