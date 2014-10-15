#include "DnDTree.h"
#include <MEngine.h>
#include "../Maratis/Maratis.h"
#include <FL/fl_message.H>

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
            MScene* scene = MEngine::getInstance()->getLevel()->getCurrentScene();
            Fl_Tree::handle(FL_PUSH);

            Fl_Tree_Item* item = first_selected_item();

            if(item == NULL)
                return 1;

            MObject3d* newParent = scene->getObjectByName(item->label());
            MObject3d* object = scene->getObjectByName(Fl::event_text());

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

            MObject3d* parent = object->getParent();

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
