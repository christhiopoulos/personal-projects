

#pragma once // #include το πολύ μία φορά

#include "common_types.h"
#include "ADTCompTree.h"
// Η παρακάτω σταθερά συμβολίζει ένα κενό δέντρο
#define REC_TREE_EMPTY (RecTree)0



//
typedef struct comp_tree* CompTree;


CompTree comp_tree_create(Pointer value,  CompTree left,  CompTree right);

int  comp_tree_size( CompTree tree);

void comp_tree_destroy( CompTree tree);

Pointer comp_tree_value( CompTree tree);

CompTree comp_tree_left( CompTree tree);

CompTree comp_tree_right( CompTree tree);

CompTree comptree_insert_last(CompTree tree, Pointer value);

CompTree comptree_remove_last(CompTree tree);

CompTree comp_tree_get_subtree(CompTree tree,int pos);

CompTree comp_tree_replace_subtree(CompTree tree,int pos,CompTree subtree);
