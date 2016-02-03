/* DataStrategy.h - interface for writing data to ClientContext
 * Copyright (c) 2016 Ivan Grokhotkov. All rights reserved.
 * This file is distributed under MIT license.
 */

#ifndef DATASTRATEGY_H
#define DATASTRATEGY_H


#include "ClientContext.h"

class DataStrategy
{
public:
    virtual ~DataStrategy() {}
    virtual size_t write(ClientContext& ctx) = 0;
    virtual void on_sent(ClientContext& ctx, size_t size) = 0;
    virtual void on_error(ClientContext& ctx) = 0;
    virtual void on_poll(ClientContext& ctx) = 0;
};

/// in DataStrategyImpl.h


#endif//DATASTRATEGY_H
