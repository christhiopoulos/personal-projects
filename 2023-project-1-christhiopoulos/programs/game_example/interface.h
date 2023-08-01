#pragma once

#include "state.h"

// Î‘ÏÏ‡Î¹ÎºÎ¿Ï€Î¿Î¹ÎµÎ¯ Ï„Î¿ interface Ï„Î¿Ï… Ï€Î±Î¹Ï‡Î½Î¹Î´Î¹Î¿Ï
void interface_init();

// ÎšÎ»ÎµÎ¯Î½ÎµÎ¹ Ï„Î¿ interface Ï„Î¿Ï… Ï€Î±Î¹Ï‡Î½Î¹Î´Î¹Î¿Ï
void interface_close();

// Î£Ï‡ÎµÎ´Î¹Î¬Î¶ÎµÎ¹ Î­Î½Î± frame Î¼Îµ Ï„Î·Î½ Ï„Ï‰ÏÎ¹Î½Î® ÎºÎ±Ï„Î¬ÏƒÏ„Î±ÏƒÎ· Ï„Î¿Ï… Ï€Î±Î¹Ï‡Î½Î´Î¹Î¿Ï
void interface_draw_frame(State state);