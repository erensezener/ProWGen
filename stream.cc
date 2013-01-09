
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



// stream.cc
//
//  This file defines the class RequestStream representing a stream of 
//  requests to be generated. 

#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <math.h>
#include "stream.h"
#include "stack.h"
#include <time.h>

int compare(int *x, int *y); //used to sort in ascending order
int compare2(Request **x, Request **y); //used to sort in descending order



//----------------------------------------------------------------------
// RequestStream::RequestStream
//   this is the constructor for the class. It initializes all the file
//   names necessary for keeping the results. 
//----------------------------------------------------------------------


RequestStream::RequestStream(char* initRequestStreamFile,  
		  char* initStatisticsFile, float initzipfSlope,
		  float initparetoTailIndex, int initTotalNoofRequests,
		  float onetimerperc, float distinctDocsPerc, float corr,
		  unsigned int initStacksize, unsigned int initStackmode,
		  float initPercAtTail, float initK,  float initMean, float initStd)
{
 requestStreamFile = initRequestStreamFile;
 statisticsFile    = initStatisticsFile;

 zipfSlope         = initzipfSlope;
 paretoTailIndex       = initparetoTailIndex;
 totalNoofRequests = initTotalNoofRequests;
 noofDistinctDocs = (int) (distinctDocsPerc/100.0 * totalNoofRequests);
 noofonetimers = (int)(onetimerperc*noofDistinctDocs/100.0);
 correlation = corr;
 stacksize = initStacksize;
 stackmode = initStackmode;
 percAtTail = initPercAtTail;
 K = initK;
 mean = initMean;
 std = initStd;
}
                    


//----------------------------------------------------------------------
// RequestStream::~RequestStream
//   this is the destructor for the class. 
//----------------------------------------------------------------------

RequestStream::~RequestStream()
{
   delete [] uniqueDoc;
}


//----------------------------------------------------------------------
// RequestStream::GeneratePopularities
//     generates a list of file  popularities to follow Zipf distribution.
//     the popularities will be stored in ascending order in a list. 
//----------------------------------------------------------------------
unsigned int*
RequestStream::GeneratePopularities()
{
  unsigned int *popularity;  //pointer to a list of integers  representing the pops
  unsigned short int  seed[] = {5, 7, 9};   

  if ((popularity = new unsigned int[noofDistinctDocs]) ==  NULL) 
   {
      printf("Error allocating memory in GeneratePopularities\n");
      exit(1);
   }

 //estimate k in the zipf formula. The middle one timer has a popularity of 
 //1, hence it can be used to estimate k
 float k = pow(noofDistinctDocs - noofonetimers/2, zipfSlope);


 //first deal with the non 1-timers 
 int popularitySum = 0; //sum of the total popularity
 for (int rank =1; rank<=noofDistinctDocs - noofonetimers; rank++)
   {
	 int freq = (int)(k / pow(rank, zipfSlope));
	 if (freq < 2)  
	   freq = 2; //do not allow any 1-timer here
	 popularity[rank-1] = freq;
	 popularitySum += freq;
    }



 //now assign the popularities of the 1-timers at the end of the array
 for (int i =  noofDistinctDocs - noofonetimers; i < noofDistinctDocs; i++)
   popularity[i] = 1;


 //if there is a difference between sum of popularities and  the desired noof requests 
 //then we need to adjust the popularities by scaling either upward or downward

 if ((totalNoofRequests - popularitySum - noofonetimers) != 0)
   {

     float scalingFactor = (float)(totalNoofRequests - noofonetimers)/popularitySum;
  
     for (int i=0; i < noofDistinctDocs - noofonetimers; i++)
       {  
	 popularity[i] = (int)(popularity[i] * scalingFactor);  
	 if (popularity[i] < 2)  popularity[i] = 2; //do not allow 1-timers
        } 
   }

 
 //sort b4 returning the array into ascending order 
  qsort(popularity, noofDistinctDocs, sizeof(int), (int(*)(const void *, const void *))compare);
 
   
  return popularity;   
}


//----------------------------------------------------------------------
// RequestStream::GenerateFileSizes
//     generates file sizes for the tail of the distribution using 
//     pareto distribution while lognormal distribution is used for the 
//     body of the distribution
//----------------------------------------------------------------------

unsigned int*
RequestStream::GenerateFileSizes()
{
    float lognormMeanSqr = pow(mean, 2);
    float lognormVariance= pow(std, 2);

    float paramMean = log( lognormMeanSqr / sqrt(lognormVariance + lognormMeanSqr));
    float paramStd  = sqrt(log((lognormVariance + lognormMeanSqr) / lognormMeanSqr));

    unsigned int *filesizes = new unsigned int[noofDistinctDocs];
    
    unsigned int noAtTail = (unsigned int)(percAtTail*noofDistinctDocs/100); //to be calculated
     
    bool usedFlag = true;
    
    double u1, u2, v1, v2, w, y, x1, x2; //params of the normal variate
    
    double normalVariate; //the lognormal variate
    
    unsigned short seed[3] = {5,7,9}; //passed to the random number function 
    
    unsigned int size; //filesize for a request
  
   //first generate the body using lognormal distribution
    unsigned int counter=0;
    while (counter < (noofDistinctDocs - noAtTail))
    {
        if (usedFlag) //are the 2 previous normal variates used? must enter this loop first time!
	   {    
	     //then generate 2 new normal variates
	     usedFlag = false;;
	     w = 2.0;
	     while (w > 1)
	     {
		 u1 = erand48(seed); //u1 & u2 are IID U(0,1)
		 u2 = erand48(seed);
		 v1 = 2*u1 - 1;
		 v2 = 2*u2 - 1;
		 w = v1*v1 + v2*v2;
	     }
	     y  = pow((-2*log(w)/w), 0.5);
	     x1 = v1*y;  //x1 & x2 are IID N(0,1) random variables
	     x2 = v2*y;
	     
	     normalVariate = x1;
	   }
	  else //second normal variate of the previous loop has not been used
	  {
	      normalVariate = x2;
	      usedFlag = true;
	  }

     //now generate a file size from the log-normal distribution and 
     //do not allow sizes that is as big as K, where the tail should start from
     size = (int)(exp(paramMean + paramStd * normalVariate));
     if (size < K)  
       {
	 filesizes[counter] = size;
	 counter++;
       }
   }

    //now generate the file sizes at the tail using Pareto disallowing files
    //larger than 50K
    counter = noofDistinctDocs-noAtTail; 
    while  (counter< noofDistinctDocs) 
     { 
	size = (int)(K / pow(erand48(seed), (1/paretoTailIndex)));
	if (size < 50*1024*1024)  //disallow files more than 50K
	  {
	    filesizes[counter] = size;
	    counter++;
	  }
     } 

  //sort in ascending order before returning 
  qsort(filesizes, noofDistinctDocs, sizeof(int), (int(*)(const void *, const void *))compare);
 
  return filesizes;
}



//----------------------------------------------------------------------
// RequestStream::GenerateAllRequests
//    this generates the requests stream using either static/dynamic finite
//    LRU stack models. The request is stored in the file specified as 
//    input to the workload generator. The file has 2 columns: the first 
//    column is the fileId and the second column is the file size.
//----------------------------------------------------------------------

void
RequestStream::GenerateAllRequests()
{

 //This part generate references based on static or dynamic models 

 Stack *stack;
 if (stackmode == 0)  //static model desired
    {
	double *cummProb = new double[stacksize];  //do not delete this array, it is passed to the stack class
	double sum = 0;

	//the stack size cannot be more than the unique docs we have
	if (stacksize > noofDistinctDocs)
	  stacksize = noofDistinctDocs;

	for (int i=0; i<stacksize; i++)
	{
	  sum += uniqueDoc[i]->GetProb();
	  cummProb[i] = sum;
	}
	
      	stack = new Stack(stacksize, cummProb);
     }
 else   //dynamic model desired
    stack = new Stack(stacksize);

 unsigned int totalReqGenerated = 0;
 unsigned short seed[3] = {1,2,3}; 
 double randNum;
 unsigned int index;

 //open the request stream file for writing

 FILE *fp = fopen(requestStreamFile, "w");

 while (totalReqGenerated < totalNoofRequests)
 {  

     randNum = erand48(seed);
    
      if (!stack->LocateDoc(randNum, noofDistinctDocs, fp))
       {
	 if (noofDistinctDocs > 0)
	   {
	     index = (unsigned int)(noofDistinctDocs*erand48(seed)); 
	    
	     Request *req = uniqueDoc[index];
	     fprintf(fp,"%10u %10u\n",req->GetFileId(),req->GetFileSize());
	     totalReqGenerated++;
	     req->DecFreq();
	     uniqueDoc[index] = uniqueDoc[noofDistinctDocs-1];
	     noofDistinctDocs--;
	     if (req->GetFreq() == 0)
	       delete req;
	     else
	       {
		 Request *temp = stack->Put(req);
		 if (temp != NULL)
		   {
		     uniqueDoc[noofDistinctDocs] = temp;
		     noofDistinctDocs++;
		   }
	       }
	   }
       }
   else
      totalReqGenerated++;
	
   ShowProgress(totalReqGenerated);
     

 }//endwhile

 fclose(fp);
 delete stack;
 //all requests have been generated into the request stream file. Note the total request
 //generated may not be the same as desired but should be close enough to within 10% difference
 
 printf("Total request generated : %d\n", totalReqGenerated);

}


//----------------------------------------------------------------------
// RequestStream::ShowProgress
//    display a progress information about the current 
//    level of the stream generation
//----------------------------------------------------------------------

void
RequestStream::ShowProgress(int reqGeneratedSoFar)
{
 printf("\r%d %%  Complete",reqGeneratedSoFar*100/totalNoofRequests); 
}


//----------------------------------------------------------------------
// RequestStream::GenerateUniqueDocs
//   this function will introduce either +ve, -ve, or zero correlation 
//   between popularity and file size. It takes 2 lists--a list storing
//   a set of popularities and and their cummulative probabilities and
//   the second one stores the filesizes and their cummulative probs. For 
//   each unique request, a popularity is generated as well as the corres-
//   ponding filesize. A uniform random number is generated which is used
//   to search the popularity list for the corresponding popularity(applying
//   interpolation if necessary). For the file size, the same random no 
//   is used to search the filesize list if +ve correlation is desired, or
//   for negative correlation the random no is subtracted from 1 before 
//   being used to search the filesize list. For zero correlation, another 
//   random no is generated for the filesize search. Note that the new 
//   list being created must be in descending order of popularity for our
//   temporal locality proc to work correctly
//----------------------------------------------------------------------

void 
RequestStream::GenerateUniqueDocs(Node* popCDF, int noofItems1,
 Node* sizeCDF, int noofItems2)
{
 uniqueDoc = new Request*[noofDistinctDocs];
 if (uniqueDoc == NULL)
 {
   printf("Error allocating memory in function GenerateUniqueDocs \n");
   exit(1);
 }
 
 short unsigned int seed[] = {5, 7, 9};
 unsigned int total = 0;
 unsigned int filesize, filepop;

 if (correlation == 0)
 {
     //first generate the popularities
     for (unsigned int count=0; count < noofDistinctDocs; count++)
      {
	  filepop = FindValue(popCDF, noofItems1, erand48(seed));
	  total += filepop;
	  Request *request = new Request(filepop, 0); //sets the file size to 0 here
	  uniqueDoc[count] = request;
      }
     
     //then generate the file sizes without correlation but disallowing files 
     //bigger than 10K to have popularity >=100
     unsigned int count =0;
     while (count < noofDistinctDocs)
     {
       filesize = FindValue(sizeCDF, noofItems2, erand48(seed));
        if ((filesize < 10*1024) || (uniqueDoc[count]->GetFreq() < 100))
	 {
          uniqueDoc[count]->SetFileSize(filesize);
	  count++;
	}
     }
 }
 else  // +ve or -ve correlation
 {
  for (unsigned int count=1; count <= noofDistinctDocs; count++)
   {
     double randnum = erand48(seed);
     filepop = FindValue(popCDF, noofItems1, randnum);
     total += filepop;
     if (correlation > 0)     //+ve correlation, so use the same random no
	filesize = FindValue(sizeCDF, noofItems2, randnum);
     else if(correlation < 0) //-ve correlation, so use 1-random no
	filesize = FindValue(sizeCDF, noofItems2, 1-randnum);
     
     //create a unique request object and then insert it into the unique doc list
     Request *request = new Request(filepop, filesize);
     uniqueDoc[count-1] = request;
   }
 }


 //we have to sort them first in descending order of popularity
 qsort(uniqueDoc, noofDistinctDocs, sizeof(Request*), (int(*)(const void *, const void *))compare2);

 if ((totalNoofRequests - total) != 0)
   {
   
     float scalingFactor = (float)(totalNoofRequests - noofonetimers)/(total - noofonetimers);
     total = noofonetimers; //recalculate total noof requests 
     for (int i=0; i < noofDistinctDocs - noofonetimers; i++)
       {  
	 int freq  = (int)(uniqueDoc[i]->GetFreq() * scalingFactor);  
	 if (freq < 2)  
	   uniqueDoc[i]->SetFreq(2); //do not allow more 1-timers
	 else  
	   uniqueDoc[i]->SetFreq(freq);
	 total += freq;
        } 
   }

 //assign the file ids now starting form 0
 for (int i=0; i< noofDistinctDocs; i++)
   uniqueDoc[i]->SetFileId(i);
 

 //now set the cummulative probability of each request based on its popularity
 for (unsigned int j= 0; j < noofDistinctDocs; j++)
   {
     uniqueDoc[j]->SetProb((double)uniqueDoc[j]->GetFreq()/ total);
   }

 totalNoofRequests = total; //this is the new total noof requests that will be generated 

}




//----------------------------------------------------------------------
// RequestStream::CDF
//      produce a cummulative distribution values from the input
//      list of integers, which is already in ascending order. inList is 
//      input array, outIndex is the number of items in the resulting CDF
//      values
//----------------------------------------------------------------------

Node*
RequestStream::CDF(unsigned int *inList, int& outIndex)
{
 if (inList == NULL)
  {
     printf("The list passed to CDF is null");
     exit(1);
  }

 Node *outList = new Node[noofDistinctDocs]; //each node in this new list will 
                   //contain each unique element and its cummulative probability
 
 if (outList == NULL) 
  {
     printf("Error allocation memory in CDF\n");
     exit(1);
  }

 int currVal, nextVal; 

 currVal = inList[0];        //current value is the first value in the list
 outIndex = 0;               //index of where unique values will go in outList
 int inIndex;
 for (inIndex = 1; inIndex < noofDistinctDocs; inIndex++) 
  {
    nextVal = inList[inIndex];
    if (nextVal != currVal) //we have come to the end of prev unique item
      {
        outList[outIndex].value = currVal;
	outList[outIndex].cummProb = (double)(inIndex)/noofDistinctDocs;
	outIndex++;  
	currVal = nextVal; //start a new set of values
      }
  }

 //the last item in the list must be  
 outList[outIndex].value = currVal;
 outList[outIndex].cummProb = (double)(inIndex)/noofDistinctDocs;
 outIndex++;   //this is the total no items in outList;

 //now copy the exact no of items into a new list and return it
 Node* newOutList = new Node[outIndex];
 if (newOutList == NULL)
 {
  printf("Error allocating memory in CDF\n"); 
  exit(1);
 }

 //copy outList to newOutList before returning the pointer to it
 for (int i=0; i < outIndex; i++)
     newOutList[i] = outList[i];
 delete [] outList;
 
 return newOutList;
}



//----------------------------------------------------------------------
// RequestStream::FindValue
//      search the input list for where the input randnum lies and 
//      interpolate if necessary
//----------------------------------------------------------------------

unsigned int 
RequestStream::FindValue(Node* a, int n, double item)
{
//n is the no of items in this list
 
 int l = 0;
 int r = n-1;
 bool done = false;
 int loc;
 while (!done)
 {
  int mid = (l+r)/2;
  if ((item <= a[mid].cummProb && mid == 0) || 
                       (item <= a[mid].cummProb && item > a[mid-1].cummProb))
  {
     loc = mid;
     done = true;
  }
  else if (item > a[mid].cummProb && ((mid +1) == r))
  {
      loc = mid+1; 
      done = true;
  }
  else if (item  > a[mid].cummProb)
      l = mid;
  else
      r = mid;
 }
  
  if ((item == a[loc].cummProb) ||  (loc == 0) )
    return a[loc].value;
  //else interpolate
  int value = (int) ceil((((item - a[loc-1].cummProb)*(a[loc].value - 
   a[loc-1].value)) / (a[loc].cummProb - a[loc-1].cummProb)) + a[loc-1].value);
 
 return value;
  
}


//----------------------------------------------------------------------
//RequestStream::OutputPopAndFileSize
//     this outputs the info about the distinct files in the workload into
//     a file. The info has only 2 column. The first column is the popularity
//     of each distinct file arranged in descending order. This could easily 
//     be extracted and used to plot the popularity ranking graph. The second 
//     column is the file size of each distinct file in the workload, but is
//     not sorted. You may have to sort it when you extract that column to 
//     plot CDF or LLCD stuffs. 
//----------------------------------------------------------------------
void 
RequestStream::OutputPopAndFileSize()
{ 
  FILE *fp = fopen(statisticsFile, "w");
  for (int i=0; i<noofDistinctDocs; i++)
      fprintf(fp, "%u %u\n",uniqueDoc[i]->GetFreq(),uniqueDoc[i]->GetFileSize());
  fclose(fp);
}


//This function is passed to the qsort routine to sort in ascending order
int compare(int *x, int *y)
{
 if ((*x) < (*y))
     return -1;
 else if ((*x) > (*y))
     return 1;
 else 
     return 0;
}


//This function sorts an array of requests object in descending order based on frequency
int compare2(Request **x, Request **y) //reverse sorting 
{
 
 if ( (*x)->GetFreq() < (*y)->GetFreq() )
     return 1;
 else if ( (*x)->GetFreq() > (*y)->GetFreq() )
     return -1;
 else 
     return 0;
}



//----------------------------------------------------------------------
// RequestStream::GenerateRequestStream
//      this is the main routine called after creating an object of this
//      class. This method calls all other methods as necessary to 
//      generate the request stream. Generating a requests stream is done
//      in several stages. Refer to the thesis to understand how
//----------------------------------------------------------------------

void 
RequestStream::GenerateRequestStream()
{

 //first generate a set of popularities for the number of distinct files in the 
 //workload and store it in an array. Note that there is just a pointer to this array

 printf("...generating starting popularities\n");
 unsigned int *popularities = GeneratePopularities();

 //generate the Cumulative distribution values from the popularities. Each value
 //in the popularityCDF array has two values: a unique value and the cumm. freq.
 //All the values are stored in the popularityCDF array as shown below which just
 //one pointer to the whole array. The array has noofElement1 elements.

 printf("...generating CDF for popularities\n"); 
 int noofElement1; //the total no of elements in CDF values
 Node *popularityCDF = CDF(popularities, noofElement1);
 delete [] popularities;  //we don't need this array again


 //Do the same above for file sizes.

 printf("...generating starting file sizes\n");
 unsigned int *filesizes = GenerateFileSizes(); 
 
 printf("...generating CDF for file sizes\n");
 int noofElement2; //the total no of elements in CDF values
 Node *filesizeCDF = CDF(filesizes, noofElement2);
 delete [] filesizes; //we don't need this array again


 //Now, use the CDFs to generate unique file sizes and popularities and introduce the 
 //desired correlation. The generated info about the unique docs in the workload will be
 //stored in an array called uniquedocs, which is an instance variable in this class.

 printf("...generating popularities & file sizes for the distinct requests with correlation\n");
 GenerateUniqueDocs(popularityCDF, noofElement1, filesizeCDF, noofElement2);


 //we don't need these arrays again. We have used them to generate the info about uniqued
 //documents in the workload.

 delete [] popularityCDF;
 delete [] filesizeCDF;

 //writes the info about each unique docs into a file. The info has only 2 column. The first
 //column is the popularity of each distinct file arranged in descending order. This could 
 //easily be extracted and used to plot the popularity ranking graph. The second column is the 
 //file size of each distinct file in the workload, but is not sorted. You may have to sort it 
 //when you extract that column to plot CDF or LLCD stuffs. 

 OutputPopAndFileSize();


 //Now, call the routine that will now generate the workload from the info already computed 
 //about each distinct file using the LRU stack approach. The request is stored in the file
 //specified as input to the workload generator. The file has 2 columns: the first column is
 //the fileId and the second column is the file size.
 
 printf("Finally, now genarating the requests\n");
 GenerateAllRequests();


 //Now all requests have been generated  
 printf("\n\nDone\n\n");
 
}

