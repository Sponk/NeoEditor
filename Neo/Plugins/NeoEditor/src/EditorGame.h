#ifndef NEO_EDITORGAME_H
#define NEO_EDITORGAME_H

#include <NeoEngine.h>

class EditorGame : public Neo::SubGame
{
public:
	virtual void update();
	virtual void draw();
	virtual void onBegin();
	virtual void onEnd();
};

#endif //NEO_EDITORGAME_H
