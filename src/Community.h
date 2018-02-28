/* ============================================================================
 * Community.h
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
 * ============================================================================
 *
 * Author: Lorenzo Rutigliano
 *
 * ============================================================================
 */

#ifndef __COMMUNITY_H
#define __COMMUNITY_H

#include <list>
#include <chrono>
#include <vector>
#include <iostream>


typedef struct node {
  double dq;
  int    k;

  node(int a, double b): k(a), dq(b) {}
  ~node() {}
  bool operator <(const node& n) const {return k<n.k;}
} CNode;  // 16B

/* ============================================================================
 * Community class represent a single community in the "community universe".
 * Each community maintains a list of its neighbors and a pointer to the
 * maximum delta Q value.
 * ============================================================================
 */

class Community {

public:
  std::list<CNode> cmembers;  // list of members
  std::list<CNode> clist;     // lists of neighbors
  CNode*      cmax;   // maximum dQ
  uint64_t    stamp;  // timestamp of last cmax
  int         id;     // community id
  
  Community();     // default constructor (for vector allocation)
  Community(int);  // custom constructor
  ~Community();    // default deconstructor

  friend std::ostream& operator <<(std::ostream&, Community&);

  inline unsigned int degree()  {return clist.size();}
  inline unsigned int members() {return cmembers.size();}
  inline unsigned int size()    {return clist.size()+cmembers.size();}

  bool add(int,double);                 // Add node in community
  bool scan_max(std::vector<double>&);  // Find member with maximum dQ

  void remove(int);                             // Remove specific node
  void shrink(std::vector<double>&);            // Remove obsolete nodes
  void merge(Community&,std::vector<double>&);  // Merge two communities

};  // 72B

inline uint64_t timestamp() {
    return std::chrono::duration_cast<std::chrono::microseconds>
              (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

#endif // __COMMUNITY_H