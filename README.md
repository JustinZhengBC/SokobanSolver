# SokobanSolver
Uses basic search algorithms to solve sokoban puzzles

# Usage
![screenshot](/screenshot.png)
The buttons mostly do what they say. `<<` rewinds to the start of the puzzles while `>>` jumps to the end. Right-click can be used to erase tiles in the editor. The player may jump around when viewing the solution one step at a time - this is because of how the search problem is formulated (more below). Long waits can be expected when solving problems with many box-target pairs.

# Building
Using the most recent version of Qt and Qt Creator, open ``SokobanSolver.pro`` and it should build without problems.

# Algorithms and Implementation
A*, lowest-cost-first search, depth-first search, and breadth-first search have been implemented. A* is recommended, as reducing search time relies heavily on the strict heuristic to quickly filter out unsolvable states. However, even then, the number of possible states is exponential in the number of box-target pairs, so problems with many boxes may take a long time to solve.

To reduce the search space, the neighbours of a state are generated not by looking at how the player can move, but directly at which boxes can be pushed. For pruning purposes, non-cost-based algorithms consider two states identical if the boxes are in the same positions and the player positions are mutually reachable without moving any boxes, while cost-based algorithms consider two states identical if the above conditions are met and, in addition, the state with the lower cost-so-far can reach the other state without exceeding its cost, in which case the higher-cost state is pruned.

The most important and time-saving role of the heuristic is to identify unsolvable states. At the beginning, it identifies edges such that once a box is pushed against it, it cannot be pushed away. It also checks each proposed state for situations where boxes have been pushed against each other and block each other from moving. More detailed explanations can be found in the comments in `levelformat.cpp`.
