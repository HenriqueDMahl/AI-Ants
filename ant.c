#include "ant.h"

int main(int argc, char ** argv){
	Matrix *m = newMatrix(TAM,TAM);
	Ant *a = newAnt(m,ANT);
	DeadAnt *d = newDeadAnt(m,DANT);

	for(int i = 0; i<TAM; i++){
		for(int j = 0; j<TAM; j++){
			printf("%d ",m->data[i][j]);
		}
		printf("\n");
	}

	initOpengl(&argc, argv, "ants", 600, 600);	//Nada apos esta funcao sera chamada ok. isso por causa do glutMainLoop() dentro dessa funcao

	return 0;
}