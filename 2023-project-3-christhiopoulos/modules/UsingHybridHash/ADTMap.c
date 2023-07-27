/////////////////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Map μέσω Hopscotch hashing
//
/////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include "ADTVector.h"
#include "ADTMap.h"


#define MAX_LOAD_FACTOR 0.5
#define NEIGHBOURS 3		 // οι γειτονικες θεσεις ,μιας θεσης i, ειναι απο i μεχρι NEIGHBOURS+i


typedef enum {
	EMPTY, OCCUPIED
} State;


typedef enum {   //για να γνωριζουμε αν το στοιχειο βρισκεται στον πινακα ή σε vector
	arr,vec
} Pos;


// Δομή του κάθε κόμβου που έχει το hash table (με το οποίο υλοιποιούμε το map)
struct map_node{
	Pointer key;		// Το κλειδί που χρησιμοποιείται για να hash-αρουμε
	Pointer value;  	// Η τιμή που αντισοιχίζεται στο παραπάνω κλειδί
	State state;		// Μεταβλητή για να μαρκάρουμε την κατάσταση των κόμβων (βλέπε διαγραφή)
	Vector vec;     	//το vector που θα χρησιμοποιησουμε για το seperate chaining
	Pos pos;        	//για να γνωριζουμε αν το στοιχειο βρισκεται στον πινακα ή σε vector
};


int prime_sizes[] = {53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241,
	786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741};


// Δομή του Map (περιέχει όλες τις πληροφορίες που χρεαζόμαστε για το HashTable)
struct map {
	MapNode array;				// Ο πίνακας που θα χρησιμοποιήσουμε για το map (remember, φτιάχνουμε ένα hash table)
	int capacity;				// Πόσο χώρο έχουμε δεσμεύσει.
	int size;					// Πόσα στοιχεία έχουμε προσθέσει
	int deleted;				// Πόσα κελιά είναι DELETED
	CompareFunc compare;		// Συνάρτηση για σύγκριση δεικτών, που πρέπει να δίνεται απο τον χρήστη
	HashFunc hash_function;		// Συνάρτηση για να παίρνουμε το hash code του κάθε αντικειμένου.
	DestroyFunc destroy_key;	// Συναρτήσεις που καλούνται όταν διαγράφουμε έναν κόμβο απο το map.
	DestroyFunc destroy_value;
};



static void rehash(Map map) {
	int old_capacity = map->capacity;	// Αποθήκευση των παλιών δεδομένων
	MapNode old_array = map->array;
	int prime_no = sizeof(prime_sizes) / sizeof(int);	                                     
	for (int i = 0; i < prime_no; i++) {					// LCOV_EXCL_LINE
		if (prime_sizes[i] > old_capacity) {                // Βρίσκουμε τη νέα χωρητικότητα, διασχίζοντας τη λίστα των πρώτων ώστε να βρούμε τον επόμενο. 
			map->capacity = prime_sizes[i]; 
			break;
		}
	}
													    // Αν έχουμε εξαντλήσει όλους τους πρώτους, διπλασιάζουμε
	if (map->capacity == old_capacity)					// LCOV_EXCL_LINE
		map->capacity *= 2;								// LCOV_EXCL_LINE

	
	map->array = malloc(map->capacity * sizeof(struct map_node));      	// Δημιουργούμε ένα μεγαλύτερο hash table
	for (int i = 0; i < map->capacity; i++){
		map->array[i].state = EMPTY;
		map->array[i].vec=vector_create(0,NULL);  //δημιουργουμε τα vectors που χρειαζονται για το seperate chaining
	}
	
	map->size = 0;
	for (int i = 0; i < old_capacity; i++){
		if (old_array[i].state == OCCUPIED){		     // Τοποθετούμε ΜΟΝΟ τα entries που όντως περιέχουν ένα στοιχείο 
			map_insert(map, old_array[i].key, old_array[i].value);	
		}
		for(int y = 0; y < vector_size(old_array[i].vec); y++){
			MapNode node=vector_get_at(old_array[i].vec,y);		   //βαζουμε τα στοιχεια τον vectors στο map που προκυπτει μετα το rehash
			map_insert(map, node->key, node->value);		
		}
		vector_set_destroy_value(old_array[i].vec,free);      //καταστρεφουμε τα παλια vector ωστε να μην εχουμε leaks
		vector_destroy(old_array[i].vec);
	}

	free(old_array);
}


Map map_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value) {
	Map map = malloc(sizeof(*map));// Δεσμεύουμε κατάλληλα τον χώρο που χρειαζόμαστε για το hash table
	map->capacity = prime_sizes[0];
	map->array = malloc(map->capacity * sizeof(struct map_node));
	for (int i = 0; i < map->capacity; i++){      
		map->array[i].state = EMPTY;        // Αρχικοποιούμε τους κόμβους που έχουμε σαν διαθέσιμους.
        map->array[i].vec=vector_create(0,NULL);  //δημιουργουμε τα vectors που χρειαζονται για το seperate chaining  	
	} 
	map->size = 0;
	map->deleted = 0;
	map->compare = compare;
	map->destroy_key = destroy_key;
	map->destroy_value = destroy_value;
	return map;
}




// Επιστρέφει τον αριθμό των entries του map σε μία χρονική στιγμή.
int map_size(Map map) {
	return map->size;
}


// Εισαγωγή στο hash table του ζευγαριού (key, item). Αν το key υπάρχει,
// ανανέωση του με ένα νέο value, και η συνάρτηση επιστρέφει true.
void map_insert(Map map, Pointer key, Pointer value) {  	// Σκανάρουμε το Hash Table μέχρι να βρούμε διαθέσιμη θέση για να τοποθετήσουμε το ζευγάρι,	
	bool already_in_map = false;   	// ή μέχρι να βρούμε το κλειδί ώστε να το αντικαταστήσουμε.
	MapNode node = NULL;
	uint pos=0;
	int flag=0;
	int counter=0;
	int hashed_key=map->hash_function(key) % map->capacity;
	for (int i = 0; i <= NEIGHBOURS+1; i++) {		
		pos=(hashed_key+i)%map->capacity;			
		if(counter==NEIGHBOURS+1&&node==NULL){  //εαν δεν υπαρχει κενη γειτονικη θεση με αυτη που κανει hash το key τοτε θα μπει στο vector
			flag=1;
			break;
		}
		if(counter==NEIGHBOURS+1&&node!=NULL){
			break;
		}
		if (map->array[pos].state == EMPTY) { 			// Βρήκαμε DELETED θέση. Θα μπορούσαμε να βάλουμε το ζευγάρι εδώ, αλλά _μόνο_ αν το key δεν υπάρχει ήδη.													
			if (node == NULL){			// Οπότε σημειώνουμε τη θέση, αλλά συνεχίζουμε την αναζήτηση, το key μπορεί να βρίσκεται πιο μετά.	
				node = &map->array[pos];
			}
              
		} else if (map->array[pos].key!=NULL&&map->compare(map->array[pos].key, key) == 0&&map->array[pos].state==OCCUPIED) {
			already_in_map = true;
			node = &map->array[pos];						// βρήκαμε το key, το ζευγάρι θα μπει αναγκαστικά εδώ (ακόμα και αν είχαμε προηγουμένως βρει DELETED θέση)
			break;											// και δε χρειάζεται να συνεχίζουμε την αναζήτηση.
		}
		counter++;	
	}

	if(flag==0){   					 // η παρακατω διαδικασια γινεται μονο οταν εχουμε βρει υποψηφια θεση για εισαγωγη, εντος των οριων που εχοuν ορισει οι γειτονικες θεσεις
		if (already_in_map) {						// Σε αυτό το σημείο, το node είναι ο κόμβος στον οποίο θα γίνει εισαγωγή.		
			if (node->key != key && map->destroy_key != NULL)    		// Αν αντικαθιστούμε παλιά key/value, τa κάνουμε destroy
				map->destroy_key(node->key);
			if (node->value != value && map->destroy_value != NULL)
				map->destroy_value(node->value);

		} 
		else {
			map->size++;	// Νέο στοιχείο, αυξάνουμε τα συνολικά στοιχεία του map
		}
		node->state = OCCUPIED; // Προσθήκη τιμών στον κόμβο
		node->key = key;
		node->value = value;
		node->pos=arr;
		return ;
	}

	else if(flag==1) {
		for(int pos = 0; pos < vector_size(map->array[hashed_key].vec); pos++){		
			MapNode node=vector_get_at(map->array[hashed_key].vec,pos);							
			if(map->compare(node->key ,key)==0){ 					//εαν υπαρχει ηδη το key στο vector τοτε απλα ανανεωνουμε το value
				node->value=value;
				return;				
			}	
		}

		MapNode pair=malloc(sizeof(struct map_node));     //φτιαχνουμε το MapNode που θα μπει στο vector
		map->size++;
		pair->key=key;
		pair->value=value;
		pair->pos=vec;
		pair->state=OCCUPIED;  
		vector_insert_last(map->array[hashed_key].vec,pair);     //βαζουμε στο τελος του vector το καινουριο στοιχειο	
	}

	float load_factor = (float)(map->size + map->deleted) / map->capacity;  		// Αν με την νέα εισαγωγή ξεπερνάμε το μέγιστο load factor, πρέπει να κάνουμε rehash.
	if (load_factor > MAX_LOAD_FACTOR) {	// Στο load factor μετράμε και τα DELETED, γιατί και αυτά επηρρεάζουν τις αναζητήσεις.
		rehash(map); 
	}
}



// Διαργραφή απο το Hash Table του κλειδιού με τιμή key
bool map_remove(Map map, Pointer key) {
	int hashed_key=map->hash_function(key) % map->capacity;
	int i=hashed_key;
	for(int y = 0; y <= NEIGHBOURS; y++){  //κανουμε neighbour αναζητησεις για το key ,αν δεν το βρουμε τοτε ειναι σε καποιο vector			
		if(map->array[i].state==OCCUPIED&&map->compare(map->array[i].key ,key)==0){	
			map->array[i].state=EMPTY;			
			map->deleted++;
	 		map->size--;					
			if(map->destroy_key!=NULL){			  //κανουμε destroy το key για να μην εχουμε leaks	
	      		map->destroy_key(map->array[i].key);	
			}
			if(map->destroy_value!=NULL){		 //κανουμε destroy το key για να μην εχουμε leaks				
	      		map->destroy_value(map->array[i].value);	
			}
			return true;
		}				
		i=(i+1)%map->capacity;
	}
	
	for(int pos = 0 ;pos < vector_size(map->array[hashed_key].vec) ; pos++){	  //ψαχνουμε το vector ωστε να βρουμε το στοιχειο που θα κανουμε remove
		MapNode node=vector_get_at(map->array[hashed_key].vec,pos);		
		if(map->compare(node->key ,key)==0){	
			int vec_size=vector_size(map->array[hashed_key].vec);
 			MapNode last=vector_get_at(map->array[hashed_key].vec,vec_size-1);	   //για να βγαλουμε το στοιχειο απο το vector το κανουμε swap με το τελευταιο και μετα κανουμε remove last
			vector_set_at(map->array[hashed_key].vec,pos,last);            
			vector_set_at(map->array[hashed_key].vec,vec_size-1,node);	 
			if(map->destroy_key!=NULL){			  //κανουμε destroy το key για να μην εχουμε leaks		
	      		map->destroy_key(node->key);
			}
			if(map->destroy_value!=NULL){			  //κανουμε destroy το value για να μην εχουμε leaks	
	      		map->destroy_value(node->value);				
			} 								
			vector_remove_last(map->array[hashed_key].vec);
			free(node);   //κανουμε free το node για να μην εχουμε leaks						 				
			map->size--;
			map->deleted++;	
			return true;
		}

	}			    
	return false;

			
}


// Αναζήτηση στο map, με σκοπό να επιστραφεί το value του κλειδιού που περνάμε σαν όρισμα.
Pointer map_find(Map map, Pointer key) {
	MapNode node=map_find_node(map,key);
	if(node!=NULL){
		return node->value;
	}
	return NULL;  
}



DestroyFunc map_set_destroy_key(Map map, DestroyFunc destroy_key) {
		map->destroy_key=destroy_key;
		return destroy_key;
	
}



DestroyFunc map_set_destroy_value(Map map, DestroyFunc destroy_value) {
	map->destroy_value=destroy_value;
		return destroy_value;
}


// Απελευθέρωση μνήμης που δεσμεύει το map
void map_destroy(Map map) {   
  	for (int i = 0; i < map->capacity; i++) {	//αποδεσμευομε την μνημη που επιαναν τα στοιχεια στο array
		if (map->array[i].state == OCCUPIED) {
			if (map->destroy_key != NULL)
				map->destroy_key(map->array[i].key);
			if (map->destroy_value != NULL)
				map->destroy_value(map->array[i].value);
		}
	}

	 	for (int i = 0; i < map->capacity; i++) {	   //αποδεσμευομε την μνημη που επιαναν τα στοιχεια στους vectors
			vector_set_destroy_value(map->array[i].vec,free);
			vector_destroy(map->array[i].vec);		
		}

	free(map->array);
	free(map);
	return;
}



/////////////////////// Διάσχιση του map μέσω κόμβων ///////////////////////////
MapNode map_first(Map map) {   
	for (int i = 0; i < map->capacity; i++){      	//βρισκουμε το πρωτο στοιχειο του map ,ειτε αυτο βρισκεται στον πινακα ,ειτε σε vector
		if (map->array[i].state == OCCUPIED){    	 //για καθε i ψαχνουμε την αντιστοιχη θεση στον πινακα και μετα τον vector, και επιστρεφουμε το πρωτο node που θα βρουμε
			return &map->array[i];
		}	

		Vector vec=map->array[i].vec;
		for(int j = 0; j < vector_size(vec); j++){
			MapNode node= vector_get_at(map->array[i].vec,j);			      
			return node;		
		}		
	}
	return NULL;
}



MapNode map_next(Map map, MapNode node) {
	int hashed_key=map->hash_function(node->key)%map->capacity;
	if(node->pos==arr){   				//εαν το στοιχειο ειναι στον πινακα τοτε επιστρεφουμε το πρωτο στοιχειο του vector, στη θεση που hashαρει το key
	Vector vec=map->array[hashed_key].vec;
	   if(vector_size(vec)!=0){	
			MapNode node2=vector_get_at(vec,0);
			return  node2;			 		
	   }			
	}

	else if(node->pos==vec){         //εαν το στοιχειο ειναι σε vector τοτε βρισκουμε το επομενο στοιχειο αυτου του vector
		for(int i = 0; i < vector_size(map->array[hashed_key].vec)-1; i++){
			MapNode node2=vector_get_at(map->array[hashed_key].vec,i);
			if(map->compare(node->key,node2->key)==0){
				return  vector_get_at(map->array[hashed_key].vec,i+1);
			}
		}
		if(map->compare(node->key,vector_get_at(map->array[hashed_key].vec,vector_size(map->array[hashed_key].vec))-1)==0){     //εαν το key ειναι ισο με το key του προτελευταιου στοιχειο του vector τοτε επιστρεφουμε το τελευταιο στοιχειο του vector
			return  vector_get_at(map->array[hashed_key].vec,vector_size(map->array[hashed_key].vec));
		}
	}
	

	for(int i = (hashed_key+1); i < map->capacity; i++){   //εαν δεν εχουμε βρει μεχρι τωρα το επομενο στοιχειο τοτε απλα βρισκουμε το επομενο occupied στοιχειο
		int pos=(i)%map->capacity;	
		Vector vec=map->array[pos].vec;
		if(map->array[pos].state==OCCUPIED){
			if(map->array[pos].key!=node->key){      //αυτος ο ελεγχος χρειαζεται σε περιπτωση που ,λογω της κυκλικοτητας του hash table,ξαναφτασουμε στο ιδιο στοιχειο
				return &map->array[pos];
			}
		}

		for(int j = 0; j < vector_size(vec); j++){
			MapNode node2=vector_get_at(vec,j);	
			if(node2->key!=node->key){	    //αυτος ο ελεγχος χρειαζεται σε περιπτωση που ,λογω της κυκλικοτητας του hash table,ξαναφτασουμε στο ιδιο στοιχειο
				return  node2;	
			}			
		}				             
	}	

	return MAP_EOF; // εαν δεν εχει βρεθει,το επομενο του στοιχειου, μεχρι τωρα τοτε δεν υπαρχει 
}



Pointer map_node_key(Map map, MapNode node) {
	if(node!=NULL){
		return node->key;
	}
	return NULL;
}



Pointer map_node_value(Map map, MapNode node) {
	if(node!=NULL){
		return node->value;
	}
	return NULL;
}



MapNode map_find_node(Map map, Pointer key) {
	int hashed_key=map->hash_function(key)%map->capacity;
	for(int i = 0; i <= NEIGHBOURS; i++){  //κανουμε neighbour αναζητησεις για το key ,αν δεν το βρουμε τοτε ειναι σε vector	
	int pos=(hashed_key+i)%map->capacity;	 
		if(map->array[pos].state==OCCUPIED&&map->compare(key,map->array[pos].key)==0){
			return &map->array[pos];	
		}		
					
	}	

	for(int i = 0; i < vector_size(map->array[hashed_key].vec); i++){	 // ψαχνουμε το vector ωστε να βρουμε το στοιχειο που ψαχνουμε 
		MapNode node=vector_get_at(map->array[hashed_key].vec,i);             					
		if(map->compare(node->key ,key)==0){
			return node;
		}	
	}
	
	return NULL;      //εαν δεν εχουμε βρει μεχρι τωρα το key τοτε δεν υπαρχει
}



// Αρχικοποίηση της συνάρτησης κατακερματισμού του συγκεκριμένου map.
void map_set_hash_function(Map map, HashFunc func) {
	map->hash_function = func;
}



uint hash_string(Pointer value) {
	// djb2 hash function, απλή, γρήγορη, και σε γενικές γραμμές αποδοτική
    uint hash = 5381;
    for (char* s = value; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;			// hash = (hash * 33) + *s. Το foo << 5 είναι γρηγορότερη εκδοχή του foo * 32.
    return hash;
}



uint hash_int(Pointer value) {
	return *(int*)value;
}



uint hash_pointer(Pointer value) {
	return (size_t)value;				// cast σε sizt_t, που έχει το ίδιο μήκος με έναν pointer
}