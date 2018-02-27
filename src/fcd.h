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
#include "Community.h"
#include "MaxHeap.h"

// #define DEBUG

// COMMONS
bool   fileExists(const string&);
int    init_universe(vector<Community>&, string);
void   init_array(vector<double>&, vector<Community>&, int);
void   init_heap(MaxHeap&, vector<Community>&, vector<double>&);
double init_Q(vector<double>&);
void   fill(vector<Community>&, vector<double>&);
void   shrink_all(vector<Community>&);
double modularity(string, vector<Community>&, vector<double>&);

// ALGORITHMS
pair<double,double> cnm(double, vector<Community>&, vector<double>&, MaxHeap&);
pair<double,double> cnm2(double, vector<Community>&, vector<double>&, MaxHeap&, int);

#endif // __FCD_H