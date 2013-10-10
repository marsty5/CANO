/*****************************************************************
 * Course: CANO - Communication Networks Optimization - 2012
 * OPL 12.4 Model: Satisfies all demands given by minimizing the 
 *		   power consumption 
 * Input: A Data file has to be given along the model
 * Authors: Maria Stylianou, João Rosa, Zafar Gilani
 * Creation Date: May 1, 2012
 * Last Mod Date: May 15, 2012
 ****************************************************************/

 // Ranges and Sets
int nN=...;				// Set of Nodes
int nL=...;				// Set of Links
int nP=...;				// Set of Paths
int nW=...;				// Set of Wavelengths
int nD=...;				// Set of Demands
int oaNum=...;

int en=...;
float et=...;
float ew=...;
int eoa=...;

int Dist=...;	
int le=...;
int loa=...;

range N=1..nN;
range L=1..nL;
range P=1..nP;
range W=1..nW;
range D=1..nD;

int h[p in P]=...;	
int DP[d in D, p in P]=...;		// Paths p are candidates for demand d
int PE[p in P, e in L]=...;		// Path p uses links e
int OD[d in D, n in N]=...; 	// Demand d has source and destination nodes	  
int NE[n in N, e in L]=...;		// Node n has links e attached to it

float oe=floor(le/loa)+2;		// Amount of amplifiers between each link

// Variables
dvar boolean E[e in L];			// Binary Var; if link e is used => 1
dvar boolean X[n in N];			// Binary Var; if node n is equiped => 1
dvar boolean Xs[p in P, w in W];// Binary Var; if node of path p and wavelength w 
								// is equiped => 1
dvar boolean y[e in L, w in W]; // Binary Var; if link e and wavelength w is used

// Objective Function
minimize (sum(e in L) E[e]*oe*eoa +
		  sum(n in N) X[n]*en +
		  sum(d in D) sum (p in P) sum(w in W) DP[d,p] * Xs[p,w] * h[p] * ew
		 );

// Constraints
subject to {
  forall (d in D)
    sum (p in P) sum(w in W) DP[d,p] * Xs[p,w] == 1;
  forall (e in L, w in W)  
	sum (p in P) PE[p,e] * Xs[p,w] == y[e,w];
 forall (e in L)
    sum(w in W) y[e,w] <= nW * E[e];
  forall (n in N)
    sum(e in L) NE[n,e] <= nL * X[n];
}

// Post-process
// Addition of the constant values to the minimized value
execute{
    var sum1 = 0;
    var sum2 = 0;
    var sum3 = 0;
    
	for(var e in L) sum1 += E[e] * oe * eoa;
	for(var n in N) sum2 += X[n] * en;
	for(var d in D) for(var p in P) for(var w in W) sum3 += DP[d][p] * Xs[p][w] * h[p] * ew;

	var pInitial = sum1 + sum2 + sum3;
	var pTotal = pInitial + oaNum*Dist*et + Dist*ew;

	writeln("Objective Function Result: " + pInitial);
	writeln("Total Minimized Amount of power: " + pTotal);
}
