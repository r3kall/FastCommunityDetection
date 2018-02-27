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

#include "fcd.h"

#define OUTPUT

tuple<int,int,int,int> stats (vector<Community>& univ, vector<double> av) {
  int n_of_singleton   = 0;
	int n_of_communities = 0;
  int max_size = 0;
  int sc;
  int cnt;
  double mean_size = 0;

	for (int c=0; c<univ.size(); c++) {
    if (av[c] > 0) {
      cnt++;
      sc = univ[c].members();
      if (sc == 1) n_of_singleton++;
      else n_of_communities++;
      if (sc > max_size) max_size=sc;
      mean_size+=sc;
    }
	}

  return make_tuple(n_of_communities,n_of_singleton,
                    max_size, int(mean_size/double(cnt)));
}


bool run(string filename, int mod, int l_scope) {
/* ========================================================================= */
  vector<double>       arrv;  /* Array of elements A (see paper). If a value
                                 is less than zero, it means that the
                                 corresponding community was merged 
                                 within another. */
  vector<Community>    univ;  /* Array of Communities. */
  MaxHeap              heap;  /* Max-Heap of pairs delta Q value. */
  int                  m;     /* Number of edges */
/* ========================================================================= */

  m = init_universe(univ, filename);
  init_array(arrv, univ, m);
  init_heap(heap, univ, arrv);
  double Q = init_Q(arrv);

  double total_time, sQ;
  if (mod == 1)
    tie(total_time, sQ) = cnm(Q, univ, arrv, heap);
  else tie(total_time, sQ) = cnm2(Q, univ, arrv, heap, l_scope);
  fill(univ, arrv);
  shrink_all(univ);

  int cms, sngl, maxs, mns;
  tie(cms, sngl, maxs, mns) = stats(univ, arrv);

  ofstream myfile;
  myfile.open ("summary_" + filename, ios::app);
  if (mod == 1) 
    myfile << "CNM-standard  ";
  else 
    myfile << "CNM-agglomerative  a-factor: " << l_scope << " ";

  myfile << "maxQ " << sQ << " total_time " << total_time << " ";
  myfile << "#singleton " << sngl << " #communities " << cms << " ";
  myfile << "max size: " << maxs << " mean size: " << mns << "\n";
  myfile.close();

#ifdef OUTPUT
  cout << "\n============================================================\n";
  if (mod == 1) cout << "CNM-standard\n";
  else cout << "CNM-agglomerative\n";
  cout << "# vertices: " << univ.size() << "\n";
  cout << "# edges: " << m << "\n";
  cout << "total time: " << total_time << " seconds\n";
  cout << "max Q: " << sQ << "\n";
  cout << "# singleton: " << sngl << "\t# communities: " << cms << "\n";
  cout << "max size: " << maxs << " mean size: " << mns << "\n";
  cout << "modularity: " << modularity(filename, univ, arrv)-Q << "\n";
  cout << "============================================================\n\n";
#endif  
  return (univ.size()<500000 && m<3000000);
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

  bool flag = run(filename, 2, l_scope);
  if (flag) run(filename, 1, 0);  
 	exit(0);
}
