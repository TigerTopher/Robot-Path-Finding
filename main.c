#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<math.h>

/* John Louise Tan
  Christopher Vizcarra*/

#define ARRAYSPACE_SMALL 100
#define ARRAYSPACE_BIG 80000
#define X_COOR_SIZE 400
#define Y_COOR_SIZE 200

int initial[2];
int goal[2];
int vertices[ARRAYSPACE_SMALL][ARRAYSPACE_SMALL][2];
int polygonCount;                  // Number of obstacles/polygon    | I-2 count
int verticesSize[ARRAYSPACE_SMALL]; // Number of vertices per  polygon | L count
int obstacleCoordinates[X_COOR_SIZE][Y_COOR_SIZE];  // Value of 1 if there is an obstacle.

float temp_obstacle[ARRAYSPACE_BIG][2];
int temp_obstacle_count = 0;

// This is temporary variable that can handle floats in building the obstacle.
// I would put x-y pairs here

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
  /*Idea:
  y - y1 = m ( x - x1)
  y - y1 = [(y2 - y1)/(x2 - x1)]*(x - x1)
  y = [ [(y2 - y1)/(x2 - x1)]*(x - x1) ] + y1
  Given an x, may formula na ako for y, for all x that are natural numbers from x1 -> x2
  So magkakaroon ako ng y floats (or maybe natural numbers as well).
  I'll have x-y pairs ngayon. I will store them all.
  */
  //printf("(%d,%d) -> (%d,%d)\n", x1, y1, x2, y2);

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
      /// obstacleCoordinates[x1][y1] = 1;
      obstacleCoordinates[x1][y1] = 1;
    }

    // CASE 1: Slope is zero
    for(i = y1; i <= y2; i++){
      // obstacleCoordinates[x1][i] = 1;
      printf("Marking: %d %d\n", x1, i);
      obstacleCoordinates[x1][i] = 1;
    }
  }
  else{
    // CASE 2: Slope is nonzero
    if(((float)(y2 - y1)/(float)(x2 - x1)) <= 1 && ((float)(y2 - y1)/(float)(x2 - x1)) >= -1)
    {
      if(x1 > x2){
        temp = x1;
        x1 = x2;
        x2 = temp;
        temp = y1;
        y1 = y2;
        y2 = temp;
      }
      for(i = x1; i <= x2; i++){
        //printf("%d %d %d %d", x1, y1, x2, y2);
        //printf("Slope: %f, x = %d, y = %f\n", (y2 - y1)*(1.0)/(x2 - x1), i, ((((y2 - y1)*(1.0)/(x2 - x1))*(i - x1)) + y1));
        temp_obstacle[temp_obstacle_count][0] = i;
        temp_obstacle[temp_obstacle_count][1] = ((y2 - y1)*(1.0)/(x2 - x1))*(i - x1) + y1;
        temp_obstacle_count++;
      }
    }
    else
    {
      if(y1 > y2){
        temp = y1;
        y1 = y2;
        y2 = temp;
        temp = x1;
        x1 = x2;
        x2 = temp;
      }
      for(i = y1; i <= y2; i++){
        printf("%d %d %d %d %d\n", x1, y1, x2, y2, i);
        temp_obstacle[temp_obstacle_count][0] = ((x2 - x1)*(1.0)/(y2 - y1))*(i - y1) + x1;
        temp_obstacle[temp_obstacle_count][1] = i;
        temp_obstacle_count++;
      }
      printf("\n");
    }
  }
}

void findObstacles(){
  /* Idea:
    Kapag nakuha mo na lahat nung edges nung polygon,
    Saka natin itrace from bottom to top, from x1 -> x2
    Find (x1, __) // Lowest -> I-round up yung float dito
    Find (x1, __) // Highest -> I-round down
    Kailangan natin yung mga vertex enclosed. Kaya round-up sa lowest tapos round-down sa highest.
    Magiging natural number na i-coconsider na sa coordinate system natin.
  */
  int i,j,k,l;
  float x1, x2; // This will be values for the boundaries of x.
  float y1, y2, y3, y4; // This will be values for the boundaries of y.
  float temp;
  for(i=0; i < polygonCount; i++){
    /* Reinitialize temp_obstacle here*/
    temp_obstacle_count = 0;
    for(j=0; j < verticesSize[i]; j++){
      if(j != verticesSize[i] - 1){
        // Trace current with next
        x1 = vertices[i][j][0];
        y1 = vertices[i][j][1];
        x2 = vertices[i][j+1][0];
        y2 = vertices[i][j+1][1];
      }
      else{
        // Trace current with first
        x1 = vertices[i][j][0];
        y1 = vertices[i][j][1];
        x2 = vertices[i][0][0];
        y2 = vertices[i][0][1];
      }
      traceObstacle(x1, y1, x2, y2);
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
    //printf("Lowest value is %d, Highest value is %d", x1, x2);
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
      //printf("For %d, lowest value is %f, highest value is %f.\n", j, y1, y2);
      // Now that we know y, let's fill the obstacles
      if(ceil(y1)!= floor(y2)){ //Round up y1 == Round down y2, only (j,y1) is tagged.
        obstacleCoordinates[j][(int)ceil(y1)] = 1;
        obstacleCoordinates[j][(int)floor(y2)] = 1;
      }
      else{ // If yun lang yung point, this means na para siyang cusp na sideways, yun lang ang obstacle sa point na iyon.
        obstacleCoordinates[j][(int)y1] = 1;
      }
    }

    y1 = 999999;
    y2 = 0;
    for(j=0; j<temp_obstacle_count;j++){
      if(temp_obstacle[j][1] <= y1){
        y1 = temp_obstacle[j][1];
      }
      if(temp_obstacle[j][1] >= y2){
        y2 = temp_obstacle[j][1];
      }
    }
    //printf("Lowest value is %.2lf, Highest value is %.2lf", y1, y2);
    for(j=y1; j<=y2; j++){   // Start from x1 -> x2
      // Find y1 (lowest) and y2 (highest) points paired with x1...
      x1 = 999999;
      x2 = 0;
      for(k=0; k<temp_obstacle_count; k++){
        if(temp_obstacle[k][1] == j){
          // Consider y now
          if(temp_obstacle[k][0] <= x1){
            x1 = temp_obstacle[k][0];
          }
          if(temp_obstacle[k][0] >= x2){
            x2 = temp_obstacle[k][0];
          }
        }
      }
      printf("For %d, lowest value is %f, highest value is %f.\n", j, x1, x2);
      // Now that we know y, let's fill the obstacles
      if(x1 != 999999 && x2 != 0){
        if(ceil(x1)!= floor(x2)){ //Round up y1 == Round down y2, only (j,y1) is tagged.
          obstacleCoordinates[(int)x2][j] = 1;
        }
        else{ // If yun lang yung point, this means na para siyang cusp na sideways, yun lang ang obstacle sa point na iyon.
          obstacleCoordinates[(int)x1][j] = 1;
        }
      }
    }
    //printf("\n%d - %d,%d\n", i, vertices[i][0][0], vertices[i][0][1]);
    //printf("\n%d\n", k);
  }
}

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
        if((initial[0] <= 0) || (initial[1] <= 0) || (initial[0] > X_COOR_SIZE) || (initial[1] > Y_COOR_SIZE)){
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
        if((goal[0] <= 0) || (goal[1] <= 0) || (goal[0] > X_COOR_SIZE) || (goal[1] > Y_COOR_SIZE)){
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
    polygonCount = i-2;                  // (i - 2) Dahil minus initial, goal.
    fclose(fp);
    findObstacles();                     // This function maps all coordinates enclosed by the obstacles.

    if(flag == 1){
      printf("Please fix input file.\n");
    }

    fp = fopen("output1.txt", "w");
    for(j = Y_COOR_SIZE - 1; j >= 0; j--){
      for(i = 0; i < X_COOR_SIZE; i++){
        fprintf(fp, "%d ", obstacleCoordinates[i][j]);
        if(obstacleCoordinates[i][j] == 1)
        {
          printf("(%d, %d)", i, j);
        }
      }
      fprintf(fp, "\n");
    }
    fclose(fp);
  }
}

/*Programmed by: Christopher Vizcarra*/
