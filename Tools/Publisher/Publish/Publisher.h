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

#ifndef __PUBLISHER_H__
#define __PUBLISHER_H__

#include <vector>

#include "PublishEvent.h"

namespace Neo
{

/*--------------------------------------------------------------------------------
 * MPublisher
 * Editor singleton to handle publishing of a Maratis project
 * By default the publisher has 10 different priority levels
 *-------------------------------------------------------------------------------*/
class Publisher
{
private:
	typedef std::vector<PublishEvent*> eventVec;
	eventVec*							m_events;
	
	int									m_maxPriorities;
	
public:
	
	Publisher();
	~Publisher();

	// instance
	static Publisher * getInstance(void)
	{
		static Publisher m_instance;
		return &m_instance;
	}
	
	void addEvent(PublishEvent* pubEvent);
	void publish(const char* projName, const char* dest, const char* exec, bool verbose);
};
}
#endif /*__PUBLISHER_H__*/
