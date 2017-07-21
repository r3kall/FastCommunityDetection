"""
main.py
"""

import logging

import definitions
from graph_processing import sparse_adjacency_list


def main():
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s %(message)s',
                        datefmt='%m/%d/%Y %H:%M:%S')

    d = sparse_adjacency_list(definitions.RAW_DATA_GRAPH_FILE)


if __name__ == '__main__':
    main()
