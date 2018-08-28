#include "ant.h"

extern Control * gc;

void keyBoardControl(unsigned char key, int x, int y){
	if (key == 'f' || key == 'F'){
		randMove();
	}
}
