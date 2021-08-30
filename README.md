# ECE650 : 

The project is to help the local police department with their installation of security cameras at traffic intersections.

The idea is for the police to be able to minimize the number of cameras they need to install, and still be effective as possible with their monitoring.

The problem is a optimization problem known as vertex cover problem, the streets were described by undirect graph using vertices and edges; the shortest path between two vertex are find by BFS; IPC (fork-pipe) link the output of random input generator (C++) to the input of the python script, and the output of the python script to the input of the shorest path finder.

The problem can be reduced to solve the minimum vertex cover problem, implement CNF-SAT with a SAT solver(minisat) to solve this problem, compared with APPROX-VC-1 and APPROX-VC-2.

A vertex is defined as: 1. each intersection 2.  or the end-point of a line segment of a street that intersects with another street.

An edge between two vertices is defined as: 1. at least one of them is an intersection 2. both lie on the same street 3. one is reachable from the other without traversing another vertex
