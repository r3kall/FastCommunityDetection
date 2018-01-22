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


/* simple stats */
tuple<int, int> stats (vector<Community>& univ) {
  int n_of_singleton   = 0;
	int n_of_communities = 0;

	for (int c=0; c<univ.size(); c++) {
    if (univ[c].community_size == 1) n_of_singleton++;
		if (univ[c].community_size > 1) n_of_communities++;
	}

  return make_tuple(n_of_communities, n_of_singleton);
}


/* [] function:  community_ranking
 * ----------------------------------------------------------------------------
 * Sort communities by IDs.
 *
 * Args:
 *    - univ : non-empty community vector.
 *
 * Returns: sorted vector of community IDs and sizes.
 */
vector< pair<int,int> > community_ranking (vector<Community>& univ) {
#ifdef DEBUG
  clock_t begin = clock();
#endif

	vector< pair<int,int> > vect;
	for (int i=0; i<univ.size(); i++)
    if (univ[i].community_size > 0)
		  vect.push_back( make_pair(univ[i].community_id, univ[i].community_size) );		

	sort(vect.begin(), vect.end(), 
    [](const pair<int,int>& a, const pair<int,int>& b) -> bool {
      // lambda function
      return a.second > b.second;
  });

#ifdef DEBUG
  clock_t end = clock();
  double elapsed = double(end - begin) / CLOCKS_PER_SEC;
  cout << "Time to rank communities: " << elapsed << " seconds" << endl;
#endif

	return vect;
}


void community_to_csv (Community& c, vector<double>& av, string filename) {

#ifdef DEBUG
  clock_t begin = clock();
#endif

  int n_internal_edges = 0;
  ofstream edgefile(filename + "_edges.csv");  // create edge file
  if (edgefile.is_open()) {
  	edgefile << "Source,Target\n";  // first row with labels
    for (list<Member>::iterator it=c.community_members.begin();
            it!=c.community_members.end(); it++) {
      if (av[it->id()] <= 0) {
        edgefile << it->id() << "," << int(abs(av[it->id()])) << "\n";
        n_internal_edges++;
      }
    }
  	edgefile.close();
  } else {
  	cerr << "Error opening CSV output file." << endl;
  	exit(1);
  }

#ifdef DEBUG
  clock_t end = clock();
  double elapsed = double(end - begin) / CLOCKS_PER_SEC;
  cout << "Time to make CSV files: " << elapsed << " seconds\n";
  cout << "community size: " << c.community_size << "\n";
  cout << "# internal edges: " << n_internal_edges << endl;
#endif
}


/* MAIN */
int main(int argc, char *argv[]) {

/* ========================================================================= */
	vector<double>       arrv;  /* Array of elements A (see paper). If a value
	                               is less than zero, it means that the
	                               corresponding community was merged 
	                               within another. */
	vector<Community>    univ;  /* Array of Communities. */
	priority_queue<Pair> heap;  /* Max-Heap of pairs delta Q value. */
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
  int chunck_size;
  if(!(ii >> chunck_size))
    cerr << "Invalid number\n";

	if (!fileExists(filename)) {
		cerr << "Extra Command Line Arguments Error: file doesn't exists" << endl;
		exit(1);
	}

  tie(univ, m) = populate_universe(filename);
  arrv = populate_array(univ, m);
  heap = populate_heap(univ, arrv);
  
  double total_time, Q;
  tie(total_time, Q) = fcd(univ, arrv, heap, m, chunck_size);

  int n_of_communities, n_of_singleton;
  tie(n_of_communities, n_of_singleton) = stats(univ);

  ofstream myfile;
  myfile.open ("summary_" + filename, ios::app);
  myfile << "chunck_size " << chunck_size << " ";
  myfile << "maxQ " << Q << " total_time " << total_time << " ";
  myfile << "n_singleton " << n_of_singleton << " ";
  myfile << "n_communities " << n_of_communities << "\n";
  myfile.close();

  cout << "\n# vertices: " << univ.size() << "\n";
  cout << "# edges: " << m << "\n";
  cout << "total time: " << total_time << "seconds\n";
  cout << "max Q: " << Q << "\n";

  vector< pair<int,int> > ranks = community_ranking(univ);
  int index;
  for (int i=0; i<ranks.size(); i++) {
    if (ranks[i].second < 10000) {
      index = ranks[i].first;
      break;
    }
  }
  // community_to_csv(univ[index], arrv, filename);

  cout << "done" << endl;
 	exit(0);
}
