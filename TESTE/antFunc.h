#include "ant.h"
#include <time.h>
#include <stdlib.h>

Matrix * newMatrix(Group * g);
Ant * newAnt(Group * g);
DeadAnt * newDeadAnt(Group * g);
int hasFreePosition(int freeValue);
void freeMatrix();
void randMove();
void localMove(int index, int toI, int toJ);
void printMatrix();
