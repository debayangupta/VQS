//
//  Col.cp
//  VQS
//
//  Created by Azza Abouzied on 12/5/10.
//  Copyright (c) 2010 Yale University. All rights reserved.
//

#ifndef __COLUMN_H_
#define __COLUMN_H_

#include "ColumnException.h"
#include <limits>
#include <map>
#include <string>
#include <string.h>

#include "datatypes.h"

#include <iostream>
#include <fstream>


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <typeinfo>

#include "boost/dynamic_bitset.hpp"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/dynamic_bitset.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

class ColumnBase{
public:
    virtual ~ColumnBase(){std::cerr << "CBase Destructed" << std::endl;};
};

template<class C>
class Column : public ColumnBase{
private:
    friend class boost::serialization::access;
    
    C min;
    C max;
    
    size_t size;
    size_t missing_count;
    
    boost::dynamic_bitset<unsigned long> missing_map;
    C* pdata;
    
    bool materialized;
    bool sorted;
    
    bool memory_mappable;
    int memory_mappable_fd;
    
    //Do not allow column copying
    Column(Column const&){};
    
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        ar & size;
        ar & boost::serialization::make_array(pdata, size);
        ar & sorted;
        ar & min;
        ar & max;
        ar & missing_count;
        ar & missing_map;
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        ar & size;
        pdata = new C[size];
        ar & boost::serialization::make_array(pdata, size);
        ar & sorted;
        ar & min;
        ar & max;
        ar & missing_count;
        ar & missing_map;
        materialized = true;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
    
    void buildMeta();
public:
    Column();
    void set(C* data, size_t size, boost::dynamic_bitset<> const* missing_map, std::string path, bool is_memory_mappable);
    
    Column(std::string const& memory_path, size_t size);
    
    const C* getData();
    
    const C& getMin(){return min;};
    const C& getMax(){return max;};
    
    size_t getSize(){return size;};
    size_t getMissingCount(){return missing_count;};
    
    const boost::dynamic_bitset<>& getMissingMap();
    
    bool is_sorted(){return sorted;};
    bool is_materialized(){return materialized;};
    
    ~Column();
};

template<class C>
Column<C>::~Column(){
    if(memory_mappable){
        munmap(pdata, size);
        close(memory_mappable_fd);
    }
    else
        delete[] pdata;
    std::cerr << "Column " << this << " Destructed: " << typeid(C).name() << std::endl;
}

template<class C>
const C* Column<C>::getData(){
    return const_cast<C *>(pdata);
}

template<class C>
const boost::dynamic_bitset<>& Column<C>::getMissingMap(){
    return const_cast<boost::dynamic_bitset<> &>(missing_map);
}

template<class C>
Column<C>::Column(){
    size = 0;
    sorted = false;
    missing_count = 0;
    materialized = false;
    memory_mappable = false;
}


template<class C>
Column<C>::Column(std::string const& memory_path, size_t size){
    memory_mappable_fd = open(memory_path.c_str(), O_RDWR, (mode_t)0600);
    pdata = (C *) mmap(0, (size-1)*sizeof(C), PROT_READ, MAP_SHARED, memory_mappable_fd, 0);
    if (pdata == MAP_FAILED) {
    	close(memory_mappable_fd);
        throw ColumnMappingException(errno, memory_path);
    }
    
    std::string missing_map_path = memory_path + ".mis";
    std::ifstream missing_stream(missing_map_path.c_str(), std::ios::in);
    if(missing_stream.good()){
        missing_stream >> missing_map;
        missing_count = missing_map.size();
    }
    else{
        missing_map.resize(size);
        missing_count = std::numeric_limits<unsigned int>::quiet_NaN();
    }
    missing_stream.close();
    materialized = true;
    memory_mappable = true;
    buildMeta();
}

template<>
void Column<mstr>::buildMeta(){
    strcpy(min, pdata[0]);
    strcpy(max, pdata[0]);
    sorted = true;
    mstr prev;
    strcpy(prev, pdata[0]);
    for(int i = 0; i < size; i++){
        if(missing_map[i] == 0){
            if(strcmp(pdata[i], min) < 0) strcpy(min, pdata[0]);
            if(strcmp(pdata[i], max) > 0) strcpy(min, pdata[0]);
            if(strcmp(pdata[i], prev) < 0) sorted = false;
        }
    }
}


template<>
void Column<std::string>::buildMeta(){
    min = pdata[0];
    max = pdata[0];
    sorted = true;
    std::string prev = pdata[0];
    for(int i = 0; i < size; i++){
        if(missing_map[i] == 0){
            if(pdata[i].compare(min) < 0) min = pdata[i];
            if(pdata[i].compare(max) > 0) max = pdata[i];
            if(pdata[i].compare(prev) < 0) sorted = false;
        }
    }
    
}


template<class C>
void Column<C>::buildMeta(){
    min = std::numeric_limits<C>::max();
    max = std::numeric_limits<C>::min();
    
    sorted = true;
    C prev = pdata[0];
    for(int i = 0; i < size; i++){
        if(missing_map[i] == 0){
            if(pdata[i] < min) min = pdata[i];
            if(pdata[i] > max) max = pdata[i];
            if(pdata[i] < prev) sorted = false;
        }
    }
}

template<class C>
void Column<C>::set(C *_data, size_t _size, boost::dynamic_bitset<> const* _missing_map, std::string path, bool is_memory_mappable){
    size = _size;
    if(_missing_map == NULL ){
        missing_map.resize(size);
        missing_count = std::numeric_limits<unsigned int>::quiet_NaN();
    }
    else{
        missing_map = *_missing_map;
        missing_count = missing_map.count();
    }
    materialized = true;
    
    
    if(path != ""){
        if(!is_memory_mappable){
            pdata = _data;
            buildMeta();
            std::ofstream ofs(path.c_str(),  std::ios::out | std::ios::app );
            if(ofs.good()){
                boost::archive::binary_oarchive oar(ofs);
                oar << *this;
            }
            else{
                ofs.close();
                throw ColumnSerializationException(errno, path);
            }
            ofs.close();
        }
        else{
            if(typeid(C) == typeid(std::string)){
                throw InvalidTypeException(path);
            }
            memory_mappable_fd = open(path.c_str(), O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
            lseek(memory_mappable_fd, (size - 1)  * sizeof(C), SEEK_SET);
            write(memory_mappable_fd, "", 1);
            pdata = (C *) mmap(0, (size-1)*sizeof(C), PROT_WRITE | PROT_READ, MAP_SHARED, memory_mappable_fd, 0);
            if (pdata == MAP_FAILED) {
                close(memory_mappable_fd);
                throw ColumnMappingException(errno, path);
            }
            else{
                memcpy(pdata, _data, size * sizeof(C));
            }
            buildMeta();
			memory_mappable = true;
        }
    }
    
    _data = NULL;
}

template class Column<double>;
template class Column<int>;
template class Column<unsigned int>;
template class Column<std::string>;
template class Column<short>;
template class Column<mstr>;


#endif
