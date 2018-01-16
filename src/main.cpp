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
 * Sort and select first r communities by size.
 *
 * Args:
 *    - univ : non-empty community vector.
 *    - r    : number of communities to be retrieved.
 *
 * Returns: sorted vector of community IDs.
 */
vector<int> community_ranking (vector<Community>& univ, int r) {
#ifdef DEBUG
  clock_t begin = clock();
#endif

	vector< pair<int,int> > vect;
	for (int i=0; i<univ.size(); i++) {
		if (univ[i].community_size > 1) {
			vect.push_back( make_pair(univ[i].community_id, univ[i].community_size) );
		}
	}

	sort(vect.begin(), vect.end(), 
    [](const pair<int,int>& a, const pair<int,int>& b) -> bool {
      // lambda function
      return a.second > b.second;
  });

	vector<int> ratings;
	for (int j=0; j<r; j++) {
		ratings.push_back(vect[j].first);
	}

#ifdef DEBUG
  clock_t end = clock();
  double elapsed = double(end - begin) / CLOCKS_PER_SEC;
  cout << "Time to rank communities: " << elapsed << " seconds" << endl;
#endif

	return ratings;
}


bool check_member (vector<Community>& univ, vector<int> ratings, int cm, int mbm) {
	for (int i=0; i<ratings.size(); i++) {
		if (ratings[i] == cm) continue;
		for (list<Member>::iterator it=univ[ratings[i]].community_members.begin(); 
      				it!=univ[ratings[i]].community_members.end(); it++) {
			if (it->id() == mbm) return true;
		}
	}
	return false;
}


void communities_to_csv (vector<Community>& univ, vector<int> ratings, 
				vector<double>& av, string filename) {

	cout << "Converting to CSV ... Start" << endl;
	string r = to_string(ratings.size());
  ofstream vertexfile(filename + "_vertices_" + r + ".csv");
  if (vertexfile.is_open()) {
		vertexfile << "Id," << "Community\n";
		for (int i=0; i<ratings.size(); i++) {
			for (list<Member>::iterator it=univ[ratings[i]].community_members.begin(); 
      				it!=univ[ratings[i]].community_members.end(); it++) {
				vertexfile << it->id() << "," << ratings[i] << "\n";
			}
		}		
		vertexfile.close();  	
  } else {
    cerr << "Error opening CSV output file." << endl;
    exit(1);
  }

  int parent;
  ofstream edgefile(filename + "_edges_" + r + ".csv");
  if (edgefile.is_open()) {
  	edgefile << "Source,Target\n";
  	for (int i=0; i<ratings.size(); i++) {
  		for (list<Member>::iterator it=univ[ratings[i]].community_members.begin(); 
      				it!=univ[ratings[i]].community_members.end(); it++) {
  			if (it->id() != ratings[i]) {
  				parent = (int) abs(av[it->id()]);
  				edgefile << it->id() << "," << parent << "\n";
  			}
  		}
  		
  		for (list<Member>::iterator it=univ[ratings[i]].community_neighs.begin(); 
      				it!=univ[ratings[i]].community_neighs.end(); it++) {
  			if (check_member(univ, ratings, ratings[i], it->id()))
  				edgefile << it->id() << "," << ratings[i] << "\n";
  		}  		
  	}
  	edgefile.close();
  } else {
  	cerr << "Error opening CSV output file." << endl;
  	exit(1);
  }

  cout << "Converting to CSV ... End" << endl;
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
  cout << "Number of edges: " << m << "\n\n";
  cout << "Total time: " << total_time << "\n";
  cout << "Max Q: " << Q << "\n" << endl;

  vector<int> ratings = community_ranking(univ, 8);
  // communities_to_csv(univ, ratings, arrv, filename);

 	exit(0);
}
