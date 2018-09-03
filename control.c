#include "ant.h"

extern Control * gc;

void keyBoardControl(unsigned char key, int x, int y){
	if (key == 'f' || key == 'F'){
		printMatrix(gc->matrix);
		printf("\n");
	}
	if (key == 'c' || key == 'C'){
		int espacosLivres = 0, qtdFormigas = 0, qtdMortas = 0;
		for (int i = 0; i < ROWS; i++){
			for (int j = 0; j < COLS; j++){
				double celula = gc->matrix->get(gc->matrix, i, j);
				if (celula == 0)
					espacosLivres++;
				else if (celula == 1)
					qtdFormigas++;
				else
					qtdMortas++;
			}
		}

		printf("---------------Dados na Matrix---------------\nEspaços Livres: %i / %i\nQtd Formigas: %i / %i\nQtd Mortas: %i / %i\n",
			espacosLivres, (ROWS * COLS), qtdFormigas, ANT, qtdMortas, DANT);

	}
}
