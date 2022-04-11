#include "stdio.h"
#include "stdlib.h"
#include "circuit.h"
#include "gui.h"

void print_electric(electric_t *electric) {
    printf("resistance:%.2f\ncurrent:%.2f\nvoltage:%.2f\nnode1:%d\nnode2:%d\n",
           electric->resistance, electric->current, electric->voltage,
           electric->node1, electric->node2);
}

void print_electrics(electric_t **electrics) {
    int count = count_electrics(electrics);
    for (int i = 0; i < count; i++)
        print_electric(electrics[i]);
}

electric_t *add_new_electric(electric_t **electrics, float resistance, int node1, int node2) {
    electric_t *electric = malloc(sizeof(electric_t));
    electric->resistance = resistance;
    electric->node1 = node1;
    electric->node2 = node2;
    add_electric(electrics, electric);
    return electric;
}

int main() {
    gui_main();
    float voltage = 10000;
    electric_t **electrics = new_electrics();
    add_new_electric(electrics, 100, ANODE, CATHODE);
    add_new_electric(electrics, 100, ANODE, CATHODE);
    printf("voltage:%.2f\n", voltage);
    printf("current:%.2f\n", run_circuit(electrics, voltage));
    print_electrics(electrics);
    return 0;
}
