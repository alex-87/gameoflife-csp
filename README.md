Game of Life with Constraint Programming
========================================

This software is a mall implementation of the [Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) using [Gecode](https://www.gecode.org/). The aim of this software is to express the rules of the Game of Life **using constraint programming**.


Game of Life Rules
------------------

Considering a `N * N` board, each cell's value is in `{0, 1}`, 1 for `enabled`, 0 for `disabled` :

  - If a cell is disabled and has 2 or less neighbours, the cell stays disabled in the next iteration
  - If a cell is disabled and has exactly 3 neighbours, the cell will be enabled in the next iteration
  - If a cell is enabled and has 2 or 3 neighbours, the cell will stay enabled in the next iteration
  - If a cell is enabled and has more than 3 neighbours, the cell will be disabled in the next iteration
  - If a cell is enabled and has less then 2 neighbours, the cell will be disabled in the next iteration


Requirement
-----------

* GCC/G++ 7
* Gecode development library


Build
-----

To build the Game of Life, open a terminal and :

```shell
./build.sh
```

Usage
-----

```shell
bin/gol-gecode <square-root length> <rounds>
```


The input (example.txt) :

```
0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0
0 0 1 1 0 0 0 0 0 0
0 0 1 1 0 0 0 0 0 0
0 0 0 0 1 0 0 0 0 0
0 0 0 0 1 1 0 0 0 0
0 0 0 0 0 0 1 0 0 0
0 0 0 0 0 1 0 0 0 0
0 0 0 0 0 1 0 0 0 0
0 0 0 0 0 0 0 0 0 0
```

Example:

```shell
bin/gol-gecode 10 30 < example.txt
```


Licenses
--------

* This software is released under the MIT License.

* [Gecode](https://www.gecode.org/) is released under the license : [https://www.gecode.org/license.html](https://www.gecode.org/license.html).

