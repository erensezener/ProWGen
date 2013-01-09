
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



// stack.cc

//This class models the finite LRU stack approach for introducing temporal locality
//into our workload. We use two different approaches to model temporal locality: Static 
//and Dynamic. The static approach seems to introduce more (but not neccessarily realistic) 
//temporal locality than the dynamic approach. This class is actually used inside stream.*
//while the requests are being generated.

#include "stack.h"
#include "string.h"
#include <stdio.h>

//----------------------------------------------------------------------
// Stack::Stack
//   this is one of the constructors for the class. this constructor will
//   be called when the stack is operating in the dynamic mode.
//----------------------------------------------------------------------


Stack::Stack(int initSize)
{
    size = initSize;
    noofElements = 0;
    dynamic = true;
    bucket = new Request*[size];
    cummProb = new double[size];  //nothing yet in this array until elements 
                                  //are put on the stack 
}

//----------------------------------------------------------------------
// Stack::Stack
//   this is one of the constructors for the class. this constructor will
//   be called when the stack is operating in the static mode. In this
//   case the cummulative probabilities of the stack positions are 
//   supplied and they are unchanged for the duration of request generation
//----------------------------------------------------------------------

Stack::Stack(int initSize, double *initCummProb)
{ 
  size = initSize;
  noofElements = 0;
  dynamic = false;
  bucket = new Request*[size];
  cummProb = initCummProb;   //just point to the passed array
}

//----------------------------------------------------------------------
// Stack::~Stack
//     this is supposed to deallocate any dynamically allocated memory
//----------------------------------------------------------------------

Stack::~Stack()                         
{
  delete [] cummProb;
  delete [] bucket;
}



//----------------------------------------------------------------------
// Stack::ReComputeCummProbs
//     this routine will just recompute the cummulative probabilities 
//     associated with all stack positions based on the elements 
//     occupying each  position
//----------------------------------------------------------------------

void 
Stack::ReComputeCummProbs()
{
  if (noofElements > 0)
    {
      cummProb[0] = bucket[0]->GetProb(); 
      for (int i=1; i<noofElements; i++)
	cummProb[i] = cummProb[i-1] + bucket[i]->GetProb(); 
    }
}
  


//----------------------------------------------------------------------
//Stack::LocateThisRequest
//   This will search for the given (randNum) probability on the list of
//   all cummulative probabilities of all requests on the stack. The 
//   approach is close to a binary search. We cannot use a binary search
//   because we are searching for a range and not a specific item. This 
//   routine is actually called fron LocateDoc (the next method).
//----------------------------------------------------------------------

int
Stack::LocateThisRequest(double randNum)
{
 if (randNum > cummProb[noofElements - 1])  //it cannot be found on the stack
   return -1;
 
 int l = 0;
 int r = noofElements - 1;
 int loc;
 int mid;
 bool found = false;
 while (!found)
 {
  mid = (l+r)/2;
  if (((randNum <= cummProb[mid]) && (mid == 0)) || ((randNum<= cummProb[mid]) && (randNum > cummProb[mid-1])))
  {
      found = true;
      loc = mid;
  }
  else if (randNum > cummProb[mid] && ((mid +1) == r))
  {
      loc = mid +1;
      found = true; 
  }
  else if (randNum  > cummProb[mid])
      l = mid;
  else
      r = mid;
 } //endwhile

 return loc;
 
}

//----------------------------------------------------------------------
// Stack::LocateDoc
//    if a given randno is within range of the cummulative probabilities 
//    of all elements on the stack, return the location of the item enclosing
//    that random no else if the randnum no is out of range or there is no
//    doc on the stack, return a negative value. Cases when no item is found
//    on the stack is easy to handle, but when the item is found on the 
//    stack, we must move it to the top of the stack and while all items above it
//    have to shift one position down. Also if the item has only one more
//    reference to be generated, it must be removed from the stack and any
//    item below it must move up one position while the no of elements on
//    the stack must be decremented by 1. In all these cases, except when it 
//    is not found, the cummulative probabilities must be recomputed for the
//    dynamic case.
//----------------------------------------------------------------------
bool
Stack::LocateDoc(double randNum, unsigned int noofDistinctDocs, FILE *fp)   
{
 if (noofElements == 0) 
   return false;


 //the idea here is that if there is nothing on the remaining list, in which case
 //all items are on the stack, we shouldn't waste a loop of just generating a random 
 //no because the random no may not be found on the stack so we can scale down the
 //passed-in random no so that it falls within the range of the cummulative probs of 
 //items on the stack
 int loc;
 if (noofDistinctDocs == 0)
   loc = LocateThisRequest(randNum*cummProb[noofElements-1]); //find it from the stack
 else
   loc = LocateThisRequest(randNum);
 if (loc < 0)
   return  false;

 //getting this far means it was found on the stack  

 Request *temp = bucket[loc];

 //generate a request for this object in the fp stream
 fprintf(fp,"%10u %10u\n",temp->GetFileId(),temp->GetFileSize());
 temp->DecFreq();

 //if there are more request for this doc, move all items above loc down one 
 //position and put the located item at the top of the stack, else remove the 
 //item and free up the space it occupies while reducing the noof elements by 1
 if (temp->GetFreq() > 0) 
  {
   //for (int i=loc; i > 0; i--)
   //bucket[i] = bucket[i-1];
   
   memmove(&bucket[1], &bucket[0], loc*sizeof(Request*)); //equivalent to the previous loop
   bucket[0] = temp;
  }
 else 
  { 
   //for (int i=loc; i< noofElements-1; i++)
   //bucket[i] = bucket[i+1];

   memmove(&bucket[loc], &bucket[loc+1], (noofElements-loc-1)*sizeof(Request*)); //equivalent to the previous loop
 
   delete temp;
   --noofElements;
  }

//now if we are using dynamic stack operation we must recompute all cummulative
//probabilities based on item in each stack bucket
 if (dynamic)
   ReComputeCummProbs();

//finally return true that the request has been generated successfully

 return true;
}

  

//----------------------------------------------------------------------
// Stack::Put
//      this will put this request on the top of the stack, move others
//      down by one position. If there is no space on the stack, may have 
//      to remove a doc from the bottom of the stack and set the removed
//      doc's "onstack" var to false. May also have to increment noofElements
//      if necessary. Lastly must recompute the cummulative probs if we are
//      using dynamic approach
//----------------------------------------------------------------------

Request*
Stack::Put(Request *req)
{
  Request* temp = NULL;
  if (noofElements == size) //needs to evict last element
   {
     temp = bucket[noofElements-1];
     --noofElements;
   }

 //now move all elements down one position
 //for (int i=noofElements; i>0; i--)
 //    bucket[i] = bucket[i-1];
 memmove(&bucket[1], &bucket[0], noofElements*sizeof(Request*)); //same as the for-loop

 //now store the new elements at the top of the stack
 bucket[0] = req;
 ++noofElements;

 if (dynamic)
  ReComputeCummProbs();

 return temp;
}



