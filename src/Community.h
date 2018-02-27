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
#include <vector>
#include <iostream>
using namespace std;


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
  list<CNode> cmembers;  // list of members
  list<CNode> clist;     // lists of neighbors
  CNode*      cmax;      // maximum dQ
  int         last;      // last iteration where cmax is modified
  int         id;        // community id  
  
  Community();     // default constructor (for vector allocation)
  Community(int);  // custom constructor
  ~Community();    // default deconstructor

  friend ostream& operator <<(ostream&, Community&);

  inline int degree()  {return clist.size();}
  inline int members() {return cmembers.size();}
  inline int size()    {return clist.size()+cmembers.size();}

  bool add(int,double);                    // Add node in community
  bool scan_max(vector<double>&);          // Find member with maximum dQ

  void remove(int);                        // Remove node with specific key
  void shrink(vector<double>&);            // Remove obsolete nodes
  void merge(Community&,vector<double>&);  // Merge two communities

};  // 64B

#endif // __COMMUNITY_H