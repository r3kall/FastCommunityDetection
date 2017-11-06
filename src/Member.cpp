/* ===========================================================================
 * Member.cpp
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

#include "Member.h"


Member::Member () {
  member_id  = -1;
  member_dq = 0;
}

Member::Member (int id, double dq) {
  member_id  = id;
  member_dq = dq;
}

ostream& operator << (ostream& outputStream, Member& arr) {
  outputStream << "member: " << arr.id();   
  outputStream << "\tdq: " << arr.dq() << "\n";
  return outputStream;

}

/* [Header] Function:  member_cmp  
 * ----------------------------------------------------------------------------
 * Compare function that defines member order.
 *
 * p1, p2: members to be ordered.
 *
 * returns:  boolean true if p1 precede p2.
 */
bool member_cmp (Member& p1, Member& p2) {
  return p1.id() < p2.id();

}