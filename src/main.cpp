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


void stats (vector<Community>& univ) {
	int n_of_communities     = 0;
	int max_community_size   = 0;
	int min_community_size   = univ.size();
	int total_community_size = 0;

	for (int c=0; c<univ.size(); c++) {
		if (univ[c].community_size > 0) {
			n_of_communities++;
    	max_community_size = max(max_community_size, (int)univ[c].community_size);
    	min_community_size = min(min_community_size, (int)univ[c].community_size);
    	total_community_size += univ[c].community_size;
  	}
	}

	double mean_community_size = total_community_size / double(n_of_communities);

	cout << "Number of Communities: " << n_of_communities << "\n";
	cout << "Max Community Size: " << max_community_size;
	cout << "\tMin Community Size: " << min_community_size;
	cout << "\tMean Community Size: " << mean_community_size << endl;
}

bool sortbysize (const pair<int,int>& a, const pair<int,int>& b) {
	return a.second > b.second;
}

vector<int> community_rating (vector<Community>& univ, int r) {
	vector< pair<int,int> > vect;
	for (int i=0; i<univ.size(); i++) {
		if (univ[i].community_size > 1) {
			vect.push_back( make_pair(univ[i].community_id, univ[i].community_size) );
		}
	}
	sort(vect.begin(), vect.end(), sortbysize);

	vector<int> ratings;
	for (int j=0; j<r; j++) {
		ratings.push_back(vect[j].first);
	}
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


void communities_to_csv (vector<Community>& univ, vector<int> ratings, string filename) {
	cout << "Converting to CSV ... Start" << endl;
  ofstream vertexfile(filename + "_vertices.csv");
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

  ofstream edgefile(filename + "_edges.csv");
  if (edgefile.is_open()) {
  	edgefile << "Source,Target\n";
  	for (int i=0; i<ratings.size(); i++) {
  		for (list<Member>::iterator it=univ[ratings[i]].community_members.begin(); 
      				it!=univ[ratings[i]].community_members.end(); it++) {
  			edgefile << it->id() << "," << ratings[i] << "\n";
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
	
	if (argc != 2) {
		cerr << "Extra Command Line Arguments Error: wrong number of arguments" << endl;
		exit(1);
	}

	istringstream ss(argv[1]);
	string filename;
	ss >> filename;

	if (!fileExists(filename)) {
		cerr << "Extra Command Line Arguments Error: file doesn't exists" << endl;
		exit(1);
	}

  tie(univ, m) = populate_universe(filename);
  arrv = populate_array(univ, m);
  heap = populate_heap(univ, arrv);

  int chunk_size;
  double total_time, Q;

  tie(chunk_size, total_time, Q) = fcd(univ, arrv, heap, m);

  cout << "\nNumber of vertices: " << univ.size() << "\n";
  cout << "Number of edges: " << m << "\n\n";
  cout << "Chunk size: " << chunk_size << "\n";
  cout << "Total time: " << total_time << "\n";
  cout << "Max Q: " << Q << "\n" << endl;

  stats(univ);
  vector<int> ratings = community_rating(univ, 8);

  communities_to_csv(univ, ratings, filename);

 	exit(0);
}