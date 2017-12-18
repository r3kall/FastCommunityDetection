/* ===========================================================================
 * fcd.h
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

#ifndef __FCD_H
#define __FCD_H

#include <tuple>
#include <queue>
#include "Community.h"


// Pair class for system-wide max-heap
class Pair {

public:
	Pair ();
	Pair (int, int, double);

	inline int community_i () {return cty;}
	inline int community_j () {return mbr;}
	inline double dq () {return pdq;}

	friend bool operator < (const Pair& p1, const Pair& p2) {
		return p1.pdq < p2.pdq;
	}

private:
	int    cty;  // community id
	int		 mbr;  // member id
	double pdq;  // delta Q
};

bool fileExists (const string&);

pair<vector<Community>, int> populate_universe (string);

vector<double> populate_array (vector<Community>&, int);

priority_queue<Pair> populate_heap (vector<Community>&, vector<double>&);

void merge_communities (Community&, Community&, vector<double>&);

tuple<double, double> fcd (
				vector<Community>&, vector<double>&, priority_queue<Pair>&, int, int);

#endif // __FCD_H