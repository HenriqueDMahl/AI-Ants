#include "ant.h"

//Caso precise usar esta variavel na main, use palavra reservada 'extern'.
//Aqui é onde esta variavel é declarada, na funcao initOpengl().

struct Control * gc = NULL;


void draw(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	imageManagement();
	drawImages();

	glutSwapBuffers();
}

void fpsControl(int v){
	if (!gc->pause){
		glutPostRedisplay();
		glutTimerFunc(1000/FPS, fpsControl, 0);
	}
}

void initOpengl(int * argc, char ** argv, char * name){
	//Alocando estrutura de controle
	gc = (Control *) malloc(sizeof(Control));
	if (gc == NULL){
		printf("Erro em alocar estrtutura de controle\n");
		exit(0);
	}
	gc->globalGroup = (Group *) malloc(sizeof(Group));
	if (gc->globalGroup == NULL)
		exit(0);
	gc->globalGroup->x = 0;
	gc->globalGroup->y = 0;
	gc->globalGroup->isGlobalGroup = 1;
	gc->globalGroup->next = NULL;
	gc->globalGroup->imageBuffer = NULL; // deve comecar com buffer de imagens vazio.
	gc->textureBuffer = NULL;

	gc->groupBuffer = NULL;
	gc->width = WIDTH / ROWS;
	gc->height = HEIGHT / COLS;
  gc->pause = 0;
	pthread_barrier_init(&(gc->barrier), NULL, ANT+1);


	//Configuracoes de contexto
	glutInit(argc, argv);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(200, 100);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow(name);

	//Configuracoes de display
	glutDisplayFunc(draw);
	glutKeyboardFunc(keyBoardControl);
	glutIgnoreKeyRepeat(GL_TRUE);

	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glOrtho(-WIDTH/2, WIDTH/2, HEIGHT/2, -HEIGHT/2, 0, 1);
	glOrtho(0, WIDTH, HEIGHT, 0, 0, 1);

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
		pthread_mutex_lock(&(d->mutexDisplayObj));
		glBindTexture(GL_TEXTURE_2D, 0); // reset texture every time.
		glPushMatrix();
			if (!d->type){
				//Imagem
				glColor4f(d->img->r, d->img->g, d->img->b, d->img->a);
				if (d->img->filename != NULL){
					glBindTexture(GL_TEXTURE_2D, *(d->img->boundTexture));
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
				glColor4f(d->txt->r, d->txt->g, d->txt->b, d->txt->a);

				int len = strlen(d->txt->text);
				glRasterPos2f(d->txt->x + buffer->x, d->txt->y + buffer->y); // parece que a posicao é (left, bottom)

				for (int i = 0; i < len; i++)
					glutBitmapCharacter(d->txt->font, d->txt->text[i]);
			}
		glPopMatrix();
		pthread_mutex_unlock(&(d->mutexDisplayObj));
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


Group * newGroup(){
	Group * g = (Group *) malloc(sizeof(Group)), *tmp=NULL;
	if (g == NULL)
		return NULL;

	g->x = 0;
	g->y = 0;
	g->isGlobalGroup = 0;
	g->imageBuffer = NULL; // grupo começa vazio
	g->next = NULL;
	g->back = NULL;

	if (gc->groupBuffer == NULL){
		//Nao ha grupos alocados ainda
		gc->groupBuffer = g;
	}else{
		tmp = gc->groupBuffer;
		while (tmp->next != NULL){
			tmp = tmp->next;
		}

		tmp->next = g;
		g->back = tmp;
	}

	tmp = NULL;

	return g;
}

//newImage([group, filename], x, y, w, h);
//	caso group == NULL, insere imagem em gc->globalGroup.
//	caso filename == NULL, apenas cria um retangulo na tela.
//	x, y, sao argumentos obrigatorios.
//	width e height (w, h) sao inicializados de acordo com a imagem.
//		se a imagem tiver (400x600), w = 400, h = 600, somente ao
//		carregar a imagem...
//		Logo, se nao conseguiu carregar a imagem, ele vai mostrar
//		um retangulo 100x100, pois estas medidas valeram 100 (default).
DisplayObj * newImage(Group * group, char * filename, float x, float y){
	DisplayObj * disp = (DisplayObj *) malloc(sizeof(DisplayObj));
	Image * img = (Image *) malloc(sizeof(Image));

	if (disp == NULL || img == NULL)
		return NULL;

	if (group == NULL)
		group = gc->globalGroup;

	//Coordinates + Size
	img->x = x;
	img->y = y;
	img->w = 100;
	img->h = 100;

	//RGB Color + Alpha
	img->r = 1;
	img->g = 1;
	img->b = 1;
	img->a = 1;

	//Texture Coordinates
	img->xT1 = 0;
	img->yT1 = 0;
	img->xT2 = 1;
	img->yT2 = 1;

	//Other parameters
	img->boundTexture = 0;
	img->group = group;
	img->filename = NULL;

	//Filename copy
	int len = -1;
	if (filename != NULL)
		len = strlen(filename);

	// mesmo que passar filename == "", nao vai carregar nada.
	if (len > 0){

		img->filename = (char *) malloc(sizeof(char)*(len+1));

		//Caso a alocação desta variavel falhe, ele apenas carrega um retangulo sem textura na tela.
		if (img->filename == NULL) {len = -1; printf("Falha ao carregar a imagem\n");}
		else{
			strcpy(img->filename, filename);
			img->filename[len] = '\0';
		}
	}

	disp->next = NULL;
	disp->back = NULL;
	disp->img = img;
	disp->type = 0;

	insertDisplayObjMethods(disp);
	insertIntoGroup(group, disp);

	//Carregando textura. Fiz separado para saber se a filename esta ok.
	if (len > 0){
		img->boundTexture = createTexture(img);
	} else if (filename != NULL) {
		printf("Textura '%s' nao foi carregada corretamente!\n", filename);
	}
	return disp;
}

DisplayObj * newText(Group * group, unsigned char * text, float x, float y, void * font){
	DisplayObj * disp = (DisplayObj *) malloc(sizeof(DisplayObj));
	Text * t = (Text *) malloc(sizeof(Text));
	if (t == NULL || disp == NULL || text == NULL)
		return NULL;

	if (group == NULL)
		group = gc->globalGroup;

	int len = strlen(text);
	t->text = (unsigned char *) malloc(sizeof(unsigned char) * (len+1));
	if (t->text == NULL)
		return NULL;

	strcpy(t->text, text);
	t->text[len] = '\0';

	//Coordinates
	t->x = x;
	t->y = y;

	//RGB Colors + Alpha
	t->r = 1;
	t->g = 1;
	t->b = 1;
	t->a = 1;

	t->group = group;

	//Font
	if (font != NULL)
		t->font = font;
	else
		t->font = GLUT_BITMAP_8_BY_13;

	disp->back = NULL;
	disp->next = NULL;
	disp->txt = t;
	disp->type = 1;

	insertDisplayObjMethods(disp);
	insertIntoGroup(group, disp);

	return disp;
}

void printGroup(Group * g){
	DisplayObj * tmp = NULL;
	int counter = 0;
	for (tmp = g->imageBuffer; tmp!=NULL; tmp=tmp->next){
		printf("[%i] ", counter++);
	}
}

//Insere um DisplayObj em um Grupo.
void insertIntoGroup(Group * g, DisplayObj * d){
	if (g == NULL || d == NULL)
		return;

	DisplayObj *tmp;
	if (g->imageBuffer == NULL){
		g->imageBuffer = d;
	}else{
		tmp = g->imageBuffer;
		while (tmp->next!=NULL){
			tmp=tmp->next;
		}
		tmp->next = d;
		d->back = tmp;
	}
}

void changeText(DisplayObj * disp, unsigned char * newText){
	if (disp->type != 1 || disp->txt == NULL || newText == NULL)
		return;

	int len = strlen(newText);
	unsigned char * tmp = realloc(disp->txt->text, (len+1)*sizeof(unsigned char));

	if (tmp == NULL)
		return;


	disp->txt->text = tmp;
	strcpy(disp->txt->text, newText);
	disp->txt->text[len] = '\0';
}

//Texture managment
int * createTexture(Image * img){
	if (img->filename == NULL)
		return NULL;

	//Pesquisando se a textura ja nao existe:
	Textures * tmp = NULL, * aux = NULL;
	for (tmp=gc->textureBuffer; tmp!=NULL; tmp=tmp->next){
		if (!strcmp(img->filename, tmp->filename))
			return tmp->texture;
	}

	//Caso contrario, a textura nao existe ainda...

	tmp = (Textures *) malloc(sizeof(Textures));
	if (!tmp)
		return NULL;

	tmp->back = NULL;
	tmp->next = NULL;

	//Alocando filename da textura
	int len = strlen(img->filename);
	tmp->filename = (char *) malloc(sizeof(char)*(len+1));
	if (tmp->filename == NULL)
		return NULL;

	strcpy(tmp->filename, img->filename);
	tmp->filename[len] = '\0';
	tmp->texture = (int *) malloc(sizeof(int));

	if (tmp->texture == NULL)
		return NULL;

	SDL_Surface * tex = IMG_Load(tmp->filename);

	if (tex != NULL){
		int mode = (tex->format->BytesPerPixel == 3)? GL_RGB : (tex->format->BytesPerPixel == 4)? GL_RGBA : -1;

		glGenTextures(1, tmp->texture);

		//Textura Interligada na pilha de texturas do Opengl
		glBindTexture(GL_TEXTURE_2D, *(tmp->texture));

		//Wrapper em X e Y
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//Nao sei direito disso.. GL_Nearest mostra uma imagem pior, mas garante mais processamento.
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		if (img != NULL){
			img->w = tex->w;
			img->h = tex->h;
		}

		//Textura carregada
		glTexImage2D(GL_TEXTURE_2D, 0, mode, tex->w, tex->h, 0,
				mode, GL_UNSIGNED_BYTE, tex->pixels);

		SDL_FreeSurface(tex);
		tex = NULL;
	}else
		return NULL;

	//inserir na lista de texturas:
	if (gc->textureBuffer == NULL){
		gc->textureBuffer = tmp;
	}else{
		aux = gc->textureBuffer;
		while(aux->next!=NULL){
			aux=aux->next;
		}
		aux->next = tmp;
		tmp->back = aux;
	}
	aux = NULL;

	return tmp->texture;
}

void removeTexture(){
	//Remove todas as texturas alocadas ate o momento.
	Textures * tmp = NULL, * aux;

	if (gc->textureBuffer == NULL)
		return;

	//Chegar ate a ultima posicao da lista
	for (tmp = gc->textureBuffer; tmp->next!=NULL; tmp=tmp->next);
	//Removendo de tras para frente
	for (; tmp->back!=NULL; tmp=tmp->back){
		aux = tmp->next;
		tmp->next = NULL;

		if (aux != NULL)
			free(aux);
	}
	aux = gc->textureBuffer;
	gc->textureBuffer = NULL;
	free(aux);
	aux = NULL;
}


//Destruicao de display objects e groups:

void removeDisplayObj(DisplayObj * disp){
	//Remover todos os objetos de um grupo nao apaga ele.
	//Esta funcao reordena as listas encadeadas automaticamente.
	if (disp == NULL)
		return;

	Group * head = NULL;
	if (!disp->type && disp->img != NULL){
		//desalocando imagem
		head = disp->img->group;

		if (disp->img->filename != NULL)
			free(disp->img->filename);

		disp->img->filename = NULL;
		disp->img->group = NULL;
		free(disp->img);
		disp->img = NULL;
	}else if (disp->type == 1 && disp->txt != NULL){
		//desalocando texto


		head = disp->txt->group;

		free(disp->txt->text);

		disp->txt->text = NULL;
		disp->txt->font = NULL;
		free(disp->txt);

		disp->txt = NULL;
	}


	if (disp->back == NULL){ // head
		head->imageBuffer = disp->next;
	}else{
		//Its not in the head
		disp->back->next = disp->next;
	}

	if (disp->next != NULL){
		//Its not in the tail
		disp->next->back = disp->back;
	}
	free(disp);
	disp = NULL;
}

void removeGroup(Group * group){
	//Remover um grupo contendo imagens, faz com que todas as imagens
	//sejam deletadas tambem.
	if (group == NULL)
		return;

	while (group->imageBuffer != NULL){
		removeDisplayObj(group->imageBuffer);
	}

	if (!group->isGlobalGroup){
		if (group->back == NULL){
			//Head
			gc->groupBuffer = group->next;
		}else{
			//Not in the head
			group->back->next = group->next;
		}

		if (group->next != NULL){
			group->next->back = group->back;
		}

		free(group);
		group = NULL;
	}
}

//------------------------------------------------------------------------------------------------------------------------

void insertDisplayObjMethods(DisplayObj * d){
	pthread_mutex_init(&(d->mutexDisplayObj), NULL);
	d->setColor = setColor;
	d->setPos = setPos;
	d->setSize = setSize;
	d->changeText = changeText;
}

//Metodos dos display objects:

void setColor(DisplayObj * d, int r, int g, int b, int a){
	pthread_mutex_lock(&(d->mutexDisplayObj));

	if (d->type == 0){
		d->img->r = r;
		d->img->g = g;
		d->img->b = b;
		d->img->a = a;
	}else if (d->type == 1){
		d->txt->r = r;
		d->txt->g = g;
		d->txt->b = b;
		d->txt->a = a;
	}

	pthread_mutex_unlock(&(d->mutexDisplayObj));
}

void setPos(DisplayObj * d, int x, int y){
	pthread_mutex_lock(&(d->mutexDisplayObj));
	if (d->type == 0){
		d->img->x = x;
		d->img->y = y;
	}else if (d->type == 1){
		d->txt->x = x;
		d->txt->y = y;
	}
	pthread_mutex_unlock(&(d->mutexDisplayObj));
}

void setSize(DisplayObj * d, int w, int h){
	if (d->type != 0)
		return;
	pthread_mutex_lock(&(d->mutexDisplayObj));
	d->img->w = w;
	d->img->h = h;
	pthread_mutex_unlock(&(d->mutexDisplayObj));
}

void setText(DisplayObj * d, unsigned char * newText){
	if (d->type != 1 || newText == NULL)
		return;
	pthread_mutex_lock(&(d->mutexDisplayObj));

	int len = strlen(newText);
	unsigned char * tmp = realloc(d->txt->text, (len+1)*sizeof(unsigned char));
	if (tmp == NULL)
		return;
	d->txt->text = tmp;
	strcpy(d->txt->text, newText);
	d->txt->text[len] = '\0';

	pthread_mutex_unlock(&(d->mutexDisplayObj));
}
