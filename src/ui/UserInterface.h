#ifndef ZOENGINE_USERINTERFACE_H
#define ZOENGINE_USERINTERFACE_H

#include "VkRenderData.h"

class UserInterface
{
public:
    bool init(VkRenderData &renderData) const;
    void createFrame(VkRenderData &renderData) const;
    void render(VkRenderData &renderData) const;
    void cleanup(VkRenderData &renderData) const;
};


#endif //ZOENGINE_USERINTERFACE_H
