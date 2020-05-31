Very-Large-Graph
================

This README file describes succintly the program computing
bounds for the diameter massive graphs.

# About

This Document is the result of academic work at EPITA under **Robert ERRA**, 
**Alexandre LETOIS** & **Mark ANGOUSTURES**'s supervision (Subject 1).

In this document we describe an approach to compute a 'good' and fast approximation of very large graph's diameter,
radius & center basing ourselves on previous work and C code: *"Fast Computation of Empirically Tight Bounds for the Diameter of Massive Graphs"* by **Clémence MAGNIEN**, **Matthieu LATAPY** & **Michel HABIB**.

This project is the result of the
- **Tom MECHINEAU** (tom.mechineau@epita.fr)
- **Geoffrey BOSSUT** (geoffrey.bossut@epita.fr)

## Modifications

The original code can be spotted directly since it is contained in files prefixed by **"magnien_"**, our code is however prefixed by **"vlg_"**.
To spot modifications made to the original code, search for **"MOD:"** in the code itself.
The **"src/diam.c"** file is however a modified version of the original.

# Quickstart

1. Compile the program:
```
 make
```
2. Run it:
```
 ./diam -center nb_iterations check_centers < data_file
```
  for computing diameter bounds with the radius and center nodes for "nb_iterations" iterations using the **multisweep** bruteforce.

or

```
 ./diam -centerconv nb_iterations < data_file
```
  for computing diameter bounds with the radius and center nodes for "nb_iterations" iterations using the leafs **convergence** method.

# Input Format

The program reads plain text ; the first line must be the number n of nodes;   
Then comes a series of lines of the form '**i j**' meaning  
that node '**i**' has degree '**j**', and then a series of lines of the  
form '**u v**' meaning that nodes '**u**' and '**v**' are linked together.  

There **must** be no duplicate lines, and '**u v**' also stands for '**v u**'.  

The nodes must be numbered from 0 to n-1. There must be no loop  
'**u u**'. The program makes basic verifications but may crash or give  
wrong answers if the input is incorrect.
Example :  
```
3  
0 2  
1 2  
2 2  
0 1  
0 2  
2 1  
```
(3 nodes, thus numbered from 0 to 2, node 0 has degree 2, node 1
has degree 2, and node 2 has degree 2 too, and the links are 0 1,
0 2 and 2 1)


# Command Line Options 

## Save giant connected component

The program includes two options of computing and saving the giant component of a graph:
```
./diam -savegiant fpath
```
This one saves the giant component to the specified file, reordering using array and index swapping.

```
./diam -savegiantbfs fpath
```
This one saves the giant component to the specified file, reordering using 2 same BFS.

## Center/Radius/Diameter Approximation

Two different options are at your choosing in order to calculate
the center, the radius and the diameter bounds approximated.
Keep in mind these are approximations but tends to get close to the truth
the more iterations there are.

The first option is a bruteforce multisweep, it first picks a random node
from the giant component of the graph. Then to apply a BFS on top of it in order
to get a possible diametrical node. 
The work involved behind the curtain is explained in the paper accompanying
this code.
```
./diam -center nb_iterations check_centers
```
Here the check_centers parameter is to specify if the program should perform a bfs on every found center nodes (check_centers!=0) at the end of the iterations or only perform on one on a random nodes to approximate better bounds/radius(check_centers==0).  
It's preferable to use check_centers at 0 when there are a lot of center nodes found.

The second option is a leaf convergence method, which is faster in most cases.
The exact implementation is detailled in the report.
```
./diam -centerconv nb_iterations
```

## Magnien Diameter Approximation

The standard way of using the program is to compute both upper and 
lower bounds for the diameter, until the difference between them is
lesser than a given value, or until a given number of iterations
have been run:

```
 ./diam -diam nb_max difference
```
  computes 'nb_max' iterations of both a double-sweep lower bound and
  a highest degree tree upper bound (see the paper for more details).
  If the difference between the best upper and lower bounds obtained
  is lesser than or equal to 'difference' at a given step before
  'nb_max' steps have been done, the program stops.

An alternative is to compute bounds for the diameter until the diameter
is estimated with a given relative error, or until a given number of
iterations have been run:
```
 ./diam -prec nb_max precision
```
  computes '**nb_max**' iterations of both a double-sweep lower bound and
  a highest degree tree upper bound (see the paper for more details).
  If the the best upper and lower bounds obtained are such that the
  diameter is estimated with a relative error of at most '**precision**'
  at a given step before '**nb_max**' steps have been done, the program
  stops.


Moreover, it is possible to compute a given number of iterations of
specific bounds (see the paper for more details on the different
bounds):
```
 ./diam -tlb|dslb|tub|rtub|hdtub nb [deg_begin]
  computes 'nb' iterations of one specific bound:
  -tlb: trivial lower bound
  -dslb: double-sweep lower bound
  -tub: trivial upper bound
  -rtub: random tree upper bound
  -hdtub: highest degree tree upper bound (this last option requires
    an additional parameter deg_begin, which skips nodes with degrees
    greater than deg_begin. 0 means start with the highest degree
    node).
```

# Output

## Center/Radius/Diameter Approximation

The program writes the results on the standard output. Thus,
if you want to save them you should redirect it, using
typically:
```
./diam -center 20 1 < data_file > result_file
```
Both -center and -centerconv also writes informations on the standard error output.

The output format consists of multiple lines per iteration, providing
the following informations:
 - iteration number
 - number of node chosen for computing the bound
 - value of the obtained bound
 - value of an approximated radius.

At the end of the program, will be displayed:
 - center nodes found
 - optimal diameter bounds found
 - approximated diameter
 - approximated optimal radius
 - number of BFS done

## Magnien Diameter Approximation

The program writes the results on the standard output. Thus,
if you want to save them you should redirect it, using
typically:
```
./diam -diam 100 0 < data_file > result_file
```
Moreover, the program writes some information on what it is
doing on the standard error output. You may want to discard
this or redirect it to a file.

The output format consists of one line per iteration, providing
the following informations:
 - iteration number
 - number of node chosen for computing the bound
 - degree of this node
 - value of the obtained bound.

Additionnally, for the **-diam** option, the values of the best bounds
obtained so far are provided.

The first line of the output specifies which column provides which
information.

# Portability

The program is written in ANSI C, with standard libraries.

# Special thanks
Epita teachers/researchers: Robert  ERRA, Alexandre LETOIS & Mark ANGOUSTURES
Fast Computation of Empirically Tight Bounds
  for the Diameter of Massive Graphs
Clémence Magnien, Matthieu Latapy, and Michel Habib,
ACM Journal of Experimental Algorithmics (JEA), 13, 2009.