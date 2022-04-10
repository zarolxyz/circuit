//
// Created by zarolxyz on 2022/4/4.
//

#ifndef CIRCUIT_GUI_H
#define CIRCUIT_GUI_H

#include "stdlib.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "circuit.h"

#define FONT_FILE "padmaa.ttf"
#define FONT_SIZE 14
#define SIZE 80
#define WIDTH 23
#define HEIGHT 12
#define ELECTRIC_WIDTH (SIZE * 4 / 5)
#define ELECTRIC_HEIGHT (SIZE * 2 / 5)
#define ELECTRIC_WIRE_LENGTH ((SIZE - ELECTRIC_WIDTH) / 2)
#define ELECTRIC_SOURCE_X 11
#define ELECTRIC_SOURCE_Y 6
#define ELECTRIC_RECTANGLE_X(x) (SIZE * (x) + (SIZE - ELECTRIC_WIDTH) / 2)
#define ELECTRIC_RECTANGLE_Y(y) (SIZE / 2 + SIZE * (y) - ELECTRIC_HEIGHT / 2)
#define ELECTRIC_UNIT 10000

typedef struct interface_electric_s {
    int x;
    int y;
    electric_t *electric;
    struct interface_electric_s *next;
} interface_electric_t;

extern SDL_Window *window;
extern SDL_Surface *surface;
extern SDL_Renderer *renderer;
extern TTF_Font *font;
extern interface_electric_t *interface_electrics;
extern int selected_x;
extern int selected_y;
extern int electric_source_voltage;
extern int total_current;

void init_gui();

void quit_gui();

void draw_text(int x, int y, char *text);

void draw_back();

void draw_electric(int x, int y, char *string1, char *string2);

void draw_circuit();

void new_interface_electric(int x, int y);

interface_electric_t *find_interface_electric(int x, int y);

void delete_interface_electric(interface_electric_t *interface_electric);

void gui_draw();

void gui_event_loop();

void gui_main();

#endif //CIRCUIT_GUI_H
