//
// Created by zarolxyz on 2022/3/19.
//

#ifndef CIRCUIT_CIRCUIT_H
#define CIRCUIT_CIRCUIT_H

#define MAXIMUM_ELECTRICS 36
#define MAXIMUM_NODE_ELECTRICS 6

#define ANODE 1
#define CATHODE 0

typedef struct electric_s {
    int resistance;
    int current;
    int voltage;
    int node1;
    int node2;
} electric_t;

typedef struct direction_s {
    electric_t *electric;
    struct direction_s **next;
} direction_t;

electric_t **new_electrics();

electric_t **copy_electrics(electric_t **electrics);

void clear_electrics_status(electric_t **electrics);

electric_t **find_node_electrics(electric_t **electrics, int node);

int count_electrics(electric_t **electrics);

int test_electric_in_electrics(electric_t **electrics, electric_t *electric);

int test_electrics_in_electrics(electric_t **electrics1, electric_t **electrics2);

void add_electric(electric_t **electrics, electric_t *electric);

int test_circuit_direction(electric_t **electrics, electric_t **flag_electrics, electric_t *electric, int node);

direction_t **new_directions();

int count_directions(direction_t **directions);

void add_direction(direction_t **directions, direction_t *direction);

direction_t *new_direction(electric_t *electric);

direction_t *
parse_circuit_direction(electric_t **electrics, electric_t **flag_electrics, electric_t *electric, int node);

direction_t **parse_circuit_directions(electric_t **electrics);

int test_equal_direction(direction_t *direction1, direction_t *direction2);

int test_in_direction(direction_t *direction1, direction_t *direction2);

int test_equal_in_directions(direction_t **directions, direction_t *direction);

direction_t *find_direction_meeting(direction_t *direction1, direction_t *direction2);

direction_t *find_directions_meeting(direction_t **directions);

direction_t *find_previous_direction(direction_t *start_direction, direction_t *direction);

direction_t **find_previous_directions(direction_t **directions, direction_t *direction);

int get_direction_resistance(direction_t *direction, direction_t *end_direction);

int calculate_resistance_and(int resistance1, int resistance2);

int get_directions_resistance(direction_t **directions);

void set_directions_current(direction_t **directions, int current);

void set_direction_current(direction_t *direction, int current);

void set_circuit_voltage(electric_t **electrics);

int run_circuit(electric_t **electrics, int voltage);

#endif //CIRCUIT_CIRCUIT_H
