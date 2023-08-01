#include <stdlib.h>
#include "ADTRecTree.h"
#include<stdio.h>
#include "ADTRecTree_utils.h"



RecTree rectree_get_subtree(RecTree tree, int pos){       
   if(tree==REC_TREE_EMPTY){
      return NULL;
   }
    if(pos==0){
       return tree;
   }
   RecTree father_tree=rectree_get_subtree(tree, (pos-1)/2);       //βρισκουμε αναδρομικα τον πατερα του tree
   if(pos%2==0){    // εαν ισχυει αυτο τοτε το tree ειναι δεξι παιδι
      return rectree_right(father_tree);     
   }
   else {   //αλλιως ειναι αριστερο
      return rectree_left(father_tree);
   }
}


RecTree rectree_replace_subtree(RecTree tree, int pos, RecTree subtree){
   if(tree==REC_TREE_EMPTY){
      return subtree;
   }
   RecTree tree_pos=rectree_get_subtree(tree,(pos));  //κανουμε destroy το tree στην θεση Pos,αρα αναδρομικα καταστρεφονται ολα τα nodes στο path απο root μεχρι pos
   rectree_destroy(tree_pos); 
   if(pos==0){
      return subtree;
   } 
   RecTree father_tree=rectree_get_subtree(tree,(pos-1)/2); //βρισκουμε τον πατερα του tree
   if(pos%2==0){       // εαν ισχυει αυτο τοτε το tree ειναι δεξι παιδι   
      RecTree tree_new1=rectree_create(rectree_value(father_tree),rectree_left(father_tree),subtree);   //το αριστερο tree του tree δεν αλλαζει
      return rectree_replace_subtree(tree,(pos-1)/2, tree_new1);  
   }
   else{ // εαν ισχυει αυτο τοτε το tree ειναι δεξι παιδι
      RecTree tree_new2=rectree_create(rectree_value(father_tree),subtree,rectree_right(father_tree));   //το δεξι tree του tree δεν αλλαζει
      return rectree_replace_subtree(tree,(pos-1)/2, tree_new2);
   }
}

