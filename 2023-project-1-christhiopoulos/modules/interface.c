#include "raylib.h"
#include "state.h"
#include "interface.h"

// Assets
void interface_init() {
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "game_example");
	SetTargetFPS(60);
    InitAudioDevice();

	// Φόρτωση εικόνων και ήχων
	
}

void interface_close() {
	CloseAudioDevice();
	CloseWindow();
}

// Draw game (one frame)
void interface_draw_frame(State state) {
	BeginDrawing();

	// Καθαρισμός, θα τα σχεδιάσουμε όλα από την αρχή
	ClearBackground(RAYWHITE);
	StateInfo info=state_info(state);	
	float ball_x=info->ball->rect.x;
    float x_offset=SCREEN_WIDTH -450-ball_x;	//το offseτ χρησιμοποειται ετσι ωστε να μετατρεπονται οι συντεταγμενες παιχνιδιου σε συντεταγμενες οθονης

	DrawRectangle(ball_x+x_offset,info->ball->rect.y,45,45, RED);  //σχεδιαζουμε την μπαλα
	List objs=state_objects(state,ball_x-SCREEN_WIDTH,ball_x+SCREEN_WIDTH);
	for(ListNode node=list_first(objs);node!=LIST_EOF;node=list_next(objs,node)){
		Object obj=list_node_value(objs,node);
		if(obj->type==PLATFORM&&obj->unstable==false){
			DrawRectangle(obj->rect.x+x_offset, obj->rect.y, obj->rect.width, obj->rect.height, GREEN);  //σχεδιαζουμε τις stable πλατφορμες
		}
		if(obj->type==PLATFORM&&obj->unstable==true){
			DrawRectangle(obj->rect.x+x_offset, obj->rect.y, obj->rect.width, obj->rect.height, RED);  //σχεδιαζουμε τις unstable πλατφορμες
		}
		if(obj->type==STAR){
			DrawRectangle(obj->rect.x+x_offset, obj->rect.y, 45, 45, GREEN); //σχεδιαζουμε τα αστερια
		}
	}

	// Σχεδιάζουμε το σκορ και το FPS counter
	DrawText(TextFormat("%04i", info->score), 20, 20, 40, GRAY);
	DrawFPS(SCREEN_WIDTH - 80, 0);

	// Αν το παιχνίδι έχει τελειώσει, σχεδιάζομαι το μήνυμα για να ξαναρχίσει
	if (!info->playing) {
		DrawText(
			"PRESS [ENTER] TO PLAY AGAIN",
			 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, GRAY
		);
	}

	// Ηχος, αν είμαστε στο frame που συνέβη το game_ove
		
	EndDrawing();
}