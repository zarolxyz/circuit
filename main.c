#include <stdio.h>
#include <stdlib.h>
#include "circuit.h"

void print_electric(electric_t *electric) {
    printf("resistance:%u\ncurrent:%u\nvoltage:%u\nnode1:%u\nnode2:%u\n",
           electric->resistance, electric->current, electric->voltage,
           electric->node1, electric->node2);
}

electric_t *add_new_electric(electric_t **electrics, unsigned int resistance, int node1, int node2) {
    electric_t *electric = calloc(1, sizeof(electric_t));;
    electric->resistance = resistance;
    electric->node1 = node1;
    electric->node2 = node2;
    add_electric(electrics, electric);
    return electric;
}

int main() {
    int voltage = 1000;
    electric_t **electrics = new_electrics();
    add_new_electric(electrics, 1000, ANODE, 2);
    add_new_electric(electrics, 720, 2, CATHODE);
    add_new_electric(electrics, 140, 2, CATHODE);
    direction_t **directions = parse_circuit_directions(electrics);
    printf("%d\n", get_directions_resistance(directions[0]->next));
    return 0;
}
