/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MPublisher.h
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

#ifndef __M_PUBLISHER_H__
#define __M_PUBLISHER_H__

#include "MPublishEvent.h"
#include <vector>

/*--------------------------------------------------------------------------------
 * MPublisher
 * Editor singleton to handle publishing of a Maratis project
 * By default the publisher has 10 different priority levels
 *-------------------------------------------------------------------------------*/
class MPublisher
{
private:
	typedef std::vector<MPublishEvent*> eventVec;
	eventVec*							m_events;
	
	int									m_maxPriorities;
	
public:
	
	MPublisher();
	~MPublisher();

	// instance
	static MPublisher * getInstance(void)
	{
		static MPublisher m_instance;
		return &m_instance;
	}
	
	void addEvent(MPublishEvent* pubEvent);
	void publish(const char* projName);
};

#endif /*__M_PUBLISHER_H__*/