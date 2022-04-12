//
// Created by zarolxyz on 2022/4/4.
//

#include "gui.h"

SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font *font;
electric_t *electric_map[WIDTH][HEIGHT - 1];
electric_t **gui_electrics;
int selected_x;
int selected_y;
float electric_source_voltage;
float total_current;
int is_connecting_wire;
int connecting_node;
int new_node;

void init_gui() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    window = SDL_CreateWindow("circuit", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH * SIZE,
                              (HEIGHT + 1) * SIZE, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    font = TTF_OpenFont(FONT_FILE, FONT_SIZE);
    if (font == NULL)
        quit_gui();
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT - 1; j++)
            electric_map[i][j] = NULL;
    }
    gui_electrics = new_electrics();
    selected_x = ELECTRIC_SOURCE_X;
    selected_y = ELECTRIC_SOURCE_Y;
    electric_source_voltage = 1;
    total_current = 0;
    is_connecting_wire = 0;
    new_node = 2;
}

void quit_gui() {
    int count = count_electrics(gui_electrics);
    for (int i = 0; i < count; i++)
        free(gui_electrics[i]);
    free(gui_electrics);
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
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    for (int i = 0; i < HEIGHT + 1; i++) {
        SDL_RenderDrawLine(renderer, 0, SIZE / 2 + SIZE * i, WIDTH * SIZE,
                           SIZE / 2 + SIZE * i);
    }
    for (int i = 0; i < WIDTH + 1; i++) {
        SDL_RenderDrawLine(renderer, SIZE * i, SIZE / 2, SIZE * i,
                           SIZE * HEIGHT + SIZE / 2);
    }
}

void draw_electric(int x, int y, char *string1, char *string2) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer, SIZE * x, SIZE / 2 + SIZE * y, SIZE * x + ELECTRIC_WIRE_LENGTH, SIZE / 2 + SIZE * y);
    SDL_RenderDrawLine(renderer, SIZE * x + SIZE - ELECTRIC_WIRE_LENGTH, SIZE / 2 + SIZE * y, SIZE * x + SIZE,
                       SIZE / 2 + SIZE * y);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(renderer, SIZE * x + ELECTRIC_WIRE_LENGTH, SIZE / 2 + SIZE * y,
                       SIZE * x + SIZE - ELECTRIC_WIRE_LENGTH, SIZE / 2 + SIZE * y);
    SDL_Rect rect = {ELECTRIC_RECTANGLE_X(x), ELECTRIC_RECTANGLE_Y(y),
                     ELECTRIC_WIDTH, ELECTRIC_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);
    draw_text(ELECTRIC_RECTANGLE_X(x), ELECTRIC_RECTANGLE_Y(y), string1);
    draw_text(SIZE * x, ELECTRIC_RECTANGLE_Y(y) + ELECTRIC_HEIGHT, string2);
}

void draw_node1_wire(int x1, int y1, int x2, int y2) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    if (x2 < x1) {
        SDL_RenderDrawLine(renderer, SIZE * x2, SIZE / 2 + SIZE * y1, SIZE * x1, SIZE / 2 + SIZE * y1);
        SDL_RenderDrawLine(renderer, SIZE * x2, SIZE / 2 + SIZE * y1, SIZE * x2, SIZE / 2 + SIZE * y2);
    } else {
        SDL_RenderDrawLine(renderer, SIZE * x1, SIZE / 2 + SIZE * y1, SIZE * x1, SIZE / 2 + SIZE * y2);
        SDL_RenderDrawLine(renderer, SIZE * x1, SIZE / 2 + SIZE * y2, SIZE * x2, SIZE / 2 + SIZE * y2);
    }
}


void draw_node2_wire(int x1, int y1, int x2, int y2) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    if (x2 > x1) {
        SDL_RenderDrawLine(renderer, SIZE * x2, SIZE / 2 + SIZE * y1, SIZE * x1, SIZE / 2 + SIZE * y1);
        SDL_RenderDrawLine(renderer, SIZE * x2, SIZE / 2 + SIZE * y1, SIZE * x2, SIZE / 2 + SIZE * y2);
    } else {
        SDL_RenderDrawLine(renderer, SIZE * x1, SIZE / 2 + SIZE * y1, SIZE * x1, SIZE / 2 + SIZE * y2);
        SDL_RenderDrawLine(renderer, SIZE * x1, SIZE / 2 + SIZE * y2, SIZE * x2, SIZE / 2 + SIZE * y2);
    }
}

void draw_circuit() {
    char electric_string1[16];
    char electric_string2[32];
    sprintf(electric_string1, "%.2fV", electric_source_voltage);
    sprintf(electric_string2, "%.2fA", total_current);
    draw_electric(ELECTRIC_SOURCE_X, ELECTRIC_SOURCE_Y, electric_string1, electric_string2);
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT - 1; j++) {
            electric_t *electric = electric_map[i][j];
            if (electric != NULL) {
                sprintf(electric_string1, "%.2fO", electric->resistance);
                sprintf(electric_string2, "%.2fV%.2fA", electric->voltage, electric->current);
                draw_electric(i, j, electric_string1, electric_string2);
                if (electric->node1 == ANODE)
                    draw_node1_wire(i, j, ELECTRIC_SOURCE_X, ELECTRIC_SOURCE_Y);
                if (electric->node1 == CATHODE)
                    draw_node1_wire(i, j, ELECTRIC_SOURCE_X + 1, ELECTRIC_SOURCE_Y);
                if (electric->node2 == ANODE)
                    draw_node2_wire(i + 1, j, ELECTRIC_SOURCE_X, ELECTRIC_SOURCE_Y);
                if (electric->node2 == CATHODE)
                    draw_node2_wire(i + 1, j, ELECTRIC_SOURCE_X + 1, ELECTRIC_SOURCE_Y);
                for (int k = 0; k < WIDTH; k++) {
                    for (int l = 0; l < HEIGHT - 1; l++) {
                        if (!(k == i && l == j) && electric_map[k][l] != NULL) {
                            if (electric->node1 == electric_map[k][l]->node1)
                                draw_node1_wire(i, j, k, l);
                            if (electric->node1 == electric_map[k][l]->node2)
                                draw_node1_wire(i, j, k + 1, l);
                            if (electric->node2 == electric_map[k][l]->node1)
                                draw_node2_wire(i + 1, j, k, l);
                            if (electric->node2 == electric_map[k][l]->node2)
                                draw_node2_wire(i + 1, j, k + 1, l);
                        }
                    }
                }
            }
        }
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderDrawLine(renderer, SIZE * selected_x, SIZE / 2 + SIZE * selected_y, SIZE * selected_x + SIZE,
                       SIZE / 2 + SIZE * selected_y);
}

electric_t *gui_new_electric() {
    electric_t *electric = malloc(sizeof(electric_t));
    electric->resistance = 1;
    electric->voltage = 0;
    electric->current = 0;
    electric->node1 = new_node;
    new_node++;
    electric->node2 = new_node;
    new_node++;
    add_electric(gui_electrics, electric);
    return electric;
}

void gui_delete_electric(electric_t *electric) {
    delete_electric(gui_electrics, electric);
    free(electric);
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
    while (1) {
        total_current = run_circuit(gui_electrics, electric_source_voltage);
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
                            if (!(selected_x == ELECTRIC_SOURCE_X && selected_y == ELECTRIC_SOURCE_Y)) {
                                if (electric_map[selected_x][selected_y] == NULL) {
                                    if ((selected_x == 0 || electric_map[selected_x - 1][selected_y] == NULL) &&
                                        electric_map[selected_x + 1][selected_y] == NULL)
                                        electric_map[selected_x][selected_y] = gui_new_electric();
                                } else {
                                    gui_delete_electric(electric_map[selected_x][selected_y]);
                                    electric_map[selected_x][selected_y] = NULL;
                                }
                            }
                            break;
                        case SDLK_q:
                            if ((selected_x == ELECTRIC_SOURCE_X && selected_y == ELECTRIC_SOURCE_Y) &&
                                electric_source_voltage - 0.1 > 0)
                                electric_source_voltage -= 0.1;
                            else {
                                if (electric_map[selected_x][selected_y] != NULL &&
                                    electric_map[selected_x][selected_y]->resistance - 0.1 > 0)
                                    electric_map[selected_x][selected_y]->resistance -= 0.1;
                            }
                            break;
                        case SDLK_w:
                            if ((selected_x == ELECTRIC_SOURCE_X && selected_y == ELECTRIC_SOURCE_Y))
                                electric_source_voltage += 0.1;
                            else {
                                if (electric_map[selected_x][selected_y] != NULL)
                                    electric_map[selected_x][selected_y]->resistance += 0.1;
                            }
                            break;
                        case SDLK_e:
                            if ((selected_x == ELECTRIC_SOURCE_X && selected_y == ELECTRIC_SOURCE_Y) &&
                                electric_source_voltage - 1 > 0)
                                electric_source_voltage -= 1;
                            else {
                                if (electric_map[selected_x][selected_y] != NULL &&
                                    electric_map[selected_x][selected_y]->resistance - 1 > 0)
                                    electric_map[selected_x][selected_y]->resistance -= 1;
                            }
                            break;
                        case SDLK_r:
                            if ((selected_x == ELECTRIC_SOURCE_X && selected_y == ELECTRIC_SOURCE_Y))
                                electric_source_voltage += 1;
                            else {
                                if (electric_map[selected_x][selected_y] != NULL)
                                    electric_map[selected_x][selected_y]->resistance += 1;
                            }
                            break;
                        case SDLK_t:
                            if ((selected_x == ELECTRIC_SOURCE_X && selected_y == ELECTRIC_SOURCE_Y) &&
                                electric_source_voltage - 10 > 0)
                                electric_source_voltage -= 10;
                            else {
                                if (electric_map[selected_x][selected_y] != NULL &&
                                    electric_map[selected_x][selected_y]->resistance - 10 > 0)
                                    electric_map[selected_x][selected_y]->resistance -= 10;
                            }
                            break;
                        case SDLK_y:
                            if ((selected_x == ELECTRIC_SOURCE_X && selected_y == ELECTRIC_SOURCE_Y))
                                electric_source_voltage += 10;
                            else {
                                if (electric_map[selected_x][selected_y] != NULL)
                                    electric_map[selected_x][selected_y]->resistance += 10;
                            }
                            break;
                        case SDLK_a:
                            if (electric_map[selected_x][selected_y] != NULL) {
                                if (!is_connecting_wire) {
                                    is_connecting_wire = 1;
                                    connecting_node = electric_map[selected_x][selected_y]->node1;
                                } else {
                                    is_connecting_wire = 0;
                                    electric_t **node_electrics;
                                    if (electric_map[selected_x][selected_y]->node1 == ANODE ||
                                        electric_map[selected_x][selected_y]->node1 == CATHODE) {
                                        node_electrics = find_node_electrics(gui_electrics, connecting_node);
                                        int count = count_electrics(node_electrics);
                                        for (int i = 0; i < count; i++) {
                                            if (node_electrics[i]->node1 == connecting_node)
                                                node_electrics[i]->node1 = electric_map[selected_x][selected_y]->node1;
                                            else
                                                node_electrics[i]->node2 = electric_map[selected_x][selected_y]->node1;
                                        }
                                    } else {
                                        node_electrics = find_node_electrics(gui_electrics,
                                                                             electric_map[selected_x][selected_y]->node1);
                                        int count = count_electrics(node_electrics);
                                        for (int i = 0; i < count; i++) {
                                            if (node_electrics[i]->node1 == electric_map[selected_x][selected_y]->node1)
                                                node_electrics[i]->node1 = connecting_node;
                                            else
                                                node_electrics[i]->node2 = connecting_node;
                                        }
                                    }
                                }
                            } else if (selected_x == ELECTRIC_SOURCE_X && selected_y == ELECTRIC_SOURCE_Y) {
                                if (!is_connecting_wire) {
                                    is_connecting_wire = 1;
                                    connecting_node = ANODE;
                                } else {
                                    is_connecting_wire = 0;
                                    electric_t **node_electrics = find_node_electrics(gui_electrics, connecting_node);
                                    int count = count_electrics(node_electrics);
                                    for (int i = 0; i < count; i++) {
                                        if (node_electrics[i]->node1 == connecting_node)
                                            node_electrics[i]->node1 = ANODE;
                                        else
                                            node_electrics[i]->node2 = ANODE;
                                    }
                                }
                            }
                            break;
                        case SDLK_s:
                            if (electric_map[selected_x][selected_y] != NULL) {
                                if (!is_connecting_wire) {
                                    is_connecting_wire = 1;
                                    connecting_node = electric_map[selected_x][selected_y]->node2;
                                } else {
                                    is_connecting_wire = 0;
                                    electric_t **node_electrics;
                                    if (electric_map[selected_x][selected_y]->node2 == ANODE ||
                                        electric_map[selected_x][selected_y]->node2 == CATHODE) {
                                        node_electrics = find_node_electrics(gui_electrics, connecting_node);
                                        int count = count_electrics(node_electrics);
                                        for (int i = 0; i < count; i++) {
                                            if (node_electrics[i]->node1 == connecting_node)
                                                node_electrics[i]->node1 = electric_map[selected_x][selected_y]->node2;
                                            else
                                                node_electrics[i]->node2 = electric_map[selected_x][selected_y]->node2;
                                        }
                                    } else {
                                        node_electrics = find_node_electrics(gui_electrics,
                                                                             electric_map[selected_x][selected_y]->node2);
                                        int count = count_electrics(node_electrics);
                                        for (int i = 0; i < count; i++) {
                                            if (node_electrics[i]->node1 == electric_map[selected_x][selected_y]->node2)
                                                node_electrics[i]->node1 = connecting_node;
                                            else
                                                node_electrics[i]->node2 = connecting_node;
                                        }
                                    }
                                }
                            } else if (selected_x == ELECTRIC_SOURCE_X && selected_y == ELECTRIC_SOURCE_Y) {
                                if (!is_connecting_wire) {
                                    is_connecting_wire = 1;
                                    connecting_node = CATHODE;
                                } else {
                                    is_connecting_wire = 0;
                                    electric_t **node_electrics = find_node_electrics(gui_electrics, connecting_node);
                                    int count = count_electrics(node_electrics);
                                    for (int i = 0; i < count; i++) {
                                        if (node_electrics[i]->node1 == connecting_node)
                                            node_electrics[i]->node1 = CATHODE;
                                        else
                                            node_electrics[i]->node2 = CATHODE;
                                    }
                                }
                            }
                            break;
                        case SDLK_d:
                            if (electric_map[selected_x][selected_y] != NULL) {
                                electric_map[selected_x][selected_y]->node1 = new_node;
                                new_node++;
                            }
                            break;
                        case SDLK_f:
                            if (electric_map[selected_x][selected_y] != NULL) {
                                electric_map[selected_x][selected_y]->node2 = new_node;
                                new_node++;
                            }
                            break;
                    }
                    break;
                case SDL_QUIT:
                    return;
            }
        }
        SDL_Delay(10);
    }
}

void gui_main() {
    init_gui();
    gui_event_loop();
    quit_gui();
}
