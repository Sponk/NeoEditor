/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MPublishEvent.h
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

#ifndef __M_PUBLISH_EVENT_H__
#define __M_PUBLISH_EVENT_H__

/*--------------------------------------------------------------------------------
 * MPublishEvent
 * Base class for publish events
 * Can be ordered in priority, but no guarantee as to order within
 * each priority.
 *-------------------------------------------------------------------------------*/
// base command class for publish events.
class MPublishEvent
{
public:
	/* execute
	 * Run the publish event
	 * projName: global path of the mproj file to be
	 *           exported
	 */
	virtual void	execute(const char* projName) = 0;

	/* getPriority
	 * Which priority queue to put thie event in
	 */
	virtual int		getPriority() = 0;
};

// Macro for automatic adding to the publish event queue
#define M_PUBLISH_EVENT_IMPLEMENT(eventName) \
class eventName##AutoAdd \
{ \
public: \
	eventName##AutoAdd() \
	{ \
		MPublisher* pub = MPublisher::getInstance(); \
		pub->addEvent(new eventName); \
	} \
}; \
static eventName##AutoAdd s_##eventName;

#endif /*__M_PUBLISH_EVENT_H__*/