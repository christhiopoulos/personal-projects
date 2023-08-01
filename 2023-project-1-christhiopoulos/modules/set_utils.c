#include "ADTSet.h"
#include<stdio.h>

Pointer set_find_eq_or_greater(Set set, Pointer  value){
    Pointer target= set_find(set,value);
    if(target==NULL){         //εαν δεν υπαρχει το στοιχειο τοτε το εισαγουμε και ψαχνουμε το επομενο 
        set_insert(set,value);
        SetNode node= set_find_node(set,value);
        SetNode node2=set_next(set,node);
        set_remove(set,value);
        if(node2==SET_EOF){   //εαν το επομενο στοιχειο δεν υπαρχει τοτε επιστρεφουμε το τελευταιο του set
	        SetNode node3=set_last(set);
	        Pointer obj3=set_node_value(set,node3);        
	        return obj3;   
        }
        Pointer obj2= set_node_value(set,node2);
        return obj2;
   }
    return target;   //εαν υπαρχει απλα το επιστρεφουμε
}


Pointer set_find_eq_or_smaller(Set set, Pointer value){
    Pointer target= set_find(set,value);
    if(target==NULL){ //εαν δεν υπαρχει το στοιχειο τοτε το εισαγουμε και ψαχνουμε το προηγουμενο 
        set_insert(set,value);
        SetNode node= set_find_node(set,value);
        SetNode node2=set_previous(set,node);
        set_remove(set,value);
        if(node2==SET_BOF){  //εαν το προηγουμενο στοιχειο δεν υπαρχει τοτε επιστρεφουμε το πρωτο του set
	        SetNode node3=set_first(set);
	        Pointer obj3=set_node_value(set,node3);          	
        	return obj3;
        }
        Pointer obj2= set_node_value(set,node2);
        return obj2;
  }
  return target; //εαν υπαρχει απλα το επιστρεφουμε
}