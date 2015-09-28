#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<math.h>

/* John Louise Tan
  Christopher Vizcarra*/

/*=============== Macro Definition ===============*/
#define ARRAYSPACE_SMALL 100
#define ARRAYSPACE_BIG 80000
#define X_COOR_SIZE 400
#define Y_COOR_SIZE 200

/*=============== Typedefs and Structs =============== */

typedef struct LIST_noder{ // Generic List Data Structure
  int x;  // Data x
  int y;  // Data y
  struct LIST_noder *next; // Next
  float distance;
  int path_cost;
} LIST_NODE;

typedef struct{ // This serves as a stack pointer or queue pointer.
  LIST_NODE *node;} NODE_POINTER;

/*=============== Global Variables ===============*/
int initial[2];
int goal[2];
int vertices[ARRAYSPACE_SMALL][ARRAYSPACE_SMALL][2];
int polygonCount;                  // Number of obstacles per polygon    | I-2 count
int verticesSize[ARRAYSPACE_SMALL]; // Number of vertices per  polygon | L count
int obstacleCoordinates[X_COOR_SIZE][Y_COOR_SIZE];  // Value of 1 if there is an obstacle.

float temp_obstacle[ARRAYSPACE_BIG][2];
int temp_obstacle_count = 0;

int num_runs = 0;

NODE_POINTER front;
NODE_POINTER rear;
NODE_POINTER stack_top;
NODE_POINTER AStar_top;

int variable1;
int variable2;
int variable3;
int successors[4];

FILE* output_file;

/*=============== Function Prototyping =============== */
// Data Structure Functions are located below main function :)
LIST_NODE* newNode(int x1, int y1);

// For Stack:
void push(NODE_POINTER* TOP, int x1, int y1);
// POP and CHECKTOP returns 1 or 0.
// If 0, this means current pointer points to NULL.
// If 1, popped or checkTop values can be found in global variable1 and variable2
int pop(NODE_POINTER* TOP);  // Output values are placed in global variables variable1 and variable2.
int checkTop(NODE_POINTER TOP); // Output values are placed in global variables variable1 and variable2.
int isEmptyStack(NODE_POINTER* TOP);


// This is temporary variable that can handle floats in building the obstacle.
// I would put x-y pairs here

// For Queue:
void enqueue(NODE_POINTER* FRONT, NODE_POINTER* REAR, int x1, int y1);
int dequeue(NODE_POINTER* FRONT, NODE_POINTER* REAR); // Output values are placed in global variables variable1 and variable2.
int checkFront(NODE_POINTER FRONT); // Output values are placed in global variables variable1 and variable2.
int checkRear(NODE_POINTER REAR); // Output values are placed in global variables variable1 and variable2.
int isEmptyQueue(NODE_POINTER* FRONT, NODE_POINTER* REAR);

void insert_AStar(NODE_POINTER* TOP, int x1, int y1, int path_cost);

void successor_AStar(int x, int y, int path_cost);

int AStar();

void printPath(){
  int i;
  int j;
  int curr_x;
  int curr_y;
  FILE* file;

  for(j = Y_COOR_SIZE - 1; j >= 0; j--){
    for(i = 0; i < X_COOR_SIZE; i++){
      if(obstacleCoordinates[i][j] == 4 || obstacleCoordinates[i][j] == 5 || obstacleCoordinates[i][j] == 6 || obstacleCoordinates[i][j] == 7){
        obstacleCoordinates[i][j] = 0;
      }
    }
  }

  // Finding the successful path...
  // We need to trace back the path from GOAL to Initial;
  fprintf(output_file, "Path from Initial to Goal\n");
  curr_x = goal[0];
  curr_y = goal[1];

  while(curr_x != initial[0] || curr_y != initial[1]){
    if (obstacleCoordinates[curr_x][curr_y]%4 == 0)
    {
      obstacleCoordinates[curr_x][curr_y] = 4;
      curr_y = curr_y + 1;
    }
    else if (obstacleCoordinates[curr_x][curr_y]%4 == 1)
    {
      obstacleCoordinates[curr_x][curr_y] = 5;
      curr_y = curr_y - 1;
    }
    else if (obstacleCoordinates[curr_x][curr_y]%4 == 2)
    {
      obstacleCoordinates[curr_x][curr_y] = 6;
      curr_x = curr_x + 1;
    }
    else if (obstacleCoordinates[curr_x][curr_y]%4 == 3)
    {
      obstacleCoordinates[curr_x][curr_y] = 7;
      curr_x = curr_x - 1;
    }
  }
  for(j = Y_COOR_SIZE - 1; j >= 0; j--){
    for(i = 0; i < X_COOR_SIZE; i++){
      if(obstacleCoordinates[i][j] == 2){
        fprintf(output_file, "I ");
      }
      else if(i == goal[0] && j == goal[1]){
        fprintf(output_file, "G ");
      }
      else if(obstacleCoordinates[i][j] == 4){ // This is mod 0
        fprintf(output_file, "v ");
      }
      else if(obstacleCoordinates[i][j] == 5 ){ // This is mod 1
        fprintf(output_file, "^ ");
      }
      else if(obstacleCoordinates[i][j] == 6){ // Mod 2
        fprintf(output_file, "< ");
      }
      else if(obstacleCoordinates[i][j] ==  7){ // Mod 3
        fprintf(output_file, "> ");
      }
      else if(obstacleCoordinates[i][j] == 1){
        fprintf(output_file, "@ ");
      }
      else{
        fprintf(output_file, ". ");
      }
    }
    fprintf(output_file, "\n");
  }
}

void removeEnter(char* buffer){
  int i;

  for(i=0; i<ARRAYSPACE_SMALL; i++){
    if(buffer[i] == 10){
      buffer[i] = 0;
      return;
    }
  }
}

void formatString(char* buffer){
  // This parses initial and goal state
  int i;

  // If input starts with '('
  if(buffer[0] == 40){
    for(i = 0; buffer[i+1] != 0; i++){
      buffer[i] = buffer[i+1];
    }
    buffer[i-1] = 0;
  }
  return;
}

int traceObstacle(int x1, int y1, int x2, int y2){
  int i;
  int j = temp_obstacle_count;
  int temp;

  // Case 1: Slope is undefined...
  if( x1 == x2 )
  {
    if(y1 > y2){
      temp = y1;
      y1 = y2;
      y2 = temp;
      temp = x1;
      x1 = x2;
      x2 = temp;
    }

    else if(y1 == y2){ // If vertex is same
      obstacleCoordinates[x1][y1] = 1;
    }

    // CASE 1: Slope is zero
    for(i = y1; i <= y2; i++){
      obstacleCoordinates[x1][i] = 1;
    }
  }
  else{
    // CASE 2: Slope is nonzero
    if(x1 > x2){
      temp = x1;
      x1 = x2;
      x2 = temp;
      temp = y1;
      y1 = y2;
      y2 = temp;
    }
    for(i = x1; i <= x2; i++){
      temp_obstacle[temp_obstacle_count][0] = i;
      temp_obstacle[temp_obstacle_count][1] = ((y2 - y1)*(1.0)/(x2 - x1))*(i - x1) + y1;
      temp_obstacle_count++;
    }
  }
}

void findObstacles(){
  int i,j,k,l;
  int x1, x2; // This will be values for the boundaries of x.
  float y1, y2; // This will be values for the boundaries of y.
  for(i=0; i < polygonCount; i++){
    /* Reinitialize temp_obstacle here*/
    temp_obstacle_count = 0;
    for(j=0; j < verticesSize[i]; j++){
      if(j != verticesSize[i] - 1){
        // Trace current with next
        traceObstacle(vertices[i][j][0], vertices[i][j][1], vertices[i][j+1][0],vertices[i][j+1][1]);
      }
      else{
        // Trace current with first
        traceObstacle(vertices[i][j][0], vertices[i][j][1], vertices[i][0][0], vertices[i][0][1]);
      }
    }

    /*After tracing the sides of the polygon, we may now mark the enclosed coordinates of obstacles. */
    // Get range of x1 and x2 by finding least and highest value of x
    x1 = 999999;
    x2 = 0;
    for(j=0; j<temp_obstacle_count;j++){
      if(temp_obstacle[j][0] <= x1){
        x1 = temp_obstacle[j][0];
      }
      if(temp_obstacle[j][0] >= x2){
        x2 = temp_obstacle[j][0];
      }
    }

    for(j=x1; j<=x2; j++){   // Start from x1 -> x2
      // Find y1 (lowest) and y2 (highest) points paired with x1...
      y1 = 999999;
      y2 = 0;
      for(k=0; k<temp_obstacle_count; k++){
        if(temp_obstacle[k][0] == j){
          // Consider y now
          if(temp_obstacle[k][1] <= y1){
            y1 = temp_obstacle[k][1];
          }
          if(temp_obstacle[k][1] >= y2){
            y2 = temp_obstacle[k][1];
          }
        }
      }

      // Now that we know y, let's fill the obstacles
      if(ceil(y1)!= floor(y2)){ //Round up y1 == Round down y2, only (j,y1) is tagged
        for(k=ceil(y1); k<=floor(y2); k++)
        obstacleCoordinates[j][k] = 1;
      }
      else{
        obstacleCoordinates[j][(int)y1] = 1;
      }
    }
  }
}

/* =============== MAIN WORKING FUNCTION =============== */
int main(){
  // Variables
  FILE *fp;
  int i;

  int j;
  int k;
  int flag = 0;

  char buffer[ARRAYSPACE_SMALL];
  char strtokBuffer[2] = ",";
  char strtokBuffer2[4] = "),(";
  char *token;

  front.node = NULL;
  rear.node = NULL;

  /* File Parsing, Initialization, Input Validation*/
  fp = fopen("input.txt", "r");
  if(fp == NULL)
    perror("Error opening file.");
  else{
    for(i = 0; fgets(buffer,ARRAYSPACE_SMALL,fp) != NULL; i++){
      removeEnter(buffer);
      if(i == 0){
        formatString(buffer);
        token = strtok(buffer, strtokBuffer);
        initial[0] = atoi(token);
        token = strtok(NULL, strtokBuffer);
        initial[1] = atoi(token);
        if((initial[0] < 0) || (initial[1] < 0) || (initial[0] > X_COOR_SIZE) || (initial[1] > Y_COOR_SIZE)){
          printf("Invalid initial state. Not in range.\n");
          flag = 1;
        }
      }
      else if(i == 1){
        formatString(buffer);
        token = strtok(buffer, strtokBuffer);
        goal[0] = atoi(token);
        token = strtok(NULL, strtokBuffer);
        goal[1] = atoi(token);
        if((goal[0] < 0) || (goal[1] < 0) || (goal[0] > X_COOR_SIZE) || (goal[1] > Y_COOR_SIZE)){
          printf("Invalid goal state. Not in range.\n");
          flag = 1;
        }
      }

      else{
        // Store obstacles
        //(x1,y2),(x1,y2),...(xn,yn)
        token = strtok(buffer, strtokBuffer2);

        j = 0;
        k = 0;
        while(token != NULL){
          if(k%2 == 0){
            vertices[i-2][j][k%2] = atoi(token);
          }
          else if(k%2 == 1){
            vertices[i-2][j][k%2] = atoi(token);
            j ++;
          }
          k++;
          token = strtok(NULL, strtokBuffer2);
        }
        verticesSize[i-2] = (k/2);
      }
    }
    polygonCount = i-2;
    findObstacles();                     // This function maps all coordinates enclosed by the obstacles.
    if(flag == 1){
      printf("Please fix input file.\n");
    }

    output_file = fopen("output.txt", "w");

    flag = BFS();
    if(flag == 0)
      fprintf(output_file, "BFS: Initial is Goal State.\n");
    else if(flag == 1){
      fprintf(output_file, "BFS Successful.\n");
    }
    else{
      fprintf(output_file, "BFS Unsuccessful.\n");
    }
    printPath();

    flag = DFS();
    if(flag == 0)
      fprintf(output_file, "\nDFS: Initial is Goal State.\n");
    else if(flag == 1){
      fprintf(output_file, "\nDFS Successful.\n");
    }
    else{
      fprintf(output_file, "\nDFS Unsuccessful.\n");
    }
    printPath();

    if(flag == 0){
      fprintf(output_file, "\nAStar: Initial is Goal State.\n");
    }
    else{
      flag = AStar();
      if(flag == 0){
        fprintf(output_file, "\nA* Unsuccessful\n");
      }
      else if(flag == 1){
        fprintf(output_file, "\nA* Successful\n");
      }
      printPath();
    }
  }

  fclose(output_file);
}

/* =============== Depth-First Search =============== */

// This just tells us if we have already visited that vertex before.
int isVisited(int x, int y){
  if(obstacleCoordinates[x][y] == 2 ||(obstacleCoordinates[x][y] == (4*num_runs)+4) || (obstacleCoordinates[x][y] == (4*num_runs)+5)  || (obstacleCoordinates[x][y] == (4*num_runs)+ 6) || (obstacleCoordinates[x][y] == (4*num_runs)+ 7 ))
    return 1;
  return 0;
}

int isVisited_BFS(int x, int y){
  if(obstacleCoordinates[x][y] == 2 ||(obstacleCoordinates[x][y] == (4*num_runs)+4) || (obstacleCoordinates[x][y] == (4*num_runs)+5)  || (obstacleCoordinates[x][y] == (4*num_runs)+ 6) || (obstacleCoordinates[x][y] == (4*num_runs)+ 7 ))
    return 1;
  return 0;
}

// We check if a node is indeed a successor.
int isSuccessor(int x, int y){
  // It is a successor if it is an unexplored node.
  // Still inbounds
  if(!(x >= 0 && x < X_COOR_SIZE)){
    return 0;
  }
  else if(!(y >= 0 && y < Y_COOR_SIZE)){
    return 0;
  }

  if(obstacleCoordinates[x][y] == 1 || obstacleCoordinates[x][y] == 2)  // Obstacle or Initial or Goal
    return 0;
  else if((obstacleCoordinates[x][y] == (4*num_runs)+4) || (obstacleCoordinates[x][y] == (4*num_runs)+5)  || (obstacleCoordinates[x][y] == (4*num_runs)+ 6) || (obstacleCoordinates[x][y] == (4*num_runs)+ 7 ))
    return 0;
  return 1;
}

int isSuccessor_BFS(int x, int y){
  // It is a successor if it is an unexplored node.
  // Still inbounds
  if(!(x >= 0 && x < X_COOR_SIZE)){
    return 0;
  }
  else if(!(y >= 0 && y < Y_COOR_SIZE)){
    return 0;
  }

  if(obstacleCoordinates[x][y] == 1 || obstacleCoordinates[x][y] == 2)  // Obstacle or Initial or Goal
    return 0;
  else if((obstacleCoordinates[x][y] == (4*num_runs)+4) || (obstacleCoordinates[x][y] == (4*num_runs)+5)  || (obstacleCoordinates[x][y] == (4*num_runs)+ 6) || (obstacleCoordinates[x][y] == (4*num_runs)+ 7 ))
    return 0;
  return 1;
}

void successor_AStar(int x, int y, int path_cost){
  if(obstacleCoordinates[x][y - 1] != 1 && (y - 1) >= 0 && obstacleCoordinates[x][y - 1] < ((4 * num_runs) + 4) && obstacleCoordinates[x][y - 1] > (-1)*((4 * num_runs) + 4) && obstacleCoordinates[x][y - 1] != 2){
    obstacleCoordinates[x][y - 1] = (-1)*((4 * num_runs) + 4);
    insert_AStar(&AStar_top, x, y - 1, path_cost);
  }
  if(obstacleCoordinates[x][y + 1] != 1 && (y + 1) < 200 && obstacleCoordinates[x][y + 1] < ((4 * num_runs) + 4) && obstacleCoordinates[x][y + 1] > (-1)*((4 * num_runs) + 4) && obstacleCoordinates[x][y + 1] != 2){
    obstacleCoordinates[x][y + 1] = (-1)*((4 * num_runs) + 5);
    insert_AStar(&AStar_top, x, y + 1, path_cost);
  }
  if(obstacleCoordinates[x + 1][y] != 1 && (x + 1) < 400 && obstacleCoordinates[x + 1][y] < ((4 * num_runs) + 4) && obstacleCoordinates[x + 1][y] > (-1)*((4 * num_runs) + 4) && obstacleCoordinates[x + 1][y] != 2){
    obstacleCoordinates[x + 1][y] = (-1)*((4 * num_runs) + 7);
    insert_AStar(&AStar_top, x + 1, y, path_cost);
  }
  if(obstacleCoordinates[x - 1][y] != 1 && (x - 1) >= 0 && obstacleCoordinates[x - 1][y] < ((4 * num_runs) + 4) && obstacleCoordinates[x - 1][y] > (-1)*((4 * num_runs) + 4) && obstacleCoordinates[x - 1][y] != 2){
    obstacleCoordinates[x - 1][y] = (-1)*((4 * num_runs) + 6);
    insert_AStar(&AStar_top, x - 1, y, path_cost);
  }
}


// ===================== BFS ===================


int BFS(){
  // stack_top is our fringe.

  int curr_x = initial[0];
  int curr_y = initial[1];
  int temp_x;
  int temp_y;
  int i;
  int j;
  int flag;

  num_runs++;

  // Set initial and goal to a certain value
  obstacleCoordinates[initial[0]][initial[1]] = 2;
  obstacleCoordinates[goal[0]][goal[1]] = 3;

  // 1. If GOAL?(initial-state) then return initial-state
  if(initial[0] == goal[0] && initial[1] == goal[1])
    return 0;

  // 2. INSERT(initial-node,FRINGE)
  enqueue(&front, &rear, curr_x, curr_y);  // Enqueue initial point

  while(1)
  {
    if(isEmptyQueue(&front, &rear) == 1){
      return -1; // Failure.
    }

    if(dequeue(&front, &rear) != 0){
      curr_x = variable1;
      curr_y = variable2;
    }

    flag = (isVisited_BFS(curr_x, curr_y) != 1);

    if( flag || obstacleCoordinates[curr_x][curr_y] == 2)
    {
      if(obstacleCoordinates[curr_x][curr_y] != 2)
        obstacleCoordinates[curr_x][curr_y] = (-1)*obstacleCoordinates[curr_x][curr_y];

      if(isSuccessor_BFS(curr_x, curr_y-1) == 1){ // Left yung action. Galing right.
        if(curr_x == goal[0] && (curr_y - 1) == goal[1]){
          obstacleCoordinates[goal[0]][goal[1]] = ((4*num_runs) + 4);
          return 1;
        }
        enqueue(&front, &rear, curr_x,curr_y - 1);
        obstacleCoordinates[curr_x][curr_y-1] = (-1)*((4*num_runs) + 4);
      }
      if(isSuccessor_BFS(curr_x, curr_y+1) == 1){ // Right yung action. Galing left.
        if(curr_x == goal[0] && (curr_y + 1) == goal[1]){
          obstacleCoordinates[goal[0]][goal[1]] = ((4*num_runs) + 5);
          return 1;
        }
        enqueue(&front, &rear, curr_x,curr_y + 1);
        obstacleCoordinates[curr_x][curr_y+1] = (-1)*((4*num_runs) + 5);
      }
      if(isSuccessor_BFS(curr_x-1,curr_y) == 1){ // Up yung action.Galing from down.
        if((curr_x-1) == goal[0] && curr_y == goal[1]){
          obstacleCoordinates[goal[0]][goal[1]] = ((4*num_runs) + 6);
          return 1;
        }
        enqueue(&front, &rear, curr_x-1,curr_y);
        obstacleCoordinates[curr_x-1][curr_y] = (-1)*((4*num_runs) + 6);
      }
      if(isSuccessor_BFS(curr_x+1,curr_y) == 1){ // Down yung action. Galing from Up.
        if((curr_x+1) == goal[0] && curr_y == goal[1]){
          obstacleCoordinates[goal[0]][goal[1]] = ((4*num_runs) + 7);
          return 1;
        }
        enqueue(&front, &rear,curr_x+1,curr_y);
        obstacleCoordinates[curr_x+1][curr_y] = (-1)*((4*num_runs) + 7);
      }
    }
  }
}


int DFS(){
  // stack_top is our fringe.

  int curr_x = initial[0];
  int curr_y = initial[1];
  int temp_x;
  int temp_y;
  int i;
  int j;
  int flag;

  num_runs++;

  // Set initial and goal to a certain value
  obstacleCoordinates[initial[0]][initial[1]] = 2;
  obstacleCoordinates[goal[0]][goal[1]] = 3;

  // 1. If GOAL?(initial-state) then return initial-state
  if(initial[0] == goal[0] && initial[1] == goal[1])
    return 0;

  // 2. INSERT(initial-node,FRINGE)
  push(&stack_top,curr_x,curr_y);

  while(1)
  {
    if(isEmptyStack(&stack_top) == 1){
      return -1; // Failure.
    }
    pop(&stack_top);
    curr_x = variable1;
    curr_y = variable2;

    flag = (isVisited(curr_x, curr_y) != 1);

    if( flag || obstacleCoordinates[curr_x][curr_y] == 2)
    {
      if(obstacleCoordinates[curr_x][curr_y] != 2)
        obstacleCoordinates[curr_x][curr_y] = (-1)*obstacleCoordinates[curr_x][curr_y];

      if(isSuccessor(curr_x, curr_y-1) == 1){ // Left yung action. Galing right.
        if(curr_x == goal[0] && (curr_y - 1) == goal[1]){
          obstacleCoordinates[goal[0]][goal[1]] = ((4*num_runs) + 4);
          return 1;
        }
        push(&stack_top,curr_x,curr_y - 1);
        obstacleCoordinates[curr_x][curr_y-1] = (-1)*((4*num_runs) + 4);
      }
      if(isSuccessor(curr_x, curr_y+1) == 1){ // Right yung action. Galing left.
        if(curr_x == goal[0] && (curr_y + 1) == goal[1]){
          obstacleCoordinates[goal[0]][goal[1]] = ((4*num_runs) + 5);
          return 1;
        }
        push(&stack_top,curr_x,curr_y + 1);
        obstacleCoordinates[curr_x][curr_y+1] = (-1)*((4*num_runs) + 5);
      }
      if(isSuccessor(curr_x-1,curr_y) == 1){ // Up yung action.Galing from down.
        if((curr_x-1) == goal[0] && curr_y == goal[1]){
          obstacleCoordinates[goal[0]][goal[1]] = ((4*num_runs) + 6);
          return 1;
        }
        push(&stack_top,curr_x-1,curr_y);
        obstacleCoordinates[curr_x-1][curr_y] = (-1)*((4*num_runs) + 6);
      }
      if(isSuccessor(curr_x+1,curr_y) == 1){ // Down yung action. Galing from Up.
        if((curr_x+1) == goal[0] && curr_y == goal[1]){
          obstacleCoordinates[goal[0]][goal[1]] = ((4*num_runs) + 7);
          return 1;
        }
        push(&stack_top,curr_x+1,curr_y);
        obstacleCoordinates[curr_x+1][curr_y] = (-1)*((4*num_runs) + 7);
      }
    }
  }
}


int AStar(){
  int curr_x = initial[0];
  int curr_y = initial[1];
  int path_cost = 0;

  num_runs++;

  obstacleCoordinates[curr_x][curr_y] = (-1)*obstacleCoordinates[curr_x][curr_y];
  insert_AStar(&AStar_top, curr_x, curr_y, path_cost);

  while(1){
    if(isEmptyStack(&AStar_top) == 1){
      return 0;
    }

    pop(&AStar_top);
    curr_x = variable1;
    curr_y = variable2;
    path_cost = variable3;

    obstacleCoordinates[curr_x][curr_y] = (-1) * obstacleCoordinates[curr_x][curr_y];

    if(curr_x == goal[0] && curr_y == goal[1]){
      return 1;
    }

    successor_AStar(curr_x, curr_y, path_cost + 1);
  }
}

// Data Structure Operators
LIST_NODE* newNode(int x1, int y1){
  LIST_NODE* temp = (LIST_NODE*) malloc(sizeof(LIST_NODE));
  if(temp == NULL)
    return NULL;

  temp->x = x1;
  temp->y = y1;
  temp->next = NULL;  // Initialize na NULL, para walang problema
  return temp;
}

void push(NODE_POINTER* TOP, int x1, int y1){
  LIST_NODE* new = newNode(x1, y1);
  new->next = (*TOP).node;
  (*TOP).node = new;
  return;
}

int isEmptyStack(NODE_POINTER* TOP){
  if((*TOP).node == NULL)
    return 1;
  return 0;
}

int pop(NODE_POINTER* TOP){
  NODE_POINTER temp;

  if(isEmptyStack(TOP) == 1)
  {
    printf("\nNothing to pop.");
    return 0;
  }
  else{
    variable1 = ((*TOP).node)->x;
    variable2 = ((*TOP).node)->y;
    variable3 = ((*TOP).node)->path_cost;

    temp.node = (*TOP).node;
    // Let temporary node point to where the top node currently points at.

      (*TOP).node = ((*TOP).node)->next;
    // Let the top node point to the next one...

    free(temp.node);                    // Free the popped node.

    return 1;
  }
}

int checkTop(NODE_POINTER TOP){
  if(TOP.node == NULL){
    variable1 = -1;
    variable2 = -1;
    return 0;
  }

  variable1 = TOP.node->x;
  variable2 = TOP.node->y;
  return 1;
}

/* Queue Operations */
// Actually, pwedeng i-call ko na lang yung checkTop sa loob nito,
// but let's just keep it this way.

void enqueue(NODE_POINTER* FRONT, NODE_POINTER* REAR, int x1, int y1){
  LIST_NODE* new = newNode(x1, y1);
  new->next = (*REAR).node;
  if(isEmptyQueue(FRONT, REAR) == 1){
    // Point siya sa NULL;
    (*FRONT).node = new;
    (*REAR).node = new;
  }
  else{
    (*REAR).node = new;
  }
  return;
}

int dequeue(NODE_POINTER* FRONT, NODE_POINTER* REAR){
  NODE_POINTER current;

  if(isEmptyQueue(FRONT, REAR) == 1)
  {
    printf("\nNothing to dequeue.");
    return 0;
  }
  else{
    current.node = (*REAR).node;

    if(current.node == (*FRONT).node){
      variable1 = current.node->x;
      variable2 = current.node->y;

      free(current.node);
      (*REAR).node = NULL;
      (*FRONT).node = NULL;
    }
    else{
      while( (current.node)->next != (*FRONT).node )
      {
        current.node = (current.node)->next;
      }

      variable1 = ((*FRONT).node)->x;
      variable2 = ((*FRONT).node)->y;

      free((*FRONT).node);

      (*FRONT).node = current.node;
      // Let front node point to current node.

      (current.node)->next = NULL;
    }

    return 1;
  }
}


int checkFront(NODE_POINTER FRONT){
  if(FRONT.node == NULL){
    printf("\nFRONT points to NULL.");
    return 0;
  }

  variable1 = FRONT.node->x;
  variable2 = FRONT.node->y;
  printf("\nFRONT points to (%d, %d)", FRONT.node->x, FRONT.node->y);
  return 1;
}

int checkRear(NODE_POINTER REAR){
  if(REAR.node == NULL){
    printf("\nREAR points to NULL.");
    return 0;
  }

  variable1 = REAR.node->x;
  variable2 = REAR.node->y;
  printf("\nREAR points to (%d, %d)", REAR.node->x, REAR.node->y);
  return 1;
}

int isEmptyQueue(NODE_POINTER* FRONT, NODE_POINTER* REAR){
  if((*FRONT).node == NULL && (*REAR).node == NULL){
    return 1;
  }
  return 0;
}

void insert_AStar(NODE_POINTER* TOP, int x1, int y1, int path_cost){
  NODE_POINTER alpha;
  NODE_POINTER beta;
  float distance = sqrt(pow((goal[0]-x1), 2) + pow((goal[1]-y1), 2));
  LIST_NODE* new = newNode(x1, y1);

  new->path_cost = path_cost;
  new->distance = distance;
  if(isEmptyStack(TOP) == 1){
    (*TOP).node = new;
    (*TOP).node->next = NULL;
  }
  else{
    alpha.node = (*TOP).node;
    beta.node = NULL;
    while(alpha.node != NULL){
      if((alpha.node->distance + alpha.node->path_cost) > (distance + path_cost)){
        break;
      }
      beta.node = alpha.node;
      alpha.node = alpha.node->next;
    }
    new->next = alpha.node;
    if(beta.node == NULL){
      (*TOP).node = new;
    }
    else{
      beta.node->next = new;
    }
  }
}

/*Programmed by: John Louise Tan and Christopher Vizcarra*/
