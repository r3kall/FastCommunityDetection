import heapq
import time

cimport cython

def compute_deltaQ(int ki, int kj, int m):
    """
    Initialize deltaQ for two communities i and j.

    :param m: number of edges.
    :param ki: node i.
    :param kj: node j.
    :return: the starting value of deltaQ for 'single' communities i and j.
    """
    return (1. / (2. * m)) - (float(ki * kj) / ((2 * m) ** 2))


def compute_Qmatrix(dict graph_dict, int m):
    """
    Initialize the deltaQ sparse matrix.

    :param graph_dict: input graph dictionary.
    :param m: number of edges.
    :return: deltaQ sparse matrix as a dictionary
    """
    Qmatrix = {}
    for i in graph_dict:
        Qmatrix[i] = {j: compute_deltaQ(
            len(graph_dict[i]), len(graph_dict[j]), m) for j in graph_dict[i]}
    return Qmatrix


def compute_H(dict Qmatrix):
    """
    Initialize a max-heap H containing the largest element of each row of the
    matrix deltaQ along with the labels i,j of the corresponding
    pair of communities.

    :param Qmatrix: sparse matrix deltaQ.
    :return: max-heap H.
    """
    H = []
    for i in Qmatrix:
        index = max(Qmatrix[i], key=Qmatrix[i].get)
        heapq.heappush(H, (-Qmatrix[i][index], i, index))
    return H


def compute_A(dict graph_dict, int m):
    """
    Initialize an ordinary dictionary with elements Ai.

    :param graph_dict: input graph dictionary.
    :param m: number of edges.
    :return: dictionary of elements ai.
    """
    a = {}
    for i in graph_dict:
        k = len(graph_dict[i])
        a[i] = float(k) / (2. * m)
    return a