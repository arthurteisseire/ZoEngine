#ifndef ZOENGINE_GLTFLOADER_H
#define ZOENGINE_GLTFLOADER_H

#include <tiny_gltf.h>
#include "VkRenderData.h"

class GLTFLoader
{
public:
    static std::optional<VMesh> LoadGLTF(const std::string& filepath);

private:
    static std::optional<int> ExtractElementSize(int accessorType);
    static std::optional<int> ExtractComponentSize(int accessorComponentType);
    static std::optional<uint32_t> GetElementByteSize(const tinygltf::Accessor &accessor);

    struct IndexInfo
    {
        VkIndexType indexType;
        uint32_t componentSize = 0;
    };
    static std::optional<IndexInfo> GetIndexInfo(const tinygltf::Accessor &accessor);
};


#endif //ZOENGINE_GLTFLOADER_H
