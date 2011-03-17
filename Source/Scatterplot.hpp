//
//  Scatterplot.cp
//  VQS
//
//  Created by Azza Abouzied on 12/8/10.
//  Copyright (c) 2010 Yale University. All rights reserved.
//

#ifndef __SC_H_
#define __SC_H_

#include <map>
#include <iostream>

#include <boost/dynamic_bitset.hpp>
#include <boost/bimap.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>

#include "Col.hpp"
#include "OperatorBase.hpp"

template<class X, class Y>
class Scatterplot : public VizOperatorBase {
private:
    boost::dynamic_bitset<> missing_map;
    
    double xscale_factor;
    double yscale_factor;
public:
    
    struct datapoint{
        X x;
        Y y;
        size_t oid;
        datapoint(size_t oid, X x, Y y):oid(oid),x(x),y(y){};
    };
    
    
    struct x_key:boost::multi_index::composite_key<
    datapoint,
    BOOST_MULTI_INDEX_MEMBER(datapoint,X,x),
    BOOST_MULTI_INDEX_MEMBER(datapoint,Y,y)//,
    //BOOST_MULTI_INDEX_MEMBER(datapoint,size_t,oid)
    >{};
    
    
    struct y_key:boost::multi_index::composite_key<
    datapoint,
    BOOST_MULTI_INDEX_MEMBER(datapoint,Y,y),
    BOOST_MULTI_INDEX_MEMBER(datapoint,X,x)//,
    //BOOST_MULTI_INDEX_MEMBER(datapoint,size_t,oid)
    >{};
    
    typedef boost::multi_index_container<
    datapoint, 
    boost::multi_index::indexed_by<
    //boost::multi_index::ordered_non_unique< boost::multi_index::member<datapoint, X, &datapoint::x> >,
    boost::multi_index::ordered_non_unique<x_key>,
    boost::multi_index::ordered_non_unique<y_key>,
    boost::multi_index::ordered_unique< boost::multi_index::member<datapoint, size_t, &datapoint::oid> >
    > 
    > 
    state_type;
    state_type state;
    
    typedef typename boost::multi_index::nth_index<state_type, 0>::type x_idx_type;
    typedef typename boost::multi_index::nth_index<state_type, 1>::type y_idx_type;
    typedef typename boost::multi_index::nth_index<state_type, 2>::type oid_idx_type;
    
    typedef typename boost::multi_index::nth_index<state_type, 0>::type::iterator x_it_type;
    typedef typename boost::multi_index::nth_index<state_type, 1>::type::iterator y_it_type;
    
    const x_idx_type* xidx;
    const y_idx_type* yidx; 
    
    Scatterplot(){};
    void set(boost::shared_ptr<Column<X> > x, boost::shared_ptr<Column<Y> > y);
    void render(std::ostream &o);
    void render(std::ostream &o, size_t pw, size_t ph, X xmin, X xmax, Y min, Y max);
};


template<class X, class Y>
void Scatterplot<X, Y>::set(boost::shared_ptr<Column<X> > ptrx, boost::shared_ptr<Column<Y> > ptry){
    assert(ptrx->getSize() == ptry->getSize());
    missing_map.resize(ptrx->getSize());
    if(ptrx->getMissingCount() != std::numeric_limits<size_t>::quiet_NaN()){
        missing_map |= ptrx->getMissingMap();
    }
    if(ptry->getMissingCount() != std::numeric_limits<size_t>::quiet_NaN()){
        missing_map |= ptry->getMissingMap();
    }
    
    const X* xd = ptrx->getData();
    const Y* yd = ptry->getData();
    size_t size = ptrx->getSize();
    
    if(missing_map.count() == 0){
        //No missing tuples --- just add'em all
        for(size_t i = 0; i < size; i++)
            state.insert(datapoint(i, xd[i], yd[i]));
    }
    else{
        for(size_t i = 0; i < size; i++){
            if(!missing_map[i])
                state.insert(datapoint(i, xd[i], yd[i]));
        }
    }
    
    xidx = &boost::multi_index::get<0>(state);
    yidx = &boost::multi_index::get<1>(state);
}

template<class X, class Y>
void Scatterplot<X, Y>::render(std::ostream &os){

    const x_it_type& x0 = xidx->begin();
    x_it_type x1 = xidx->end();
    x1--;
    
    const y_it_type& y0 = yidx->begin();
    y_it_type y1 = yidx->end();
    y1--;
    
    render(os, 400, 400, x0->x, x1->x, y0->y, y1->y);
}

template<class X, class Y>
void Scatterplot<X, Y>::render(std::ostream &os, size_t pw, size_t ph, X xmin, X xmax, Y ymin, Y ymax){
    xscale_factor = pw/(xmax - xmin);
    yscale_factor = ph/(ymax - ymin);
    
    //Always pick the smaller range in terms of points

    const x_it_type& x0 = xidx->upper_bound(xmin);
    const x_it_type& x1 = xidx->lower_bound(xmax);

    const y_it_type& y0 = yidx->upper_bound(ymin);
    const y_it_type& y1 = yidx->lower_bound(ymax);
    
    size_t prev_px, prev_py;
    size_t px, py = 0;
    size_t count = 1;
    size_t max_count = 0;
    
    os << "{data: [";
    if(std::distance(x0, x1) < std::distance(y0, y1)){
        for(x_it_type i = x0; i != x1; ++i){
            if(i->y >= ymin && i->y <= ymax){
                px = (size_t) (i->x)*xscale_factor;
                py = (size_t) (i->y)*yscale_factor;
                if(px == prev_px && py == prev_py){
                    count++;
                }
                else{
                    max_count = (count > max_count) ? count : max_count;
                    os << "{x: " << i->x << ", y: " << i->y << ", z:" << count << "}," << std::endl;
                    count = 1;
                    prev_px = px; prev_py = py;
                }
            }
        }
    }
    else{
        for(y_it_type i = y0; i != y1; ++i){
            if(i->x >= xmin && i->x <= xmax){
                px = (size_t) (i->x)*xscale_factor;
                py = (size_t) (i->y)*yscale_factor;
                if(px == prev_px && py == prev_py){
                    count++;
                }
                else{
                    max_count = (count > max_count) ? count : max_count;
                    os << "{x: " << i->x << ", y: " << i->y << ", z:" << count << "}," << std::endl;
                    count = 1;
                    prev_px = px; prev_py = py;
                }
            }
        }
    }
    os << "], max_count: " << max_count << ", missing: " << missing_map.count() << "}" << std::endl;  
}

template class Scatterplot<double, double>;
#endif