/* ============================================================================
 * MaxHeap.h
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
#ifndef __MAXHEAP_H
#define __MAXHEAP_H

#include <queue>


typedef struct element {
  uint64_t stamp;
  double dq;  
  int i;
  int j;
  element(int a, int b, double d, uint64_t st): i(a), j(b), dq(d), stamp(st) {}
  ~element() {}
} Element;  // 24B

inline bool operator <(const struct element& e1, const struct element& e2) {
  return e1.dq < e2.dq;
}

class MaxHeap {

public:
  inline int  size() {return pq.size();}  
  inline bool empty() {return pq.empty();}
  inline void push(int i, int j, double dq, uint64_t st) {pq.emplace(i, j, dq, st);}

  void pop(int& a, int& b, uint64_t& c) {
    a = pq.top().i;
    b = pq.top().j;
    c = pq.top().stamp;
    pq.pop();
  }

  void clear() {
    while (!pq.empty())
      pq.pop();
  }

private:
  std::priority_queue<Element> pq;
};  // 
#endif // __MAXHEAP_H