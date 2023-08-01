#include <stdlib.h>
#include "ADTVector.h"
#include "ADTList.h"
#include "state.h"
#include "set_utils.h"
#include "ADTSet.h"
#include <stdio.h>

// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

int compare_objects(Pointer a, Pointer b) { //η συναρτηση που χρειαζεται για να διαταξει τα στοιχεια στο map
	Object obj1=a;
	Object obj2=b; 
	if(obj1->rect.x==obj2->rect.x){        //εαν εχουν το ιδιο χ αλλα το ενα ειναι αστερι και το αλλο πλατφορμα τοτε πρεπει να υπαρχουν και τα δυο
        return obj1->type-obj2->type;	  //τα type ειναι στην πραγματικοτητα ακεραιοι αφου προκυπτουν απο enumeration	
	 }               				   
	float ia = obj1->rect.x;
	float ib = obj2->rect.x;
	return ia - ib;
}

struct state {
	Object current_platform;  //εχω βαλει το current και το next platform μεσα στο struct ωστε να εχω ευκολη προσβαση σε αυτα 
	Object next_platform;
	Set objects;			// περιέχει στοιχεία Object (Πλατφόρμες, Αστέρια)
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
		set_insert(state->objects, platform);

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
			set_insert(state->objects, star);
		}

		start_x = platform->rect.x + platform->rect.width;	// μετακίνηση των επόμενων αντικειμένων προς τα δεξιά
		
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
	state->objects = set_create(compare_objects, NULL);
	add_objects(state, 0);

	// Δημιουργούμε την μπάλα τοποθετώντας τη πάνω στην πρώτη πλατφόρμα
	SetNode first_platform_node = set_first(state->objects);
	Object first_platform=set_node_value(state->objects,first_platform_node);
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
	return &state->info;
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη x είναι ανάμεσα στο x_from και x_to.


List state_objects(State state, float x_from, float x_to) { //δημιουργω 2 objects με τα χ που θελω και παιρνω τα στοιχεια του set απο x_from μεχρι x_to 
	List result=list_create(NULL);
	Object test=malloc(sizeof(struct object));
	test->rect.x=x_from;
	Object test2=malloc(sizeof(struct object));
	test2->rect.x=x_to;
	for(SetNode node=set_find_node(state->objects,set_find_eq_or_smaller(state->objects,test));node!=set_find_node(state->objects,set_find_eq_or_greater(state->objects,test2 ));node=set_next(state->objects,node)){
		Object obj=set_node_value(state->objects,node);
		list_insert_next(result,LIST_BOF,obj);
	}
	return result;
}

// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

;
void state_update(State state, KeyState keys) {
	if((state->info.paused==false&&state->info.playing==true)||(state->info.paused==true&&keys->n)){ //το παιχνιδι ενημερωνεται μονο οταν δεν ειναι paused ή οταν ειναι paused και ειναι πατημενο το n	
		Object test_platform_1=set_find_eq_or_smaller(state->objects,state->info.ball);
		if(test_platform_1->type==PLATFORM){
			state->current_platform=test_platform_1;
		}
		Object test_platform_2=set_find_eq_or_greater(state->objects,state->info.ball);
		if(test_platform_2->type==PLATFORM){
			state->next_platform=test_platform_2;
		}	//εχω βρει την προηγουμενη και την επομενη πλατφορμα απο την μπαλα ωστε να εξετασω συγκρουσεις μονο με αυτεσ
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
		Object platform_x_from=malloc(sizeof(struct object));
		platform_x_from->rect.x=state->info.ball->rect.x-2*SCREEN_WIDTH;
		Object platform_x_to=malloc(sizeof(struct object));// δημιουργω 2 objects με x1 και χ2 ετσι ωστε να μην διατρεξω ολα τα στοιχια του vector αλλα μονο αυτα που εχουν νοημα
		platform_x_to->rect.x=state->info.ball->rect.x+2*SCREEN_WIDTH;  //θα διατρεξω τα στοιχεια του set απο το χ της μπαλας σε αποσταση 2 οθονων
		Object lastplatform=NULL;
		for(SetNode node=set_find_node(state->objects,set_find_eq_or_smaller(state->objects,platform_x_from));node!=set_find_node(state->objects,set_find_eq_or_greater(state->objects, platform_x_to));node=set_next(state->objects,node)){
			if(CheckCollisionRecs(state->info.ball->rect,state->next_platform->rect)){ //εξετασω συγκρουσεις μονο με την τωρινη και την επομενη πλατφορμα απο την μπαλα
			state->info.ball->vert_mov=IDLE;
			state->info.ball->rect.y=state->
			next_platform->rect.y-40;
			break;
			}
			if(CheckCollisionRecs(state->info.ball->rect,state->current_platform->rect)){
				state->info.ball->vert_mov=IDLE;
				state->info.ball->rect.y=state->current_platform->rect.y-40;
				break;
			}
			if(CheckCollisionRecs(state->info.ball->rect,state->current_platform->rect)==false&&state->info.ball->vert_mov!=JUMPING){
				state->info.ball->vert_speed=1.5;
				state->info.ball->vert_mov=FALLING;
				break;
				}	
			if(CheckCollisionRecs(state->info.ball->rect,state->next_platform->rect)==false&&state->info.ball->vert_mov!=JUMPING){
				state->info.ball->vert_speed=1.5;
				state->info.ball->vert_mov=FALLING;
				break;
			}	
			if((CheckCollisionRecs(state->info.ball->rect,state->next_platform->rect)||CheckCollisionRecs(state->info.ball->rect,state->current_platform->rect))&&state->info.ball->vert_mov==FALLING)	{
				state->info.ball->vert_mov=IDLE;
				state->info.ball->rect.y=state->current_platform->rect.y-40;
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
		for(SetNode node=set_find_node(state->objects,set_find_eq_or_smaller(state->objects,platform_x_from));node!=set_find_node(state->objects,set_find_eq_or_greater(state->objects, platform_x_to));node=set_next(state->objects,node)){
			Object obj=set_node_value(state->objects,node);
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
			if(obj->type==PLATFORM&&obj->unstable==true&&CheckCollisionRecs(state->info.ball->rect,obj->rect)){
				obj->vert_mov=FALLING;
			}
		}
		for(SetNode node=set_find_node(state->objects,set_find_eq_or_smaller(state->objects,platform_x_from));node!=set_find_node(state->objects,set_find_eq_or_greater(state->objects, platform_x_to));node=set_next(state->objects,node)){
			Object obj=set_node_value(state->objects,node);
			if(obj->type==STAR){
				if(CheckCollisionRecs(state->info.ball->rect,obj->rect)){
					set_remove(state->objects,obj);
					state->info.score+=10;
					break;
				}	
			}
		}
		for(SetNode node=set_find_node(state->objects,set_find_eq_or_smaller(state->objects,platform_x_from));node!=set_find_node(state->objects,set_find_eq_or_greater(state->objects,platform_x_to ));node=set_next(state->objects,node)){
			Object obj=set_node_value(state->objects,node);
			if(obj->type==PLATFORM&&obj->vert_mov==FALLING&&obj->rect.y==SCREEN_HEIGHT){
				set_remove(state->objects,obj);
				break;
			}
		}
		for(SetNode node=set_first(state->objects);node!=SET_EOF;node=set_next(state->objects,node)){
			Object obj=set_node_value(state->objects,node);
			if(obj->type==PLATFORM){
				lastplatform=obj;
			}
		}
		if(state->info.ball->rect.x>lastplatform->rect.x-SCREEN_WIDTH){//εαν η μπαλα ξεπερασει το χ της τελευταιας πλατφορμας τοτε βαζουμε νεες πλατφορμες μεσω της add_objects
			add_objects(state,lastplatform->rect.x+250);  // εχω βαλει το +250 ετσι ωστε η νεα πλατφορμα να μην ειναι ακριβως διπλα στην προηγουμενη
			state->speed_factor=state->speed_factor*1.1;
		}
		if(state->info.ball->rect.y>=SCREEN_HEIGHT){//εαν η μπαλα φτασει το y της οθονης τοτε το παιχνιδι σταματαει
			state->info.playing =false;
			state->speed_factor=0;
		}
	}
	if(keys->p ){ //εαν πατησουμε p τοτε η καταστατη του pause αντιστρεφεται
    	state->info.paused = !state->info.paused;
    }	
}

// Καταστρέφει την κατάσταση state ελευ®θερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
		set_destroy(state->objects);
		 free(state);
	free(state->info.ball);
	

}