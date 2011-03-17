//
//  Schema.cp
//  VQS
//
//  Created by Azza Abouzied on 12/6/10.
//  Copyright (c) 2010 Yale University. All rights reserved.
//

#ifndef __SCHEMA_H_
#define __SCHEMA_H_

#include <boost/shared_ptr.hpp>
#include <boost/dynamic_bitset.hpp>
#include <iostream>
#include <string>
#include "boost/archive/binary_oarchive.hpp"
#include "boost/archive/binary_iarchive.hpp"

#include "boost/serialization/map.hpp"
#include "boost/serialization/string.hpp"

#include "datatypes.h"
#include <map>
#include <vector>
#include <string>
#include "ColumnAccessManager.hpp"
#include "directory.h"

class SchemaExistsException: public std::exception{
public:
    std::string tablename;
    SchemaExistsException(std::string tname){
        tablename = tname;
    }
    virtual const char* what() const throw(){
        std::string errmsg = "Schema Exists Exception: " + tablename;
        return errmsg.c_str();
    }
    ~SchemaExistsException() throw(){};
};

class NoSchemaException: public std::exception{
public:
    std::string tablename;
    NoSchemaException(std::string tname){
        tablename = tname;
    }
    virtual const char* what() const throw(){
        std::string errmsg = "Schema does not exist Exception: " + tablename;
        return errmsg.c_str();
    }
    ~NoSchemaException() throw(){};
};

class Schema {
private:

    friend class boost::serialization::access;
    
    template<class Archive>
    void serialize(Archive &ar, const unsigned int){
        ar & tablename;
        ar & size; 
        ar & num_columns;
        ar & colmap;
        ar & memory_mappable;
    };
    
    size_t size;
    size_t num_columns;
    
    std::string tablename;
    
    bool memory_mappable;
    
    std::map<std::string, std::string> colmap;

public:
    
    Schema(){};
    Schema(std::string _tablename, size_t _size, size_t _cols, const std::vector<std::string> _col_names, const std::vector<std::string> _col_types, bool _memory_mappable);
    
    
    static void getSchema(Schema&, std::string);
    static bool exists(std::string);
    
    template <class C>
    boost::shared_ptr<Column<C> > getColumn(std::string);
    template<class C>
    boost::shared_ptr<Column<C> > writeToColumn(std::string colname, C* data, boost::dynamic_bitset<> missing_map);
    
    std::string getType(std::string colname){return colmap[colname];}
    
    ~Schema();
};

#endif

Schema::~Schema(){
    std::cerr << "Schema " << tablename << " at " << this << " Destructed" << std::endl;
}

bool Schema::exists(std::string tablename){
	std::string filepath = SCHEMAPATH + tablename;
	std::ifstream schemafile;
    schemafile.open(filepath.c_str(), std::ios::binary);
	if(schemafile.fail()){
		schemafile.clear(std::ios::failbit);
		return false;
	}
	else{
        schemafile.close();
		return true;
	}
}

Schema::Schema(std::string _tablename, size_t _size, size_t _cols, const std::vector<std::string> _col_names, const std::vector<std::string> _col_types, bool _memory_mappable){
    
    tablename = _tablename;
    
    if(Schema::exists(_tablename)){
        throw SchemaExistsException(_tablename);
    }
    
    memory_mappable = _memory_mappable;
    size = _size;
    num_columns = _cols;
    
    std::string datapath = DATAPATH + tablename;
    for(int i = 0; i < num_columns; i++){
        colmap[_col_names[i]] = _col_types[i];
    }
    recursive_mkdir(datapath, S_IRWXU  | S_IRWXG | S_IROTH);

    std::string location = SCHEMAPATH + tablename;
    std::ofstream schemafile(location.c_str(), std::ios::binary);
    assert(schemafile.good());
    boost::archive::binary_oarchive oa(schemafile);
    oa << *this;
    schemafile.close();
}

void Schema::getSchema(Schema& s, std::string tablename){
    if(Schema::exists(tablename)){
        std::string location = SCHEMAPATH + tablename;
        std::ifstream ifs(location.c_str(), std::ios::binary);
        assert(ifs.good());
        boost::archive::binary_iarchive ia(ifs);
        ia & s;
    }
    else{
        throw NoSchemaException(tablename);
    }
}

template <class C>
boost::shared_ptr<Column <C> > Schema::getColumn(std::string colname){
    if(memory_mappable){
        return ColumnAccessManager::Instance().getColumn<C>(DATAPATH + tablename + "/" + colname, size);
    }
    else{
        return ColumnAccessManager::Instance().getColumn<C>(DATAPATH + tablename + "/" + colname);
    }
}

template<class C>
boost::shared_ptr<Column<C> > Schema::writeToColumn(std::string colname, C* data, boost::dynamic_bitset<> missing_map){
    std::string colpath = DATAPATH + tablename;
    colpath +=  "/" + colname;
    return ColumnAccessManager::Instance().writeColumn(data, size, missing_map, colpath, memory_mappable);
}



void initialize_dir(void);
