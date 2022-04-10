//
// Created by zarolxyz on 2022/4/4.
//

#include "gui.h"

SDL_Window *window;
SDL_Surface *surface;
SDL_Renderer *renderer;
TTF_Font *font;
interface_electric_t *interface_electrics;
int selected_x;
int selected_y;
int electric_source_voltage;
int total_current;

void init_gui() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    window = SDL_CreateWindow("circuit", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH * SIZE,
                              (HEIGHT + 1) * SIZE, SDL_WINDOW_SHOWN);
    surface = SDL_GetWindowSurface(window);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    font = TTF_OpenFont(FONT_FILE, FONT_SIZE);
    if (font == NULL)
        quit_gui();
    selected_x = ELECTRIC_SOURCE_X;
    selected_y = ELECTRIC_SOURCE_Y;
    electric_source_voltage = ELECTRIC_UNIT;
    total_current = 0;
}

void quit_gui() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    if (font != NULL) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
    SDL_Quit();
    exit(0);
}

void draw_text(int x, int y, char *text) {
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_Rect src_rect = {0, 0, text_surface->w, text_surface->h};
    SDL_FreeSurface(text_surface);
    SDL_Rect dst_rect = {x, y, text_surface->w, text_surface->h};
    SDL_RenderCopy(renderer, text_texture, &src_rect, &dst_rect);
}

void draw_back() {
    SDL_SetRenderDrawColor(renderer, 96, 96, 96, 255);
    for (int i = 0; i < HEIGHT + 1; i++) {
        SDL_RenderDrawLine(renderer, 0, SIZE / 2 + SIZE * i, WIDTH * SIZE,
                           SIZE / 2 + SIZE * i);
    }
    for (int i = 0; i < WIDTH + 1; i++) {
        SDL_RenderDrawLine(renderer, SIZE * i, SIZE / 2, SIZE * i,
                           SIZE * HEIGHT + SIZE / 2);
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderDrawLine(renderer, SIZE * selected_x, SIZE / 2 + SIZE * selected_y, SIZE * selected_x + SIZE,
                       SIZE / 2 + SIZE * selected_y);
}

void draw_electric(int x, int y, char *string1, char *string2) {
    if (x == selected_x && y == selected_y)
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    else
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, SIZE * x, SIZE / 2 + SIZE * y, SIZE * x + ELECTRIC_WIRE_LENGTH, SIZE / 2 + SIZE * y);
    SDL_RenderDrawLine(renderer, SIZE * x + SIZE - ELECTRIC_WIRE_LENGTH, SIZE / 2 + SIZE * y, SIZE * x + SIZE,
                       SIZE / 2 + SIZE * y);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(renderer, SIZE * x + ELECTRIC_WIRE_LENGTH, SIZE / 2 + SIZE * y,
                       SIZE * x + SIZE - ELECTRIC_WIRE_LENGTH, SIZE / 2 + SIZE * y);
    SDL_Rect rect = {ELECTRIC_RECTANGLE_X(x), ELECTRIC_RECTANGLE_Y(y),
                     ELECTRIC_WIDTH, ELECTRIC_HEIGHT};
    if (x == selected_x && y == selected_y)
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    else
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);
    draw_text(ELECTRIC_RECTANGLE_X(x), ELECTRIC_RECTANGLE_Y(y), string1);
    draw_text(SIZE * x, ELECTRIC_RECTANGLE_Y(y) + ELECTRIC_HEIGHT, string2);
}

void draw_circuit() {
    if (selected_x == ELECTRIC_SOURCE_X && selected_y == ELECTRIC_SOURCE_Y)
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    else
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    char electric_string1[16];
    char electric_string2[32];
    sprintf(electric_string1, "%dV", electric_source_voltage / ELECTRIC_UNIT);
    sprintf(electric_string2, "%dA", total_current / ELECTRIC_UNIT);
    draw_electric(ELECTRIC_SOURCE_X, ELECTRIC_SOURCE_Y, electric_string1, electric_string2);
    interface_electric_t *interface_electric;
    for (interface_electric = interface_electrics;
         interface_electric != NULL; interface_electric = interface_electric->next) {
        if (interface_electric->x == selected_x && interface_electric->y == selected_y) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }
        sprintf(electric_string1, "%dO", interface_electric->electric->resistance / ELECTRIC_UNIT);
        sprintf(electric_string2, "%dV%dA", interface_electric->electric->voltage / ELECTRIC_UNIT,
                interface_electric->electric->current / ELECTRIC_UNIT);
        draw_electric(interface_electric->x, interface_electric->y, electric_string1, electric_string2);
    }
}

void new_interface_electric(int x, int y) {
    if (x == ELECTRIC_SOURCE_X && y == ELECTRIC_SOURCE_Y)
        return;
    interface_electric_t *new_interface_electric = malloc(sizeof(interface_electric_t));
    new_interface_electric->electric = malloc(sizeof(electric_t));
    new_interface_electric->next = NULL;
    new_interface_electric->x = x;
    new_interface_electric->y = y;
    new_interface_electric->electric->node1 = new_interface_electric->electric->node2 = NODE_NULL;
    new_interface_electric->electric->resistance = ELECTRIC_UNIT;
    new_interface_electric->electric->voltage = 0;
    new_interface_electric->electric->current = 0;
    if (interface_electrics == NULL) {
        interface_electrics = new_interface_electric;
    } else {
        interface_electric_t *interface_electric;
        for (interface_electric = interface_electrics;
             interface_electric->next != NULL; interface_electric = interface_electric->next);
        interface_electric->next = new_interface_electric;
    }
}

interface_electric_t *find_interface_electric(int x, int y) {
    interface_electric_t *interface_electric;
    for (interface_electric = interface_electrics;
         interface_electric != NULL; interface_electric = interface_electric->next) {
        if (interface_electric->x == x && interface_electric->y == y)
            return interface_electric;
    }
    return NULL;
}

void delete_interface_electric(interface_electric_t *interface_electric) {
    if (interface_electrics == NULL)
        return;
    interface_electric_t *tmp_interface_electric;
    for (tmp_interface_electric = interface_electrics;
         tmp_interface_electric->next != NULL; tmp_interface_electric = tmp_interface_electric->next) {
        if (tmp_interface_electric->next == interface_electric)
            break;
    }
    tmp_interface_electric->next = interface_electric->next;
}

void gui_draw() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    draw_back();
    draw_circuit();
    SDL_RenderPresent(renderer);
}

void gui_event_loop() {
    SDL_Event event;
    interface_electric_t *interface_electric;
    while (1) {
        gui_draw();
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            if (selected_y > 0)
                                selected_y--;
                            break;
                        case SDLK_DOWN:
                            if (selected_y < HEIGHT)
                                selected_y++;
                            break;
                        case SDLK_LEFT:
                            if (selected_x > 0)
                                selected_x--;
                            break;
                        case SDLK_RIGHT:
                            if (selected_x < WIDTH - 1)
                                selected_x++;
                            break;
                        case SDLK_SPACE:
                            interface_electric = find_interface_electric(selected_x, selected_y);
                            if (interface_electric == NULL)
                                new_interface_electric(selected_x, selected_y);
                            else
                                delete_interface_electric(interface_electric);
                            break;
                    }
                    break;
                case SDL_QUIT:
                    quit_gui();
                    break;
            }
        }
        SDL_Delay(10);
    }
}

void gui_main() {
    init_gui();
    gui_event_loop();
}
