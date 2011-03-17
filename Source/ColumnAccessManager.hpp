//
//  ColumnAccessManager.cp
//  VQS
//
//  Created by Azza Abouzied on 12/6/10.
//  Copyright (c) 2010 Yale University. All rights reserved.
//

#ifndef __COLUMNACCESSMGR_H_
#define __COLUMNACCESSMGR_H_

#include "Col.hpp"
#include "MeyersSingleton.h"
#include "datatypes.h"

#include <iostream>
#include <fstream>
#include <map>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/dynamic_bitset.hpp>



typedef boost::shared_ptr<ColumnBase> ColumnPtr;

class _ColumnAccessManager{
private:
    std::map<std::string, ColumnPtr> _colMap;
public:
    _ColumnAccessManager(){};
    ~_ColumnAccessManager();
    
    
    template<class C>
    boost::shared_ptr < Column<C> > getColumn(std::string serialization_path);
    
    template<class C>
    boost::shared_ptr < Column<C> > getColumn(std::string memory_path, size_t size);
    
    template<class C>
    boost::shared_ptr < Column<C> > writeColumn(C* data, size_t size, boost::dynamic_bitset<> missing_map, std::string path, bool is_memory_mappable);
};

typedef Singleton<_ColumnAccessManager> ColumnAccessManager;

_ColumnAccessManager::~_ColumnAccessManager(){
    std::cerr << "Column Access Manager Destructed" << std::endl;
    _colMap.clear();
}

template<class C>
boost::shared_ptr< Column<C> > _ColumnAccessManager::getColumn(std::string memory_path, size_t size){
    if(_colMap.find(memory_path) == _colMap.end()){
        boost::shared_ptr<Column<C> > ptr(new Column<C>(memory_path, size));
        _colMap[memory_path] = ptr;
    }
    return boost::shared_static_cast<Column<C> >(_colMap[memory_path]);
}

template<class C>
boost::shared_ptr< Column<C> > _ColumnAccessManager::getColumn(std::string serialization_path){
    if(_colMap.find(serialization_path) == _colMap.end()){
        
        boost::shared_ptr<Column<C> > ptr(new Column<C>());
        //Column<C> col;
        std::ifstream ifs(serialization_path.c_str(), std::ios::in);
        assert(ifs.good());
        boost::archive::binary_iarchive ia(ifs);
        ia >> *(ptr.get());
        
        _colMap[serialization_path] = ptr;
    }
    return boost::shared_static_cast<Column<C> >(_colMap[serialization_path]);
    
}

template<class C>
boost::shared_ptr< Column<C> > _ColumnAccessManager::writeColumn(C* data, size_t _size, boost::dynamic_bitset<> missing_map, std::string path, bool is_memory_mappable){
    boost::shared_ptr<Column<C> > ptr(new Column<C>());
    _colMap[path] = ptr;
    boost::shared_static_cast<Column<C> >(_colMap[path])->set(data, _size, &missing_map, path, is_memory_mappable);
    return boost::shared_static_cast<Column<C> >(_colMap[path]);
}


template boost::shared_ptr<Column<double> > _ColumnAccessManager::getColumn<double>(std::string);
template boost::shared_ptr<Column<int> > _ColumnAccessManager::getColumn<int>(std::string);
template boost::shared_ptr<Column<std::string> > _ColumnAccessManager::getColumn<std::string>(std::string);
template boost::shared_ptr<Column<short> > _ColumnAccessManager::getColumn<short>(std::string);
template boost::shared_ptr<Column<mstr> > _ColumnAccessManager::getColumn<mstr>(std::string);
template boost::shared_ptr<Column<unsigned int> > _ColumnAccessManager::getColumn<unsigned int>(std::string);

template boost::shared_ptr<Column<double> > _ColumnAccessManager::getColumn<double>(std::string, size_t);
template boost::shared_ptr<Column<int> > _ColumnAccessManager::getColumn<int>(std::string, size_t);
template boost::shared_ptr<Column<std::string> > _ColumnAccessManager::getColumn<std::string>(std::string, size_t);
template boost::shared_ptr<Column<short> > _ColumnAccessManager::getColumn<short>(std::string, size_t);
template boost::shared_ptr<Column<mstr> > _ColumnAccessManager::getColumn<mstr>(std::string, size_t);
template boost::shared_ptr<Column<unsigned int> > _ColumnAccessManager::getColumn<unsigned int>(std::string, size_t);

template boost::shared_ptr<Column<double> > _ColumnAccessManager::writeColumn<double>(double*, size_t, boost::dynamic_bitset<>, std::string, bool);
template boost::shared_ptr<Column<int> > _ColumnAccessManager::writeColumn<int>(int*, size_t, boost::dynamic_bitset<>, std::string, bool);
template boost::shared_ptr<Column<std::string> > _ColumnAccessManager::writeColumn<std::string>(std::string*, size_t, boost::dynamic_bitset<>, std::string, bool);
template boost::shared_ptr<Column<short> > _ColumnAccessManager::writeColumn<short>(short*, size_t, boost::dynamic_bitset<>, std::string, bool);
template boost::shared_ptr<Column<mstr> > _ColumnAccessManager::writeColumn<mstr>(mstr*, size_t, boost::dynamic_bitset<>, std::string, bool);
template boost::shared_ptr<Column<unsigned int> > _ColumnAccessManager::writeColumn<unsigned int>(unsigned int*, size_t, boost::dynamic_bitset<>, std::string, bool);

#endif