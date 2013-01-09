
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




// request.cc
//
//    This class models a single request object in the workload



#include "request.h"

//----------------------------------------------------------------------
// Request::Request
//   this is the constructor for the class. It initializes the 
//   frequency and the file size of this object
//----------------------------------------------------------------------


Request::Request(unsigned int initFreq, unsigned int initFileSize)
{
    freq   = initFreq;
    fileSize = initFileSize;
}


//----------------------------------------------------------------------
// Request::Request
//   this is an empty constructor for the class. Nothing to initialize
//----------------------------------------------------------------------

Request::Request()
{
}


//----------------------------------------------------------------------
// Request::~Request
//   this is the destructor for the class. Nothing to deallocate 
//----------------------------------------------------------------------

Request::~Request()
{
}

//----------------------------------------------------------------------
// Request::GetFileId
//   returns the fileid of this object
//----------------------------------------------------------------------

unsigned int
Request::GetFileId()
{
  return fileId;
}

//----------------------------------------------------------------------
// Request::SetFileId
//   sets the fileid of this object to id
//----------------------------------------------------------------------

void
Request::SetFileId(unsigned int id)
{
  fileId = id;
}

 
//----------------------------------------------------------------------
// Request::GetFreq
//   returns the freq
//----------------------------------------------------------------------

unsigned int
Request::GetFreq()
{
  return freq;
}

//----------------------------------------------------------------------
// Request::SetFreq
//   returns the freq
//----------------------------------------------------------------------
void
Request::SetFreq(unsigned int fr)
{
  freq = fr;
}


//----------------------------------------------------------------------
// Request::SetProb
//  sets the probability of referencing this file based on concentration
//  of references.
//----------------------------------------------------------------------

void
Request::SetProb(double probability)
{
  prob = probability;
}
  
//----------------------------------------------------------------------
// Request:: GetProb
//   returns the probability of referencing this file
//----------------------------------------------------------------------

double
Request::GetProb()
{
   return prob;
}


//----------------------------------------------------------------------
// Request::GetFileSize()
//      returns file size 
//----------------------------------------------------------------------

unsigned int
Request::GetFileSize() 
{
   return fileSize;
}

//----------------------------------------------------------------------
// Request::SetFileSize()
//      sets file size 
//----------------------------------------------------------------------

void
Request::SetFileSize(unsigned int fs) 
{
   fileSize = fs;
}



//----------------------------------------------------------------------
// Request::DecFreq
//       decrements the remaining freq
//----------------------------------------------------------------------

void 
Request::DecFreq()
{
   --freq;
}



// end of request.cc
