/*********************************************************************************************
 * Course: CANO - Communication Networks Optimization - 2012
 * C Program: Model that satisfies demands 
 *            using paths with the minimum power consumption
 * Input: A Data file has to be given as an argument to the executable
 * Authors: Maria Stylianou
 * Creation Date: May 16, 2012
 * Last Mod Date: May 29, 2012
 *********************************************************************************************/
// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>		// For strings variables
#include <math.h>		// For maths functions
#include <time.h>		// For time()

// Constants
#define MAX_COST 99
#define NO_ELEMENTS 6
#define NO_CONSUMPTIONS 4
#define NO_LENGTHS 3
#define NO_FLAGS 7
#define LINE_SIZE 200
#define PATHS_LIMIT 20

// Functions
int dijkstra(int ***cost, int source, int target, int no_nodes);
void parseFile(FILE *fp, int *elements, float *consumptions,
	      int *lengths, int *flags, int ***dp, int ***pe, int ***nc,
	      int ***ne, int ***od, int ***cost, int ***lw);
void printTable(char *name, int **table, int lines, int columns);
void create2Dtable(int ***lw, int newLinks, int noWaves);
int satisfyDemand(int ***nc, int source, int initial, int destination, int noNodes, 
		  int noWaves, int *shortest, int *current, int *min, int *noSol,
		  int *hops,int ***solutions, float *consumptions, int oe, int *newLinks);
int searchCurrentPath(int *current,int id);
void printPath(int *current);
void addToPath(int *current, int id);
void copyPath2Sol(int *current, int *solutions,int hops, int ***nc, int noWaves, 
		  float *consumptions, int oe, int *newLinks);
void assignWavelength(int ***nc, int *shortest, int dw[][2], int curDem, int noDem, int noWaves);
/*********************************************************************************************/
/*********************************************************************************************/
// Search in the current path if the node with ID=id exists
int searchCurrentPath(int *current,int id){
  int i;
  for (i=0; current[i]!=-1; i++)
    if (id==current[i])
      return 1;
  return 0;
}
/*********************************************************************************************/
// Add node with ID=id in the current path
void addToPath(int *current, int id){
  int i=0;
  while (current[i]!=-1)
    i++;
  current[i]=id;
}
/*********************************************************************************************/
// Print current path
void printPath(int *current){
  int i=2;
  printf("Path: ");
  while (current[i]!=-1)
    printf("%d ", current[i++]);
  printf("\n");
}
/*********************************************************************************************/
// Copy current calculated path into table with solutions.
// First column keeps number of paths
// Second column keeps number of links
void copyPath2Sol(int *current, int *solutions,int hops, int ***nc, int noWaves, 
		  float *consumptions, int oe, int *newLinks){
 int i,j;
 solutions[0]=hops;
 j=0;
 while (current[j]!=-1)
 {
   solutions[j+2]=current[j];
   j++;
 }
 
 j=1; (*newLinks)=0;
 while (current[j]!=-1)
 {
    if ((*nc)[current[j-1]][current[j]]==noWaves)
      (*newLinks)++;
    
    j++;
 }    
 solutions[1]=*newLinks;		// number of new links
}
/*********************************************************************************************/
// Find the path with the less links that consume the least power for satisfying the demand
// Receives a source and a destination
// Finds all possible paths from source to destination
// Returns the path with the least power consumed (in a table with the number of nodes)
int satisfyDemand(int ***nc, int source, int initial, int destination, int noNodes, 
		  int noWaves, int *shortest, int *current, int *min, int *noSol,
		  int *hops, int ***solutions, float *consumptions, int oe, int *newLinks){
  
  int i,j, k=0;			// Counters
  int exists=0;			// Flag: 1 if node has already been visisted
  int flag=0;

  /* We are in the source-node. For each node that is connected with the source:
   * Call this function recursively, giving the current node as the initial node
   * for the new function call. 
   * Checks:
   * If the node connected to initial has already been checked
   * If the node connected to initial, is also connected to destination (then we are done)
   * If hops==0 then return -1 (no solution) 
   */
  
  //printf("INITIAL:%d\t",initial);
  for (i=0; i<noNodes; i++)
  {
    if (initial==destination)
    {	
	copyPath2Sol(current, (*solutions)[*noSol], *hops, nc, noWaves, 
		     consumptions, oe, newLinks); // Add path to solutions-table
	printf("Solution #%d\t", (*noSol)+1);
	printPath((*solutions)[*noSol]);
	
	// Delete last node
	if ((*solutions)[*noSol][1]<*min)
	{
	  *min=(*solutions)[*noSol][1];
	  k=0;
	  while ((*solutions)[*noSol][k+2]!=-1)
	  {
	    shortest[k]=(*solutions)[*noSol][k+2];
	    k++;
	  }
	  while (k<noNodes)
	  {
	    shortest[k]=-1;
	    k++;
	  }
	}
	(*noSol)++;
      
	// Empty current path array. Keep first column full (it's the source node)
	j=1;
	while (current[j]!=-1 && current[j+1]!=-1)
	  j++;
	current[j]=-1;
	return 0;
    } // end of if initial==destination
    
    if ((*nc)[initial][i]>0 && (*nc)[initial][i]<=noWaves)		// if node is connected
    {
      flag==1;
      // if node is not previously visited, then add node in the current path
      exists=searchCurrentPath(current, i);
      if (!exists)			// If node has not been visited
      {
	(*hops)++;			// Increase #hops
	//printf("nc[%d,%d] exists=%d\thops=%d\n",initial,i,exists, *hops);
	addToPath(current,i);		// Add node into the path
      }
      else
	continue;			// go to next node
      
      satisfyDemand(nc, source, i, destination, noNodes, noWaves, shortest, 
		    current, min, noSol,hops, solutions, consumptions, oe, newLinks);
      (*hops)--;
    } // end of if code is connected and there are available wavelengths
    
    if (source == initial)
    {
      // Empty current path array. Keep first column full (it's the source node)
      j=1;
      while (current[j]!=-1)
      {
	current[j]=-1;
	j++;
      }  
    } // End of source==initial
  } // End of for-loop 
  
  /* if current node has no other neighbour to go, 
   * or if his neighbours are included in the path
   * then, DELETE that node from the path
   */
  if (flag==0)
  {
    j=1;
    while (current[j]!=-1 && current[j+1]!=-1)
    j++;
    current[j]=-1;
  }
  return 0;
}
/*********************************************************************************************/
void assignWavelength(int ***nc, int *shortest, int dw[][2], int curDem, int noDem, int noWaves){
  int i=0;
  int used=0; 
  
  // Assign a random wavelength for the current demand
  do{
    used=0;
    dw[curDem][1] = rand() % noWaves;		// random wavelength
    for (i=0; i<noDem; i++)
    {
      if (i!=curDem)  
      {
	if (dw[curDem][1]==dw[i][1])
	{
	  used=1;
	}
      }
    }
  }while(used==1);

  i=0;
  // Decrease the number of available wavelengths
  while (shortest[i+1]!=-1)
  {
    (*nc)[shortest[i]][shortest[i+1]]--;
    (*nc)[shortest[i+1]][shortest[i]]--;
    i++;
  }
  printf("For Demand=%d --> Use Wavelength=%d\n", curDem, dw[curDem][1]);
}
/*********************************************************************************************/
//  parseFile(fp, elements, consumptions, lengths, flags,
	    //dp, pe, nc, ne, od, cost);
void parseFile(FILE *fp, int *elements, float *consumptions,
  int *lengths, int *flags, int ***dp, int ***pe, int ***nc,
  int ***ne, int ***od, int ***cost, int ***lw)
{
  char line[LINE_SIZE];			// use: get line from file
  int e,c,l,f,i,j;			// counters
  char dummy[10];			// use: dummy purposes
  int dump_node=-1;			// use: dumb purposes
  char *token;				// use: parse string
  char *search = "\t";		// use: parse string
  
  printf("*************************************************************\n");
  printf("READ DATA FROM FILE\n");
  printf("*************************************************************\n");
  while (fgets(line, LINE_SIZE, fp) != NULL)		// Read a line from file
  {
    // Enable corresponding flag
    if (line[0]=='/' && line[3]=='E')		// When it reads "/xxE" --> data for number of elements follow
      flags[0]=1;
    else
    if (line[0]=='/' && line[3]=='C')		// When it reads "/xxC" --> data for consumptions follow
      flags[1]=1;
    else
    if (line[0]=='/' && line[3]=='L')		// When it reads "/xxL" --> data for lengths follow
      flags[2]=1;
    else
    if (line[0]=='D' && line[1]=='P')		// When it reads "DP" --> data for table DP
    {
      flags[3]=1;
    }
    else
    if (line[0]=='P' && line[1]=='E')		// When it reads "PE" --> data for table PE
    {
      flags[4]=1;
    }
    else
    if (line[0]=='O' && line[1]=='D')		// When it reads "OD" --> data for table OD
    {
      flags[5]=1;
    }
    else
    if (line[0]=='N' && line[1]=='E')		// When it reads "NE" --> data for table NE
    {
      flags[6]=1;
    }
    
    // Execute code for corresponding flag
    /******************ELEMENTS**********************************************/
    if (flags[0]==1){				// Data for number of elements
      printf("Elements:\n");
      for (e=0; e<NO_ELEMENTS; e++){		// Read & Save them
	fgets(line,LINE_SIZE,fp);
	sscanf(line,"%s%d",dummy,&elements[e]);
	printf("%6s, elements[%d]=%d\n",dummy, e,elements[e]);
      }
      flags[0]=0;				// Disable flag
    }
    else 
    /******************CONSUMPTIONS*****************************************/
      if (flags[1]==1)				// Data for consumptions
      {
	printf("Consumptions:\n");
	for (c=0; c<NO_CONSUMPTIONS; c++){	// Read & Save them
	  fgets(line,LINE_SIZE,fp);
	  sscanf(line,"%s%f",dummy,&consumptions[c]);
	  printf("%6s, consumptions[%d]=%.4f\n",dummy, c,consumptions[c]);
	}
	flags[1]=0;				// Disable flag
      }
    else 
    /******************LENGTHS**********************************************/
      if (flags[2]==1)				// Data for lenths
      {
	printf("Lengths:\n");
	for (l=0; l<NO_LENGTHS; l++)		// Read & Save them
	{
	  fgets(line,LINE_SIZE,fp);
	  sscanf(line,"%s%d",dummy,&lengths[l]);
	  printf("%6s, lengths[%d]=%d\n",dummy, l,lengths[l]);
	}
	flags[2]=0;				// Disable flag
      }
    else
    /******************TABLE DP**********************************************/
      if (flags[3]==1)				// Data for DP table
      {
	// Create dynamically the table DP
	create2Dtable(dp, elements[4], elements[2]);
	// Save data in the table
	for (i=0; i<elements[4]; i++)
	{
	  fgets(line,LINE_SIZE,fp);
	  strtok(line, search);
	  for (j=0; j<elements[2]; j++)
	    sscanf(strtok(NULL, search), "%d", &(*dp)[i][j]);
	}
	flags[3]=0;
	strcpy(line,"\0");
      }
    else
      /******************TABLE PE**********************************************/
      if (flags[4]==1)				// Data for PE table
      {
	// Create dynamically the table PE
	create2Dtable(pe, elements[2], elements[1]);
	// Save data in the table
	for (i=0; i<elements[2]; i++)
	{
	  fgets(line,LINE_SIZE,fp);
	  strtok(line, search);
	  for (j=0; j<elements[1]; j++)
	  {
	    sscanf(strtok(NULL, search), "%d", &(*pe)[i][j]);
	  }
	}
	flags[4]=0;
      }
    else
      /******************TABLE OD**********************************************/
      if (flags[5]==1)				// Data for OD table
      {
	int num=0;
	int k=0;
	
	// Create dynamically the table OD
	create2Dtable(od, elements[4], 2);
	
	// Save data in the table
	for (i=0; i<elements[4]; i++)
	{
	  k=0;
	  fgets(line,LINE_SIZE,fp);
	  strtok(line, search);
	  for (j=0; j<elements[0]; j++)
	  {
	    sscanf(strtok(NULL, search), "%d", &num);
	    if (num==1)
	      (*od)[i][k++]=j;
	  }
	}
	flags[5]=0;
      }
    else
      /******************TABLE NE**********************************************/
      if (flags[6]==1)				// Data for NE table
      {
	// Create dynamically the table NE
	create2Dtable(ne, elements[0], elements[1]);
	
	// Save data in the table
	for (i=0; i<elements[0]; i++)
	{
	  fgets(line,LINE_SIZE,fp);
	  strtok(line, search);
	  for (j=0; j<elements[1]; j++)
	    sscanf(strtok(NULL, search), "%d", &(*ne)[i][j]);
	}
	/*******************TABLE NC******************************************/
	/* Create a table for nodes connected */
	create2Dtable(nc, elements[0], elements[0]);
	
	// Initialize table NC with maximum cost of link
	// link with MAX_COST is translated like it does not exist in our structure
	for (i=0; i<elements[0]; i++)
	  for (j=0; j<elements[0]; j++)
	    (*nc)[i][j]=MAX_COST;
	  
	for (j=0; j<elements[1];j++) 	// for each link
	{
	  dump_node=-1;
	  for (i=0; i<elements[0]; i++) // for each node
	  {
	    if ((*ne)[i][j]==1) 	// if there is 1 in the table
	    { 
	      if (dump_node!=-1){
		(*nc)[i][dump_node]=elements[3];	// save in the table of nodes 1 for the ndes
		(*nc)[dump_node][i]=elements[3];
		break;
	      }
	      dump_node=i;
	    }
	  }
	}
	flags[6]=0;
      }
  } /*************************** end of while ***********************************/  
  // Create & Initialize a table for links and wavelengths.
  create2Dtable(lw, elements[1], elements[3]);
  for (i=0; i<elements[1]; i++)
    for (j=0; j<elements[3]; j++)
      (*lw)[i][j]=0;
    
  printTable("DP", *dp, elements[4], elements[2]);
  printTable("PE", *pe, elements[2], elements[1]);
  printTable("OD", *od, elements[4], 2);
  printTable("NE", *ne, elements[0], elements[1]);
  printTable("NC", *nc, elements[0], elements[0]);
  
}
/*********************************************************************************************/
// Create dynamicly a 2D table
void create2Dtable(int ***lw, int newLinks, int noWaves){
  int i;
  
  // Create dynamically the table LW
  *lw = (int **)malloc(sizeof(int*)*newLinks);
  for (i=0; i<newLinks; i++)
    (*lw)[i] = (int *)malloc(sizeof(int)*noWaves);	
}
/*********************************************************************************************/
void printTable(char *name, int **table, int lines, int columns){
   int i,j;
  printf("%s=\n\t      ",name);
  for (j=0; j<columns; j++)
     printf("%3d",j);
   printf("\n\n\t");
  for (i=0; i<lines; i++)
  {
    printf("%3d   ",i);
    for (j=0; j<columns; j++)
      printf("%3d",table[i][j]); 
    printf("\n\t");
  }
  printf("\n");
}
/*********************************************************************************************/
// MAIN
main(int argc, char *argv[]){
  FILE *fp;				// file pointer
  int i,j,k;				// counters
  int elements[NO_ELEMENTS];		// input data for numbers of elements
  float consumptions[NO_CONSUMPTIONS];	// input data for consumptions
  int lengths[NO_LENGTHS];		// input data for lengths
  int flags[NO_FLAGS];			// use: flag variables for different occasions
// flags[0]: for numbers of elements 	// flags[1]: for consumptions
// flags[2]: for lengths		// flags[3]: for DP table
// flags[4]: for PE table		// flags[5]: for OD table
// flags[6]: for NE table
  int **dp;				// table DP: paths per Demand
  int **pe;				// table PE: links per Path
  int **nc;				// table NC: nodes connected
  int **ne;				// table NE: links per Node
  int **od;				// table OD: source and destination of demand. 
					// input data to be extracted from the input file
  int **cost;				// table COST: cost of each link of the system
  int **lw;				// table LW: links-wavelengths
  int *shortest;			// shortest: shortest path for satisfying a demand
  int **wl;				// table WL: 
  int *current;				// table current: contains nodes of current path
  int min=20;
  int noSol=0;
  int hops=0;
  int curDem=0;    
  int oe=0;
  int newNodes=0;				
  int newLinks=0;
  float powerNodes=0, powerLinks=0;	// Power for added nodes and links for satisfying a demand
  float powerWaves=0;			// Power for added wavelengths for satisfying a demand (always 1)
  float powerTransponders=0;		// Power consumptions in transponders
  float power=0,totalPower=0;		// Power consumed for a new demand and power in total
  struct timeval tim;
  double timeStart, timeEnd; 		// Time: start and end
  double execTime = 0; 			// Operation Lattency
  int **solutions;
  
  // Check usage validity
  if ( argc != 2 ) 		/* 1 argument should be given - file's name */
  {
    printf("Error: usage: %s filename\n", argv[0] ); /* argv[0] holds the program name */
    printf("Exiting. . .\n");
    exit(-1);
  }
  
  // Check file validity
  fp = fopen( argv[1], "r" );
  if (fp == 0)
  {
    printf("Error: fopen: Could not open file\n" );
    printf("Exiting. . .\n");
    exit(-1);
  }
    
  // Tables Initializations
  for (i=0; i<NO_ELEMENTS; i++)
    elements[i]=0;
  for (i=0; i<NO_CONSUMPTIONS; i++)
    consumptions[i]=0;
  for (i=0; i<NO_LENGTHS; i++)
    lengths[i]=0;
  for (i=0; i<NO_FLAGS; i++)
    flags[i]=0;
  
  parseFile(fp, elements, consumptions, lengths, flags,
	    &dp, &pe, &nc, &ne, &od, &cost, &lw);
  
  oe=(lengths[1]/lengths[2])+2;
  int nu[elements[0]];				// table NU: nodes used - flags
  // initialiazation of array with nodes used
  for (j=0; j<elements[0]; j++)
    nu[j]=0;
      
  // Table demand-wavelength
  // It shows which wavelength is assigned to each demand
  int dw[elements[4]][2];	
  
  // Create & Initialize table of demands satisfied
  int satisDem[elements[4]];
  for (i=0; i<elements[4]; i++)
    satisDem[i]=0;
  
  /* Array with nodes of the current traversed path */
  current=(int *)malloc(sizeof(int)*PATHS_LIMIT);
  for (i=0; i<PATHS_LIMIT; i++)
    current[i]=-1;
  
  /* Shortest array keeps the nodes included in the shortest path 
   *from one source to another */
  shortest = (int *)malloc(sizeof(int*)*elements[0]);
  for (i=0; i<elements[0]; i++)
    shortest[i]=-1;
  
  printf("*************************************************************\n");
  printf("SATISFY DEMANDS\n");
  printf("*************************************************************\n");
  
  create2Dtable(&solutions, PATHS_LIMIT, PATHS_LIMIT);


  gettimeofday(&tim, NULL); /* Current Time - 1st */
  timeStart=tim.tv_sec+(tim.tv_usec/1000000.0);
  
  // Run Function for satisfying each demand
  for (i=0; i<elements[4]; i++)
  {
    newNodes=0; newLinks=0;
    // initialiazation of array
      for (j=0; j<PATHS_LIMIT; j++)
	for (k=0; k<PATHS_LIMIT; k++)
	  solutions[j][k]=-1;
      
    do{
      curDem=rand() % elements[4];
      if (satisDem[curDem]==0)
      {
	satisDem[curDem]=1;
	break;
      }
    }while (satisDem[curDem]==1);
  
    current[0]=od[curDem][0];
    printf("*************************************************************\n");
    printf("Demand #%d: FROM Source=%d TO Destination=%d\n", curDem, od[curDem][0], od[curDem][1]);
    printf("*************************************************************\n");
    satisfyDemand(&nc, od[curDem][0], od[curDem][0], od[curDem][1], elements[0], 
		elements[3], shortest, current, &min, &noSol, &hops, &solutions, 
		consumptions, oe, &newLinks);
    printf("Less Expensive Path : ");
    powerTransponders=elements[5]*lengths[0]*consumptions[1];
    j=0;
    while (shortest[j]!=-1)
    {
      if(nu[shortest[j]]!=1)
      {
	newNodes++;
	nu[shortest[j]]=1;
      }
      
      printf("%d ",shortest[j]);
      j++;
    }
    printf("\n");
    assignWavelength(&nc, shortest, dw, curDem, elements[4], elements[3]);
    printTable("NC", nc, elements[0], elements[0]);
     noSol=0; hops=0;
    for (j=0; j<elements[0]; j++)
      shortest[j]=-1;
    for (j=0; j<PATHS_LIMIT; j++)
      current[j]=-1;
    
    powerLinks=min*oe*consumptions[3];
    powerNodes=newNodes*consumptions[0];
    powerWaves=consumptions[2];
    power=powerNodes+powerLinks+powerWaves;
    totalPower+=power;
    
    printf("New Nodes=%d, PowerNodes=%.4f\n",newNodes, powerNodes);
    printf("New Links=%d, PowerLinks=%.4f\n", min, powerLinks);
    printf("New Wave=1, PowerWaves=%.4f\n",powerWaves);
    printf("Additional Power=%.4f\n",power); 
    printf("Total Power=%.4f\n",totalPower);
    min=20;
  } //End of for each demand
  printf("*************************************************************\n");
  printf("\nTotal Power=%.4f\n",totalPower);
  printf("Constant PowerTransponders=%.4f\n",powerTransponders);
  totalPower+=powerTransponders;
  printf("Total Power with Transponders=%.4f\n",totalPower);
  gettimeofday(&tim, NULL); 			// Current Time - 2nd
  timeEnd=tim.tv_sec+(tim.tv_usec/1000000.0);
  execTime=timeEnd-timeStart;
  execTime*=1000;				// From seconds to milliseconds
  printf("Total Execution Time: %.3f ms.\n\n",execTime);
  printf("*************************************************************\n");

}
