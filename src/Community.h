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

#include <cstddef>
#include <vector>
#include <list>

#include "Member.h"

/* ============================================================================
 * Community class represent a single community in the "community universe".
 * Each community maintains a list of its neighbors and a pointer to the
 * maximum delta Q value.
 * ============================================================================
 */

class Community {

public:

  int          community_id;       // community id.
  list<Member> community_neighs;   // list of neighbors.
  size_t       community_degree;   // number of neighbors.
  Member*      community_max;      // neighbor with maximum deltaQ value.
  list<Member> community_members;  // list of effective members.
  size_t       community_size;     // number of members.

  Community  ();
  Community  (int);
  ~Community ();

  friend ostream& operator << (ostream&, Community&);  

  // Insert a new member at the end of the neighbor list and 
  // increase community size.
  void add_member (Member& p) {
    community_neighs.push_back(p);
    community_degree++;
  }
  
  void sort_pairs ();  // Sort the pair vector
  
  bool contains (int);  // Contains function

  // Scan the community neighbors to find the member with maximum delta Q value
  int scan_max (vector<double>&);

  void remove_element (int);  // remove a member from the list

  void m_union (Community&);  // merge members lists

  void c_union (Community&, vector<double>&);  // merge two communities
};

#endif // __COMMUNITY_H