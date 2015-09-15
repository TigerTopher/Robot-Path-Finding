#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

#define ARRAYSPACE_SMALL 100
#define ARRAYSPACE_BIG 1000
#define X_COOR_SIZE 200
#define Y_COOR_SIZE 400

int initial[2];
int goal[2];
int vertices[ARRAYSPACE_SMALL][ARRAYSPACE_SMALL][2];
int polygonCount;                  // Number of obstacles/polygon    | I-2 count
int verticesSize[ARRAYSPACE_SMALL]; // Number of vertices per polygon | L count
int obstacleCoordinates[X_COOR_SIZE][Y_COOR_SIZE];  // Value of 1 if there is an obstacle.

float temp_obstacle[ARRAYSPACE_BIG][2];
int temp_obstacle_count = 0;

// This is temporary variable that can handle floats in building the obstacle.
// I would put x-y pairs here

/* Notes:
  Need to use the following algorithms:
  a.) BFS
  b.) DFS
  c.) A* Search

  We have an m x n grid.
  200 x 400

  Run a number of test cases of different complexity in terms of:
  a.) Number,
  b.) Shape,
  c.) Size,
  d.) Difficulty of Solution

  Develop 5 Test cases.

  Check Input Validity
    a.) Is the initial state or the goal state enclosed inside an obstacle?
*/
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

  printf("(%d,%d) -> (%d,%d)\n", x1, y1, x2, y2);

  if( x1 == x2 ){
    if(y1 > y2){
      temp = y1;
      y1 = y2;
      y2 = temp;
      temp = x1;
      x1 = x2;
      x2 = temp;
    }

    else if(y1 == y2){ // If vertex is same
      /// obstacleCoordinates[x1][y1] = 1;
      temp_obstacle[temp_obstacle_count][0] = x1;
      temp_obstacle[temp_obstacle_count][1] = y1;
      temp_obstacle_count++;
    }

    // CASE 1: Slope is zero
    for(i = y1; i <= y2; i++){
      // obstacleCoordinates[x1][i] = 1;
      //printf("Marking: %d %d\n", x1, i);
      temp_obstacle[temp_obstacle_count][0] = x1;
      temp_obstacle[temp_obstacle_count][1] = i;
      temp_obstacle_count++;
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
      printf("%d %d %d %d", x1, y1, x2, y2);
      printf("Slope: %f, x = %d, y = %f\n", (y2 - y1)*(1.0)/(x2 - x1), i, ((((y2 - y1)*(1.0)/(x2 - x1))*(i - x1)) + y1));
      //temp_obstacle[temp_obstacle_count][0] = i;
      //temp_obstacle[temp_obstacle_count][1] = ((y2 - y1)(*1.0)/(x2 - x1))(i - x1) + y1
    }
  }
  /*
    y - y1 = m ( x - x1)
    y - y1 = [(y2 - y1)/(x2 - x1)]*(x - x1)
    y = [ [(y2 - y1)/(x2 - x1)]*(x - x1) ] + y1
    Given an x, may formula na ako for y, for all x that are natural numbers from x1 -> x2
    So magkakaroon ako ng y floats (or maybe natural numbers as well).
    I'll have x-y pairs ngayon. I will store them all.

  */
}

void findObstacles(){
  /* Idea:
    Kapag nakuha mo na lahat nung edges nung polygon,
    Saka natin itrace from top to bottom.
    Top would be the vertex with highest value for y.
    Lowest would be... lowest value for y.
  */
  int i,j;
  for(i=0; i < polygonCount; i++){
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
  }
  /* Do tracing here*/

}

int main(){
  // Variables
  FILE *fp;
  int i;

  int j;
  int k;

  char buffer[ARRAYSPACE_SMALL];
  char strtokBuffer[2] = ",";
  char strtokBuffer2[4] = "),(";
  char *token;

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
      }
      else if(i == 1){
        formatString(buffer);
        token = strtok(buffer, strtokBuffer);
        goal[0] = atoi(token);
        token = strtok(NULL, strtokBuffer);
        goal[1] = atoi(token);
      }

      else{
        // Store obstacles
        //(x1,y2),(x1,y2),...(xn,yn)
        token = strtok(buffer, strtokBuffer2);

        j = 0;
        k = 0;
        while(token != NULL){
          //printf("%s %d %d\n", token, j, k);
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
    polygonCount = i-2;                  // (i - 2) Dahil minus initial, goal.
    findObstacles();
    /* //How to print the polygons...
    for(i = 0; i < polygonCount; i++){
      printf("Polygon %d: ", i);
      for(j = 0; j < verticesSize[i]; j++){
        for(k = 0; k < 2; k++){
          printf("%d ", vertices[i][j][k]);
        }
        printf("\n");
      }
    }*/

  }
}

/*Programmed by: Christopher Vizcarra*/
