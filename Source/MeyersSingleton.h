//
//  MeyersSingleton.h
//  VQS
//
//  Created by Azza Abouzied on 11/8/10.
//  Copyright (c) 2010 Yale University. All rights reserved.
//
#ifndef __MEYERS_SINGLETON_H
#define __MEYERS_SINGLETON_H

template <class T>
class Singleton
{
public:
    static T& Instance() {
        static T _instance;
        return _instance;
    }
private:
    Singleton();          // ctor hidden
    ~Singleton();          // dtor hidden
    Singleton(Singleton const&);    // copy ctor hidden
    Singleton& operator=(Singleton const&);  // assign op hidden
};

#endif