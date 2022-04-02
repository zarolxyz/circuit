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
    unsigned int resistance;
    unsigned int current;
    unsigned int voltage;
    unsigned int direct;
    int node1;
    int node2;
} electric_t;

typedef struct direct_s {
    electric_t *electric;
    struct direct_s **next;
} direct_t;

electric_t **new_electrics();

electric_t **copy_electrics(electric_t **electrics);

void clear_electrics_status(electric_t **electrics);

electric_t **find_node_electrics(electric_t **electrics, unsigned int node);

int count_electrics(electric_t **electrics);

int test_electric_in_electrics(electric_t **electrics, electric_t *electric);

int test_electrics_in_electrics(electric_t **electrics1, electric_t **electrics2);

void add_electric(electric_t **electrics, electric_t *electric);

void delete_electric(electric_t **electrics, electric_t *electric);

int test_circuit_direct(electric_t **electrics, electric_t **flag_electrics, electric_t *electric, unsigned int node);

direct_t **new_directs();

int count_directs(direct_t **directs);

void add_direct(direct_t **directs, direct_t *direct);

direct_t *new_direct();

direct_t *parse_circuit_direct(electric_t **electrics, electric_t **flag_electrics, electric_t *electric, int node);

direct_t **parse_circuit_directs(electric_t **electrics);

#endif //CIRCUIT_CIRCUIT_H
