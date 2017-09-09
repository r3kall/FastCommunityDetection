"""
graph_processing.py

Processing of the input raw data to data structures.
"""

import os
import gzip
import time
import pickle
import logging
from collections import defaultdict

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

