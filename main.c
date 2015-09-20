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
#define X_COOR_SIZE 200
#define Y_COOR_SIZE 400

/*=============== Typedefs and Structs =============== */
/* Same lang naman ang representation ng Stack at Queue, which
  is a list. Nagkakatalo lang sa function. So hayaan mong pangalanan
  ko ang generic data structure bilang LIST */

typedef struct LIST_noder{ // Generic List DS
  int x;  // Data x
  int y;  // Data y
  struct LIST_noder *next; // Next
} LIST_NODE;

typedef struct{ // This serves as a stack pointer or pointer sa queue.
  LIST_NODE *node;  // Pwedeng top, front, rear
} NODE_POINTER;

/*=============== Global Variables ===============*/
int initial[2];
int goal[2];
int vertices[ARRAYSPACE_SMALL][ARRAYSPACE_SMALL][2];
int polygonCount;                  // Number of obstacles/polygon    | I-2 count
int verticesSize[ARRAYSPACE_SMALL]; // Number of vertices per  polygon | L count
int obstacleCoordinates[X_COOR_SIZE][Y_COOR_SIZE];  // Value of 1 if there is an obstacle.

float temp_obstacle[ARRAYSPACE_BIG][2];
int temp_obstacle_count = 0;

NODE_POINTER front;
NODE_POINTER rear;

int variable1;
int variable2;

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
      //printf("Marking: %d %d\n", x1, i);
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
      //printf("%d %d %d %d", x1, y1, x2, y2);
      //printf("Slope: %f, x = %d, y = %f\n", (y2 - y1)*(1.0)/(x2 - x1), i, ((((y2 - y1)*(1.0)/(x2 - x1))*(i - x1)) + y1));
      temp_obstacle[temp_obstacle_count][0] = i;
      temp_obstacle[temp_obstacle_count][1] = ((y2 - y1)*(1.0)/(x2 - x1))*(i - x1) + y1;
      temp_obstacle_count++;
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

    k = 0;
    for(j=0; j<X_COOR_SIZE; j++){
      for(l=0; l<Y_COOR_SIZE; l++){
        if(obstacleCoordinates[j][l] == 1){
          //printf("(%d,%d)\n", j, l);
          k++;
        }
      }
    }
    //printf("\n%d - %d,%d\n", i, vertices[i][0][0], vertices[i][0][1]);
    //printf("\n%d\n", k);
  }
}

/* =============== TESTING FUNCTION =============== */
void test(){
  int choice_Q = 1;
  int input1, input2;

  // For Stack 1
  LIST_NODE* S1_current;
  NODE_POINTER S1_TOP;
  S1_TOP.node = NULL;

  // For Queue 1
  LIST_NODE* Q1_current;
  NODE_POINTER Q1_FRONT;
  NODE_POINTER Q1_REAR;
  Q1_FRONT.node = NULL;
  Q1_REAR.node = NULL;

  while(choice_Q != 0){
    printf("\n[0] Quit\n[1] Enqueue\n[2] Dequeue\n[3] See Front of Queue\n[4] See Rear of Queue\n[5] Check if Queue Empty\n[6] Push\n[7] Pop\n[8] See Top of Stack\n[9] Check if Stack Empty\nEnter choice: ");
    scanf("%d", &choice_Q);
    getchar();

    if(choice_Q == 1){
      printf("\nEnter x: ");
      scanf("%d", &input1);
      getchar();
      printf("Enter y: ");
      scanf("%d", &input2);
      getchar();
      enqueue(&Q1_FRONT, &Q1_REAR, input1, input2);
    }
    else if(choice_Q == 2){
      dequeue(&Q1_FRONT, &Q1_REAR);
    }
    else if(choice_Q == 3){
      checkFront(Q1_FRONT);
    }
    else if(choice_Q == 4){
      checkRear(Q1_REAR);
    }
    else if(choice_Q == 5){
      printf("Is Queue Empty? ");
      if(isEmptyQueue(&Q1_REAR, &Q1_FRONT) == 1)
        printf("\nYes.");
      else{
        printf("\nNo.");
      }
    }
    else if(choice_Q == 6){
      printf("\nEnter x: ");
      scanf("%d", &input1);
      getchar();
      printf("Enter y: ");
      scanf("%d", &input2);
      getchar();
      push(&S1_TOP, input1, input2);
    }
    else if(choice_Q == 7){
      pop(&S1_TOP);
    }
    else if(choice_Q == 8){
      checkTop(S1_TOP);
    }
    else if(choice_Q == 9){
      printf("Is Stack Empty? ");
      if(isEmptyStack(&S1_TOP) == 1)
        printf("\nYes.");
      else{
        printf("\nNo.");
      }
    }
  }

  /* // Para kay Stack Test
  pop(&S1_TOP);
  checkTop(S1_TOP); // Top points to NULL...
  push(&S1_TOP, 1, 2);
  checkTop(S1_TOP); // Top now points to 1,2
  push(&S1_TOP, 6, 7);
  checkTop(S1_TOP); // Top now points to 6,7
  pop(&S1_TOP);
  checkTop(S1_TOP); // Top now points to 6,7
  pop(&S1_TOP);
  pop(&S1_TOP);
  push(&S1_TOP, 5,6);
  */

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

  test();

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
    findObstacles();                     // This function maps all coordinates enclosed by the obstacles.

    if(flag == 1){
      printf("Please fix input file.\n");
    }

    /*
    for(i = 0; i < X_COOR_SIZE; i++){
      for(j = 0; j < Y_COOR_SIZE; j++){
        if(obstacleCoordinates[i][j] == 1)
          printf("%d, %d\n", i,j);
      }
    }
    */

  }
}

/* Search Algorithm from Slides
1. If GOAL?(initial-state) then return initial-state
2. INSERT(initial-node, FRINGE)
3. Repeat:
  a. If FRINGE is empty then return failure
  b. n <- REMOVE(FRINGE)
  c. s <- STATE(n)
  d. For every state s' in SUCCESSORS(s)
    i. Create a node n'
    ii. If GOAL?(s') then retur path or goal state
    iii. INSERT(n', FRINGE)

*/


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

    temp.node = (*TOP).node;
    // Let temporary node point to where the top node currently points at.

    (*TOP).node = ((*TOP).node)->next;
    // Let the top node point to the next one...

    free(temp.node);                    // Free the popped node.

    printf("\nPOPS(%d,%d)", variable1, variable2);

    return 1;
  }
}

int checkTop(NODE_POINTER TOP){
  if(TOP.node == NULL){
    printf("\nTOP points to NULL.");
    return 0;
  }

  variable1 = TOP.node->x;
  variable2 = TOP.node->y;
  printf("\nTOP points to (%d, %d)", TOP.node->x, TOP.node->y);
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

    printf("\nDEQUEUES: (%d,%d)", variable1, variable2);
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

/*Programmed by: John Louise Tan and Christopher Vizcarra*/
