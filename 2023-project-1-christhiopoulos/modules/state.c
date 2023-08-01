#include <stdlib.h>
#include "ADTVector.h"
#include "ADTList.h"
#include "state.h"
#include<stdio.h>

// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Object last_platform;   //κραταω σε μεταβλητη το last_platform ωστε να εχω ευκολη προσβαση σε αυτο
	Vector objects;			// περιέχει στοιχεία Object (Πλατφόρμες, Αστέρια)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
};

// Δημιουργεί και επιστρέφει ένα αντικείμενο

static Object create_object(ObjectType type, float x, float y, float width, float height, VerticalMovement vert_mov, float speed, bool unstable) {
	Object obj = malloc(sizeof(*obj));
	obj->type = type;
	obj->rect.x = x;
	obj->rect.y = y;
	obj->rect.width = width;
	obj->rect.height = height;
	obj->vert_mov = vert_mov;
	obj->vert_speed = speed;
	obj->unstable = unstable;
	return obj;
}


// Προσθέτει αντικείμενα στην πίστα (η οποία μπορεί να περιέχει ήδη αντικείμενα).
// Τα αντικείμενα ξεκινάνε από x = start_x, και επεκτείνονται προς τα δεξιά.
//
// ΠΡΟΣΟΧΗ: όλα τα αντικείμενα έχουν συντεταγμένες x,y σε ένα
// καρτεσιανό επίπεδο.
// - Στο άξονα x το 0 είναι η αρχή της πίστας και οι συντεταγμένες
//   μεγαλώνουν προς τα δεξιά.
//
// - Στον άξονα y το 0 είναι το πάνω μέρος της πίστας, και οι συντεταγμένες μεγαλώνουν
//   προς τα _κάτω_. Η πίστα έχει σταθερό ύψος, οπότε όλες οι
//   συντεταγμένες y είναι ανάμεσα στο 0 και το SCREEN_HEIGHT.
//
// Πέρα από τις συντεταγμένες, αποθηκεύουμε και τις διαστάσεις width,height
// κάθε αντικειμένου. Τα x,y,width,height ορίζουν ένα παραλληλόγραμμο, οπότε
// μπορούν να αποθηκευτούν όλα μαζί στο obj->rect τύπου Rectangle (ορίζεται
// στο include/raylib.h). Τα x,y αναφέρονται στην πάνω-αριστερά γωνία του Rectangle.

//η lastplatform ειναι global μεταβλητη καθως την χρησιμοποιω και στην add_objects και στην state_update
static void add_objects(State state, float start_x) {

	// Προσθέτουμε PLATFORM_NUM πλατφόρμες, με τυχαία χαρακτηριστικά.

	for (int i = 0; i < PLATFORM_NUM; i++) {
		Object platform = create_object(
			PLATFORM,
			start_x + 150 + rand() % 80,						// x με τυχαία απόσταση από το προηγούμενο στο διάστημα [150, 230]
			SCREEN_HEIGHT/4 + rand() % SCREEN_HEIGHT/2,			// y τυχαία στο διάστημα [SCREEN_HEIGHT/4, 3*SCREEN_HEIGHT/4]
			i == 0 ? 250 : 50 + rand()%200,						// πλάτος τυχαία στο διάστημα [50, 250] (η πρώτη πάντα 250)
			20,													// ύψος
			i < 3 || rand() % 2 == 0 ? MOVING_UP : MOVING_DOWN,	// τυχαία αρχική κίνηση (οι πρώτες 3 πάντα πάνω)
			0.6 + 3*(rand()%100)/100,							// ταχύτητα τυχαία στο διάστημα [0.6, 3.6]
			i > 0 && (rand() % 10) == 0							// το 10% (τυχαία) των πλατφορμών είναι ασταθείς (εκτός από την πρώτη)
		);
		vector_insert_last(state->objects, platform);

		// Στο 50% των πλατφορμών (τυχαία), εκτός της πρώτης, προσθέτουμε αστέρι
		if(i != 0 && rand() % 2 == 0) {
			Object star = create_object(
				STAR,
				start_x + 200 + rand() % 60,				// x με τυχαία απόσταση από την προηγούμενη πλατφόρμα στο διάστημα [200,260]
				SCREEN_HEIGHT/8 + rand() % SCREEN_HEIGHT/2,	// y τυχαία στο διάστημα [SCREEN_HEIGHT/8, 5*SCREEN_HEIGHT/8]
				30, 30,										// πλάτος, ύψος
				IDLE,										// χωρίς κίνηση
				0,										 	// ταχύτητα 0
				false										// 'unstable' πάντα false για τα αστέρια
			);
			vector_insert_last(state->objects, star);
		}

		start_x = platform->rect.x + platform->rect.width;
		if(i==PLATFORM_NUM-1){   //κραταμε το τελευταιο platform ωστε να χρησιμοποιηθει μετα
			state->last_platform=platform;
		}	// μετακίνηση των επόμενων αντικειμένων προς τα δεξιά
	}
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = false;				// Χωρίς να είναι paused.
	state->speed_factor = 1;				// Κανονική ταχύτητα
	state->info.score = 0;				// Αρχικό σκορ 0

	// Δημιουργούμε το vector των αντικειμένων, και προσθέτουμε αντικείμενα
	// ξεκινώντας από start_x = 0.
	state->objects = vector_create(0, NULL);
	add_objects(state, 0);

	// Δημιουργούμε την μπάλα τοποθετώντας τη πάνω στην πρώτη πλατφόρμα
	Object first_platform = vector_get_at(state->objects, 0);
	state->info.ball = create_object(
		BALL,
		first_platform->rect.x,			// x στην αρχή της πλατφόρμας
		first_platform->rect.y - 40,	// y πάνω στην πλατφόρμα
		45, 45,							// πλάτος, ύψος
		IDLE,							// χωρίς αρχική κατακόρυφη κίνηση
		0,								// αρχική ταχύτητα 0
		false							// "unstable" πάντα false για την μπάλα
	);

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
	// Προς υλοποίηση
	return &state->info;
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη x είναι ανάμεσα στο x_from και x_to.

List state_objects(State state, float x_from, float x_to) {   //διατρεχουμε το vector με τα objects και βαζουμε σε λιστα μονο οσα ειναι απο x_from εως x_to
	List result=list_create(NULL);
	for(VectorNode node=vector_first(state->objects);node!=VECTOR_EOF;node=vector_next(state->objects,node)){
		Object obj=vector_node_value(state->objects,node);
		if(obj->rect.x >=x_from&&obj->rect.x<=x_to){
			list_insert_next(result,LIST_BOF,obj);
		}
	}
	return result;
}
// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {
	if((state->info.paused==false&&state->info.playing==true)||(state->info.paused==true&&keys->n)){	  //το παιχνιδι ενημερωνεται μονο οταν δεν ειναι paused ή οταν ειναι paused και ειναι πατημενο το n	
    	state->info.ball->rect.x+=(keys->right?6*state->speed_factor:keys->left?state->speed_factor*1:4*state->speed_factor);
		if(keys->up&&state->info.ball->vert_mov==IDLE){
			state->info.ball->vert_speed=22;
			state->info.ball->vert_mov=JUMPING;
		}
		if(state->info.ball->vert_mov==JUMPING){	
			state->info.ball->rect.y-=(state->info.ball->vert_speed);
			state->info.ball->vert_speed=((0.85)*(state->info.ball->vert_speed));
		}
		if(state->info.ball->vert_speed<=0.5){
			state->info.ball->vert_mov=FALLING;
		}
		for(VectorNode node=vector_first(state->objects);node!=VECTOR_EOF;node=vector_next(state->objects,node)){
			Object obj=vector_node_value(state->objects,node);
			if(CheckCollisionRecs(state->info.ball->rect,obj->rect)&&obj->type==PLATFORM){    //αμα η μπαλα ερθει σε επαφη με πλατφορμα ακολουθει το y της πλατφομας
				state->info.ball->vert_mov=IDLE;
				state->info.ball->rect.y=obj->rect.y-40;
				break;	
			}	
		    else if(CheckCollisionRecs(state->info.ball->rect,obj->rect)==false&&state->info.ball->vert_mov!=JUMPING) {		  
				state->info.ball->vert_speed=1.5;
				state->info.ball->vert_mov=FALLING;
			}
			if(CheckCollisionRecs(state->info.ball->rect,obj->rect)&&obj->type==PLATFORM&&state->info.ball->vert_mov==FALLING)	{
				state->info.ball->vert_mov=IDLE;
				state->info.ball->rect.y=obj->rect.y-40;
				break;	
			}
		}	
		if(state->info.ball->vert_mov==FALLING&&state->info.ball->vert_speed<7){
			state->info.ball->vert_speed=(2.1)*(state->info.ball->vert_speed)*state->speed_factor;
			state->info.ball->rect.y+=1.5*state->info.ball->vert_speed;
		}
		if(state->info.ball->vert_mov==FALLING&&state->info.ball->vert_speed>=7){
			state->info.ball->rect.y+=1.5*state->info.ball->vert_speed;
		}
		for(VectorNode node=vector_first(state->objects);node!=VECTOR_EOF;node=vector_next(state->objects,node)){
			Object obj=vector_node_value(state->objects,node);
			if(obj->type==PLATFORM){
				if(obj->vert_mov==MOVING_UP){
					obj->rect.y-=obj->vert_speed*state->speed_factor;
					if(obj->rect.y<SCREEN_HEIGHT/4.0){
						obj->vert_mov=MOVING_DOWN;
					}
				}
				if(obj->vert_mov==MOVING_DOWN){
					obj->rect.y+=obj->vert_speed*state->speed_factor;
					if(obj->rect.y>(3.0*SCREEN_HEIGHT/4.0)){
						obj->vert_mov=MOVING_UP;
					}
				}
			}
			if(obj->vert_mov==FALLING){
				obj->rect.y+=4*state->speed_factor;			
			}	
				if(obj->type==PLATFORM&&obj->unstable==true&&CheckCollisionRecs(state->info.ball->rect,obj->rect)){  //αμα η πλατφορμα ειναι unstable τοτε ,με το που ακουμπησει η μπαλα πανω της ,αρχιζει να πεφτει
				obj->vert_mov=FALLING;
			}
		}
		int pos=0;   //μετρητης για τη θεση του obj,που θα κανουμε delete,στο vector
 		for(VectorNode node=vector_first(state->objects);node!=VECTOR_EOF;node=vector_next(state->objects,node)){
			pos++;
			Object obj=vector_node_value(state->objects,node);
			if(CheckCollisionRecs(state->info.ball->rect,obj->rect)&&obj->type==STAR){   //θα κανουμε remove το obj βαζωντας το στην τελευταια θεση και κανοντας vector_remove_last
				state->info.score+=10;
				int x=vector_size(state->objects);
    			Object last_obj=vector_get_at(state->objects,x-1);
				if(obj==last_obj){ //θα κανουμε remove το obj βαζωντας το στην τελευταια θεση και κανοντας vector_remove_last
					vector_remove_last(state->objects);
					break;
				}
				else{
					vector_set_at(state->objects,pos-1,last_obj);
					vector_set_at(state->objects,x-1,obj);
					vector_remove_last(state->objects);
					break;
				}	
			}	
		}
		pos=0;
		for(VectorNode node=vector_first(state->objects);node!=VECTOR_EOF;node=vector_next(state->objects,node)){
			pos++;
			Object obj=vector_node_value(state->objects,node);
			if(obj->type==PLATFORM&&obj->vert_mov==FALLING&&obj->rect.y==SCREEN_HEIGHT){//θα κανουμε remove το obj βαζωντας το στην τελευταια θεση και κανοντας vector_remove_last		
				int x=vector_size(state->objects);
    			Object last_obj=vector_get_at(state->objects,x-1);
				if(obj==last_obj){
					vector_remove_last(state->objects);//θα κανουμε remove το obj βαζωντας το στην τελευταια θεση και κανοντας vector_remove_last
					break;
				}
				else{
					vector_set_at(state->objects,pos-1,last_obj);
					vector_set_at(state->objects,x-1,obj);
					vector_remove_last(state->objects);
					break;
				}	
			}	
		}
		if(state->info.ball->rect.x>state->last_platform->rect.x-SCREEN_WIDTH){  //εαν η μπαλα ξεπερασει το χ της τελευταιας πλατφορμας τοτε βαζουμε νεες πλατφορμες μεσω της add_objects
			add_objects(state,state->last_platform->rect.x+250);   // εχω βαλει το +250 ετσι ωστε η νεα πλατφορμα να μην ειναι ακριβως διπλα στην προηγουμενη
			state->speed_factor=state->speed_factor*1.1;
		}
		if(state->info.ball->rect.y>=SCREEN_HEIGHT){ //εαν η μπαλα φτασει το y της οθονης τοτε το παιχνιδι σταματαει
			state->info.playing =false;
			state->speed_factor=0;
		}
	}	
	if(keys->p ){   //εαν πατησουμε p τοτε η καταστατη του pause αντιστρεφεται
        state->info.paused = !state->info.paused; 
    }	
}
// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	vector_destroy(state->objects);
	free(state);
	free(state->info.ball);
}