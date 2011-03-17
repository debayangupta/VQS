//
//  OperatorBase.hpp
//  VQS
//
//  Created by Azza Abouzied on 12/12/10.
//  Copyright (c) 2010 Yale University. All rights reserved.
//

#ifndef __OP_H_
#define __OP_H_

#include <iostream>

class OperatorBase{
public:
    virtual ~OperatorBase(){std::cerr << "CBase Destructed" << std::endl;};
};

class VizOperatorBase : public OperatorBase{
public:
    virtual void render(std::ostream &os) = 0;
};
#endif