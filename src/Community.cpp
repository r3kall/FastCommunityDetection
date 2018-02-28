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

#include "Community.h"
using namespace std;


Community::Community() {
  id = -1;
  stamp = 0;
}

Community::Community(int idx) {
  id = idx;
  stamp = 0;
}

Community::~Community() {  
  cmax = NULL;
  clist.clear();
  cmembers.clear();
}

ostream& operator <<(ostream& os, Community& c) {
  os << "id: " << c.id << "\nmembers:\n";
  for (auto it=c.cmembers.begin(); it!=c.cmembers.end(); ++it)
    os << it->k << " \t" << it->dq << "\n";
  os << "neighbors:\n";
  for (auto it=c.clist.begin(); it!=c.clist.end(); ++it)
    os << it->k << " \t" << it->dq << "\n";  
  return os;
}


bool Community::add(int k, double v) {
  for (auto it=clist.begin(); it!=clist.end(); ++it) {
    if (it->k > k) {
      clist.emplace(it, k, v);
      return true;
    }
    if (it->k == k) return false;
  }
  clist.push_back(CNode(k, v));
  return true;
}


/* [header] function:  scan_max
 * ----------------------------------------------------------------------------
 * Scan the community neighborhood to find the member with maximum 
 * delta Q value.
 *
 * Args:
 *    - av : vector of double.
 *
 * Returns: 1 if new maximum is found, 0 if same value, -1 if invalid. 
 */
bool Community::scan_max(vector<double>& av) {
  cmax = NULL;
  double bestdq = 0;
  for (list<CNode>::iterator it=clist.begin(); it!=clist.end(); ++it) {
    if ((av[it->k] > 0) && (it->dq > bestdq)) {
        bestdq = it->dq;
        cmax = &(*it);  
    }
  }
  return (cmax != NULL);
}


void Community::remove(int k) {
  for (list<CNode>::iterator it=clist.begin(); it!=clist.end(); ++it) {
    if (it->k == k) {
      clist.erase(it);
      return;        
    }
  }  
}


void Community::shrink(vector<double>& av) {  
  for (list<CNode>::iterator it=clist.begin(); it!=clist.end();)
    if (av[it->k] < 0)
      it = clist.erase(it);
    else ++it;
}


void Community::merge(Community& cm, vector<double>& av) {
  list<CNode>::iterator ax = clist.begin();
  list<CNode>::iterator bx = cm.clist.begin();
  while (1) {
    if (ax == clist.end()) {
      // update from bx to by, equation (10b)
      for (; bx!=cm.clist.end(); ++bx)
        bx->dq -= 2.0*av[id]*av[bx->k];
      // append the remaining b list to a
      clist.splice(ax, cm.clist);      
      break;
    }

    if (bx == cm.clist.end()) {
      // update from ax to ay, equation (10c)
      for (;ax!=clist.end();) {
        if (ax->k == cm.id)
          ax = clist.erase(ax);
        else {
          ax->dq -= 2.0*av[cm.id]*av[ax->k];
          ++ax;
        }
      }
      break;
    }

    if (ax->k == cm.id) {
      ax = clist.erase(ax);
      continue;
    }

    if (ax->k < bx->k) {
      // update ax, equation (10c)
      ax->dq -= 2.0*av[cm.id]*av[ax->k];
      ++ax;
    } else if (ax->k > bx->k) {
      // update bx, equation (10b)
      bx->dq -= 2.0*av[id]*av[bx->k];
      // transfer the node from list b to a
      clist.splice(ax, cm.clist, bx);
      bx = cm.clist.begin();
    } else {  // equals
      // update ax, equation (10a)
      ax->dq += bx->dq;
      ++ax;
      // remove equal node from b
      cm.clist.pop_front();
      bx = cm.clist.begin();
    }
  }
}
