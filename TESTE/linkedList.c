#include "ant.h"

LinkedList * linkedList_new(void * obj){
  LinkedList * l = (LinkedList *) malloc(sizeof(LinkedList));
  if (!l)
    return NULL;

  l->data = obj;
  l->next = NULL;
  l->back = NULL;

  return l;
}

void linkedList_add(LinkedList * list, void * obj){
  LinkedList * tmp = list;
  if (list->obj == NULL)
    list->obj = obj;
  else{
    while (tmp->next->obj != NULL){
      tmp = tmp->next;
    }
    tmp->next->obj = obj;
    tmp->next->back = tmp;
  }
}
