#!/usr/bin/env python
# -*- coding: utf-8 -*-

""" graph_processing.py

Processing of the input raw data to data structures.

The 'sparse_adjacency_matrix' function process and store the input graph as a
dictionary of lists, where each key is a vertex of the graph and each list is
composed by its neighbors.

"""


import os
import gzip
import time
import pickle
import logging

import definitions


def sparse_adjacency_matrix(filename):
    """
    From an input file of the type "u v" per row, where u and v are different
    nodes. After completion saves the dictionary in pickle format, if called a
    second time, if the pickle file exists, it loads him.

    :param filename: path of the input raw file.

    :return: a dictionary with integer keys and lists of integer as values.
    """

    output_dict = {}
    if os.path.isfile(definitions.UNDIRECTED_SPARSE_GRAPH_FILE):
        # loads the pickle file
        with open(definitions.UNDIRECTED_SPARSE_GRAPH_FILE, 'rb') as f:
            output_dict = pickle.load(f)
            logging.info('Undirected-sparse graph pickle file found and loaded\n')
    else:
        # creates the undirected sparse graph
        logging.info('Undirected-sparse graph pickle file not found, '
                     'procede to make a new one\n')
        logcount = 1
        st = time.time()
        with gzip.open(filename, 'rt') as f:
            for line in f:
                if logcount % 1000000 == 0:
                    logging.info('up to line %d', logcount)
                # l = line.split(' ')  # split the line at the space
                l = line.split('\t')  # split the line at the space
                u = int(l[0].strip())
                v = int(l[1].strip())

                # set the default as a list if empty
                output_dict.setdefault(u, [])
                output_dict.setdefault(v, [])

                if v not in output_dict[u]:
                    output_dict[u].append(v)

                if u not in output_dict[v]:
                    output_dict[v].append(u)
                logcount += 1

        ft = time.time() - st
        logging.info('time to get the output: %d mins', ft//60)

        # stores the graph as a pickle file
        with open(definitions.UNDIRECTED_SPARSE_GRAPH_FILE, 'wb') as f:
            pickle.dump(output_dict, f, protocol=pickle.HIGHEST_PROTOCOL)

    return output_dict


def number_of_edges(graph_dict):
    """
    Compute the number of edges of an undirected graph, represented by a
    dictionary as adjacency (sparse) matrix.

    :param graph_dict: a dictionary, result of the 'sparse_adjacency_matrix'
                       function, representing a graph.

    :return: number of edges (in the undirected graph).
    """
    edges = 0
    for k in graph_dict:
        edges += len(graph_dict[k])
    return edges / 2


def _prune(graph, step, th=3):
    """

    :param Qmatrix:
    :return:
    """

    edges = {k: [v for v in graph[k]] for k in graph.keys() if (len(graph[k]) < th)}
    logging.info("Number of pruned nodes at step %d: %d" % (step, len(edges)))

    for x in edges:
        for y in edges[x]:
            graph[y].remove(x)

    for x in edges:
        del graph[x]

    return graph


def _remove_cycles(graph):
    """

    :param Qmatrix:
    :return:
    """
    cycles = 0
    for k in graph:
        if k in graph[k]:
            graph[k].remove(k)
            cycles += 1
    logging.info('Graph cycles found: %d' % cycles)
    return graph


def _check(graph):
    """

    :param Qmatrix:
    :return:
    """
    for i in graph:
        if len(graph[i]) == 0:
            logging.info('!! Delta matrix with entries of length zero')
            return False
        for j in graph[i]:
            if i == j:
                logging.info('!! Delta matrix with entries with cycles')
                return False
            if i not in graph[j]:
                logging.info('!! Delta matrix with directed entries')
                return False
    return True


def _check_a(a):
    """

    :param a:
    :return:
    """
    for x in a:
        if a[x] == 0:
            raise Exception('!! Vector A not correct !!')


def cleaning(graph):
    """

    :param graph:
    :return:
    """

    def zero_length(g):
        """  """
        for x in g:
            if len(g[x]) == 0:
                return False
        return True

    graph = _remove_cycles(graph)
    graph = _prune(graph, 0)

    flag = zero_length(graph)
    step = 1
    while not flag:
        graph = _prune(graph, step, th=1)
        flag = zero_length(graph)
        step += 1

    if not _check(graph):
        raise Exception('!! Graph not correct !!')

    return graph