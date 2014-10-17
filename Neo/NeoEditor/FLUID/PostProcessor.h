#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <MEngine.h>

class PostProcessor : public MPostProcessor
{
    int m_textureWidth;
    int m_textureHeight;
public:
    bool draw(MOCamera *camera);
    void updateResolution();
};

#endif
