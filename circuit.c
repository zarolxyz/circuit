//
// Created by zarolxyz on 2022/3/19.
//

#include <stdlib.h>
#include <string.h>
#include "circuit.h"

electric_t **new_electrics() {
    electric_t **electrics = malloc(sizeof(electric_t *) * (MAXIMUM_ELECTRICS + 1));
    electrics[0] = NULL;
    return electrics;
}

int count_electrics(electric_t **electrics) {
    int count = 0;
    while (electrics[count] != NULL) count++;
    return count;
}

electric_t **copy_electrics(electric_t **electrics) {
    electric_t **copied_electrics = malloc(sizeof(electric_t *) * (MAXIMUM_ELECTRICS + 1));
    int count = count_electrics(electrics);
    for (int i = 0; i < count; i++)
        copied_electrics[i] = electrics[i];
    copied_electrics[count] = NULL;
    return copied_electrics;
}

void clear_electrics_status(electric_t **electrics) {
    electric_t *electric;
    for (int i = 0; electrics[i] != NULL; i++) {
        electric = electrics[i];
        electric->voltage = 0;
        electric->direct = 0;
        electric->current = 0;
    }
}

int test_electric_in_electrics(electric_t **electrics, electric_t *electric) {
    for (int i = 0; electrics[i] != NULL; i++) {
        if (electrics[i] == electric)
            return 1;
    }
    return 0;
}

int test_electrics_in_electrics(electric_t **electrics1, electric_t **electrics2) {
    for (int i = 0; electrics2[i] != NULL; i++) {
        if (test_electric_in_electrics(electrics1, electrics2[i]))
            return 1;
    }
    return 0;
}

electric_t **find_node_electrics(electric_t **electrics, unsigned int node) {
    electric_t **found_electrics = new_electrics();
    int count = 0;
    for (int i = 0; electrics[i] != NULL; i++) {
        if (electrics[i]->node1 == node || electrics[i]->node2 == node) {
            found_electrics[count] = electrics[i];
            count++;
        }
    }
    found_electrics[count] = NULL;
    return found_electrics;
}

void add_electric(electric_t **electrics, electric_t *electric) {
    int count = count_electrics(electrics);
    electrics[count] = electric;
    electrics[count + 1] = NULL;
}

void delete_electric(electric_t **electrics, electric_t *electric) {
    int count = count_electrics(electrics);
    for (int i = 0; i < count; i++) {
        if (electrics[i] == electric) {
            electrics[i] = electrics[count - 1];
            electrics[count - 1] = NULL;
            break;
        }
    }
}

int test_circuit_direct(electric_t **electrics, electric_t **flag_electrics, electric_t *electric, unsigned int node) {
    while (1) {
        if (node == ANODE)
            return 0;
        if (node == CATHODE)
            return 1;
        electric_t **node_electrics = find_node_electrics(electrics, node);
        delete_electric(node_electrics, electric);
        if (test_electrics_in_electrics(flag_electrics, node_electrics)) {
            free(node_electrics);
            return 0;
        }
        add_electric(flag_electrics, electric);
        int count = count_electrics(node_electrics);
        if (count == 0) {
            free(node_electrics);
            return 0;
        } else if (count == 1) {
            electric = node_electrics[0];
            if (electric->node1 == node)
                node = electric->node2;
            else
                node = electric->node1;
            free(node_electrics);
        } else {
            for (int i = 0; i < count; i++) {
                electric_t **pass_flag_electrics = copy_electrics(flag_electrics);
                electric_t *pass_electric = node_electrics[i];
                int pass_node;
                if (pass_electric->node1 == node)
                    pass_node = pass_electric->node2;
                else
                    pass_node = pass_electric->node1;
                if (test_circuit_direct(electrics, pass_flag_electrics, pass_electric, pass_node)) {
                    free(node_electrics);
                    return 1;
                }
                free(pass_flag_electrics);
            }
            free(node_electrics);
            return 0;
        }
    }
}

direct_t **new_directs() {
    direct_t **directs = malloc(sizeof(direct_t *) * (MAXIMUM_NODE_ELECTRICS + 1));
    directs[0] = NULL;
    return directs;
}

int count_directs(direct_t **directs) {
    int count = 0;
    while (directs[count] != NULL) count++;
    return count;
}

void add_direct(direct_t **directs, direct_t *direct) {
    int count = count_directs(directs);
    directs[count] = direct;
    directs[count + 1] = NULL;
}

direct_t *new_direct(electric_t *electric) {
    direct_t *direct = malloc(sizeof(direct_t));
    direct->electric = electric;
    direct->next = NULL;
    return direct;
}

direct_t *parse_circuit_direct(electric_t **electrics, electric_t **flag_electrics, electric_t *electric, int node) {
    if (!test_circuit_direct(electrics, flag_electrics, electric, node))
        return NULL;
    direct_t *direct = new_direct(electric);
    direct_t *current_direct = direct;
    while (node != CATHODE) {
        electric_t **node_electrics = find_node_electrics(electrics, node);
        delete_electric(node_electrics, electric);
        int count = count_electrics(node_electrics);
        if (count == 0) {
            free(node_electrics);
            return direct;
        } else if (count == 1) {
            current_direct->next = new_directs();
            add_direct(current_direct->next, new_direct(node_electrics[0]));
            current_direct = current_direct->next[0];
            add_electric(flag_electrics, node_electrics[0]);
            if (node_electrics[0]->node1 == node)
                node = node_electrics[0]->node2;
            else
                node = node_electrics[0]->node1;
            free(node_electrics);
        } else {
            current_direct->next = new_directs();
            for (int i = 0; i < count; i++) {
                electric_t **pass_flag_electrics = copy_electrics(flag_electrics);
                electric_t *pass_electric = node_electrics[i];
                int pass_node;
                if (pass_electric->node1 == node)
                    pass_node = pass_electric->node2;
                else
                    pass_node = pass_electric->node1;
                if (test_circuit_direct(electrics, pass_flag_electrics, pass_electric, pass_node)) {
                    add_direct(current_direct->next,
                               parse_circuit_direct(electrics, pass_flag_electrics, pass_electric, pass_node));
                }
                free(pass_flag_electrics);
            }
            free(node_electrics);
            return direct;
        }
    }
    return direct;
}

direct_t **parse_circuit_directs(electric_t **electrics) {
    electric_t **flag_electrics;
    electric_t **node_electrics = find_node_electrics(electrics, ANODE);
    direct_t **directs = new_directs();
    direct_t *direct;
    int node;
    int count = count_electrics(node_electrics);
    for (int i = 0; i < count; i++) {
        flag_electrics = new_electrics();
        if (node_electrics[i]->node1 == ANODE)
            node = node_electrics[i]->node2;
        else
            node = node_electrics[i]->node1;
        direct = parse_circuit_direct(electrics, flag_electrics, node_electrics[i], node);
        if (direct != NULL)
            add_direct(directs, direct);
        free(flag_electrics);
    }
    free(node_electrics);
    return directs;
}
