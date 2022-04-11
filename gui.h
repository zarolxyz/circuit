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

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern TTF_Font *font;
extern electric_t *electric_map[WIDTH][HEIGHT - 1];
extern electric_t **gui_electrics;
extern int selected_x;
extern int selected_y;
extern float electric_source_voltage;
extern float total_current;
extern int is_connecting_wire;
extern int connecting_node;
extern int new_node;

void init_gui();

void quit_gui();

void draw_text(int x, int y, char *text);

void draw_back();

void draw_electric(int x, int y, char *string1, char *string2);

void draw_node1_wire(int x1, int y1, int x2, int y2);

void draw_node2_wire(int x1, int y1, int x2, int y2);

void draw_circuit();

electric_t *gui_new_electric();

void gui_delete_electric(electric_t *electric);

void gui_draw();

void gui_event_loop();

void gui_main();

#endif //CIRCUIT_GUI_H
