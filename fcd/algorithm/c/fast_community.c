/*
 * ===========================================================================
 * fast_community.c
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ===========================================================================
 *
 * Author: Lorenzo Rutigliano
 *
 * ===========================================================================
 */


/*
 * ===========================================================================
 * Implementation of the community structure algorithm originally published by
 * Clauset et al in:
 *
 * A. Clauset, M.E.J. Newman and C. Moore, "Finding community structure in
 * very large networks.". Phys. Rev. E 70, 066111 (2004).
 *
 * The data structures being used are slightly different and they are
 * described most closely in:
 *
 * K. Wakita, T. Tsurumi, "Finding community structure in mega-scale social
 * networks.". arXiv:cs/0702048v1.
 *
 *
 * We maintain a sorted vector of communities, each of which containing a
 * sorted list of their neighboring communities, along with their modularity
 * delta. Each community store also the one neighbor which would result in the
 * highest increase in modularity after a join. The maximums are also stored
 * in a max-heap. Another sorted vector is used to store partial result.
 * ===========================================================================
 */

/*
 * Todo:
 *     - construction and deletion
 *     - merge
 *     - test
 */


/* Structure storing community j and its modularity delta with community i */
typedef struct s_comm_pair {

  long int cm;    /* community member j */
  double   dq;    /* modularity delta of the pair (i, j) */

  struct s_comm_pair *next;

} community_pair;

/* Structure storing a community */
typedef struct {

  long int id;    /* community i */
  long int size;  /* size of the community  */
  double   av;    /* value of element a (see paper) */
  community_pair* neighs;  /* reference to neighboring communities */
  community_pair* maxdq;   /* reference to maximal modularity score */

} community_set;

/* Global community list structure */
typedef struct {

  long int n;         /* number of communities */
  community_set* m;   /* list of communities */

} community_uni;
