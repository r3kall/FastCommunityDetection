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
#include <sstream>
#include <fstream>
#include <cmath>
#include <ctime>

#include "fcd.h"


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


int maximal(string filename) {
  int i, j;
  int maximal = -1;
  ifstream myfile(filename);
  if (myfile.is_open()) {
    while (myfile >> i >> j)
      maximal = max(maximal, max(i, j));
    myfile.close();
  } else {
    cerr << "Error opening input file\n";
    exit(1);
  }
  return maximal;
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
void process_dataset (string filename, vector<Community>& univ, int m) {  
  cout << "Start process dataset...\n";
  ofstream outfile(filename + "_preprocess.txt");
  if (outfile.is_open()) {
    outfile << univ.size() << "\n";
    outfile << m << "\n";
    for (int x=0; x<univ.size(); x++) {
      outfile << univ[x].id << "\n";      
      for (auto it=univ[x].clist.begin(); it!=univ[x].clist.end(); ++it)
        outfile << it->k << " ";
      outfile << "\n";
    }
    outfile.close();
  } else {
    cerr << "Error opening output file.\n";
    exit(1);
  }
  cout << "Finish process dataset.\n";
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
    cout << " It will be done a new one !!\n";
    
    int i, j;
    int max_id = maximal(filename); // search for maximal id
    univ.resize(max_id+1);  // resize community universe
    ifstream myfile(filename);
    if (myfile.is_open()) {
      while (myfile >> i >> j) {
        if (i == j)
          continue;
        univ[i].add(j, 0);
        univ[j].add(i, 0);
      }
      myfile.close();
    } else {
      cerr << "Error opening input file\n";
      exit(1);
    }

    // id correction
    cout << "Assigning IDs...\n";
    for (int id=0; id<univ.size(); id++)
      univ[id].id = id;

    // edge count
    int edges = 0;
    cout << "Counting number of edges...\n";
    for (int c=0; c<univ.size(); c++)
      edges += univ[c].clist.size();
    edges /= 2;

    cout << "Saving dataset...\n";
    process_dataset(filename, univ, edges);

    return edges;
  }

  // read the preprocessed file
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
      univ[x].id = x;  // set id

      getline(prefile, line);
      iss = istringstream(line);
      while (iss >> y)
        univ[x].add(y,0);  // add neighbors
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
double cdq (int ki, int kj, int m) {
  return 2.0*(double(0.5/m) - (ki * kj)/(4.0*m*m));
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
int init_universe (vector<Community>& univ, string filename) {
#ifdef DEBUG
  clock_t begin = clock();
#endif
  int m = read_data_set(filename, univ);  
  for (int i=0; i<univ.size(); i++)
    for (auto it=univ[i].clist.begin(); it!=univ[i].clist.end(); ++it)
      it->dq = cdq(univ[i].clist.size(), univ[it->k].clist.size(), m);

#ifdef DEBUG 
  clock_t end = clock();
  double elapsed = double(end - begin) / CLOCKS_PER_SEC;
  cout << "Time to populate community universe: " << elapsed << " seconds" << endl;
#endif
  return m;
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
void init_array (vector<double>& av, vector<Community>& univ, int m) {
#ifdef DEBUG
  clock_t begin = clock();
#endif
  double k = (double)(0.5/m);
  for (int i=0; i<univ.size(); i++)
    av.push_back(univ[i].clist.size() * k);

#ifdef DEBUG
  clock_t end = clock();
  double elapsed = double(end - begin) / CLOCKS_PER_SEC;
  cout << "Time to populate double array: " << elapsed << " seconds" << endl;
#endif
}


/* [header] function:  populate_heap
 * ----------------------------------------------------------------------------
 * Header function that initialized the max-heap of pairs.
 *
 * Args:
 *    - univ: non-empty community vector.
 *    - av:   non-empty double vector.
 *
 * Returns: a priority queue (max-heap) of pairs ordered by delta Q.
 */
void init_heap (MaxHeap& h, vector<Community>& univ, vector<double>& av) {  
#ifdef DEBUG
  clock_t begin = clock();
#endif
  for (int i=0; i<univ.size(); i++) {
    univ[i].cmax = NULL;
    if (av[i] > 0 && univ[i].scan_max(av))
      h.push(i, univ[i].cmax->k, univ[i].cmax->dq, 0);
  }
#ifdef DEBUG
  clock_t end = clock();
  double elapsed = double(end - begin) / CLOCKS_PER_SEC;
  cout << "Time to populate max-heap: " << elapsed << " seconds ";
  cout << "with " << h.size() << " elements\n";
#endif
}


/* [] function:  init_q
 * ----------------------------------------------------------------------------
 * Initialize Q.
 *
 * Args:
 *    - av: non-empty double vector.
 *
 * Returns: starting value of Q.
 */
double init_Q (vector<double>& av) {
  double Q = 0.0;
  for (int i=0; i<av.size(); i++) {
    Q += -2.0*av[i]*av[i];
  }
  return Q;
}


double modularity(string filename, vector<Community>& univ, vector<double>& av) {
  vector<Community> origin;  
  int m = read_data_set(filename, origin);
  vector<int> owners(univ.size(), -1);
  for (int i=0; i<univ.size(); i++) {
    if (av[i]>0 && univ[i].members()>0) {
      owners[i]=i;
      for (auto it=univ[i].cmembers.begin(); it!=univ[i].cmembers.end(); ++it)
        owners[it->k] = i;
    }
  }

  vector<int> internal(univ.size(), 0);
  vector<int> external(univ.size(), 0);
  for (int i=0; i<univ.size(); i++)
    if (av[i] > 0 && univ[i].members()>0)
      for (auto it=univ[i].cmembers.begin(); it!=univ[i].cmembers.end(); ++it) {
        external[i] += origin[it->k].degree();
        for (auto jt=origin[it->k].clist.begin(); jt!=origin[it->k].clist.end(); ++jt)
          if (owners[jt->k] == i && it->k<jt->k) internal[i]++;
      }

  double mod = 0.0;
  for (int i=0; i<univ.size(); i++)
    if (av[i] > 0 && univ[i].members()>0)
      mod += 2.0*((internal[i]/double(2*m)) - pow( av[i], 2.0 ));

  return mod;
}


bool convergence(vector<Community>& univ, vector<double>& av, MaxHeap& h) {
  for (auto&& c:univ) c.last = 0;
  h.clear();
  init_heap(h, univ, av);
  return h.empty();
}


void fill(vector<Community>& univ, vector<double>& av) {
  for (int v=0; v<univ.size(); v++) {
    if (av[v]>0 && univ[v].degree()>0)
      univ[v].cmembers.emplace(univ[v].cmembers.begin(), v, 0);
    if (univ[v].members()>1)
      univ[v].cmembers.sort();
  }
}


void shrink_all(vector<Community>& univ) {
  for (auto&& c:univ)
    c.clist.clear();
}


bool validity(Community& a, Community& b, int relative, vector<double>& av, MaxHeap& h, int& actual) {
  if (av[a.id] <= 0) return false;

  if (av[b.id] <= 0) {
    if (a.scan_max(av))
      h.push(a.id, a.cmax->k, a.cmax->dq, ++actual);
    a.last = actual;
    return false;
  }

  if (a.last != relative) return false;
  return true;
}


void merge(Community& a, Community& b, vector<double>& av, MaxHeap& h, int iter) {
  if (a.size() >= b.size()) {
    b.remove(a.id);
    a.merge(b, av);
    a.cmembers.splice(a.cmembers.end(), b.cmembers);
    av[a.id] += av[b.id];
    av[b.id] = -(a.id);
    if (a.scan_max(av))
      h.push(a.id, a.cmax->k, a.cmax->dq, iter);
    a.last = iter;
  } else {
    a.remove(b.id);
    b.merge(a, av);
    b.cmembers.splice(b.cmembers.end(), a.cmembers);
    av[b.id] += av[a.id];
    av[a.id] = -(b.id);
    if (b.scan_max(av))
      h.push(b.id, b.cmax->k, b.cmax->dq, iter);
    b.last = iter;
  }
}


pair<double, double> cnm (double Q,
                          vector<Community>& univ, 
                          vector<double>& av, 
                          MaxHeap& heap) {
  
  int x, y, rel;
  double elapsed;
  clock_t begin, end;

  int iter=0;
  double sQ = Q;
  clock_t begin_total = clock();
  do {    
    while (!heap.empty()) {
      // pop candidates communities
      heap.pop(x, y, rel);
      // check validity
      if (!validity(univ[x], univ[y], rel, av, heap, iter)) continue;

#ifdef DEBUG
      begin = clock();
#endif
      // update iteration
      iter++;
      // update Q
      sQ += univ[x].cmax->dq;
      // start merge sequence
      merge(univ[x], univ[y], av, heap, iter);

#ifdef DEBUG
      end = clock();
      elapsed = double(end - begin) / CLOCKS_PER_SEC;
      // cout << "x: " << x << " y: " << y << " time: " << elapsed << endl;
      if (iter%100000 == 0) {
        cout<< "iter: "<< iter;
        cout<< "  time elapsed: "<< double(clock()-begin_total)/CLOCKS_PER_SEC;
        cout<< "  partial Q: "<< sQ<< "\n";         
      }
#endif

    }  // end first while loop    
  } while (!convergence(univ, av, heap));

  clock_t end_total = clock();
  double elapsed_total = double(end_total - begin_total) / CLOCKS_PER_SEC;
  return make_pair(elapsed_total, sQ);
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
 *    - chunk_size: trivial.
 *
 * Returns: tuple of <total_time, Q>
 */
pair<double, double> cnm2 (double Q,
                           vector<Community>& univ, 
                           vector<double>& av, 
                           MaxHeap& heap,
                           int l_scope) {
  
	int x, y, l, rel;
  double elapsed;
  clock_t begin, end;

  int iter=0;
  double sQ = Q;
  vector<bool> touched(univ.size(), false);
  vector<pair<int,int>> candidates;

  clock_t begin_total = clock();
  do {
    while (!heap.empty() || candidates.size()>0) {
      l=0;
      while (!heap.empty() && l<l_scope) {
        heap.pop(x, y, rel);           
        if (validity(univ[x], univ[y], rel, av, heap, iter)) {
          candidates.push_back(make_pair(x,y));
          l++;
        }
      }  // end second while loop

      for (int i=0; i<candidates.size(); i++) {
        tie(x,y) = candidates[i];
        if (touched[x] || touched[y]) continue;

        touched[x] = true;
        touched[y] = true;
        
  #ifdef DEBUG
        begin = clock();
  #endif
        iter++;
        sQ += univ[x].cmax->dq;
        merge(univ[x], univ[y], av, heap, iter);
                    
  #ifdef DEBUG
        end = clock();
        elapsed = double(end - begin) / CLOCKS_PER_SEC;
        // cout << "x: " << x << " y: " << y << " time: " << elapsed << endl;
        if (iter%100000 == 0) {
          cout<< "iter: "<< iter;
          cout<< "  time elapsed: "<< double(clock()-begin_total)/CLOCKS_PER_SEC;
          cout<< "  partial Q: "<< sQ<< "\n";      
        }
  #endif
      }  // end first for loop

      for (int i=0; i<candidates.size(); i++) {
        tie(x,y) = candidates[i];
        touched[x] = false;
        touched[y] = false;
      }  // end second for loop
      candidates.clear();  // remove all candidates
    }  // end first while loop
    l_scope = 2;
  } while (!convergence(univ, av, heap));

  //if (!convergence(univ, av, heap))
    //tie(ignore, sQ) = cnm(sQ, univ, av, heap);

  clock_t end_total = clock();
  double elapsed_total = double(end_total - begin_total) / CLOCKS_PER_SEC;
  return make_pair(elapsed_total, sQ);
}
