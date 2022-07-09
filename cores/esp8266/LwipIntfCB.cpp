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

#define NETIF_STATUS_CB_SIZE 3

static int       netifStatusChangeListLength = 0;
LwipIntf::CBType netifStatusChangeList[NETIF_STATUS_CB_SIZE];

extern "C" void netif_status_changed(struct netif* netif)
{
    // override the default empty weak function
    for (int i = 0; i < netifStatusChangeListLength; i++)
    {
        netifStatusChangeList[i](netif);
    }
}

bool LwipIntf::stateChangeSysCB(LwipIntf::CBType&& cb)
{
    if (netifStatusChangeListLength >= NETIF_STATUS_CB_SIZE)
    {
#if defined(DEBUG_ESP_CORE)
        DEBUGV("NETIF_STATUS_CB_SIZE is too low\n");
#endif
        return false;
    }

    netifStatusChangeList[netifStatusChangeListLength++] = cb;
    return true;
}

bool LwipIntf::stateUpCB(LwipIntf::CBType&& cb)
{
    return stateChangeSysCB(
        [cb](netif* nif)
        {
            if (netif_is_up(nif))
                schedule_function(
                    [cb, nif]()
                    {
                        cb(nif);
                    });
        });
}
