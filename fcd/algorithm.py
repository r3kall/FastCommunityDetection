"""
algorithm.py
"""

import heapq
import time


def compute_deltaQ(ki, kj, m):
    """
    Initialize deltaQ for two communities i and j.

    :param m: number of edges.
    :param ki: node i.
    :param kj: node j.
    :return: the starting value of deltaQ for 'single' communities i and j.
    """
    return (1. / (2. * m)) - (float(ki * kj) / ((2 * m) ** 2))


def compute_Qmatrix(graph_dict, m):
    """
    Initialize the deltaQ sparse matrix.

    :param graph_dict: input graph dictionary.
    :param m: number of edges.
    :return: deltaQ sparse matrix as a dictionary
    """
    Qmatrix = dict()
    for i in graph_dict:
        Qmatrix[i] = {j: compute_deltaQ(
            len(graph_dict[i]), len(graph_dict[j]), m) for j in graph_dict[i]}
    return Qmatrix


def compute_H(Qmatrix):
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


def compute_a(graph_dict, m):
    """
    Initialize an ordinary dictionary with elements ai.

    :param graph_dict: input graph dictionary.
    :param m: number of edges.
    :return: dictionary of elements ai.
    """
    a = {}
    for i in graph_dict:
        k = len(graph_dict[i])
        a[i] = float(k) / (2. * m)
    return a


def maxQ(H):
    """ Return the largest value in the heap, but its negative value """
    return heapq.heappop(H)


def update_Qmatrix(Qmatrix, H, i, j, a):
    """
    Update rules.

    :param Qmatrix: sparse matrix deltaQ.
    :param i: community i.
    :param j: community j.
    :param a: element a.
    :return: updated Qtrees.
    """

    for k in Qmatrix[j]:
        if k not in Qmatrix[i]:
            # equation (10c)
            Qmatrix[j][k] -= (2 * a[i] * a[k])

    for k in Qmatrix[i]:
        if k in Qmatrix[j]:
            # equation (10a)
            Qmatrix[j][k] += Qmatrix[i][k]
        else:
            # equation (10b)
            # here a new key is added to the j-th row
            Qmatrix[j][k] = Qmatrix[i][k] - (2 * a[j] * a[k])

    # remove self reference
    Qmatrix[j].pop(j, None)

    update_H(Qmatrix, H, a, j)

    return Qmatrix


def update_H(Qmatrix, H, a, n):
    """ Update the heap H. """
    index = 0
    maximum = 0.
    for x in Qmatrix[n]:
        if (Qmatrix[n][x] > maximum) and (a[x] > 0):
            maximum = Qmatrix[n][x]
            index = x

    if maximum > 0:
        heapq.heappush(H, (-Qmatrix[n][index], n, index))


def update_a(i, j, a):
    """ Update array of elements a. """
    a[j] += a[i]
    a[i] = 0
    return a
