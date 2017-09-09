"""
definitions.py
"""

import os


# Directories
ROOT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))  # This is the Project Root
FILE_DIR = os.path.join(ROOT_DIR, 'files')  # This is the directory of stored files

# Files
# RAW_DATA_GRAPH_FILE = os.path.join(FILE_DIR, 'wiki-topcats.txt.gz')
# UNDIRECTED_SPARSE_GRAPH_FILE = os.path.join(FILE_DIR, 'undirected_sparse_graph.pickle')
RAW_DATA_GRAPH_FILE = os.path.join(FILE_DIR, 'amazon.txt.gz')
UNDIRECTED_SPARSE_GRAPH_FILE = os.path.join(FILE_DIR, 'undirected_sparse_graph_test.pickle')
