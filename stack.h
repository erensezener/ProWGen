
/* 

Copyright (c) 2000 Mudashiru Busari, Department of Computer Science, University
of Saskatchewan.

Permission to use, copy, modify, and distribute this software and its documentation
for any purpose, without fee, and without written agreement is hereby granted, 
provided that the above copyright notice and the following paragraph appears in all
copies of this software.

IN NO EVENTS SHALL I BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL,
OR CONSEQUENCIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION.
THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND I HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*/



// stack.h

//This class models the finite LRU stack approach for introducing temporal locality
//into our workload. We use two different approaches to model temporal locality: Static 
//and Dynamic. The static approach seems to introduce more (but not neccessarily realistic) 
//temporal locality than the dynamic approach. This class is actually used inside stream.*
//while the requests are being generated.

#include "request.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef STACK_H
#define STACK_H

class Stack{
 public:
     
    Stack(int initSize); //this constructor will be called when the stack is operating in the 
                         //dynamic mode.

    Stack(int initSize, double initCummProb[]); //this constructor will be called when the stack is 
                             //operating in the static mode. The initCummProb is an array of the 
                             //probabilities to assign to each location in the stack 
                             
    ~Stack(); //Destructor
 
    bool LocateDoc(double randno, unsigned int noofDistinctDocs, FILE *fp); //if the randno is within range of the cummulative
                                  //probabilities of all elements on the stack, return 
                                  //the location of the document and do necessary adjustments
    
    Request* Put(Request *req); //this will put this request on the top of the stack and 
                            //may have to recompute the stack probabilities if dynamic 
                            //approach is being used
   
 private:
     void ReComputeCummProbs(); //this routine will just recompute the probabilities 
                                //associated with stack positions based on the 
                                //elements currently on the stack

    
    int LocateThisRequest(double randNum); //finds where the random no falls in the sets of 
                                           //cummulative probabilities

    bool dynamic;  //this boolean var is set to true if the stack is operating in the dynamic mode
    
    int  size;   //size of the stack
    
    int  noofElements; //current no of elements on the stack
    
    double  *cummProb; //this is a pointer to the cummulative probabilities  of all elements on the stack.
    		      //This array has size elements 
                     
    Request **bucket; //This is a pointer to pointers of all request objects currently on the stack
   
};

#endif //STACK_H

