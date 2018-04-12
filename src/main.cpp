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

#include <sys/stat.h>
#include <fstream> 
#include <sstream>
#include <algorithm>
#include <chrono>

#include "fcd.h"
using namespace std;

#define OUTPUT
#define COMP

tuple<int,int,int,int> stats (vector<Community>& univ, vector<double> av) {
  int n_of_singleton   = 0;
	int n_of_communities = 0;
  int max_size = 0;
  int sc;
  int cnt = 0;
  double mean_size = 0;

	for (int c=0; c<univ.size(); c++) {
    sc = univ[c].members();
    if (av[c] > 0 && sc > 0) {
      cnt++;      
      if (sc < 3) n_of_singleton++;
      else n_of_communities++;
      if (sc > max_size) max_size=sc;
      mean_size+=sc;
    }
	}

  return make_tuple(n_of_communities,n_of_singleton,
                    max_size, int(mean_size/double(cnt)));
}


vector<int> ownership(vector<Community>& univ) {
  vector<int> own(univ.size(), -1);
  for (auto&& c: univ)
    for (auto&& m: c.clist)
      if (m.member)
        own[m.k] = c.id;
  return own;
}


vector<int> selection(vector<Community>& univ, int t, int m) {
  int lowerbound = 256;
  int upperbound = 32768;
  int midbound = 2048;
  vector<int> res;  
  vector<pair<int,int>> order;

  for (auto&& c: univ)
    if (c.size() > 0)
      order.push_back(make_pair(c.members(), c.id));

  sort(order.begin(), order.end());
  reverse(order.begin(), order.end());

  int cnt = 0;
  for (int i=0; i<order.size(); i++) {
    if (cnt >= t) break;
    if (order[i].first >= lowerbound && order[i].first <= upperbound) {
      res.push_back(order[i].second);
      cnt++;
    }
  }

  vector<int> mid;
  vector<int> own = ownership(univ);

  for (int i: res) {
    cnt = 0;
    for (auto&& v: univ[i].clist) {
      if (cnt >= 64) break;
      if (!v.member && own[v.k] >= 0) {
        if (find(res.begin(), res.end(), own[v.k]) == res.end()) {
          if (univ[own[v.k]].members() >= lowerbound && univ[own[v.k]].members() <= midbound) {
            if (find(mid.begin(), mid.end(), own[v.k]) == mid.end()) {
              mid.push_back(own[v.k]);
              cnt++;
            }
          }
        }
      }
    }
  }

  random_shuffle(mid.begin(), mid.end());
  for (int i=0; i<m; i++)
    res.push_back(mid[i]);

  return res;
}


void StructureToTSV(string filename, bool ms, 
                    vector<Community>& univ, vector<int>& selection) {
  vector<int> own = ownership(univ);
  vector<bool> visited(univ.size(), false);
  vector<Community> origin;
  int m = init_universe(origin, filename);
  list<CNode>::iterator it;
  list<int> queue;
  int front;

  ofstream myfile;
  if (ms) myfile.open (filename + "_struct_ms.tsv");
  else myfile.open (filename + "_struct_std.tsv");
  myfile << "Source\tTarget\n";

  for (int i: selection) {
    front = univ[i].clist.front().k;
    visited[front] = true;
    queue.push_back(front);

    while (!queue.empty()) {
      front = queue.front();
      queue.pop_front();
      for (it=origin[front].clist.begin(); it!=origin[front].clist.end(); ++it) {
        if (find(selection.begin(),selection.end(),own[it->k]) != selection.end()) {
          if (!visited[it->k]) {
            myfile << front << "\t" << it->k << "\n";
            visited[it->k] = true;
            queue.push_back(it->k);
          }
        }
      }
    }    
    for (int b=0; b<visited.size(); b++)
      visited[b] = false;
  }
  myfile.close();
}


void SizeToTSV(string filename, bool ms, vector<Community>& univ) {
  ofstream myfile;
  if (ms) myfile.open (filename + "_info_ms.tsv");
  else myfile.open (filename + "_info_std.tsv");
  myfile << "Id\tSize\n";
  for (auto&& c: univ)
    if (c.size() > 0)
      myfile << c.id << "\t" << c.members() << "\n";
  myfile.close();
}


void CommunityToTSV(string filename, bool ms, 
                    vector<Community>& univ, vector<int> selection) {
  ofstream ids;
  if (ms) ids.open (filename + "_id_ms.tsv");
  else ids.open (filename + "_id_std.tsv");
  ids << "Id\tCommunity\n";
  for (int i=0; i<selection.size(); i++)
    for (auto it=univ[selection[i]].clist.begin(); 
        it!=univ[selection[i]].clist.end(); ++it)
      if (it->member)
        ids << it->k << "\t" << selection[i] << "\n";
  ids.close();
}


bool run(string filename, bool ms, int l_scope) {
/* ========================================================================= */
  vector<double>       arrv;  /* Array of elements A (see paper). If a value
                                 is less than zero, it means that the
                                 corresponding community was merged 
                                 within another. */
  vector<Community>    univ;  /* Array of Communities. */
  MaxHeap              heap;  /* Max-Heap of pairs delta Q value. */
  int                  m;     /* Number of edges */
/* ========================================================================= */

#ifdef OUTPUT
  cout << "============================================================\n";
  if (!ms) cout << "CNM-standard\n\n";
  else cout << "CNM-multistep\n\n";
#endif
  m = init_universe(univ, filename);
  init_array(arrv, univ, m);
  init_heap(heap, univ, arrv);
  double Q = init_Q(arrv);

  double total_time, sQ;
  if (!ms) tie(total_time, sQ) = cnm(Q, univ, arrv, heap);
  else tie(total_time, sQ) = cnm2(Q, univ, arrv, heap, l_scope);

#ifdef DEBUG
  clock_t post_begin = clock();
#endif
  vector<int> sel = selection(univ, 3, 7);
  fill(univ, arrv);
  shrink_all(univ);
#ifdef DEBUG 
  clock_t post_end = clock();
  double post_elapsed = double(post_end - post_begin) / CLOCKS_PER_SEC;
  cout << "Time to post-processing: " << post_elapsed << " seconds" << endl;
#endif

  int cms, sngl, maxs, mns;
  tie(cms, sngl, maxs, mns) = stats(univ, arrv);

  ofstream myfile;
  myfile.open ("summary_" + filename, ios::app);
  if (!ms) myfile << "CNM-standard  ";
  else myfile << "CNM-multistep  l_scope: " << l_scope << " ";
  myfile << "maxQ " << sQ << " total_time " << total_time << " ";
  myfile << "#minors " << sngl << " #communities " << cms << " ";
  myfile << "max size: " << maxs << " mean size: " << mns << "\n";
  myfile.close();

#ifdef DEBUG
  clock_t begin = clock();
#endif
  SizeToTSV(filename, ms, univ);
  CommunityToTSV(filename, ms, univ, sel);
  StructureToTSV(filename, ms, univ, sel);
#ifdef DEBUG 
  clock_t end = clock();
  double elapsed = double(end - begin) / CLOCKS_PER_SEC;
  cout << "Time to generate tsv files: " << elapsed << " seconds" << endl;
#endif

#ifdef OUTPUT
  cout << "# vertices: " << univ.size() << "\n";
  cout << "# edges: " << m << "\n";
  cout << "total time: " << total_time << " seconds\n";
  cout << "max Q: " << sQ << "\n";
  cout << "#minors: " << sngl << "\t#communities: " << cms << "\n";
  cout << "max size: " << maxs << " mean size: " << mns << "\n";
  cout << "============================================================\n\n";
#endif  
  return (univ.size()<600000 && m<3000000);
}


/* MAIN */
int main(int argc, char *argv[]) {
/* ========================================================================= */
	vector<double>       arrv;  /* Array of elements A (see paper). If a value
	                               is less than zero, it means that the
	                               corresponding community was merged 
	                               within another. */
	vector<Community>    univ;  /* Array of Communities. */
	MaxHeap              heap;  /* Max-Heap of pairs delta Q value. */
	int                  m;     /* Number of edges */
/* ========================================================================= */
	
	if (argc != 3) {
		cerr << "Extra Command Line Arguments Error: wrong number of arguments" << endl;
		exit(1);
	}

	istringstream ss(argv[1]);
	string filename;
	ss >> filename;
  
  istringstream ii(argv[2]);
  int l_scope;
  if(!(ii >> l_scope))
    cerr << "Invalid number\n";

	if (!fileExists(filename)) {
		cerr << "Extra Command Line Arguments Error: file doesn't exists" << endl;
		exit(1);
	}

  bool flag = run(filename, true, l_scope);
#ifdef COMP
  if (flag) run(filename, false, 0);
#endif
  exit(0);
}
