//
//  ColumnException.h
//  VQS
//
//  Created by Azza Abouzied on 12/6/10.
//  Copyright (c) 2010 Yale University. All rights reserved.
//

#ifndef __COLEXCEPTION_H_
#define __COLEXCEPTION_H_


#include <sys/stat.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <string>
#include <exception>

class ColumnMappingException: public std::exception{
public:
    std::string errmsg;
    int errorcode;
    ColumnMappingException(int ec, std::string err):errorcode(ec), errmsg(err){};
    virtual const char* what() const throw(){
        std::string msg = "Mapping Exception: " + errmsg +  " " + std::strerror(errorcode);
        return msg.c_str();
    };
    ~ColumnMappingException() throw(){};
};

class ColumnSerializationException: public std::exception{
public:
    std::string errmsg;
    int errorcode;
    ColumnSerializationException(int ec, std::string err):errorcode(ec), errmsg(err){};
    virtual const char* what() const throw(){ 
        std::string msg = "Serialization Exception: " +  errmsg +  " " + std::strerror(errorcode);
        return msg.c_str();
    };
    ~ColumnSerializationException() throw(){};
};

class InvalidTypeException: public std::exception{
public:
    std::string errmsg;
    int errorcode;
    InvalidTypeException(std::string err): errmsg(err){};
    virtual const char* what() const throw(){ 
        std::string msg = "Invalid Type Exception: " +  errmsg ;
        return msg.c_str();
    };
    ~InvalidTypeException() throw(){};
};

#endif
