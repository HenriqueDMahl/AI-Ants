#include "ant.h"

extern Control * gc;


/*

    Erros que observei:
      --> Algumas formigas conseguem pegar itens das outras formigas, mesmo que ja
      estejam carregando...

*/

float contarEmVolta2(int x, int y, int radius, DeadAnt *d){
  Matrix * m = gc->matrix;
  int cont = 0;
  float f = 0.0;
  int max = (radius*2 + 1)*(radius*2 + 1) - 1;
  //Percorrendo a area em buscas de itens proximos
  for (int i = x-radius; i <= x+radius; i++){
    for (int j = y-radius; j <= y+radius; j++){
      //Condicional para nao sair dos limites da matrix
      if (i >= 0 && j >= 0 && i < m->rows && j < m->cols && !(i == x && j == y)){
        //Contar a quantidade de formigas mortas visiveis para a formiga
        if (m->data[i][j] == 2){ // se possui formigas mortas disponiveis em sua volta:
          cont++;
        }

      }

    }
  }
  for(int i = 0; i < QTDPARAM; i++){
    f += (1 - (sqrt((pow(x-i,2)+pow(y-j,2))))/d->d[i]);// TRANSFORMAR EM VETOR 
  }
  return f*(1/max*max);
}

int contarEmVolta(int x, int y, int radius){
  Matrix * m = gc->matrix;
  int cont = 0;
  //Percorrendo a area em buscas de itens proximos
  for (int i = x-radius; i <= x+radius; i++){
    for (int j = y-radius; j <= y+radius; j++){
      //Condicional para nao sair dos limites da matrix
      if (i >= 0 && j >= 0 && i < m->rows && j < m->cols && !(i == x && j == y)){

        //Contar a quantidade de formigas mortas visiveis para a formiga
        if (m->data[i][j] == 2){ // se possui formigas mortas disponiveis em sua volta:
          cont++;
        }

      }

    }
  }
  return cont;
}


//Recebe a formiga na posicao antiga
//Recebe (x, y), posicoes novas
//Deve decidir se a formiga deve pegar o item, que com certeza estara na mesma posicao (x, y)
//Se a formiga pega o item nesta chamada, return TRUE, senao FALSE.
int pegar(Ant *a, int x, int y){
  Matrix  * m = gc->matrix;
  DeadAnt * d = gc->arrayDeadAnt, *n=NULL;
  int cont = 0;
  float chance = 0.0;
  int radius = 1;
  float max = (radius*2 + 1)*(radius*2 + 1);

  //Se a formiga nao esta carregando, entao ha uma change de pegar item
  if(!a->carregando){

    cont = contarEmVolta(x, y, radius);
    // f = (1/64)*
    chance = 1 - (cont/max) + 0.01;
    /*
    for (int k = 0; k<DANT; k++){
      pthread_mutex_lock(&(d[k].mutexDeadAnt));
      if (d[k].i == x && d[k].j == y && !d[k].sendoCarregada){
        contarEmVolta2(x,y,radius,&d[k]);
        n = &(d[k]);
        n->sendoCarregada = 1;
        pthread_mutex_unlock(&(d[k].mutexDeadAnt));
        break;
      }
      pthread_mutex_unlock(&(d[k].mutexDeadAnt));
    }

    if (n != NULL){
        a->corpse = n;
        a->carregando = 1;
        m->data[x][y] = 0;
        return 1;//Formiga esta carregando item a partir de agora.
    }
  }
    */
    if( ( rand() % 100 < chance * 100 )){
      // pegar referencia da formiga no centro (mas ela nao pode ja estar sendo carregada)

      for (int k = 0; k<DANT; k++){
        pthread_mutex_lock(&(d[k].mutexDeadAnt));
        if (d[k].i == x && d[k].j == y && !d[k].sendoCarregada){
          n = &(d[k]);
          n->sendoCarregada = 1;
          pthread_mutex_unlock(&(d[k].mutexDeadAnt));
          break;
        }
        pthread_mutex_unlock(&(d[k].mutexDeadAnt));
      }

      if (n != NULL){
          a->corpse = n;
          a->carregando = 1;
          m->data[x][y] = 0;
          return 1;//Formiga esta carregando item a partir de agora.
      }
    }
  }
  return 0; // nao chegou a carregar nada, ou ja estava carregando antes.
}

/*
    Condicionais:

    ->Formiga pode pegar o item e larga-lo no mesmo local
    ->Formiga nao pode largar item em uma posicao que ja exista um item largado
    ->Formiga pode lagar item em cima de outra formiga.

    ->Formula para largar item: (chance = cont/max + 0.01)
*/
int largar(Ant * a, int x, int y){
  Matrix  * m = gc->matrix;
  int cont = 0, radius = 1;
  float chance = 0.0f, max = (radius*2 + 1)*(radius*2+1);
  if (a->carregando){
    cont = contarEmVolta(x, y, radius);
    chance = ((float) cont)/max + 0.01f;

    if (rand() % 100 < chance * 100){
      //Formiga possui referencia à formiga morta: 'corpse'
      pthread_mutex_lock(&(a->corpse->mutexDeadAnt));
      a->corpse->sendoCarregada = 0;
      a->corpse->i = x;
      a->corpse->j = y;
      pthread_mutex_unlock(&(a->corpse->mutexDeadAnt));

      a->corpse->imagem->setPos(a->corpse->imagem, x*gc->width + gc->width/2, y*gc->width + gc->width/2);
      a->corpse->imagem->setColor(a->corpse->imagem, 1, 1, 1, 1);

      a->carregando = 0;
      a->corpse = NULL;

      m->data[x][y] = 2;
      return 1; // largou item.
    }
  }

  return 0; // continua carregando item
}

void randMove(){
  //srand(time(NULL));
  Matrix  * m = gc->matrix;
  Ant     * a = gc->arrayAnt;

  int i = 0, j = 0;
  for(int k = 0; k<ANT; k++){
    //Escolher posicao aleatoria [-1, 1] (sempre se movimenta).
    //Loop garante que as posicoes nao saem para fora da matriz.
    do{
      //Mudancas nesta formula.. antes elas so se moviam para a diagonal.
      i = (rand() % 3) - 1; // Possibilidades ==> (-1, 0, +1)
      j = (rand() % 3) - 1; // possuem chances de ficar paradas.
    }while(!(a[k].i + i >= 0         &&
             a[k].i + i <= m->rows-1 &&
             a[k].j + j >= 0         &&
             a[k].j + j <= m->cols-1));

    //oldPosition = Valor da matriz na posicao anterior (antes de se mover)
    int * oldPosition = &(m->data[a[k].i][a[k].j]), *newPosition = &(m->data[a[k].i+i][a[k].j+j]);
    int oldI = a[k].i,   oldJ = a[k].j;
    int newI = a[k].i+i, newJ = a[k].j+j;
    //Caso a posicao atual da matriz garanta que ha uma formiga morta:
    if((*newPosition) == 2){
      pegar(&(a[k]), newI, newJ);
    }else{
      //Caso nao haja nenhuma formiga na nova posicao, largar nela.
      largar(&(a[k]), newI, newJ);
    }

    //Quando sair da posicao antiga, se tiver 1, mudar para 0 'nada'.
    if ((*oldPosition) == 1)
    	m->data[oldI][oldJ] = 0;

    //Atualizar posicao da formiga
    a[k].i = newI;
    a[k].j = newJ;

    //Caso a formiga esta carregando item, a posicao nova dela deve ser = 1.
    //Alem disso, deve atualizar a posicao da formiga
    if (a[k].carregando){
      a[k].corpse->i = newI;
      a[k].corpse->j = newJ;
    }

    //Apos movimentar: se nao houver nada na proxima posicao:
    if (!(*newPosition))
      m->data[newI][newJ] = 1;

  	//Move a imagem da formiga, e de sua formiga morta, caso esteja carregando.
    localMove(k, newI, newJ);
  }

  m = NULL;
  a = NULL;
}


//Move a imagem da formiga, e a de que a formiga estaria segurando.
void localMove(int index, int toI, int toJ){
    Ant * a = &(gc->arrayAnt[index]);

    a->imagem->img->x = toI*gc->width  + gc->width/2;
    a->imagem->img->y = toJ*gc->height + gc->height/2;

    if (a->carregando){
      a->corpse->imagem->img->x = toI*gc->width  + gc->width/2;
      a->corpse->imagem->img->y = toJ*gc->height + gc->height/2;

      a->corpse->imagem->img->g = 0;
      a->corpse->imagem->img->r = 0;
    }

    a = NULL;
}


///-------------------------------------------------------------------------
//Funcoes e metodos usados nas threads.


void randMoveMethod(Ant * a){
  //srand(time(NULL));
  Matrix * m = gc->matrix;
  int i = 0, j = 0;
  //Escolha uma posicao aleatoria:
  do{
      i = (rand() % 3) - 1;
      j = (rand() % 3) - 1;
    }while(
          !( a->i + i >= 0         &&
             a->i + i <= m->rows-1 &&
             a->j + j >= 0         &&
             a->j + j <= m->cols-1   )
    );

  int oldI = a->i,     oldJ = a->j;
  int newI = a->i + i, newJ = a->j + j;

  //Aqui deve ocorrer bloquamento pois uma formiga nao pode
  //  pegar ou largar itens ao mesmo tempo.

  pthread_mutex_lock(&(m->mutexMatrix));

  if (m->data[newI][newJ] == 2){
    pegar(a, newI, newJ);
  }else{
    largar(a, a->i + i, a->j + j);
  }

  if (m->data[oldI][oldJ] == 1)
    m->data[oldI][oldJ] = 0;

  if (m->data[newI][newJ] == 0)
    m->data[newI][newJ] = 1;

  pthread_mutex_unlock(&(m->mutexMatrix));

  //Atualizando a posicao da formiga:
  a->i = newI;
  a->j = newJ;

  //Retirei a funcao localmove(), pois fica redundante..
  a->imagem->setPos(a->imagem, newI*gc->width + gc->width/2, newJ*gc->height + gc->height/2);
  if (a->carregando){
    a->corpse->imagem->setPos(a->corpse->imagem, newI*gc->width + gc->width/2, newJ*gc->height + gc->height/2);


    pthread_mutex_lock(&(a->corpse->mutexDeadAnt));
    a->corpse->i = a->i;
    a->corpse->j = a->j;
    pthread_mutex_unlock(&(a->corpse->mutexDeadAnt));
  }
}


//Thread de cada formiga que comecara a ser executada assim que elas forem criadas.
void * formigaMainLoop(void * p){
  Ant * a = (Ant *) p;
  if (a == NULL)
    return NULL;

  pthread_barrier_wait(&(gc->barrier));

  while (1){
    a->randmove(a);
  }
}
