#include "ant.h"

struct Control * gc = NULL;

void draw(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	imageManagement();
	drawImages();

	glutSwapBuffers();
}

void fpsControl(int v){
	glutPostRedisplay();
	glutTimerFunc(1000/FPS, fpsControl, 0);
}

void initOpengl(int * argc, char ** argv, char * name, int width, int height){
	//Alocando estrutura de controle
	gc = (Control *) malloc(sizeof(Control));
	if (gc == NULL){
		printf("Erro em alocar estrtutura de controle\n");
		exit(0);
	}
	gc->globalGroup = (Group *) malloc(sizeof(Group));
	if (gc->globalGroup == NULL)
		exit(0);
	gc->globalGroup->imageBuffer = NULL; // deve comecar com buffer de imagens vazio.
	gc->groupBuffer = NULL;


	//Configuracoes de contexto
	glutInit(argc, argv);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(200, 100);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow(name);

	//Configuracoes de display
	glutDisplayFunc(draw);

	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-width/2, width/2, height/2, -height/2, 0, 1);
	
	glMatrixMode(GL_MODELVIEW);
	glShadeModel( GL_SMOOTH );
	glClearDepth( 1.0f );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glClearColor(0.0, 0.0, 0.0, 1.0);

	//Enable use of textures 2d
    glEnable(GL_TEXTURE_2D);

    //Enable transparency - Uso isso para carregar imagens RGBA
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Funcao customizada para controlar FPS. Apenas usa timers do proprio glut.
    fpsControl(0);

    glutMainLoop();
}

//Funcao que sera usada para gerenciar criacao / destruicao de imagens.
//So vou implementar mesmo se for realmente necessario criar imagens em threads.
//Se apenas inicializar as variaveis na main, isso nao sera necesario.
// -- Funcionamento esta escrito na OBS de "ant.h"
void imageManagement(){
	return;
}

void showImages(Group * buffer){
	DisplayObj * d = NULL;
	for (d = buffer->imageBuffer; d != NULL; d = d->next){
		//Renderizando imagem
		printf("here\n");
		if (!d->type){
			//Imagem
			if (d->img->filename != NULL){
				glBindTexture(GL_TEXTURE_2D, d->img->boundTexture);
			}

			glBegin(GL_TRIANGLES);
				glNormal3d(0, 0, 1);
				glTexCoord2f(d->img->xT1, d->img->yT1); glVertex2f(d->img->x - d->img->w/2 + buffer->x, d->img->y - d->img->h/2 + buffer->y);
				glTexCoord2f(d->img->xT1, d->img->yT2); glVertex2f(d->img->x - d->img->w/2 + buffer->x, d->img->y + d->img->h/2 + buffer->y);
				glTexCoord2f(d->img->xT2, d->img->yT2); glVertex2f(d->img->x + d->img->w/2 + buffer->x, d->img->y + d->img->h/2 + buffer->y);
				
				glTexCoord2f(d->img->xT1, d->img->yT1); glVertex2f(d->img->x - d->img->w/2 + buffer->x, d->img->y - d->img->h/2 + buffer->y);
				glTexCoord2f(d->img->xT2, d->img->yT1); glVertex2f(d->img->x + d->img->w/2 + buffer->x, d->img->y - d->img->h/2 + buffer->y);
				glTexCoord2f(d->img->xT2, d->img->yT2); glVertex2f(d->img->x + d->img->w/2 + buffer->x, d->img->y + d->img->h/2 + buffer->y);
			glEnd();
		}else if (d->type == 1){
			//Texto
			int len = strlen(d->txt->text);
			glRasterPos2f(d->txt->x + buffer->x, d->txt->y + buffer->y); // parece que a posicao é (left, bottom)

			for (int i = 0; i < len; i++)
				glutBitmapCharacter(GLUT_BITMAP_8_BY_13, d->txt->text[i]);
		}
	}
	d = NULL;
}

//Funcao chamada todo frame.
void drawImages(){
	Group * buffer = NULL;

	//Mostrar imagens de todos os grupos.
	for (buffer = gc->groupBuffer; buffer != NULL; buffer = buffer->next){
		showImages(buffer);		
	}
	//No final, mostrar imagens do grupo global.
	showImages(gc->globalGroup);

	buffer = NULL;
}

//--------------------------------------------------------------------------------------------
//Criacao de imagens, textos e grupos.
//	OBS: Nao chame esses caras em threads, pode dar crash.
//			-- Unica funcao na real que vai dar crash seria glGenTextures(); dentro de uma thread.
//			-- Logo, se a funcao chamada nao tiver isso, nao vai bugar.

//Outro dia implemento isso.

Group * newGroup(){
	return NULL;
}


Image * newImage(){
	return NULL;
}

Text * newText(){
	return NULL;
}