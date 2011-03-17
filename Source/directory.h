//
//  directory.h
//  VQS
//
//  Created by Azza Abouzied on 11/3/10.
//  Copyright (c) 2010 Yale University. All rights reserved.
//

#ifndef __DIRECTORY_H_
#define __DIRECTORY_H_

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <string>
#include <exception>

class FileException : public std::exception{
public:
    int errcode;
    FileException(int code, std::string file){
        perror(file.c_str());
        errcode = code;};
    virtual const char* what() const throw(){
        return std::strerror(errcode);
    }
};

void recursive_mkdir(std::string path, mode_t mode) throw (FileException);
//int recursive_mkdir(const char* path, mode_t mode);

#endif