///////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT RecTree μέσω links
//
///////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "ADTRecTree.h"
#include<stdio.h>
#include"ADTMap.h"
// Προς υλοποίηση...


//  Ένα δέντρο είναι pointer σε αυτό το struct

int compare_address(Pointer a, Pointer b) {       //η συναρτηση που χρειαζεται για να διαταξει τα στοιχεια στην pqueue και στο set
return a - b;
}


Map map_val;
Map map_left;
Map map_right;
Map map_siz;
Pointer size;
Pointer i=(void*)0;   // o pointer δειχνει στη μνημη με τιμη 0
int flag=0;   //το flag χρειαζεται για να δημιουργηθουν τα maps μονο την πρωτη φορα που καλειται η create

// Δημιουργεί και επιστρέφει ένα νέο δέντρο, με τιμή (στη ρίζα) value και υποδέντρα left και right.

RecTree rectree_create(Pointer value, RecTree left, RecTree right) {
  if(value==NULL){
    return NULL;
  }
  i++;
  if(flag==0){
    flag=1;
    map_val=map_create(compare_address,NULL,NULL);
    map_left=map_create(compare_address,NULL,NULL);
    map_right=map_create(compare_address,NULL,NULL);
    map_siz=map_create(compare_address,NULL,NULL);
  }
  RecTree tree=(RecTree)i;
  int size=rectree_size(left)+ rectree_size(right)+1;
  map_insert(map_siz,i,(int*)(size_t)size);
  map_insert(map_val,i,value);
  map_insert(map_left,i,left);
  map_insert(map_right,i,right);
  return tree;   
}


// Επιστρέφει τον αριθμό στοιχείων που περιέχει το δέντρο.
int rectree_size(RecTree tree) {
  if(tree!=REC_TREE_EMPTY){
    return (size_t)map_find(map_siz,tree);
  }
  return 0;
}


// Ελευθερώνει όλη τη μνήμη που δεσμεύει το δέντρο tree.

void rectree_destroy(RecTree tree) {
  if(tree==REC_TREE_EMPTY){
    return;
  }
  map_remove(map_val,tree);
  if(map_size(map_val)==0){
    map_destroy(map_siz);
    map_destroy(map_val);
    map_destroy(map_left);
    map_destroy(map_right); 
    flag=0;
  }
	return ;

}

// Επιστρέφουν την τιμή (στη ρίζα), το αριστερό και το δεξί υποδέντρο του δέντρου tree.

Pointer rectree_value(RecTree tree) {
  if(tree==REC_TREE_EMPTY){
    return NULL;
  }
	return map_find(map_val,tree);
}

RecTree rectree_left(RecTree tree) {
  if(tree==REC_TREE_EMPTY){
    return NULL;
  }
	return map_find(map_left,tree);
}

RecTree rectree_right(RecTree tree) {
  if(tree==REC_TREE_EMPTY){
    return NULL;
  }
	return map_find(map_right,tree);
}

