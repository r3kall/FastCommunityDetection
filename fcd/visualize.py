# -*- coding: utf-8 -*-

import numpy as np
import matplotlib.pyplot as plt


def modularity_evo(mod_list):
    """

    :param mod_list:
    :return:
    """

    x = np.arange(len(mod_list))
    plt.plot(x, mod_list)
    plt.xlabel("x-th join")
    plt.ylabel("modularity Q")
    plt.show()
