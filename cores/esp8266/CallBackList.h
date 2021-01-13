#ifndef __CALLBACKLIST_H__
#define __CALLBACKLIST_H__


/*
 CallBackList, An implemention for handling callback execution

 Copyright (c) 2019 Herman Reintke. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Arduino.h>
#include <memory>
#include <list>
#include <utility>

namespace experimental
{
namespace CBListImplentation
{

template<class cbFunctionT>
class CallBackList
{
public:
	CallBackList (){};

    struct CallBackInfo
    {
		CallBackInfo(cbFunctionT f) : cbFunction(f, true){};
		CallBackInfo(cbFunctionT f, bool ar) : cbFunction(f), _allowRemove(ar) {};
		cbFunctionT  cbFunction;
    	bool _allowRemove = true;
    	bool allowRemove()
    	{
    		return _allowRemove;
    	}
     };
    using CallBackHandler = std::shared_ptr<CallBackInfo> ;
    std::list<CallBackHandler> callBackEventList;

    CallBackHandler add(cbFunctionT af, bool ad = true) {
    	CallBackHandler handler = std::make_shared<CallBackInfo>(CallBackInfo(af,ad));
    	callBackEventList.emplace_back(handler);
    	return handler;
    }

    void remove(CallBackHandler& dh) {
    	callBackEventList.remove(dh);
	}

    template<typename... Args>
    int execute(Args... params) {
        for(auto it = std::begin(callBackEventList); it != std::end(callBackEventList); ) {
        	CallBackHandler &handler = *it;
            if (handler->allowRemove() && handler.unique()) {
                it = callBackEventList.erase(it);
            }
            else {
             	handler->cbFunction(params...);
                ++it;
            }
        }
        return callBackEventList.size();
    }
};

} //CBListImplementation
}//experimental

#endif // __CALLBACKLIST_H__
