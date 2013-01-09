


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



//This file contains the main program with sample configuration of parameters
//needed to synthesize a typical proxy workload. However, you can modify this 
//particular file such that a separate configuration file exists from which values
//for these parameters can be read. This way, multiple configuration files can 
//be passed in turn to the program.

#include "stream.h"
#include <iostream>
#include <stdlib.h>
#include <cstdio>

int main(int argc, char *argv[])
{

//This file will contain the requests stream to generate. A two-culumn file with
//fileid followed by file size

char* requestStreamFile = "test-requests3.dat"; 

//this file will be used to store information about unique files in the workload.
//it is also a 2-column file with each line representing a unique file in the 
//workload. The first column is the popularity and the second column is the 
//file size. The first column of the two columns could be extracted later for generating 
//and estimating popularity (Zipf) slope, and the 2nd column for file size distribution 
//curves or LLCD plot for estimating tail index slope. 

char* statisticsFile = "test-uniquedocs3.dat";


//this is the desired Zipf slope of the workload, usually within 0 and 1.0
float zipfSlope = 0.75; 

//the heavy tail index of the workload, usually within 0 and 2.0
float paretoTailIndex = 1.2; 

//the total number of requests desired in the workload
int totalNoofRequests =  200000;

//percentage of distinct documents of total number of requests. This is a typical value, but usually
//it is between 25% to 40%. If you are synthesizing a workload to mimic an empirical one, you may 
//need to measure this value out and set this variable appropriately.
float distinctDocsPerc = 30;


//percentage of 1-timers desired out of the distinct documents. For proxy workload it is between 
//50-70%. You are free to set this value at whatever value you want of course.
float oneTimerPerc = 70; 

//This allows correlation to be introduced between popularity and file size. A value of 0 will 
//introduce the normal close-to-zero correlation. Set this value at -1 if you want negative correllation,
//or 1 if you want positive correlation. Note that the correlation introduced will not be exactly
//0, -1, or 1, but will be close in each case.
float correlation = 0; 

//This is the size of the LRU stack used to introduce temporal locality. Larger values will be take 
//more computational time due to movement of elements of the stack.
unsigned int stacksize = 1000; 

//There are two stack modes used to introduce temporal locality: Static and Dynamic. If you want static 
//configure this variable to be 0, else use 1. Workloads with static seems to introduce more temporal 
//locality than dynamic. The dynamic is more representative of empirical ones. So, to mimic an empirical
//workload, use dynamic.
unsigned int stackmode = 0; 

//this is the percentage of documents at the tail of the file size distribution, out of the distinc
//documents
float percAtTail = 20; 

//the beginning of the tail 
float K = 10000;

//this is the mean and standard deviation of the lognormal values used to model the body of the file
//size distribution. If you are mimicking an empirical workload and the file size distribution does 
//not match very well, you might need to adjust these values, otherwise these values seem to work
//okay.
float mean =  14000;
float std = 22000;


//declare a request stream object (see stream.h and .cc)
RequestStream*  proxyWorkload;


//call the constructor with the parameters initialized above
proxyWorkload = new RequestStream(requestStreamFile,statisticsFile,
 	  zipfSlope,paretoTailIndex,totalNoofRequests, oneTimerPerc, 
	  distinctDocsPerc, correlation, stacksize, stackmode, percAtTail,
	  K, mean, std);
 

//call the method that will generate the workload
proxyWorkload->GenerateRequestStream();
  

//done with this object.
delete proxyWorkload;

}
                  


