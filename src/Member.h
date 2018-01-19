/* ============================================================================
 * Member.h
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

#ifndef __MEMBER_H
#define __MEMBER_H

#include <iostream>
using namespace std;

/* ============================================================================
 * This class represent neighboring communities.
 * For each pair (community, member) exists a delta Q value.
 * ============================================================================
 */

class Member {

public:
  Member ();
  Member (int, double);

  friend ostream& operator << (ostream&, Member&);
  
  inline int id () {return member_id;}
  inline double dq () {return member_dq;}
  inline void setdq (double newdq) {member_dq = newdq;}

private:  
  double member_dq;  // delta Q of (community, member)
  int member_id;     // community member id

};

bool member_cmp (Member&, Member&);  // Define member order

#endif //__MEMBER_H