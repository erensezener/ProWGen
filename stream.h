
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



// stream.h
//
//  This class defines a class RequestStream representing a stream of 
//  requests to be generated. 


#include "request.h"

#ifndef STREAM_H
#define STREAM_H

//This class (Node) is just for miscelaneous use
class Node{  //store a value and its cummulative probability
 public:
     int value;
     double cummProb;
};




class RequestStream {
 public:
   
    RequestStream(char* initRequestStreamFile,
		  char* initStatisticsFile, float initZipfSlope,
		  float initParetoTailIndex, int initTotalNoofRequests,
		  float  oneTimersPerc, float  distinctDocsPerc, float corr,
		  unsigned int initStacksize, unsigned int initStackmode,
		  float initPercAtTail, float initK, float initMean, float initStd); 
                    

    ~RequestStream(); //Destructor

   

    void GenerateRequestStream(); //This is the main method called from main.cc


 private:
    void OutputPopAndFileSize();
    unsigned int* GeneratePopularities(); //generates a list of popularities
                                  //following the zipf distribution. 

    unsigned int* GenerateFileSizes(); //generate file sizes from pareto & lognormal
                               //distributions
    
    Node* CDF(unsigned int *inList, int& outIndex); //produce a cummulative probability from the 
                             //input list and returns the result as another list

    void GenerateUniqueDocs(Node* popCDF, int noofItems1, Node* sizeCDF, int noofItems2);
                                   //this function will introduce either +ve,
                                   //-ve, or zero correlation between popularity 
                                   //and file sizes of documents

    unsigned int FindValue(Node *a, int n, double item);//locates where  the random number                         
                                              //is located in the list and interpolate if necessary

    void GenerateAllRequests(); //this will generate the requests into a file specified in main.cc

     void ShowProgress(int reqGeneratedSoFar); //display a progress information
                          // about the current level of the stream generation
    
    char* requestStreamFile; //the final output file containing the requests
    char* zipfFile;          //this file will contain the zipf ranking and freq
    char* statisticsFile;    //some statistics about the generated stream(pop and file size of unique docs)
    float zipfSlope;         //zipf parameter 
    float paretoTailIndex;   //pareto distribution parameter
    int   maxPopularity;     //frequency of the most popular file
    int   noofDistinctDocs;  //number of distinct documents requested
    int   totalNoofRequests; //the total no of requests generated
    int noofonetimers; //the number timers of the distinct docs in the requests stream
    float correlation; //a +ve, -ve, or zero value to indicate correlation
                       //between popularity and file size
    unsigned int stacksize;
    unsigned int stackmode;
 
    float percAtTail; //percentage of the disitinct docs at the tail of the distribution
    float K; //this indicates the beginning of the tail of the distribution
    float mean, std;  //mean and standard dev of the lognormal variate used
                      //to model the body of the file size distribution

    Request **uniqueDoc;  // the unique files in the workload
};


#endif  // STREAM_H
