
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



// request.h

//This class models a request object in the workload.



#ifndef REQUEST_H
#define REQUEST_H

class Request {
 public:
    Request(unsigned int initFreq, unsigned int initFileSize);

    Request(); //  constructor without parameters

    ~Request(); //Destructor

    unsigned int  GetFileId();  // returns the fileid if needed
    
    void SetFileId(unsigned int id); //sets the fileid to id

    unsigned int  GetFreq();    // returns the frequency if needed

    void SetFreq(unsigned int fr); //sets the freq to f

    unsigned int  GetFileSize(); //returns file size

    void SetFileSize(unsigned int fs); //sets the file size

    void DecFreq(); //decrements the freq
   
    void SetProb(double prob); 
    double GetProb();

 private:
    unsigned int fileId;        // the file id
    unsigned int freq;          // the number of references to be generated for this file
    unsigned int fileSize;      // the file size of this request
    double prob;                // the prob of referencing this file out of all distinct files
    
};

#endif //REQUEST_H



