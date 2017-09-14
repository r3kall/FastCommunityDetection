# -*- coding: utf-8 -*-

import time
import logging

import definitions
from algorithm.construction import compute_Qmatrix, compute_A, compute_H
from algorithm.update import detection
from graph_processing import sparse_adjacency_matrix, number_of_edges


def construction():
    print("  CONSTRUCTION TEST BEGINS...\n")

    G = sparse_adjacency_matrix(definitions.RAW_DATA_GRAPH_FILE)

    m = number_of_edges(G)  # number of edges
    n = len(G)  # number of vertices

    print('    Number of nodes: %d' % n)
    print('    Number of edges: %d' % m)
    print("  " + "-------" * 7)

    st = time.time()
    Qmatrix = compute_Qmatrix(G, m)
    print("    time to initialize Qmatrix:  %f seconds" % (time.time() - st))

    st = time.time()
    # A = compute_A(G, m)
    print("    time to initialize A:  %f seconds" % (time.time() - st))

    del G

    st = time.time()
    H = compute_H(Qmatrix)
    print("    time to initialize H:  %f seconds" % (time.time() - st))

    print("\n  CONSTRUCTION TEST ENDS")
    return Qmatrix, H, A, m


def update(Qmatrix, A, H):
    print("  UPDATE TEST BEGINS...\n")

    detection(Qmatrix, A, H)

    print("\n  UPDATE TEST ENDS")
    return Qmatrix, H, A, m


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG,
                        format='%(asctime)s %(message)s',
                        datefmt='%m/%d/%Y %H:%M:%S')

    print(" !== START ==!\n")
    # Qmatrix, H, A, m = construction()
    construction()
    print()
    # update(Qmatrix, A, H)
    print("\n !== END ==!")
