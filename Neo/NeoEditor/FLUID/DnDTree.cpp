//========================================================================
//	Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
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

#include "DnDTree.h"
#include <NeoEngine.h>
#include "../Maratis/Maratis.h"
#include <FL/fl_message.H>

using namespace Neo;

int DnDTree::handle(int e)
{
    switch(e)
    {
    case FL_DND_ENTER:
    case FL_DND_DRAG:
    case FL_DND_RELEASE:
        return 1;
        break;

    case FL_DRAG:
        {
            Fl_Tree_Item* item = first_selected_item();
            if(!item || !strcmp(item->label(), "ROOT"))
                return 1;

            Fl::copy(item->label(), strlen(item->label()), 0);
            Fl::dnd();
            return 1;
        }
        break;

    case FL_PASTE:
        {
            Scene* scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();
            Fl_Tree::handle(FL_PUSH);

            Fl_Tree_Item* item = first_selected_item();

            if(item == NULL)
                return 1;

            Object3d* newParent = scene->getObjectByName(item->label());
            Object3d* object = scene->getObjectByName(Fl::event_text());

            if(newParent == NULL && object)
            {
                object->unLink();
                this->do_callback(this, (long) 1);
                return 1;
            }

            if(!object || !newParent)
            {
                fl_message("Object you tried to drag does not exist! This shouldn't happen!");
                return 0;
            }

            Object3d* parent = object->getParent();

            if(object == newParent)
                return 1;

            if(parent == newParent)
                return 1;

            if(object == parent)
                return 1;

            if(object == newParent->getParent())
                Maratis::getInstance()->unlinkTwoObjects(object, newParent);

            if(parent)
                Maratis::getInstance()->unlinkTwoObjects(parent, object);

            Maratis::getInstance()->linkTwoObjects(newParent, object);

            this->do_callback(this, (long) 1);
            return 1;
        }
    }

    return Fl_Tree::handle(e);
}
