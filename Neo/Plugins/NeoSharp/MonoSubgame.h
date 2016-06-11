#ifndef NEOPLUGIN_MONOSUBGAME_H_
#define NEOPLUGIN_MONOSUBGAME_H_

#include <NeoEngine.h>

class MonoSubgame: public Neo::SubGame
{
public:
	MonoSubgame();
	virtual ~MonoSubgame();

	void onBegin();
	void update();
	void draw() {}
	void onEnd();
};

#endif /* NEOPLUGIN_MONOSUBGAME_H_ */
