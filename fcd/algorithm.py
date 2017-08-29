"""
algorithm.py
"""

import heapq


def compute_deltaQ(ki, kj, m):
    """
    Initialize deltaQ for two communities i and j.

    :param m: number of edges.
    :param ki: node i.
    :param kj: node j.
    :return: the starting value of deltaQ for 'single' communities i and j.
    """
    return (1. / (2. * m)) - (float(ki * kj) / ((2 * m) ** 2))


def compute_Qtrees(graph_dict, m):
    """
    Initialize the deltaQ sparse matrix.

    :param graph_dict: input graph dictionary.
    :param m: number of edges.
    :return: deltaQ sparse matrix as a dictionary
    """
    Qtrees = dict()
    for i in graph_dict:
        Qtrees[i] = {j: compute_deltaQ(
            len(graph_dict[i]), len(graph_dict[j]), m) for j in graph_dict[i]}
    return Qtrees


def compute_H(Qtrees):
    """
    Initialize a max-heap H containing the largest element of each row of the
    matrix deltaQ along with the labels i,j of the corresponding
    pair of communities.

    :param Qtrees: sparse matrix deltaQ.
    :return: max-heap H.
    """
    H = []
    for i in Qtrees:
        maximum = 0.
        index = 0
        for j in Qtrees[i]:
            if Qtrees[i][j] > maximum:
                maximum = Qtrees[i][j]
                index = j
        heapq.heappush(H, (-maximum, i, index))

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


def update_Qtrees(Qtrees, H, i, j, a):
    """
    Update rules.

    :param Qtrees: sparse matrix deltaQ.
    :param i: community i.
    :param j: community j.
    :param a: element a.
    :return: updated Qtrees.
    """

    for k in Qtrees[i]:
        if k in Qtrees[j]:
            # equation (10a)
            Qtrees[j][k] += Qtrees[i][k]
        else:
            # equation (10b)
            Qtrees[j][k] = Qtrees[i][k] - (2 * a[j] * a[k])

        # remove element i from k-th row
        Qtrees[k].pop(i, None)
        # update the heap H for each key k
        H = update_H(Qtrees, H, k)

    for k in Qtrees[j]:
        if k not in Qtrees[i]:
            # equation (10c)
            Qtrees[j][k] -= (2 * a[i] * a[k])

    """
    for k in Qtrees:
        if k in Qtrees[i]:
            if k in Qtrees[j]:
                # equation (10a)
                Qtrees[j][k] = Qtrees[i][k] + Qtrees[j][k]
            else:
                # equation (10b)
                Qtrees[j][k] = Qtrees[i][k] - (2 * a[j] * a[k])
        elif k in Qtrees[j]:
            # equation (10c)
            Qtrees[j][k] -= (2 * a[i] * a[k])
        # remove i-th column
        if i in Qtrees[k]:
            Qtrees[k].pop(i, None)
    """

    """
    # remove the self-reference
    if j in Qtrees[j]:
        Qtrees[j].pop(j, None)
    """

    # remove element i from the j-th row
    Qtrees[j].pop(i, None)

    # remove the i-th row
    Qtrees.pop(i, None)

    H = update_H(Qtrees, H, j)

    return Qtrees


def update_H(Qtrees, H, j):
    """ Update the heap H. """
    maximum = 0.
    index = 0
    for item in Qtrees[j]:
        if Qtrees[j][item] > maximum:
            maximum = Qtrees[j][item]
            index = item
    heapq.heappush(H, (-maximum, j, index))

    return H


def update_a(i, j, a):
    """ Update array of elements a. """
    a[j] += a[i]
    a[i] = 0
    return a
