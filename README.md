# Self-organizing tree models for image synthesis

This is a C++ implementation of the main ideas from [Wojciech Palubicki, Kipp
Horel, Steven Longay, Adam Runions, Brendan Lane, Radomír Měch, and Przemyslaw
Prusinkiewicz. 2009. Self-organizing tree models for image synthesis. In ACM
SIGGRAPH 2009 papers (SIGGRAPH '09), Hugues Hoppe (Ed.). ACM, New York, NY,
USA, Article 58](https://doi.org/10.1145/1576246.1531364).

## Paper overview

The paper is based on the assumption that the form of a developing tree emerges
from a self-organizing process dominated by the competition of buds and
branches for light or space, and regulated by internal signaling mechanisms.

The method integrates three elements of tree development: local control of
branching geometry, competition of buds and branches for space or light, and
regulation of this competition through an internal signaling mechanism.

### Overall structure

0. Seedling structure

0. Calculate local environment of buds
0. Determine bud fate
0. Append new shoots
0. Shed branches
0. Update branch width

0. Grown tree

### Terminology

The point at which one or more leaves are or were attached to a stem is a **node**.
The part of the stem between two nodes is an **internode**.
A **lateral bud** is created in the axil of each leaf.
An internode with attached leaf and bud forms a **metamer**.

A bud may have four different fates.

+ Produce a new metamer;
+ Produce a flower;
+ Remain dormant;
+ Abort;

Endogenous information flow may be _acropetal_ (from the base of the tree
towards its extremities), _basipetal_ (from the extremities to the base) or
bidirectional.

## Calculation of the environmental input

There are two methods for doing this assessment: **space colonization** and
**shadow propagation**.

### Space colonization

In each iteration, the process estimates the availability or quality of the
space surrounding each **bud** (a number Q), and the optimal direction of shoot
growth (a vector V).

Each bud is surrounded by a spherical occupancy zone of radius _ρ_ and has a
conical perception volume characterized by the perception angle θ and the
distance _r_. Typical values are θ ≈ 90◦, _ρ_ = 2 internode lengths, and _r_ =
4 to 6 internode lengths.

The space available for tree growth is represented by a set S of marker points
M. In the simplest case, these points are generated algorithmically, with
uniform distribution, at the beginning of the simulation. In each iteration of
the simulation, markers within the occupancy zone of any bud are deleted from
the set S. The buds then compete for the remaining points. A marker M within
the perception volume of a single bud A is associated with A. A marker M within
the perception volume of several buds is associated with the closest of these
buds. A bud A has space for growth (and Q = 1) if the set S(A) of markers
associated with A is nonempty; otherwise bud A has no available space (and Q =
0).

The optimal growth direction V is calculated as the normalized sum of the
normalized vectors towards all markers in S(A).

## Calculation of bud fate

This can be done through the **extended Borchert-Honda model** and the
**priority model**.

## Addition of new shoots

## Shedding of branches

## Calculation of branch diameter

