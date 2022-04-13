//
// Created by zarolxyz on 2022/4/13.
//

#ifndef CIRCUIT_MAIN_H
#define CIRCUIT_MAIN_H

#include "stdio.h"
#include "stdlib.h"
#include "circuit.h"
#include "gui.h"

void print_electric(electric_t *electric);

void print_electrics(electric_t **electrics);

electric_t *add_new_electric(electric_t **electrics, float resistance, int node1, int node2);

#endif //CIRCUIT_MAIN_H
