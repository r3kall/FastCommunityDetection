# FastCommunityDetection
Finding community structure in very large networks, with fast heuristic algorithm by Girvan-Newman and a Multi-Step variation.

## How to use
```C++
make
./fcd dataset scope    [example: ./fcd amazon0505 24]
```
## Input Format
The input should be a list of edges, separated by tab or space.
The graph can be either directed or undirected, but the algorithm will build an undirected version of it.
```
1 2
1 3
2 4
ecc...
```

## References
[1] Clauset et al. (2004) https://arxiv.org/abs/cond-mat/0408187

[2] Schuetz and Amedeo (2008) https://arxiv.org/abs/0712.1163
