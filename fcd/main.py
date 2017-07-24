"""
main.py
"""

import time
import logging

import algorithm
import definitions
from graph_processing import sparse_adjacency_matrix, number_of_edges


def main():
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s %(message)s',
                        datefmt='%m/%d/%Y %H:%M:%S')
    logging.info('Input graph acquired')
    g = sparse_adjacency_matrix(definitions.RAW_DATA_GRAPH_FILE)

    # g has string type keys and integer type values, we have to
    # convert keys to integer.
    g = {int(k): v for k, v in g.items()}

    maxQ = 0        # max Q achieved
    max_step = 0    # max number of steps
    Q = 0           # actual Q

    m = number_of_edges(g)  # number of edges
    n = len(g)  # number of vertices

    logging.info('Data Structures Initialization\n')

    st = time.time()
    Qtrees = algorithm.init_Qtrees(g, m)
    logging.info("time to initialize Qtrees:  %f seconds" % (time.time() - st))

    st = time.time()
    H = algorithm.init_H(Qtrees)
    logging.info("time to initialize H:  %f seconds" % (time.time() - st))

    st = time.time()
    a = algorithm.init_a(g, m)
    logging.info("time to initialize a:  %f seconds" % (time.time() - st))

    """
    step = 0
    print('i', '\t\t', 'j', '\t\t', 'Q', '\t\t', 'deltaQ', '\t\t', 'step')
    br = 0
    while H:
        deltaQ, i, j = algorithm.maxQ(H)
        Q -= deltaQ

        st = time.time()
        Qtrees = algorithm.update_Qtrees(Qtrees, i, j, a)
        onet = time.time()
        H = algorithm.init_H(Qtrees)  # most expensive op
        twot = time.time()
        a = algorithm.update_a(i, j, a)
        threet = time.time()

        logging.info("time to compute Qtrees:  %f seconds" % (onet - st))
        logging.info("time to compute H:  %f seconds" % (twot - onet))
        logging.info("time to compute a:  %f seconds" % (threet - twot))

        step += 1

        print(i, '\t\t', j, '\t\t', round(Q, 7), '\t\t', -deltaQ, '\t\t', step)

        br += 1
        if br > 10:
            break
    """

if __name__ == '__main__':
    main()
