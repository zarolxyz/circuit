//
// Created by zarolxyz on 2022/4/4.
//

#include "gui.h"

SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font *font;
electric_t *electric_map[WIDTH][HEIGHT + 1];
electric_t **electrics;
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
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT + 1; j++)
            electric_map[i][j] = NULL;
    }
    electrics = new_electrics();
    selected_x = ELECTRIC_SOURCE_X;
    selected_y = ELECTRIC_SOURCE_Y;
    electric_source_voltage = 1;
    total_current = 0;
    is_connecting_wire = 0;
    new_node = 2;
}

void quit_gui() {
    int count = count_electrics(electrics);
    for (int i = 0; i < count; i++)
        free(electrics[i]);
    free(electrics);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}

void draw_text(int x, int y, char *text) {
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_Rect src_rect = {0, 0, text_surface->w, text_surface->h};
    SDL_FreeSurface(text_surface);
    SDL_Rect dst_rect = {x, y, text_surface->w, text_surface->h};
    SDL_RenderCopy(renderer, text_texture, &src_rect, &dst_rect);
    SDL_DestroyTexture(text_texture);
}

void draw_electric(int x, int y, char *string1, char *string2) {
    x = SIZE * (x) + (SIZE - ELECTRIC_WIDTH) / 2;
    y = SIZE / 2 + SIZE * (y) - ELECTRIC_HEIGHT / 2;
    SDL_Rect rect = {x, y, ELECTRIC_WIDTH, ELECTRIC_HEIGHT};
    SDL_RenderDrawRect(renderer, &rect);
    draw_text(x, y, string1);
    draw_text(x, y + ELECTRIC_HEIGHT, string2);
}

void draw_node1_wire(int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(renderer, SIZE * x1, SIZE / 2 + SIZE * y1, SIZE * x1 + ELECTRIC_WIRE_LENGTH,
                       SIZE / 2 + SIZE * y1);
    if (x2 < x1) {
        SDL_RenderDrawLine(renderer, SIZE * x2, SIZE / 2 + SIZE * y1, SIZE * x1, SIZE / 2 + SIZE * y1);
        SDL_RenderDrawLine(renderer, SIZE * x2, SIZE / 2 + SIZE * y1, SIZE * x2, SIZE / 2 + SIZE * y2);
    } else {
        SDL_RenderDrawLine(renderer, SIZE * x1, SIZE / 2 + SIZE * y1, SIZE * x1, SIZE / 2 + SIZE * y2);
        SDL_RenderDrawLine(renderer, SIZE * x1, SIZE / 2 + SIZE * y2, SIZE * x2, SIZE / 2 + SIZE * y2);
    }
}


void draw_node2_wire(int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(renderer, SIZE * x1, SIZE / 2 + SIZE * y1, SIZE * x1 - ELECTRIC_WIRE_LENGTH,
                       SIZE / 2 + SIZE * y1);
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
    sprintf(electric_string1, "%.1fV", electric_source_voltage);
    sprintf(electric_string2, "%.2fA", total_current);
    draw_electric(ELECTRIC_SOURCE_X, ELECTRIC_SOURCE_Y, electric_string1, electric_string2);
    draw_node1_wire(ELECTRIC_SOURCE_X, ELECTRIC_SOURCE_Y, ELECTRIC_SOURCE_X, ELECTRIC_SOURCE_Y);
    draw_node2_wire(ELECTRIC_SOURCE_X + 1, ELECTRIC_SOURCE_Y, ELECTRIC_SOURCE_X + 1, ELECTRIC_SOURCE_Y);
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT - 1; j++) {
            electric_t *electric = electric_map[i][j];
            if (electric != NULL) {
                sprintf(electric_string1, "%.0fO", electric->resistance);
                sprintf(electric_string2, "%.1fV%.2fA", electric->voltage, electric->current);
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
    SDL_RenderDrawLine(renderer, SIZE * selected_x, SIZE / 2 + SIZE * selected_y, SIZE * selected_x + SIZE,
                       SIZE / 2 + SIZE * selected_y);
}

electric_t *new_electric() {
    electric_t *electric = malloc(sizeof(electric_t));
    electric->resistance = 1;
    electric->voltage = 0;
    electric->current = 0;
    electric->node1 = new_node;
    new_node++;
    electric->node2 = new_node;
    new_node++;
    add_electric(electrics, electric);
    return electric;
}

void destroy_electric(electric_t *electric) {
    delete_electric(electrics, electric);
    free(electric);
}

void event_loop() {
    int is_circuit_update = 0;
    SDL_Event event;
    while (1) {
        if (is_circuit_update) {
            is_circuit_update = 0;
            total_current = run_circuit(electrics, electric_source_voltage);
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        draw_circuit();
        SDL_RenderPresent(renderer);
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_w:
                            if (selected_y > 0)
                                selected_y--;
                            break;
                        case SDLK_s:
                            if (selected_y < HEIGHT)
                                selected_y++;
                            break;
                        case SDLK_a:
                            if (selected_x > 0)
                                selected_x--;
                            break;
                        case SDLK_d:
                            if (selected_x < WIDTH - 1)
                                selected_x++;
                            break;
                        case SDLK_l:
                            is_circuit_update = 1;
                            if (!(selected_x == ELECTRIC_SOURCE_X && selected_y == ELECTRIC_SOURCE_Y)) {
                                if (electric_map[selected_x][selected_y] == NULL)
                                    electric_map[selected_x][selected_y] = new_electric();
                                else {
                                    destroy_electric(electric_map[selected_x][selected_y]);
                                    electric_map[selected_x][selected_y] = NULL;
                                }
                            }
                            break;
                        case SDLK_q:
                            is_circuit_update = 1;
                            if ((selected_x == ELECTRIC_SOURCE_X && selected_y == ELECTRIC_SOURCE_Y) &&
                                electric_source_voltage - 0.1 > 0)
                                electric_source_voltage -= 0.1;
                            else {
                                if (electric_map[selected_x][selected_y] != NULL &&
                                    electric_map[selected_x][selected_y]->resistance - 1 > 0)
                                    electric_map[selected_x][selected_y]->resistance -= 1;
                            }
                            break;
                        case SDLK_e:
                            is_circuit_update = 1;
                            if ((selected_x == ELECTRIC_SOURCE_X && selected_y == ELECTRIC_SOURCE_Y))
                                electric_source_voltage += 0.1;
                            else {
                                if (electric_map[selected_x][selected_y] != NULL)
                                    electric_map[selected_x][selected_y]->resistance += 1;
                            }
                            break;
                        case SDLK_j:
                            is_circuit_update = 1;
                            if (electric_map[selected_x][selected_y] != NULL) {
                                if (!is_connecting_wire) {
                                    is_connecting_wire = 1;
                                    connecting_node = electric_map[selected_x][selected_y]->node1;
                                } else {
                                    is_connecting_wire = 0;
                                    electric_t **node_electrics;
                                    if (electric_map[selected_x][selected_y]->node1 == ANODE ||
                                        electric_map[selected_x][selected_y]->node1 == CATHODE) {
                                        node_electrics = find_node_electrics(electrics, connecting_node);
                                        int count = count_electrics(node_electrics);
                                        for (int i = 0; i < count; i++) {
                                            if (node_electrics[i]->node1 == connecting_node)
                                                node_electrics[i]->node1 = electric_map[selected_x][selected_y]->node1;
                                            else
                                                node_electrics[i]->node2 = electric_map[selected_x][selected_y]->node1;
                                        }
                                        free(node_electrics);
                                    } else {
                                        int node = electric_map[selected_x][selected_y]->node1;
                                        node_electrics = find_node_electrics(electrics, node);
                                        int count = count_electrics(node_electrics);
                                        for (int i = 0; i < count; i++) {
                                            if (node_electrics[i]->node1 == node)
                                                node_electrics[i]->node1 = connecting_node;
                                            else
                                                node_electrics[i]->node2 = connecting_node;
                                        }
                                        free(node_electrics);
                                    }
                                }
                            } else if (selected_x == ELECTRIC_SOURCE_X && selected_y == ELECTRIC_SOURCE_Y) {
                                if (!is_connecting_wire) {
                                    is_connecting_wire = 1;
                                    connecting_node = ANODE;
                                } else {
                                    is_connecting_wire = 0;
                                    electric_t **node_electrics = find_node_electrics(electrics, connecting_node);
                                    int count = count_electrics(node_electrics);
                                    for (int i = 0; i < count; i++) {
                                        if (node_electrics[i]->node1 == connecting_node)
                                            node_electrics[i]->node1 = ANODE;
                                        else
                                            node_electrics[i]->node2 = ANODE;
                                    }
                                    free(node_electrics);
                                }
                            }
                            break;
                        case SDLK_k:
                            is_circuit_update = 1;
                            if (electric_map[selected_x][selected_y] != NULL) {
                                if (!is_connecting_wire) {
                                    is_connecting_wire = 1;
                                    connecting_node = electric_map[selected_x][selected_y]->node2;
                                } else {
                                    is_connecting_wire = 0;
                                    electric_t **node_electrics;
                                    if (electric_map[selected_x][selected_y]->node2 == ANODE ||
                                        electric_map[selected_x][selected_y]->node2 == CATHODE) {
                                        node_electrics = find_node_electrics(electrics, connecting_node);
                                        int count = count_electrics(node_electrics);
                                        for (int i = 0; i < count; i++) {
                                            if (node_electrics[i]->node1 == connecting_node)
                                                node_electrics[i]->node1 = electric_map[selected_x][selected_y]->node2;
                                            else
                                                node_electrics[i]->node2 = electric_map[selected_x][selected_y]->node2;
                                        }
                                        free(node_electrics);
                                    } else {
                                        int node = electric_map[selected_x][selected_y]->node2;
                                        node_electrics = find_node_electrics(electrics, node);
                                        int count = count_electrics(node_electrics);
                                        for (int i = 0; i < count; i++) {
                                            if (node_electrics[i]->node1 == node)
                                                node_electrics[i]->node1 = connecting_node;
                                            else
                                                node_electrics[i]->node2 = connecting_node;
                                        }
                                        free(node_electrics);
                                    }
                                }
                            } else if (selected_x == ELECTRIC_SOURCE_X && selected_y == ELECTRIC_SOURCE_Y) {
                                if (!is_connecting_wire) {
                                    is_connecting_wire = 1;
                                    connecting_node = CATHODE;
                                } else {
                                    is_connecting_wire = 0;
                                    electric_t **node_electrics = find_node_electrics(electrics, connecting_node);
                                    int count = count_electrics(node_electrics);
                                    for (int i = 0; i < count; i++) {
                                        if (node_electrics[i]->node1 == connecting_node)
                                            node_electrics[i]->node1 = CATHODE;
                                        else
                                            node_electrics[i]->node2 = CATHODE;
                                    }
                                    free(node_electrics);
                                }
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

int main() {
    init_gui();
    event_loop();
    quit_gui();
    return 0;
}
