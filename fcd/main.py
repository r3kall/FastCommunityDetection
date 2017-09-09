"""
main.py
"""

import time
import logging

import algorithm
import visualize
import definitions
from graph_processing import sparse_adjacency_matrix, number_of_edges


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
        step +=1

    if not _check(graph):
        raise Exception('!! Graph not correct !!')

    return graph


def main():
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s %(message)s',
                        datefmt='%m/%d/%Y %H:%M:%S')

    g = sparse_adjacency_matrix(definitions.RAW_DATA_GRAPH_FILE)
    g = cleaning(g)

    maxQ = 0        # max Q achieved
    max_step = 0    # max number of steps
    Q = 0           # actual Q

    m = number_of_edges(g)  # number of edges
    n = len(g)  # number of vertices

    logging.info('Number of nodes: %d' % n)
    logging.info('Number of edges: %d\n' % m)

    logging.info('Data Structures Initialization')

    st = time.time()
    Qmatrix = algorithm.compute_Qmatrix(g, m)
    _check(Qmatrix)
    logging.info("time to initialize Qmatrix:  %f seconds" % (time.time() - st))

    st = time.time()
    a = algorithm.compute_a(g, m)
    _check_a(a)
    logging.info("time to initialize a:  %f seconds" % (time.time() - st))

    del g

    st = time.time()
    H = algorithm.compute_H(Qmatrix)
    logging.info("time to initialize H:  %f seconds" % (time.time() - st))

    # list to record the modularity evolution.
    modularity = list()
    modularity.append(0.)

    step = 0
    print('\n\n\ti', '\t\t', 'j', '\t\t', 'Q', '\t\t', 'deltaQ', '\t\t',
          'step', '\t\t', 'time')
    br = 0
    init = time.time()
    while H:
        deltaQ, i, j = algorithm.maxQ(H)

        if a[i] == 0:
            continue

        if a[j] == 0:
            algorithm.update_H(Qmatrix, H, a, i)
            continue

        Q -= deltaQ  # update modularity.
        if (step % 100) == 0:
            modularity.append(-deltaQ)

        st = time.time()
        # update the deltaQ matrix according to equations.
        Qmatrix = algorithm.update_Qmatrix(Qmatrix, H, i, j, a)
        onet = time.time()

        # update a.
        a = algorithm.update_a(i, j, a)
        twot = time.time()

        logging.debug("time to compute Qmatrix:  %f seconds" % (onet - st))
        logging.debug("time to compute a:  %f seconds" % (twot - onet))

        step += 1

        print('\t', i, '\t\t', j, '\t\t', round(Q, 5), '\t\t',
              round(-deltaQ, 5), '\t\t', step, '\t\t', (twot - st))

        if -deltaQ > maxQ:
            maxQ = -deltaQ
            max_step = step

        br += 1
        if br > 10000:
            break

    print("Max Q:  %f" % maxQ)
    print("Max step:  %d" % max_step)
    print("Total time:  %f mins" % ((time.time() - init) / 60))

    del Qmatrix
    del H
    del a

    visualize.modularity_evo(modularity)


if __name__ == '__main__':
    main()
