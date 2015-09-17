# CS 180 MP 1
* Programmed by: John Louise Tan and Christopher Ivan Vizcarra
* CS 180 THV
* Submitted to: Professor Carlo Raquel
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
