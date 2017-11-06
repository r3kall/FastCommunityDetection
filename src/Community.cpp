/* ===========================================================================
 * Community.cpp
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

#include <algorithm>

#include "Community.h"

#define TH 8192*2


Community::Community () {
  community_id      = -1;
  community_size    = 1;
  community_max     = NULL;
}

Community::Community (int id) {
  community_id      = id;
  community_size    = 1;
  community_max     = NULL;
}

Community::~Community () {  
  community_max = NULL;
  community_neighs.clear();
}

ostream& operator << (ostream& outputStream, Community& c) {

  outputStream << "community: " << c.community_id;   
  outputStream << "  \tsize: " << c.community_size << "\n";

  for (list<Member>::iterator it=c.community_neighs.begin(); it!=c.community_neighs.end(); it++) {
    outputStream << "\t|---" << (*it);
  }

  return outputStream;

}

/* [Header] Function:  sort_pairs
 * ----------------------------------------------------------------------------
 * Sort neighbors list based on member order.
 */
void Community::sort_pairs () {
  community_neighs.sort(member_cmp);
}

/* [Header] Function:  contains
 * ----------------------------------------------------------------------------
 * j:  member id.
 *
 * returns:  boolean true if the community contains member j
 */
bool Community::contains (int j) {
  for (list<Member>::iterator it=community_neighs.begin(); 
          it!=community_neighs.end(); it++) {
    if ( (*it).id() == j )
      return true;
  }
  return false;
}

/* [Header] Function:  scan_max
 * ----------------------------------------------------------------------------
 * Scan the community neighboring to find the member with maximum delta Q value
 *
 * av:  vector of double.
 *
 * returns:  
 */
int Community::scan_max (vector<double>& av) {

  Member* best   = NULL;
  double bestdq  = 0;
  
  for (list<Member>::iterator it=community_neighs.begin(); 
          it!=community_neighs.end(); it++) {

    if (av[(*it).id()] > 0) {
      if (it->dq() > bestdq) {
        bestdq = (*it).dq();
        best   = &(*it);
      }
    }
  }  

  if (best != NULL) {
    if (community_max != NULL) {
      if (community_max->id() != best->id()) {
        community_max = best;
        return 1;
      } else return 0;
    } else {
      community_max = best;
      return 1;
    }  
  } else return -1;
}

/* [Header] Function:  remove_element
 * ----------------------------------------------------------------------------
 * Remove a specified element from the neighbor list.
 *
 * mbr: member id to be removed.
 */
void Community::remove_element (int mbr) {

  for (list<Member>::iterator it=community_neighs.begin(); 
          it!=community_neighs.end(); it++) {
    if ( it->id() == mbr ) {
      community_neighs.erase(it);
      break;
    }
  }
}

/* [Header] Function:  merge
 * ---------------------------------------------
 * Merge two communities and change the delta Q according to equations.
 *
 * cm:  the community that will be merged.
 * av:  vector of double.
 */
void Community::c_union (Community& c, vector<double>& av) {

  // define iterators
  list<Member>::iterator xbegin = community_neighs.begin();
  list<Member>::iterator xend   = community_neighs.end();
  list<Member>::iterator ybegin = c.community_neighs.begin();
  list<Member>::iterator yend   = c.community_neighs.end();

  while(true) {

    // if this community neighbor list ends, then append all the remaining list
    // of community c and update coherently.
    if (xbegin == xend) {
      community_neighs.splice(xend, c.community_neighs);

      // Each appended element correspond to equation (10b).
      for (; xbegin != community_neighs.end(); ++xbegin) {
        // if (av[(*xbegin).id()] > 0)  // check if valid
          // update deltaQ
          (*xbegin).setdq( 
              (*xbegin).dq() - (2 * av[community_id] * av[(*xbegin).id()])
            );
      }
      break;
    }

    // if community c neighbor list ends, finish.
    if (ybegin == yend) break;

    if ((*xbegin).id() < (*ybegin).id()) {
      // equation (10c)
      // if (av[(*xbegin).id()] > 0) // check if valid
        (*xbegin).setdq( 
            (*xbegin).dq() - (2 * av[c.community_id] * av[(*xbegin).id()]) );
      ++xbegin;
    
    } else if ((*ybegin).id() < (*xbegin).id()) {      

      // equation (10b)
      // if (av[(*ybegin).id()] > 0) // check if valid
        (*ybegin).setdq( 
            (*ybegin).dq() - (2 * av[community_id] * av[(*ybegin).id()]) 
          );

      community_neighs.splice(xbegin, c.community_neighs, ybegin);
      ybegin = c.community_neighs.begin();

    } else { // equal elements
      // equation (10a)
      // if (av[(*xbegin).id()] > 0)
        (*xbegin).setdq( (*xbegin).dq() + (*ybegin).dq() );

      ++xbegin;
      c.community_neighs.pop_front();
      ybegin = c.community_neighs.begin();
    }
  }
}
