/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MPublisher.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2012 Philipp Geyer <nistur.com>
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

#include "MPublisher.h"

MPublisher::MPublisher()
: m_maxPriorities(10)
{
	m_events = new eventVec[m_maxPriorities];
}

MPublisher::~MPublisher()
{
	for(int i = 0; i < m_maxPriorities; ++i)
	{
		for(int j = 0; j < m_events[i].size(); ++j)
		{
			delete m_events[i][j];
		}
		m_events[i].clear();
	}
	delete [] m_events;
}

void MPublisher::addEvent(MPublishEvent* pubEvent)
{
	if(!pubEvent)
		return;

	// make sure we cap at whatever our max is
	int priority = pubEvent->getPriority();
	if(priority >= m_maxPriorities)
		priority = m_maxPriorities - 1;

	m_events[priority].push_back(pubEvent);

}

void MPublisher::publish(const char* projName)
{
	for(int prio = 0; prio < m_maxPriorities; ++prio)
	{
		for(int ev = 0; ev < m_events[prio].size(); ++ev)
		{
			m_events[prio][ev]->execute(projName);
		}
	}
}