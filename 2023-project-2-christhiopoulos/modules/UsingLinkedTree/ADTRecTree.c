///////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT RecTree μέσω links
//
///////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "ADTRecTree.h"
#include<stdio.h>

// Προς υλοποίηση...


//  Ένα δέντρο είναι pointer σε αυτό το struct
struct rec_tree {
 Pointer value;
 RecTree left1;
 RecTree right1;
 RecTree root;
 int size;
  
};

// Δημιουργεί και επιστρέφει ένα νέο δέντρο, με τιμή (στη ρίζα) value και υποδέντρα left και right.

RecTree rectree_create(Pointer value, RecTree left, RecTree right) {
	if(value==NULL){    
		return NULL;
	}	
	RecTree tree=malloc(sizeof(struct rec_tree));	
	tree->left1=left;
    tree->right1=right;
	tree->size=1+rectree_size(tree->left1)+rectree_size(tree->right1);
	tree->value=value;
	return tree;
}

// Επιστρέφει τον αριθμό στοιχείων που περιέχει το δέντρο.

int rectree_size(RecTree tree) {
	if(tree!=REC_TREE_EMPTY){
		return tree->size;
	}
	return 0;
}


// Ελευθερώνει όλη τη μνήμη που δεσμεύει το δέντρο tree.

void rectree_destroy(RecTree tree) {
	if(tree!=REC_TREE_EMPTY){
		free(tree);
	}
	return;
}

// Επιστρέφουν την τιμή (στη ρίζα), το αριστερό και το δεξί υποδέντρο του δέντρου tree.

Pointer rectree_value(RecTree tree) {
	if(tree==REC_TREE_EMPTY){
		return NULL;
	}
	return tree->value;
}


RecTree rectree_left(RecTree tree) {
	if(tree==REC_TREE_EMPTY){
		return NULL;
	}
	return tree->left1;	
}


RecTree rectree_right(RecTree tree) {
	if(tree==REC_TREE_EMPTY){
		return NULL;
	}
	return tree->right1;	
}

