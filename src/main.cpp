/* ===========================================================================
 * main.cpp
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

#include "fcd.h"


int main() {	
  
  priority_queue<Pair> pq;
  vector<Community> univ;
  vector<double> av;
  int m;

  tie(univ, m) = populate_universe();
  av = populate_array(univ, m);
  pq = populate_heap(univ, av);

  fcd(univ, av, pq, m);

 	exit(0);
}