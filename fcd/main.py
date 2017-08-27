"""
main.py
"""

import time
import logging

import algorithm
import definitions
from graph_processing import sparse_adjacency_matrix, number_of_edges


def main():
    logging.basicConfig(level=logging.DEBUG,
                        format='%(asctime)s %(message)s',
                        datefmt='%m/%d/%Y %H:%M:%S')
    g = sparse_adjacency_matrix(definitions.RAW_DATA_GRAPH_FILE)

    maxQ = 0        # max Q achieved
    max_step = 0    # max number of steps
    Q = 0           # actual Q

    m = number_of_edges(g)  # number of edges
    n = len(g)  # number of vertices

    logging.info('Number of nodes: %d' % n)
    logging.info('Number of edges: %d\n' % m)

    logging.info('Data Structures Initialization')

    st = time.time()
    Qtrees = algorithm.compute_Qtrees(g, m)
    logging.info("time to initialize Qtrees:  %f seconds" % (time.time() - st))

    st = time.time()
    a = algorithm.compute_a(g, m)
    logging.info("time to initialize a:  %f seconds" % (time.time() - st))

    del g

    st = time.time()
    H = algorithm.compute_H(Qtrees)
    logging.info("time to initialize H:  %f seconds" % (time.time() - st))

    step = 0
    print('\n\n\ti', '\t\t', 'j', '\t\t', 'Q', '\t\t', 'deltaQ', '\t\t',
          'step', '\t\t', 'time')
    br = 0
    while H:
        deltaQ, i, j = algorithm.maxQ(H)

        # if community i or j doesn't exists in the deltaQ matrix anymore,
        # ignore it and continue.
        if Qtrees.get(i, None) is None or Qtrees.get(j, None) is None:
            continue

        Q -= deltaQ  # update modularity.

        st = time.time()
        # update the deltaQ matrix according to equations.
        Qtrees = algorithm.update_Qtrees(Qtrees, i, j, a)
        onet = time.time()

        # update the max-heap H, adding the new maximum value in the j-th row.
        H = algorithm.update_H(Qtrees, H, j)
        twot = time.time()

        # update a.
        a = algorithm.update_a(i, j, a)
        threet = time.time()

        logging.debug("time to compute Qtrees:  %f seconds" % (onet - st))
        logging.debug("time to compute H:  %f seconds" % (twot - onet))
        logging.debug("time to compute a:  %f seconds" % (threet - twot))

        step += 1

        print('\t', i, '\t\t', j, '\t\t', round(Q, 5), '\t\t',
              round(-deltaQ, 5), '\t\t', step, '\t\t', (threet - st))

        br += 1
        if br > 25:
            break

    del Qtrees
    del H
    del a


if __name__ == '__main__':
    main()
