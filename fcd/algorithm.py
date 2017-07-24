"""
algorithm.py
"""

import time
import heapq
import logging


def init_deltaQ(ki, kj, m):
    """
    Initialize deltaQ for two communities i and j.

    :param m: number of edges.
    :param ki: node i.
    :param kj: node j.
    :return: the starting value of deltaQ for 'single' communities i and j.
    """
    return (1.0 / (2.0 * m)) - (float(ki * kj) / ((2 * m) ** 2))


def init_Qtrees(graph_dict, m):
    """
    Initialize the deltaQ sparse matrix.

    :param graph_dict: input graph dictionary.
    :param m: number of edges.
    :return: deltaQ sparse matrix as a dictionary
    """
    Qtrees = {}
    for i in graph_dict:
        cm = graph_dict[i]
        ki = len(cm)
        Qtrees[i] = {}
        for j in cm:
            kj = len(graph_dict[j])
            Qtrees[i][j] = init_deltaQ(ki, kj, m)
    return Qtrees


def init_H(Qtrees):
    """
    Initialize a max-heap H containing the largest element of each row of the
    matrix deltaQ along with the labels i,j of the corresponding
    pair of communities.

    :param Qtrees: sparse matrix deltaQ
    :return: max-heap H
    """
    H = []
    tot = 0.0
    for i in Qtrees:
        maximum = 0.0
        jindex = 0

        st0 = time.time()

        for j in Qtrees[i]:
            if Qtrees[i][j] > maximum:
                maximum = Qtrees[i][j]
                jindex = j

        tot += (time.time() - st0)
        # we have to store the elements in the heap as their negative value,
        # because python heap is a min-heap
        heapq.heappush(H, (-maximum, i, jindex))
    logging.debug('time to compute inner cycles of H:  %f' % tot)
    return H


def init_a(graph_dict, m):
    """
    Initialize an ordinary dictionary with elements ai.

    :param graph_dict: input graph dictionary.
    :param m: number of edges.
    :return: dictionary of elements ai.
    """
    a = {}
    for i in graph_dict:
        k = len(graph_dict[i])
        a[i] = float(k) / (2.0 * m)
    return a


def maxQ(H):
    """ Return the largest value in the heap, but its negative value """
    return heapq.heappop(H)


def update_Qtrees(Qtrees, i, j, a):
    """
    Update rules.

    :param Qtrees: sparse matrix deltaQ.
    :param i: community i.
    :param j: community j.
    :param a: element a.
    :return: updated Qtrees.
    """

    # equation (10a)
    for k in Qtrees[i]:
        if k in Qtrees[j]:
            Qtrees[j][k] = Qtrees[i][k] + Qtrees[j][k]

    # equation (10b) and (10c)
    for k in Qtrees:
        if k in Qtrees[i] and k not in Qtrees[j]:
            Qtrees[j][k] = Qtrees[i][k] - (2 * a[j] * a[k])
        elif k in Qtrees[j] and k not in Qtrees[i]:
            Qtrees[j][k] = Qtrees[j][k] - (2 * a[i] * a[k])

    # remove i key and update j for each row k
    for k in Qtrees:
        if i in Qtrees[k]:
            # remove the i-th column
            Qtrees[k].pop(i, None)
        if j in Qtrees[k]:
            # update j-th column
            Qtrees[k][j] = Qtrees[k][j] - (2 * a[i] * a[k])

    # remove the self-reference
    if j in Qtrees[j]:
        Qtrees[j].pop(j, None)

    # remove the i-th row
    Qtrees.pop(i, None)

    return Qtrees


def update_a(i, j, a):
    """ Update array of elements a """
    a[j] += a[i]
    a[i] = 0
    return a
