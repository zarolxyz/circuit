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

electric_t **find_node_electrics(electric_t **electrics, int node) {
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

int test_circuit_direction(electric_t **electrics, electric_t **flag_electrics, electric_t *electric, int node) {
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
                if (test_circuit_direction(electrics, pass_flag_electrics, pass_electric, pass_node)) {
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

direction_t **new_directions() {
    direction_t **directions = malloc(sizeof(direction_t *) * (MAXIMUM_NODE_ELECTRICS + 1));
    directions[0] = NULL;
    return directions;
}

int count_directions(direction_t **directions) {
    int count = 0;
    while (directions[count] != NULL) count++;
    return count;
}

direction_t **copy_directions(direction_t **directions) {
    direction_t **copied_directions = new_directions();
    int count = count_directions(directions);
    for (int i = 0; i < count; i++)
        copied_directions[i] = directions[i];
    copied_directions[count] = NULL;
    return copied_directions;
}

void add_direction(direction_t **directions, direction_t *direction) {
    int count = count_directions(directions);
    directions[count] = direction;
    directions[count + 1] = NULL;
}

void delete_direction(direction_t **directions, direction_t *direction) {
    int count = count_directions(directions);
    for (int i = 0; i < count; i++) {
        if (directions[i] == direction) {
            directions[i] = directions[count - 1];
            directions[count - 1] = NULL;
            break;
        }
    }
}

direction_t *new_direction(electric_t *electric) {
    direction_t *direction = malloc(sizeof(direction_t));
    direction->electric = electric;
    direction->next = NULL;
    return direction;
}

direction_t *
parse_circuit_direction(electric_t **electrics, electric_t **flag_electrics, electric_t *electric, int node) {
    if (!test_circuit_direction(electrics, flag_electrics, electric, node))
        return NULL;
    direction_t *direction = new_direction(electric);
    direction_t *current_direction = direction;
    while (node != CATHODE) {
        electric_t **node_electrics = find_node_electrics(electrics, node);
        delete_electric(node_electrics, electric);
        int count = count_electrics(node_electrics);
        if (count == 0) {
            free(node_electrics);
            return direction;
        } else if (count == 1) {
            current_direction->next = new_directions();
            add_direction(current_direction->next, new_direction(node_electrics[0]));
            current_direction = current_direction->next[0];
            add_electric(flag_electrics, node_electrics[0]);
            if (node_electrics[0]->node1 == node)
                node = node_electrics[0]->node2;
            else
                node = node_electrics[0]->node1;
            free(node_electrics);
        } else {
            current_direction->next = new_directions();
            for (int i = 0; i < count; i++) {
                electric_t **pass_flag_electrics = copy_electrics(flag_electrics);
                electric_t *pass_electric = node_electrics[i];
                int pass_node;
                if (pass_electric->node1 == node)
                    pass_node = pass_electric->node2;
                else
                    pass_node = pass_electric->node1;
                if (test_circuit_direction(electrics, pass_flag_electrics, pass_electric, pass_node)) {
                    add_direction(current_direction->next,
                                  parse_circuit_direction(electrics, pass_flag_electrics, pass_electric, pass_node));
                }
                free(pass_flag_electrics);
            }
            free(node_electrics);
            return direction;
        }
    }
    return direction;
}

direction_t **parse_circuit_directions(electric_t **electrics) {
    electric_t **flag_electrics;
    electric_t **node_electrics = find_node_electrics(electrics, ANODE);
    direction_t **directions = new_directions();
    direction_t *direction;
    int node;
    int count = count_electrics(node_electrics);
    for (int i = 0; i < count; i++) {
        flag_electrics = new_electrics();
        if (node_electrics[i]->node1 == ANODE)
            node = node_electrics[i]->node2;
        else
            node = node_electrics[i]->node1;
        direction = parse_circuit_direction(electrics, flag_electrics, node_electrics[i], node);
        if (direction != NULL)
            add_direction(directions, direction);
        free(flag_electrics);
    }
    free(node_electrics);
    return directions;
}

int test_equal_direction(direction_t *direction1, direction_t *direction2) {
    if (direction1 == NULL || direction2 == NULL) {
        if (direction1 == direction2)
            return 1;
        else
            return 0;
    }
    if (direction1->electric == direction2->electric)
        return 1;
    return 0;
}

int test_in_direction(direction_t *direction1, direction_t *direction2) {
    if (direction2 == NULL)
        return 1;
    else if (direction1 == NULL)
        return 0;
    while (!test_equal_direction(direction1, direction2)) {
        if (direction1->next == NULL)
            return 0;
        direction1 = direction1->next[0];
    }
    return 1;
}

int test_equal_in_directions(direction_t **directions, direction_t *direction) {
    for (int i = 0; directions[i] != NULL; i++) {
        if (test_equal_direction(directions[i], direction))
            return 1;
    }
    return 0;
}

direction_t *find_direction_meeting(direction_t *direction1, direction_t *direction2) {
    while (!test_in_direction(direction2, direction1)) {
        if (direction1->next == NULL)
            return NULL;
        direction1 = direction1->next[0];
    }
    return direction1;
}

direction_t *find_directions_meeting(direction_t **directions) {
    int count = count_directions(directions);
    direction_t *direction_meeting = directions[0];
    for (int i = 1; i < count; i++) {
        direction_meeting = find_direction_meeting(direction_meeting, directions[i]);
    }
    return direction_meeting;
}

direction_t *find_previous_direction(direction_t *start_direction, direction_t *direction) {
    while (start_direction->next != NULL && !test_equal_direction(start_direction->next[0], direction))
        start_direction = start_direction->next[0];
    return start_direction;
}

direction_t **find_previous_directions(direction_t **directions, direction_t *direction) {
    direction_t **previous_directions = new_directions();
    for (int i = 0; directions[i] != NULL; i++) {
        direction_t *previous_direction = find_previous_direction(directions[i], direction);
        if (!test_equal_in_directions(previous_directions, previous_direction))
            add_direction(previous_directions, previous_direction);
    }
    return previous_directions;
}

int get_direction_resistance(direction_t *direction, direction_t *end_direction) {
    if (direction == NULL)
        return 0;
    int resistance = 0;
    while (!test_equal_direction(direction, end_direction)) {
        if (direction->next == NULL) {
            resistance += direction->electric->resistance;
            break;
        }
        int count = count_directions(direction->next);
        if (count == 1) {
            resistance += direction->electric->resistance;
            direction = direction->next[0];
        } else {
            resistance += get_directions_resistance(direction->next);
            direction = find_directions_meeting(direction->next);
        }
    }
    return resistance;
}

int calculate_resistance_and(int resistance1, int resistance2) {
    return (resistance1 * resistance2) / (resistance1 + resistance2);
}

int get_directions_resistance(direction_t **directions) {
    int count = count_directions(directions);
    int resistance = -1;
    direction_t *direction_meeting = find_directions_meeting(directions);
    direction_t **previous_directions = find_previous_directions(directions, direction_meeting);
    int count_previous_directions = count_directions(previous_directions);
    for (int i = 0; i < count_previous_directions; i++) {
        direction_t **found_directions = new_directions();
        for (int j = 0; j < count; j++) {
            if (test_in_direction(directions[j], previous_directions[i]))
                add_direction(found_directions, directions[j]);
        }
        int count_found_directions = count_directions(found_directions);
        if (count_found_directions == 1) {
            if (resistance == -1)
                resistance = get_direction_resistance(found_directions[0], direction_meeting);
            else
                resistance = calculate_resistance_and(resistance,
                                                      get_direction_resistance(found_directions[0], direction_meeting));
        } else {
            calculate_resistance_and(resistance, get_directions_resistance(found_directions) + get_direction_resistance(
                    find_directions_meeting(found_directions), direction_meeting));
        }
        free(found_directions);
    }
    free(previous_directions);
    return resistance;
}

int run_circuit(electric_t **electrics, int voltage) {
    clear_electrics_status(electrics);
    direction_t **directions = parse_circuit_directions(electrics);
}