#include "ant.h"

int main(){
  Matrix *m = newMatrix(TAM,TAM);
  Ant *a = newAnt(m,ANT);
  DeadAnt *d = newDeadAnt(m,DANT);

  for(int i = 0; i<TAM; i++){
    for(int j = 0; j<TAM; j++){
      printf("%d ",m->data[i][j]);
    }
    printf("\n");
  }

  return 0;
}
