/*
  LwipIntfCB.cpp

  network generic callback implementation

  Original Copyright (c) 2020 esp8266 Arduino All rights reserved.
  This file is part of the esp8266 Arduino core environment.

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

#include <LwipIntf.h>
#include <Schedule.h>
#include <debug.h>

static constexpr size_t LwipIntfCallbacks = 3;

static LwipIntf::CBType callbacks[LwipIntfCallbacks];
static size_t           size = 0;

// override empty weak function from glue-lwip
extern "C" void netif_status_changed(struct netif* netif)
{
    for (size_t index = 0; index < size; ++index)
    {
        callbacks[index](netif);
    }
}

bool LwipIntf::statusChangeCB(LwipIntf::CBType cb)
{
    if (size < LwipIntfCallbacks)
    {
        callbacks[size++] = std::move(cb);
        return true;
    }
#if defined(DEBUG_ESP_CORE)
    DEBUGV("LwipIntf::CB %zu/%zu, cannot add more!\n", size, size);
#endif

    return false;
}

bool LwipIntf::stateUpCB(LwipIntf::CBType cb)
{
    return statusChangeCB(
        [cb](netif* interface)
        {
            if (netif_is_up(interface))
            {
                cb(interface);
            }
        });
}

bool LwipIntf::stateDownCB(LwipIntf::CBType cb)
{
    return statusChangeCB(
        [cb](netif* interface)
        {
            if (!netif_is_up(interface))
            {
                cb(interface);
            }
        });
}
