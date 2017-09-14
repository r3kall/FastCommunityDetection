#cython: boundscheck=False, wraparound=False, nonecheck=False

import heapq
import time

cimport cython

def detection(dict Qmatrix, dict A, list H):

    cdef double Q = 0  # actual modularity.
    cdef int step = 0  # number of steps.
    cdef double deltaQ
    cdef int i, j

    print "\ti    \tj    \tQ    \tdeltaQ    \tstep    \ttime"

    cdef int br = 0
    cdef int li = 100

    init = time.time()
    while H:
        st = time.time()
        deltaQ, i, j = heapq.heappop(H)

        if A[i] <= 0:
            continue

        if A[j] <= 0:
            # update H[i]
            update_H(Qmatrix[i], A, H, i)
            continue

        Q -= deltaQ  # update modularity.

        qt = time.time()
        # 1. update Qmatrix(i, j)
        update_Qmatrix(Qmatrix, A, i, j)
        print "time to matrix update: %f" % (time.time() - qt)

        # 2. update H(j)
        update_H(Qmatrix[j], A, H, j)

        # 3. update A(i, j)
        update_A(i, j, A)

        step += 1

        print "\t", i, "    \t", j, "    \t", round(Q, 5), "    \t", \
        -deltaQ, "    \t", step, "    \t", (time.time() - st)

        br += 1
        if br > li:
            break

    print("Total time:  %f mins" % ((time.time() - init) / 60))
    print("Actual Q:  %f" % Q)


cdef void update_Qmatrix(dict Qmatrix, dict A, int i, int j):

    # remove self reference
    Qmatrix[i].pop(j, None)

    for k in Qmatrix[j]:
        if (A[k] > 0) and (k not in Qmatrix[i]):
            # equation (10c)
            Qmatrix[j][k] -= (2 * A[i] * A[k])

    for k in Qmatrix[i]:
        if A[k] > 0:
            if k in Qmatrix[j]:
                # equation (10a)
                Qmatrix[j][k] += Qmatrix[i][k]
            else:
                # equation (10b)
                # here a new key is added to the j-th row
                Qmatrix[j][k] = Qmatrix[i][k] - (2 * A[j] * A[k])

    # remove i-th row
    Qmatrix.pop(i, None)


cdef void update_H(dict Qrow, dict A, list H, int n):
    """ Update the heap H. """
    cdef double maximum = 0
    cdef int index = -1
    for x in Qrow:
        if (A[x] > 0) and (Qrow[x] > maximum):
            maximum = Qrow[x]
            index = x
    if index >= 0:
        heapq.heappush(H, (-Qrow[index], n, index))


cdef void update_A(int i, int j, dict a):
    """ Update array of elements a. """
    a[j] += a[i]
    a[i] = -1