# CS 180 Machine Problem 1

This is our CS 180 Artificial Intelligence Machine Problem 1 which is all about Robot path finding using Depth-First Search, Breadth-First Search, and A* Search implemented in C.

* Programmed by: jolotan416 and TigerTopher
* CS 180 THV
* Submitted to: Professor Raquel
* This file is formatted using a markdown language.

## Important Notes:
* Input is found in input.txt . Be sure to create it or else the program won't run. Below is an example of an input.txt file.
```
(0,0)
(199,399)
(0,0),(10,0),(10,5),(5,10),(0,5)
(50,50),(50, 60),(60,60),(60,50)
```
(0,0) is the initial state.
(199,399) is the goal state.
The third line gives the 5 vertices of a polygon/obstacle.
The fourth line gives the 4 vertices of another polygon/obstacle.

* Polygons as input is assumed to be simple, convex polygons only, not complex, concave polygons.

* For old C compilers. compile C file using command `$ gcc main.c -lm -o main.exe`. This includes `#include<math.h>` library. This is needed since I used its floor and ceiling function.

* Size is assumed to be 200x400.

* Smart-input is assumed

* Just change `#define` values to change size and more.

## Data Structure Documentation:
### Why Not Implement Doubly-Linked List (abbr. DL / DL-L)?
---
#### Pros:
  * ** Convenience for Fringe Representation: ** DL-L can represent both Stack and Queue. Moreover, only one allocation function and pointer type are needed.

  * Same isEmpty() Function. Empty if Front and Back points the same node

#### Cons:
  * ** Performance Issue:** It is more efficient to split the Data Structure into a Stack and a Queue, since it will omit the unnecessary "prev" pointer. This saves us 8 bytes for every node!

### Queue:
---
* Insert (Enqueue) : Insert sa may front
* Remove (Dequeue) : Remove sa may back
* checkNext : Check node napinopoint ni front

Queue Functions:

1. `void enqueue(NODE_POINTER* FRONT, NODE_POINTER* REAR, int x1, int y1);`
  * Sample function call: `enqueue(&Q1_FRONT, &Q1_REAR, input1, input2);`
2. `int dequeue(NODE_POINTER* FRONT, NODE_POINTER* REAR);`
  * The x and y values of the dequeued node are placed in global variables variable1 and variable2.
  * Sample function call:
  `dequeue(&Q1_FRONT, &Q1_REAR);`
3. `int checkFront(NODE_POINTER FRONT);`
  * Returns 0 if FRONT points to NULL, 1 if otherwise.
  * If 1, Front x,y values are placed in global variables variable1 and variable2.
  * Sample function call:
  `checkFront(Q1_FRONT);`
4. `int checkRear(NODE_POINTER REAR);`
  * Returns 0 if REAR points to NULL, 1 if otherwise.
  * If 1, Rear x,y values are placed in global variables variable1 and variable2.
  * Sample function call:
  `checkRear(Q1_REAR);`
5. `int isEmptyQueue(NODE_POINTER* FRONT, NODE_POINTER* REAR);`
  * Returns 1 if Empty, 0 if otherwise.
  * Sample function call:
  `isEmptyQueue(&Q1_REAR, &Q1_FRONT)`


### Stack:
---
* Insert (Push) : Insert sa may front (top)
* Remove (Pop) : Remove sa may front (top)
* checkNext: Check node napinopoint ni front

Stack Functions:

1. `void push(NODE_POINTER* TOP, int x1, int y1);`
  * If 0, this means current pointer points to NULL.
  * If 1, popped or checkTop values can be found in global variable1 and variable2
  * Sample function call:
  `push(&S1_TOP, input1, input2);`
2. `int pop(NODE_POINTER* TOP);`
  * Returns 0 if there is nothing to pop.
  * Returns 1 if otherwise. Popped x,y values are placed in global variables variable1 and variable2.
  * Sample function call:
  `pop(&S1_TOP);`
3. `int checkTop(NODE_POINTER TOP);`
  * Return 0 if TOP points to NULL
  * Returns 1 if otherwise. Top x,y values are placed in global variables variable1 and variable2.
  * Sample function call:
  `checkTop(S1_TOP);`
4. `int isEmptyStack(NODE_POINTER* TOP);`
  * Returns 1 if Empty, 0 if otherwise.
  * Sample function call:
  `isEmptyStack(&S1_TOP)`

### Convention for Fringe:
Depth-First Search
---
Reason: Eliminate unnecessary clean up.
Initially, num_runs is 0. +1 after finishing dfs, bfs, a* operation.

0 or any numbers not in this list -> Unexplored Node
1 -> Obstacle
2 -> Initial state
3 -> Goal State
(4*num_runs) + 4 -> From down
(4*num_runs) + 5 -> From Up
(4*num_runs) + 6 -> From Right
(4*num_runs) + 7 -> From Left
