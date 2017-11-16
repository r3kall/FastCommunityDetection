/* ============================================================================
 * fcd.cpp
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

#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <ctime>

#include "fcd.h"


Pair::Pair () {
  cty = -1;
  mbr = -1;
  pdq = 0;
}

Pair::Pair (int i, int j, double q) {
  cty = i;
  mbr = j;
  pdq = q;
}


/* [header] function:  fileExists
 * ----------------------------------------------------------------------------
 * Check if a file exists.
 * Args:
 *    - file: filename
 * Returns: true if and only if the file exists, false else.
 */
bool fileExists (const string& file) {
  struct stat buf;
  return (stat(file.c_str(), &buf) == 0);
}


/* [] function:  process_dataset
 * ----------------------------------------------------------------------------
 * Write community vector in a text file in a proper way.
 * Args:
 *    - filename: trivial.
 *    - univ:     community vector.
 *    - m:        number of edges.
 *
 * Returns: 0 if success, 1 otherwise.
 */
int process_dataset (string filename, vector<Community>& univ, int m) {  
  cout << "Start process dataset..." << endl;
  ofstream outfile(filename + "_preprocess.txt");
  if (outfile.is_open()) {
    outfile << univ.size() << "\n";
    outfile << m << "\n";
    for (int x=0; x<univ.size(); x++) {
      outfile << univ[x].community_id << "\n";
      for (list<Member>::iterator it=univ[x].community_neighs.begin(); 
          it!=univ[x].community_neighs.end(); it++) {
        outfile << it->id() << " ";
      }
      outfile << "\n";
    }
    outfile.close();
  } else {
    cerr << "Error opening output file." << endl;
    return 1;
  }
  cout << "Finish process dataset." << endl;
  return 0;
}


/* [] function:  read_data_set
 * ----------------------------------------------------------------------------
 * Given a formatted text file representing the graph, it builds the correspon-
 * ding representation with the community invariants.
 *
 * Args:
 *    - filename: trivial.
 *    - univ: empty vector to be initialized.
 *
 * Returns: number of edges in the graph.
 */
int read_data_set (string filename, vector<Community>& univ) {

  // name of the preprocessed file.
  string outfilename = filename + "_preprocess.txt";
  if (!fileExists(outfilename)) {
    cout << "Preprocessed version of file " << filename << " doesn't exists.";
    cout << " It will be done a new one !!" << endl;
    int i, j, rs;
    ifstream myfile(filename);

    if (myfile.is_open()) {
      int iter = 0;
      while (myfile >> i >> j) {
        if (iter % 1000000 == 0) cout << "step: " << iter << endl;
        if (i == j)
          continue;

        if ( i >= univ.size() || j >= univ.size()) {
          rs = max(i, j) + 1;
          univ.resize(rs);
          cout << "Resize at step " << iter << " of " << rs << endl;
        }

        univ[i].community_id = i;
        if (!univ[i].contains(j)) 
          univ[i].add_member(*(new Member(j, 0)));

        univ[j].community_id = j;
        if (!univ[j].contains(i))
          univ[j].add_member(*(new Member(i, 0)));

        iter++;
      }
      myfile.close();
    } else {
      cout << "Error opening input file" << endl;
      exit(1);
    }

    // edge count
    int edges = 0;
    cout << "Counting number of edges..." << endl;
    for (int c=0; c<univ.size(); c++)
      edges += univ[c].community_degree;
    edges /= 2;    

    cout << "Saving dataset..." << endl;
    process_dataset(filename, univ, edges);

    return edges;
  }
 
  cout << "Preprocessed version of " << filename << " exists !!" << endl;
  // read the processed file  
  ifstream prefile(outfilename);
  string line;
  int n, m, x, y;
  if (prefile.is_open()) {
    // get number of nodes
    getline(prefile, line);
    istringstream iss(line);
    iss >> n;

    // get number of edges
    getline(prefile, line);
    iss = istringstream(line);
    iss >> m;

    // resize community vector
    univ.resize(n);

    while (getline(prefile, line)) {
      iss = istringstream(line);
      iss >> x;
      univ[x].community_id = x;
      // TODO: posso rimuovere nel file l'id, sono ordinati.

      getline(prefile, line);
      iss = istringstream(line);
      while (iss >> y) {
        // add to community x
        univ[x].add_member(*(new Member(y, 0)));
      }
    }
    prefile.close();
  }

  return m;
}


/* [] function:  compute_dq
 * ----------------------------------------------------------------------------
 * Compute the starter value of delta Q.
 *
 * Args:
 *    - ki: degree of community i.
 *    - kj: degree of community j.
 *    - m : number of edges.
 *
 * Returns: the computed delta Q.
 */
double c_dq (int ki, int kj, int m) {
  return 2.0*(1.0/(2.0*m) - (ki * kj)/(4.0*m*m));
}


/* [header] function:  populate_universe
 * ----------------------------------------------------------------------------
 * Exposed function that initialized the community universe/vector.
 *
 * Args:
 *    - filename: dataset filename.
 *
 * Returns: the community vector and the number of edges.
 */
pair<vector<Community>, int> populate_universe (string filename) {

  int j, m;
  double cdq;
  vector<Community> univ;
  clock_t begin = clock();

  m = read_data_set(filename, univ);
  
  for (int i=0; i<univ.size(); i++) {
    for (list<Member>::iterator it=univ[i].community_neighs.begin(); 
        it!=univ[i].community_neighs.end(); it++) {
      j = (*it).id();
      cdq = c_dq( univ[i].community_degree, univ[j].community_degree, m );
      (*it).setdq(cdq);
    }
    univ[i].sort_pairs();
    univ[i].community_members.push_back(*(new Member(i, 0)));
  }
  
  clock_t end = clock();
  double elapsed = double(end - begin) / CLOCKS_PER_SEC;
  cout << "Time to populate community universe: " << elapsed << " seconds" << endl;

  return make_pair(univ, m);
}


/* [header] function:  populate_array
 * ----------------------------------------------------------------------------
 * Header function that initialized the double vector.
 *
 * Args:
 *    - univ: non-empty community vector.
 *    - m:    number of edges.
 *
 * Returns: the double vector.
 */
vector<double> populate_array (vector<Community>& univ, int m) {

  clock_t begin = clock();
  vector<double> av;
  double k = (double)(0.5/m);
  for (int i=0; i<univ.size(); i++)
    av.push_back( univ[i].community_degree * k );
  clock_t end = clock();
  double elapsed = double(end - begin) / CLOCKS_PER_SEC;
  cout << "Time to populate double array: " << elapsed << " seconds" << endl;

  return av;
}


/* [header] function:  populate_heap
 * ----------------------------------------------------------------------------
 * Header function that initialized the max-heap of pairs.
 *
 * Args:
 *    - univ: non-empty community vector.
 *    - av:   non-empty double vector.
 *
 * Returns: a priority queue (max-heap) of Pairs ordered by delta Q.
 */
priority_queue<Pair> populate_heap (vector<Community>& univ, vector<double>& av) {
  
  clock_t begin = clock();
  priority_queue<Pair> pq;
  double ratio;

  for (int i=0; i<univ.size(); i++) {
    univ[i].scan_max(av);
    if (univ[i].community_max != NULL) {
      pq.push(*(new Pair(univ[i].community_id, 
        univ[i].community_max->id(), univ[i].community_max->dq())));
    }
  }
  clock_t end = clock();
  double elapsed = double(end - begin) / CLOCKS_PER_SEC;
  cout << "Time to populate max-heap: " << elapsed << " seconds" << endl;
  
  return pq;
}


/* [header] function:  merge_communities
 * ----------------------------------------------------------------------------
 * Header function that merge community b into a.
 *
 * Args:
 *    - a:  community that will be merged.
 *    - b:  community to merge into the other.
 *    - av: non-empty double vector.
 */
void merge_communities (Community& a, Community& b, vector<double>& av) {

    // remove community a from the neighbor list of b.
    b.remove_element(a.community_id);    

    // merge community neighborhood b into community a.
    a.c_union(b, av);

    // merge members lists.
    a.m_union(b);

    // update degree.
    a.community_degree = a.community_neighs.size();
    b.community_degree = 0;

    // update sizes.
    a.community_size = a.community_members.size();
    b.community_size = 0;

    // update av.
    av[a.community_id] = av[a.community_id] + av[b.community_id];
    av[b.community_id] = - a.community_id;
}


/* [] function:  init_q
 * ----------------------------------------------------------------------------
 * Initialize Q.
 *
 * Args:
 *    - av: non-empty double vector.
 *    - m : number of edges.
 */
double init_Q (vector<double>& av, int m) {
  double Q = 0.0;
  double k = (double)(0.5/m);
  for (int i=0; i<av.size(); i++) {
    Q -= av[i] * av[i];
  }
  return Q;
}


double fcd_low_degree (vector<Community>& univ, vector<double>& av) {

  int m_id;
  double low_dQ = 0;
  for (int i=0; i<univ.size(); i++) {
    if (univ[i].community_degree == 1) {
      if (univ[i].community_max != NULL) {
        m_id = univ[i].community_max->id();
        if (av[m_id] > 0) {
          low_dQ += univ[i].community_max->dq();
          merge_communities(univ[m_id], univ[i], av);
        }
      }
    }
  }
  return low_dQ;
}


/* [header] function:  fcd
 * ----------------------------------------------------------------------------
 * Header function that run the fast community detection algorithm.
 *
 * Args:
 *    - univ: community vector.
 *    - av  : double vector.
 *    - heap: max-heap.
 *    - m   : number of edges.
 *
 * Returns: tuple of <chunk_size, total_time, Q>
 */
tuple<int, double, double> fcd (vector<Community>& univ, 
        vector<double>& av, priority_queue<Pair>& heap, int m) {
  
	Pair p;
	int x, y, sm;
  double elapsed;
  tuple<int, double, double> res_tuple;

  double Q = init_Q(av, m);
  int c = 0;

  // prune one-degree communities
  Q += fcd_low_degree(univ, av);

  queue<Pair> queue;
  int chunck_size      = 64;
  double epsilon       = 1 / double(2*m);  
  double max_epsilon   = 2 / double(univ.size());
  double epsilon_coeff = (max_epsilon / epsilon) / double(univ.size());
  int max_queue_size   = min(int(univ.size()/2), chunck_size);

  clock_t begin_total = clock();
	while (!heap.empty() || !queue.empty()) {

    // if heap not empty
    while (!heap.empty()) {
      p = heap.top();
      x = p.community_i();
      y = p.community_j();
      heap.pop();

      if (av[x] <= 0) continue;

      if (av[y] <= 0) {
        // scan max for community x
        sm = univ[x].scan_max(av);
        // if valid max
        if (sm > 0) {
          heap.push(*(new Pair(univ[x].community_id, 
                               univ[x].community_max->id(), 
                               univ[x].community_max->dq()
                              )));
        }       
        continue;   
      }

      if (queue.empty()) {
        queue.push(p);
        continue;
      }

      if (queue.size() >= max_queue_size) break;

      if (abs(p.dq() - queue.front().dq()) < epsilon) {
        queue.push(p);
        continue;
      }

      break;
    }

    if (queue.size() < max_queue_size && epsilon < max_epsilon)
      epsilon += epsilon_coeff;

    while (!queue.empty()) {  // start to process queue
      p = queue.front();
      x = p.community_i();
      y = p.community_j();
      queue.pop();

      if (av[x] <= 0) continue;

      if (av[y] <= 0) {
        // scan max for community x
        sm = univ[x].scan_max(av);
        // if valid max
        if (sm > 0) {
          heap.push(*(new Pair(univ[x].community_id, 
                               univ[x].community_max->id(), 
                               univ[x].community_max->dq()
                              )));
        }       
        continue;
      }
      
      clock_t begin = clock();

      // update Q
      Q += univ[x].community_max->dq();

      // if x and y are valid communities, merge them
      merge_communities(univ[x], univ[y], av);

      // update maximum delta Q member of community x
      sm = univ[x].scan_max(av);
      if (sm > 0) { 
        // insert new max in the heap
        heap.push(*(new Pair(univ[x].community_id, 
                             univ[x].community_max->id(), 
                             univ[x].community_max->dq()
                            )));
      }

      clock_t end = clock();
      elapsed = double(end - begin) / CLOCKS_PER_SEC;
      cout << "x: " << x << " y: " << y << " time: " << elapsed << endl;      
    }
    c++;
	}   

  clock_t end_total = clock();
  double elapsed_total = double(end_total - begin_total) / CLOCKS_PER_SEC;

  return make_tuple(chunck_size, elapsed_total, Q);
}