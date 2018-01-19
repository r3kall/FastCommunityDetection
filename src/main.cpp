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
      return a.first < b.first;
  });

#ifdef DEBUG
  clock_t end = clock();
  double elapsed = double(end - begin) / CLOCKS_PER_SEC;
  cout << "Time to rank communities: " << elapsed << " seconds" << endl;
#endif

	return vect;
}

bool check_member (int q, Community& c) {
  if (q == c.community_id) return true;
  for (list<Member>::iterator it=c.community_members.begin(); 
          it!=c.community_members.end(); it++) {
    if (it->id() > q) return false;
    if (it->id() == q) return true;
  }
  return false;
}

bool check_link (vector<Community>& dummy, Community& a, Community& b) {
  int b_min_member = b.community_members.front().id();
  int b_max_member = b.community_members.back().id();

  for (list<Member>::iterator it=dummy[a.community_id].community_neighs.begin();
          it!=dummy[a.community_id].community_neighs.end(); it++) {
    
    if ( b_min_member <= it->id() <= b_max_member ) {
      if (it->id() == b_max_member || it->id() == b_min_member) return true;
      if (check_member(it->id(), b)) return true;
    }
  }

  for (list<Member>::iterator it1=a.community_members.begin(); 
          it1!=a.community_members.end(); it1++) {
    for (list<Member>::iterator it2=dummy[it1->id()].community_neighs.begin();
            it2!=dummy[it1->id()].community_neighs.end(); it2++) {
      
      if ( b_min_member <= it2->id() <= b_max_member ) {
        if (it2->id() == b_max_member || it2->id() == b_min_member) return true;
        if (check_member(it2->id(), b)) return true;
      }
    }
  }
  return false;
}


void communities_to_csv (vector<Community>& univ, vector< pair<int,int> >& comm, string filename) {

#ifdef DEBUG
  clock_t begin = clock();
#endif

	cout << "Converting to CSV ... Start" << endl;

  ofstream vertexfile(filename + "_vertices.csv");
  if (vertexfile.is_open()) {
		vertexfile << "Id," << "Size\n";
		for (int i=0; i<comm.size(); i++)			
			vertexfile << comm[i].first << "," << comm[i].second << "\n";				
		vertexfile.close();
  } else {
    cerr << "Error opening CSV output file." << endl;
    exit(1);
  }

  int m, ind, cRef;
  vector<Community> dummy;
  tie(dummy, m) = populate_universe(filename);
  // IDEA 1: merge members neighbors and then check for all

  ofstream edgefile(filename + "_edges.csv");  // create edge file
  if (edgefile.is_open()) {
  	edgefile << "Source,Target\n";  // first row with labels
    for (int i=0; i<comm.size()/4; i++) {
      ind = comm[i].first;
      cout << "checking community " << i << " out of " << comm.size() << "\n"; 
      for (int j=0; j<comm.size(); j++) {
        if (comm[j].first == ind) continue;
        if (check_link(dummy, univ[ind], univ[comm[j].first]))
          vertexfile << ind << "," << comm[i].first << "\n";
      }
    }
  	edgefile.close();
  } else {
  	cerr << "Error opening CSV output file." << endl;
  	exit(1);
  }

  cout << "Converting to CSV ... End" << endl;
#ifdef DEBUG
  clock_t end = clock();
  double elapsed = double(end - begin) / CLOCKS_PER_SEC;
  cout << "Time to make CSV files: " << elapsed << " seconds" << endl;
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

  cout << "\nNumber of vertices: " << univ.size() << "\n";
  cout << "Number of edges: " << m << "\n";
  cout << "Total time: " << total_time << "\n";
  cout << "Max Q: " << Q << "\n" << endl;

  vector< pair<int,int> > ranks = community_ranking(univ);
  communities_to_csv(univ, ranks, filename);

 	exit(0);
}
