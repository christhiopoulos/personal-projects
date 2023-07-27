/////////////////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Map μέσω Hopscotch hashing
//
/////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include "ADTMap.h"

// Κάθε θέση i θεωρείται γεινοτική με όλες τις θέσεις μέχρι και την i + NEIGHBOURS
#define NEIGHBOURS 3
#define MAX_LOAD_FACTOR 0.5


typedef enum {
	EMPTY, OCCUPIED
} State;


// Δομή του κάθε κόμβου που έχει το hash table (με το οποίο υλοιποιούμε το map)
struct map_node{
	Pointer key;		// Το κλειδί που χρησιμοποιείται για να hash-αρουμε
	Pointer value;  	// Η τιμή που αντισοιχίζεται στο παραπάνω κλειδί
	State state;		// Μεταβλητή για να μαρκάρουμε την κατάσταση των κόμβων (βλέπε διαγραφή)
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



Map map_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value) {
	// Δεσμεύουμε κατάλληλα τον χώρο που χρειαζόμαστε για το hash table
	Map map = malloc(sizeof(*map));
	map->capacity = prime_sizes[0];
	map->array = malloc(map->capacity * sizeof(struct map_node));

	// Αρχικοποιούμε τους κόμβους που έχουμε σαν διαθέσιμους.
	for (int i = 0; i < map->capacity; i++)
		map->array[i].state = EMPTY;

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



// Συνάρτηση για την επέκταση του Hash Table σε περίπτωση που ο load factor μεγαλώσει πολύ.
static void rehash(Map map) {
	
	// Αποθήκευση των παλιών δεδομένων
	int old_capacity = map->capacity;
	MapNode old_array = map->array;
  
	// Βρίσκουμε τη νέα χωρητικότητα, διασχίζοντας τη λίστα των πρώτων ώστε να βρούμε τον επόμενο. 
	int prime_no = sizeof(prime_sizes) / sizeof(int);	// το μέγεθος του πίνακα
	for (int i = 0; i < prime_no; i++) {					// LCOV_EXCL_LINE
		if (prime_sizes[i] > old_capacity) {
			map->capacity = prime_sizes[i]; 
			break;
		}
	}
	// Αν έχουμε εξαντλήσει όλους τους πρώτους, διπλασιάζουμε
	if (map->capacity == old_capacity)					// LCOV_EXCL_LINE
		map->capacity *= 2;								// LCOV_EXCL_LINE

	// Δημιουργούμε ένα μεγαλύτερο hash table
	map->array = malloc(map->capacity * sizeof(struct map_node));
	for (int i = 0; i < map->capacity; i++)
		map->array[i].state = EMPTY;

	map->size = 0;
	for (int i = 0; i < old_capacity; i++){
		MapNode node=&old_array[i];
		if (node->state == OCCUPIED){     // Τοποθετούμε ΜΟΝΟ τα entries που όντως περιέχουν ένα στοιχείο (το rehash είναι και μία ευκαιρία να ξεφορτωθούμε τα deleted nodes)
			map_insert(map, node->key, node->value);
		}
	}

	free(old_array);  	//Αποδεσμεύουμε τον παλιό πίνακα ώστε να μήν έχουμε leaks		

}  


// Εισαγωγή στο hash table του ζευγαριού (key, item). Αν το key υπάρχει,
// ανανέωση του με ένα νέο value, και η συνάρτηση επιστρέφει true.
void map_insert(Map map, Pointer key, Pointer value) {

	bool already_in_map = false;
	MapNode node = NULL;
	int flag=0;
	int pos = 0;
	int counter=0;
	int hashed_key=map->hash_function(key) % map->capacity;
	for (int i = 0; i <= NEIGHBOURS+1; i++) {		
		pos=(hashed_key+i)%map->capacity;			
		if(counter==NEIGHBOURS+1&&node==NULL){  //εαν δεν υπαρχει κενη γειτονικη θεση με αυτη που κανει hash το key τοτε θα φτιαξουμε εμεις μια 
			flag=1;
			break;
		}
		if(counter==NEIGHBOURS+1&&node!=NULL){
			break;
		}
		if (map->array[pos].state == EMPTY) {
			// Βρήκαμε EMPTY θέση. Θα μπορούσαμε να βάλουμε το ζευγάρι εδώ, αλλά _μόνο_ αν το key δεν υπάρχει ήδη.
			// Οπότε σημειώνουμε τη θέση, αλλά συνεχίζουμε την αναζήτηση, το key μπορεί να βρίσκεται πιο μετά.
			if (node == NULL){
				node = &map->array[pos];
			}
              
		} else if(map->array[pos].state==OCCUPIED){
			if(map->compare(map->array[pos].key, key) == 0) {
			already_in_map = true;
			node = &map->array[pos];	// βρήκαμε το key, το ζευγάρι θα μπει αναγκαστικά εδώ (ακόμα και αν είχαμε προηγουμένως βρει DELETED θέση)
			break;											// και δε χρειάζεται να συνεχίζουμε την αναζήτηση.
			}
		}
		counter++;	
	}


	if(flag==0){    // η παρακατω διαδικασια γινεται μονο οταν εχουμε βρει υποψηφια θεση για εισαγωγη, εντος των οριων που εχοuν ορισει οι γειτονικες θεσεις													
		if (already_in_map) {
			if (node->key != key && map->destroy_key != NULL)  
				map->destroy_key(node->key);        // Αν αντικαθιστούμε παλιά key/value, τa κάνουμε destroy
			if (node->value != value && map->destroy_value != NULL)
				map->destroy_value(node->value);
		} 
		else{									
			map->size++;   // Νέο στοιχείο, αυξάνουμε τα συνολικά στοιχεία του map	
		}
		// Προσθήκη τιμών στον κόμβο
		node->key = key;
		node->value = value;
		node->state = OCCUPIED;
		float load_factor = (float)(map->size + map->deleted) / map->capacity;
		if (load_factor > MAX_LOAD_FACTOR){  	// Αν με την νέα εισαγωγή ξεπερνάμε το μέγιστο load factor, πρέπει να κάνουμε rehash.
			rehash(map); 			// Στο load factor μετράμε και τα DELETED, γιατί και αυτά επηρρεάζουν τις αναζητήσεις.	
   		}         					
		return;	

	}

	else if(flag==1){	
		for(int i = map->hash_function(key) % map->capacity; i<map->capacity; i++){		
			MapNode node=&map->array[i]; //με αυτην  την διαδικιασια φτιαχνουμε μια empty θεση ,σε γειτονικη θεση με αυτην που κανει hash το key					
			if(node->state==EMPTY){			
				if(abs(i-hashed_key)<=NEIGHBOURS){	   //αν ειναι γειτονες τοτε απλα αλλαζουμε τα στοιχεια της κενης θεσης
					node->key=key;
					node->value=value;				
                   	node->state=OCCUPIED;
					map->size++;
					float load_factor = (float)(map->size + map->deleted) / map->capacity;
					if (load_factor > MAX_LOAD_FACTOR){ // Αν με την νέα εισαγωγή ξεπερνάμε το μέγιστο load factor, πρέπει να κάνουμε rehash.
		   				rehash(map);	// Στο load factor μετράμε και τα DELETED, γιατί και αυτά επηρρεάζουν τις αναζητήσεις.	
					}
					return;	
																			
				}
				for(int y = hashed_key; y < i; y++){										
					MapNode node2=&map->array[y];	
                	if(node2->state==OCCUPIED&&abs(i-map->hash_function(node2->key)%map->capacity)<=NEIGHBOURS){		  // κανουμε swap μια κενη θεση με καποια αλλη θεση ,αναμεσα στην κενη και την κανονικη θεση του key,εφοσον αυτο επιτρεπεται απο τους γειτονικους ππεριορισμους				
						node->key=node2->key;          //ολη η διαδικασια γινεται μεχρι η κενη θεση να μεταφερθει σε γειτονικη θεση απο την κανονικη του key 
						node->value=node2->value;
						node->state=OCCUPIED;
						node2->state=EMPTY;    
						i=map->hash_function(key) % map->capacity-1;              
						break;					
															
					}
				}
			}			
		}
	}

   rehash(map);    //αν δεν μπορουμε να μεταφερουμε καποια κενη θεση σε γειτονικη θεση ,τοτε κανουμε rehash και ξαναπροσπαθουμε για insert
   map_insert(map,key,value);	

}


// Διαργραφή απο το Hash Table του κλειδιού με τιμή key
bool map_remove(Map map, Pointer key) {
	MapNode node = map_find_node(map, key);
	if (node == MAP_EOF)
		return false;

	// destroy
	if (map->destroy_key != NULL)
		map->destroy_key(node->key);
	if (map->destroy_value != NULL)
		map->destroy_value(node->value);

	// θέτουμε ως "deleted", ώστε να μην διακόπτεται η αναζήτηση, αλλά ταυτόχρονα να γίνεται ομαλά η εισαγωγή
	node->state = EMPTY;
	map->deleted++;
	map->size--;
	return true;
}


// Αναζήτηση στο map, με σκοπό να επιστραφεί το value του κλειδιού που περνάμε σαν όρισμα.
Pointer map_find(Map map, Pointer key) {
	MapNode node = map_find_node(map, key);
	if (node != MAP_EOF)
		return node->value;
	else
		return NULL;
}


DestroyFunc map_set_destroy_key(Map map, DestroyFunc destroy_key) {
	DestroyFunc old = map->destroy_key;
	map->destroy_key = destroy_key;
	return old;
}

DestroyFunc map_set_destroy_value(Map map, DestroyFunc destroy_value) {
	DestroyFunc old = map->destroy_value;
	map->destroy_value = destroy_value;
	return old;
}


// Απελευθέρωση μνήμης που δεσμεύει το map
void map_destroy(Map map) {
	for (int i = 0; i < map->capacity; i++) {
		if (map->array[i].state == OCCUPIED) {
			if (map->destroy_key != NULL)
				map->destroy_key(map->array[i].key);
			if (map->destroy_value != NULL)
				map->destroy_value(map->array[i].value);
		}
	}

	free(map->array);
	free(map);
}


/////////////////////// Διάσχιση του map μέσω κόμβων ///////////////////////////

MapNode map_first(Map map) {
	//Ξεκινάμε την επανάληψή μας απο το 1ο στοιχείο, μέχρι να βρούμε κάτι όντως τοποθετημένο
	for (int i = 0; i < map->capacity; i++)
		if (map->array[i].state == OCCUPIED)
			return &map->array[i];

	return MAP_EOF;
}


MapNode map_next(Map map, MapNode node) {
	// Το node είναι pointer στο i-οστό στοιχείο του array, οπότε node - array == i  (pointer arithmetic!)
	for (int i = node - map->array + 1; i < map->capacity; i++)
		if (map->array[i].state == OCCUPIED){
			return &map->array[i];
}

	return MAP_EOF;
}


Pointer map_node_key(Map map, MapNode node) {
	return node->key;
}


Pointer map_node_value(Map map, MapNode node) {
	return node->value;
}


MapNode map_find_node(Map map, Pointer key) {
	int counter=0;
	int pos=0;
	int hashed_key=map->hash_function(key)%map->capacity;
		for (int i = 0; i <= NEIGHBOURS+1; i++) {		
			pos=(hashed_key+i)%map->capacity; 						// linear probing, γυρνώντας στην αρχή όταν φτάσουμε στη τέλος του πίνακα
       		if(counter==NEIGHBOURS+1){  	 	 
				return NULL;
			}
			if (map->array[pos].state == OCCUPIED&& map->compare(map->array[pos].key, key) == 0){   // Μόνο σε OCCUPIED θέσεις , ελέγχουμε αν το key είναι εδώ
				return &map->array[pos];
			}
			counter++;			
		}

	return MAP_EOF;     //αν μεχρι NEIGHBOUR αναζητησεις δεν εχουμε βρει το key τοτε δεν υπαρχει
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
