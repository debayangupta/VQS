//
//  BarChart.hpp
//  VQS
//
//  Created by Azza Abouzied on 12/12/10.
//  Copyright (c) 2010 Yale University. All rights reserved.
//

#ifndef __BC_H_
#define __BC_H_

#include <map>
#include <iostream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/bimap.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <Col.hpp>
#include <OperatorBase.hpp>

template<class X>
class BarChart : public VizOperatorBase {
private:
    
    boost::dynamic_bitset<> missing_map;
public:
    
    struct datapoint{
        size_t oid;
        X x;
        datapoint(size_t oid, X x):oid(oid),x(x){};
    };
    
    typedef boost::multi_index_container<
        datapoint, 
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_non_unique< boost::multi_index::member<datapoint, X, &datapoint::x> >,
            boost::multi_index::ordered_unique< boost::multi_index::member<datapoint, size_t, &datapoint::oid> >
        > 
    > 
    link_type;
    
    link_type link;
    
    typedef typename boost::multi_index::nth_index<link_type, 0>::type x_idx_type;
    typedef typename boost::multi_index::nth_index<link_type, 1>::type oid_idx_type;
    
    typedef typename boost::multi_index::nth_index<link_type, 0>::type::iterator x_it_type;
    typedef typename boost::multi_index::nth_index<link_type, 1>::type::iterator oid_it_type;
    
    const x_idx_type* xidx;
    const oid_idx_type* oididx; 
    
    std::map<X, size_t> state;
    
    BarChart(){};
    void set(boost::shared_ptr<Column<X> > x);
    void render(std::ostream &o);
};

template<class X>
void BarChart<X>::set(boost::shared_ptr<Column<X> > ptrx){
    if(ptrx->getMissingCount() != std::numeric_limits<size_t>::quiet_NaN()){
        missing_map = ptrx->getMissingMap();
    }
    
    const X* xd = ptrx->getData();
    size_t size = ptrx->getSize();
    if(missing_map.count() == 0){
        for(size_t i = 0; i < size; i++){
            link.insert(datapoint(i, xd[i]));
            state[xd[i]]++;
        }
    }
    else{
        for(size_t i = 0; i < size; i++){
            if(!missing_map[i]){
                link.insert(datapoint(i, xd[i]));
                state[xd[i]]++;
            }
        }
    }
}

template<class X>
void BarChart<X>::render(std::ostream &os){
    os << "{data: [";
    size_t max_count = 0;
    typename std::map<X, size_t>::iterator it;
    for(it = state.begin(); it != state.end(); it++){
        max_count = it->second > max_count ? it->second : max_count;
        os << "{" << it->first << ": " << it->second  << " }," << std::endl;
    }
    os << "], missing: " << missing_map.count() << ", max_count: " << max_count << "}" << std::endl;
}

template class BarChart<std::string>;
//template class BarChart<mstr>;
template class BarChart<short>;
template class BarChart<int>;
template class BarChart<unsigned int>;
template class BarChart<double>;
#endif