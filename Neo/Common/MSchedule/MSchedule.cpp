/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MSchedule.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
//  Maratis, Copyright (c) 2014 Yannick Pflanzer <www.scary-squid.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//========================================================================

#include "MSchedule.h"
#include "MSDLThread/MSDLThread.h"
#include <vector>
#include <MWindow.h>

using namespace std;

vector<MScheduleEvent> events;

MSDLSemaphore scheduleSemaphore;

MScheduleEvent::MScheduleEvent(void (*nevent)(void *, bool*), void *data, bool* success) :
m_event(nevent),
m_data(data),
m_success(success)
{

}

void MScheduleEvent::Run()
{
    if(m_event)
        m_event(m_data, m_success);

    if(m_isDone)
        *m_isDone = true;
}

void MUpdateScheduledEvents()
{
    for(int i = 0; i < events.size(); i++)
    {
        events[i].Run();
    }

    events.clear();
}

void MSchedule(MScheduleEvent event)
{
    bool isDone = false;

    event.SetIsDone(&isDone);

    //MSemaphoreWaitAndLock(&scheduleSemaphore);
    events.push_back(event);
    //MSemaphoreUnlock(&scheduleSemaphore);

    MWindow::getInstance()->sleep(200);
}

void MInitSchedule()
{
    scheduleSemaphore.Init(1);
}
