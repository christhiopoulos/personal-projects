

#include <stdlib.h>
#include "ADTCompTree.h"
#include<stdio.h>
#include "ADTRecTree_utils.h"
// Προς υλοποίηση...


//  Ένα δέντρο είναι pointer σε αυτό το struct
struct comp_tree {
 Pointer value;
 RecTree left1;
 RecTree right1;
 RecTree root;
 int size;
};


// Δημιουργεί και επιστρέφει ένα νέο δέντρο, με τιμή (στη ρίζα) value και υποδέντρα left και right.
CompTree comp_tree_create(Pointer value, CompTree left, CompTree  right) {
	RecTree tree=rectree_create(value,(RecTree)left,(RecTree)right);
	return (CompTree)tree;
}


// Επιστρέφει τον αριθμό στοιχείων που περιέχει το δέντρο.
int comp_tree_size(CompTree tree) {
	int tree2=rectree_size((RecTree)tree);
    return tree2;
}


// Ελευθερώνει όλη τη μνήμη που δεσμεύει το δέντρο tree.
void comp_tree_destroy(CompTree tree) {
    rectree_destroy((RecTree)tree);
}


// Επιστρέφουν την τιμή (στη ρίζα), το αριστερό και το δεξί υποδέντρο του δέντρου tree.
Pointer comp_tree_value(CompTree tree) {
	Pointer tree_val=rectree_value((RecTree)tree);
	return tree_val;
}


CompTree comp_tree_left(CompTree tree) {
	RecTree tree2=rectree_left((RecTree)tree);
	return (CompTree)tree2;
}


CompTree comp_tree_right(CompTree tree) {
	RecTree tree2=rectree_right((RecTree)tree);
	return (CompTree)tree2;
}


CompTree comp_tree_get_subtree(CompTree tree,int pos){
	RecTree tree2=rectree_get_subtree((RecTree) tree,pos);
	return (CompTree)tree2;
}


CompTree comp_tree_replace_subtree(CompTree tree,int pos,CompTree subtree){
	RecTree tree2=rectree_replace_subtree((RecTree) tree,pos,(RecTree) subtree);
	return (CompTree)tree2;
}


CompTree comptree_insert_last(CompTree tree, Pointer value){
	int size=0;
	if(tree==NULL){
		size=0;
	}
	else{
 		size=comp_tree_size(tree);
	}
	CompTree tree2=comp_tree_create(value,NULL,NULL);
	CompTree tree3=comp_tree_replace_subtree(tree, size, tree2);   //βαζουμε στην τελευταια θεση του tree το νεο δεντρο που δημιοργησαμε
	return tree3;
}


CompTree comptree_remove_last(CompTree tree){
	int tree_size=comp_tree_size(tree);
	CompTree tree2=comp_tree_replace_subtree(tree,tree_size-1,NULL);  //το τελευταιο subtree του tree γινεται NULL και καταστεφεται (μεσω της replace)
	return tree2;
}