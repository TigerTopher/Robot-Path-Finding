#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

#define ARRAYSPACE_SMALL 100
#define X_COOR_SIZE 200
#define Y_COOR_SIZE 400

int initial[2];
int goal[2];
int vertices[ARRAYSPACE_SMALL][ARRAYSPACE_SMALL][2];
int obstacleCount;                  // Number of obstacles/polygon    | I-2 count
int verticesSize[ARRAYSPACE_SMALL]; // Number of vertices per polygon | L count
int obstacleCoordinates[X_COOR_SIZE][Y_COOR_SIZE];  // Value of 1 if there is an obstacle.

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

  if( x1 == x2 ){
    // Slope is zero in this case.
    for(i = y1; i<=y2; i++){
      obstacleCoordinates[x1][i] = 1;
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
        verticesSize[i-2] = k/2;
      }
    }
    obstacleCount = i-2;                  // (i - 2) Dahil minus initial, goal.

    traceObstacle(1,0,1,2);
    /* //How to print the polygons...
    for(i = 0; i < obstacleCount; i++){
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
