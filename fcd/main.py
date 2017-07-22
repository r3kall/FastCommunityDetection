"""
main.py
"""

import time
import logging

import algorithm
import definitions
from graph_processing import sparse_adjacency_matrix, number_of_edges


def main():
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s %(message)s',
                        datefmt='%m/%d/%Y %H:%M:%S')
    logging.info('Input graph acquired')
    g = sparse_adjacency_matrix(definitions.RAW_DATA_GRAPH_FILE)

    maxQ = 0
    max_step = 0
    Q = 0

    m = number_of_edges(g)  # number of edges
    n = len(g)  # number of vertices

    logging.info('Data Structures Initialization\n')
    Qtrees = algorithm.init_Qtrees(g, m)
    H = algorithm.init_H(Qtrees)
    a = algorithm.init_a(g, m)

    step = 0
    print('i', '\t\t', 'j', '\t\t', 'Q', '\t\t', 'deltaQ', '\t\t', 'step')
    br = 0
    while H:
        deltaQ, i, j = algorithm.maxQ(H)
        Q -= deltaQ

        Qtrees = algorithm.update_Qtrees(Qtrees, i, j, a)
        H = algorithm.init_H(Qtrees)
        a = algorithm.update_a(i, j, a)

        step += 1

        print(i, '\t\t', j, '\t\t', round(Q, 7), '\t\t', deltaQ, '\t\t', step)

        br += 1
        if br > 100:
            break


if __name__ == '__main__':
    main()
