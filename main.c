#include <stdio.h>
#include <stdlib.h>
#include "circuit.h"

void print_electric(electric_t *electric) {
    printf("resistance:%u\ncurrent:%u\nvoltage:%u\ndirect:%u\nnode1:%u\nnode2:%u\n",
           electric->resistance, electric->current, electric->voltage,
           electric->direct, electric->node1, electric->node2);
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
    add_new_electric(electrics, 1000, 2, CATHODE);
    add_new_electric(electrics, 120, 2, CATHODE);
    direct_t **directs = parse_circuit_directs(electrics);
    print_electric(directs[0]->next[0]->electric);
    return 0;
}
