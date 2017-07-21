"""
definitions.py
"""

import os


# Directories
ROOT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))  # This is the Project Root
FILE_DIR = os.path.join(ROOT_DIR, 'files')  # This is the directory of stored files

# Files
RAW_DATA_GRAPH_FILE = os.path.join(FILE_DIR, 'raw_data_graph')
UNDIRECTED_SPARSE_GRAPH_FILE = os.path.join(FILE_DIR, 'undirected_sparse_graph.json')
