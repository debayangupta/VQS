//
//  directory.cpp
//  VQS
//
//  Created by Azza Abouzied on 11/3/10.
//  Copyright (c) 2010 Yale University. All rights reserved.
//

#include "directory.h"
#include <string.h>
#include <stdlib.h>
#include <iostream>


int recursive_mkdir(const char* path, mode_t mode){
    int rv = 0;
    char *tokpath = (char *)malloc(strlen(path)*sizeof(char));
    char *dirpath = (char *)malloc(strlen(path)*sizeof(char));
    
    strcpy(tokpath, path);
    char *dir;
    dir = strtok(tokpath, "/");
    if(path[0] == '/'){
        dirpath[0] = '/';   
    }
    strcat(dirpath, dir);
    rv = mkdir(dirpath, mode);
    if(rv != 0 && errno != EEXIST){
        return rv;
    }
    dir = strtok(NULL, "/");
    while(dir != NULL){
        strcat(dirpath, "/");   
        strcat(dirpath, dir);
        rv = mkdir(dirpath, mode);
        if(rv != 0 && errno != EEXIST)
            return rv;
        dir = strtok(NULL, "/");
    }
    free(tokpath);
    free(dirpath);
    return rv;
}

void recursive_mkdir(std::string path, mode_t mode) throw (FileException){
    int rv = recursive_mkdir(path.c_str(), mode);
    if(rv != 0){
        throw (FileException(errno, path));
    }
}