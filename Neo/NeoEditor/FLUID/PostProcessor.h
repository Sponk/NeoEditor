#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <NeoEngine.h>

class PostProcessor : public Neo::MPostProcessor
{
    int m_textureWidth;
    int m_textureHeight;
public:
    bool draw(Neo::OCamera *camera);
    void updateResolution();
};

#endif
