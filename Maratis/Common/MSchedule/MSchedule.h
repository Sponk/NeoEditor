/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MSchedule.h
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

#ifndef MSCHEDULE_H
#define MSCHEDULE_H

#include <MThread.h>

class MScheduleEvent
{
    void (*m_event)(void*, bool*);
    void* m_data;
    bool* m_success;
    bool* m_isDone;

public:
    MScheduleEvent(void (*nevent)(void*, bool*), void* data, bool* success);
    MScheduleEvent() {}

    void Run();
    void SetIsDone(bool* done) { m_isDone = done; }
};

void MUpdateScheduledEvents();
void MSchedule(MScheduleEvent event);
void MInitSchedule();

#endif