//////////////////////////////////////////////////////////////////
//
// Test για το state.h module
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include "ADTSet.h"
#include "state.h"
#include "set_utils.h"
int compare_ints(Pointer a, Pointer b) {       //η συναρτηση που χρειαζεται για να διαταξει τα στοιχεια στην pqueue και στο set
int* ia = a;
int* ib = b;
return *ia - *ib;
}
int* create_int(int value) {
	int* pointer = malloc(sizeof(int));		// δέσμευση μνήμης	
	*pointer = value;						// αντιγραφή του value στον νέο ακέραιο
	return pointer;
}

void test_state_create() {

	State state = state_create();
	TEST_ASSERT(state != NULL);

	StateInfo info = state_info(state);
	TEST_ASSERT(info != NULL);

	TEST_ASSERT(info->playing);
	TEST_ASSERT(!info->paused);
	TEST_ASSERT(info->score == 0);
	TEST_ASSERT(info->ball->rect.height==45);
    TEST_ASSERT(info->ball->vert_mov==IDLE);
}

void test_state_update() {
	//τεστ για την λειτουργια της state_create και της state_info
	State state = state_create();
	TEST_ASSERT(state != NULL && state_info(state) != NULL);


	// Πληροφορίες για τα πλήκτρα (αρχικά κανένα δεν είναι πατημένο)
	struct key_state keys = { false, false, false, false, false, false, false };


	// Χωρίς κανένα πλήκτρο, η μπάλα μετακινείται 4 pixels δεξιά
	Rectangle old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	Rectangle new_rect = state_info(state)->ball->rect;
	TEST_ASSERT( new_rect.x == old_rect.x + 4 );


	// Με πατημένο το δεξί βέλος, η μπάλα μετακινείται 6 pixels δεξιά
	keys.right = true;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;
	TEST_CHECK( new_rect.x == old_rect.x + 6 );


	// οταν η μπαλα πηδαει τοτε παει προς τα πανω οσο ειναι και η κατακορυφη ταχυτητα της και η ταχυτητα μειωνεται κατα 15%
	keys.right = false;
	state_info(state)->ball->vert_mov =JUMPING;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;
	TEST_CHECK( new_rect.y == state_info(state)->ball->rect.y-state_info(state)->ball->vert_speed);
	float old_speed= state_info(state)->ball->vert_speed;
	state_update(state, &keys);
	float new_speed= state_info(state)->ball->vert_speed;
	TEST_CHECK( new_speed ==0.85*old_speed);


	// οταν η μπαλα βρισκεται σε κατασταση falling τοτε παει προς τα κατω οσο ειναι και η κατακορυφη ταχυτητα της και η ταχυτητα αυξανεται κατα 110%
	state_info(state)->ball->vert_mov=FALLING;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;
	TEST_CHECK( new_rect.y == state_info(state)->ball->rect.y-state_info(state)->ball->vert_speed);
	old_speed= state_info(state)->ball->vert_speed;
	state_update(state, &keys);
	new_speed= state_info(state)->ball->vert_speed;
	TEST_CHECK( new_speed ==2.1*old_speed);


	// Με πατημένο το αριστερο  βέλος, η μπάλα μετακινείται 6 pixels δεξια
    keys.left=true;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;
	TEST_CHECK( new_rect.x == old_rect.x+1);


	//τεσταρω αν μπανει με τον σωστο τροπο στην κατασταση jumping
	keys.left=false;
	keys.up=true;
	state_info(state)->ball->vert_mov=IDLE;
	state_update(state, &keys);
	TEST_CHECK( state_info(state)->ball->vert_mov==JUMPING);
	state_update(state, &keys);


	 //εαν η ταχυτητα της μπαλας γινει λιγοτερη απο 0.5,καθως βρισκεται σε κατασταση jumping,τοτε αρχιζει και πεφτει
	state_info(state)->ball->vert_speed=0.45;    
	state_update(state, &keys);
	TEST_CHECK( state_info(state)->ball->vert_mov==FALLING);
	keys.up=false;
      

	//οταν η μπαλα ειναι πανω σε μια πλατφορμα ειναι idle,το y της ακολουθει την πλατφορμα και βρισκεται σε συγκρουση με αυτη
	Object obj=NULL;
	Object obj2=NULL;
	List list=state_objects(state,0,SCREEN_WIDTH);
	for(ListNode node=list_first(list);node!=LIST_EOF;node=list_next(list,node)){
		obj2=list_node_value(list,node);
		if(obj2->type==PLATFORM){
			obj=obj2;
			break;
		}
	}
	state_info(state)->ball->rect.x=obj->rect.x;
	state_info(state)->ball->rect.y=obj->rect.y;
	state_update(state, &keys);
	TEST_CHECK(CheckCollisionRecs(state_info(state)->ball->rect,obj->rect)==true);	
	TEST_CHECK(state_info(state)->ball->vert_mov==IDLE);
	TEST_CHECK(state_info(state)->ball->rect.y=obj->rect.y-40);


    //οταν το χ της μπαλας δεν βρισκεται πανω σε μια πλατφορμα τοτε αυτη αρχιζει να πεφτει
	state_info(state)->ball->rect.x=obj->rect.x+obj->rect.width+50;
	state_update(state, &keys);	
	TEST_CHECK(state_info(state)->ball->vert_mov==FALLING);


	//οταν η μπαλα ειναι σε κατασταση falling και συγκρουστει με πλατφορμα τοτε γινεται idle
	state_info(state)->ball->vert_mov=FALLING;
	state_info(state)->ball->rect.x=obj->rect.x;
	state_info(state)->ball->rect.y=obj->rect.y;
	state_update(state, &keys);
	TEST_CHECK(state_info(state)->ball->vert_mov==IDLE);
	

	//οταν η μπαλα ακουμπησει το κατω μερος της οθονης το παιχνιδι εχει τελιωσει
	state_info(state)->ball->rect.y=SCREEN_WIDTH+100;
	state_update(state, &keys);
	TEST_CHECK( state_info(state)->playing==false);
	keys.n=true;
	

	//οταν πατιεται το p και το παιχνδι πριν ειναι paused=false τοτε γινεται paused=true
	keys.p=true;
	state_update(state, &keys);
	TEST_CHECK( state_info(state)->paused==true);
	keys.p=false;
	

	//ελεγχος για συγκρουση πλατφορμας με αστερι και για αυξηση του σκορ κατα 10
	list=state_objects(state,0,10*SCREEN_WIDTH);
	for(ListNode node=list_first(list);node!=LIST_EOF;node=list_next(list,node)){
		obj2=list_node_value(list,node);
		if(obj2->type==STAR){
			obj=obj2;
			break;
		}
	 }
    int old_score= state_info(state)->score;
	state_info(state)->ball->rect.x=obj->rect.x;
	state_info(state)->ball->rect.y=obj->rect.y;
	state_update(state, &keys);
	int new_score= state_info(state)->score;
	TEST_CHECK(CheckCollisionRecs(state_info(state)->ball->rect,obj->rect)==true);
	TEST_CHECK(new_score=old_score+10);
}


 void test_state_objects(){
	//τεστ για το αν η state objects επιστρεφει μια μη κενη λιστα, για διαφορες τιμες του x_from και x_to
	State state = state_create();
	TEST_ASSERT(state != NULL && state_info(state) != NULL);
	List test=state_objects(state,0,SCREEN_WIDTH);
	TEST_ASSERT( test!= NULL );
	List test2=state_objects(state,0,2*SCREEN_WIDTH);
	TEST_ASSERT( test2!= NULL );
	List test3=state_objects(state,SCREEN_WIDTH,2*SCREEN_WIDTH);
	TEST_ASSERT( test3!= NULL );
}


void test_set_utils(){
	//τεστ για την λειτουργια των συναρτησεων που περιγραφονται στο set_utils αρχειο
	Set set=set_create(compare_ints,NULL);
	for(int i=0;i<10;i++){
		set_insert(set,create_int(i));
	}
	int* test=create_int(6);
	int *a=set_find_eq_or_greater(set,test);
	TEST_CHECK( *test==*a);
	test=create_int(12);
	int *b=set_find_eq_or_smaller(set,test);
	TEST_CHECK( *b==9);
}

// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_state_create", test_state_create },
	{ "test_state_update", test_state_update },
    { "test_state_objects", test_state_objects },  
	{" test_set_utils",test_set_utils},
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};