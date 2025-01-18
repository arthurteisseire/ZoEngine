#ifndef ZOENGINE_CUSTOMCUBE_H
#define ZOENGINE_CUSTOMCUBE_H


#include "VkRenderData.h"

class CustomCube
{
public:
    void init();

    BasicMesh getVertexData();

private:
    BasicMesh mVertexData;
};


#endif //ZOENGINE_CUSTOMCUBE_H
