/*
 *  Pipeline.hpp
 *  VisualQuerySys
 *
 *  Created by Azza Abouzied on 12/16/10.
 *  Copyright 2010 Yale University. All rights reserved.
 *
 */

#include <vector>

#include <boost/shared_ptr.hpp>

#include <OperatorBase.hpp>
#include <Col.hpp>

class Pipeline{
private:
	struct node{
		OperatorBase prev;
		OperatorBase next;
		
	}
	std::vector<VizOperatorBase> interfaceOps;
	std::vector<boost::shared_ptr<ColumnBase> > datasources;
	std::vector<OperatorBase> allOps;
	std::vector<std::vector<size_t> > graph;
	
}