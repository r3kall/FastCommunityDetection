"""
graph_processing.py

Processing of the input raw data to data structures.
"""

import os
import gzip
import json
import time
import logging
from collections import defaultdict

import definitions


def sparse_adjacency_matrix(filename):
    """
    From an input file of the type "u v" per row, where u and v are different
    nodes. After completion saves the dictionary in JSON format, if called a
    second time, if the JSON exists, it loads him.

    :param filename: path of the input raw file.
    :return: a dictionary with integer keys and lists of integer as values.
    """
    # TODO open as gzip file and generalize for other inputs
    output_dict = {}
    if os.path.isfile(definitions.UNDIRECTED_SPARSE_GRAPH_FILE):
        # loads the JSON file
        with open(definitions.UNDIRECTED_SPARSE_GRAPH_FILE, 'r') as f:
            output_dict = json.load(f)
            logging.info('Undirected-sparse graph JSON file found and loaded')
    else:
        # creates the undirected sparse graph
        logging.info('Undirected-sparse graph JSON file not found, '
                     'procede to make a new one')
        logcount = 1
        st = time.time()
        with open(filename, 'r') as f:
            for line in f:
                if logcount % 1000000 == 0:
                    logging.info('up to line %d', logcount)
                l = line.split(' ')  # split the line at the space
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
        logging.info('time to get the output: %f seconds', ft)

        # stores the graph as a JSON file
        with open(definitions.UNDIRECTED_SPARSE_GRAPH_FILE, 'w') as f:
            json.dump(output_dict, f)

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

