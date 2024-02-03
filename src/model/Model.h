#ifndef ZOENGINE_MODEL_H
#define ZOENGINE_MODEL_H


#include "VkRenderData.h"

class Model
{
public:
    void init();

    BasicMesh getVertexData();

private:
    BasicMesh mVertexData;
};


#endif //ZOENGINE_MODEL_H
