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
//#define COMP

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


vector<int> top(vector<Community>& univ, int s) {
  vector<int> res;  
  vector<pair<int,int>> order;
  for (auto&& c: univ)
    if (c.size() > 0)
      order.push_back(make_pair(c.size(), c.id));

  sort(order.begin(), order.end());
  reverse(order.begin(), order.end());
  for (int i=0; i<s; i++)
    res.push_back(order[i].second);

  return res;
}


void SizeToCSV(string filename, bool ms, vector<Community>& univ) {
  ofstream myfile;
  if (ms) myfile.open (filename + "_info_ms.csv");
  else myfile.open (filename + "_info_std.csv");
  myfile << "id,size\n";
  for (auto&& c: univ)
    if (c.size() > 0)
      myfile << c.id << "," << c.members() << "\n";
  myfile.close();
}


void StructureToSCV(string filename, bool ms, vector<Community>& univ) {
  vector<int> own(univ.size(), -1);
  for (auto&& c: univ)
    for (auto&& m: c.clist)
      if (m.member)
        own[m.k] = c.id;

  vector<int> top10 = top(univ, 2);

  vector<Community> origin;
  int m = init_universe(origin, filename);

  ofstream myfile;
  if (ms) myfile.open (filename + "_struct_ms.csv");
  else myfile.open (filename + "_struct_std.csv");
  myfile << "Source,Target,Weight\n";
  for (int i: top10) {
    for (auto&& m: univ[i].clist) {
      for (auto&& n: origin[m.k].clist) {
        if (own[n.k] == i)
          myfile << m.k << "," << n.k << ",2.0" << "\n";
        else
          myfile << m.k << "," << n.k << ",1.0" << "\n";        
      }
    }
  }
  myfile.close();


  ofstream myfile2, myfile3;
  if (ms) myfile2.open (filename + "_id_ms.csv");
  else myfile2.open (filename + "_id_std.csv");
  myfile2 << "Id,Community\n";
  for (int i: top10)
    for (auto&& m: univ[i].clist)
      myfile2 << m.k << "," << i << "\n";
  myfile2.close();
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
  fill(univ, arrv);  
  shrink_all(univ);

  int cms, sngl, maxs, mns;
  tie(cms, sngl, maxs, mns) = stats(univ, arrv);

  ofstream myfile;
  myfile.open (filename + "_summary", ios::app);
  if (!ms) myfile << "CNM-standard  ";
  else myfile << "CNM-multistep  l_scope: " << l_scope << " ";
  myfile << "maxQ " << sQ << " total_time " << total_time << " ";
  myfile << "#minors " << sngl << " #communities " << cms << " ";
  myfile << "max size: " << maxs << " mean size: " << mns << "\n";
  myfile.close();

  SizeToCSV(filename, ms, univ);
  StructureToSCV(filename, ms, univ);  

#ifdef OUTPUT
  cout << "# vertices: " << univ.size() << "\n";
  cout << "# edges: " << m << "\n";
  cout << "total time: " << total_time << " seconds\n";
  cout << "max Q: " << sQ << "\n";
  cout << "#minors: " << sngl << "\t#communities: " << cms << "\n";
  cout << "max size: " << maxs << " mean size: " << mns << "\n";
  cout << "============================================================\n\n";
#endif  
  return (univ.size()<800000 && m<3000000);
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
