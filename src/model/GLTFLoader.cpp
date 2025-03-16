#include "GLTFLoader.h"
#include "Logger.h"

struct AttributeInfo
{
    const uint8_t *ptr; // pointer to the attribute data
    size_t stride;      // byte stride (or element size if tightly packed)
    size_t count;       // number of elements
};

static std::optional<AttributeInfo> GetAttributeInfo(const tinygltf::Model &model,
                                                     const tinygltf::Primitive &primitive,
                                                     const std::string &attributeName,
                                                     size_t defaultSize)
{
    // Check if the attribute exists in the primitive.
    auto it = primitive.attributes.find(attributeName);
    if (it == primitive.attributes.end())
    {
        Logger::error("Attribute '%s' not found in primitive\n", __FUNCTION__, attributeName.c_str());
        return std::nullopt;
    }

    int accessorIndex = it->second;
    if (accessorIndex < 0 || accessorIndex >= static_cast<int>(model.accessors.size()))
    {
        Logger::error("Accessor index %i for attribute '%s' is out of range\n", __FUNCTION__, accessorIndex,
                      attributeName.c_str());
        return std::nullopt;
    }
    const tinygltf::Accessor &acc = model.accessors[accessorIndex];

    if (acc.bufferView < 0 || acc.bufferView >= static_cast<int>(model.bufferViews.size()))
    {
        Logger::error("Error: Buffer view index %i for attribute '%s' is out of range\n", __FUNCTION__, acc.bufferView,
                      attributeName.c_str());
        return std::nullopt;
    }
    const tinygltf::BufferView &view = model.bufferViews[acc.bufferView];

    if (view.buffer < 0 || view.buffer >= static_cast<int>(model.buffers.size()))
    {
        Logger::error("Error: Buffer index %i for attribute '%s' is out of range.\n", __FUNCTION__, view.buffer,
                      attributeName.c_str());
        return std::nullopt;
    }
    const tinygltf::Buffer &buffer = model.buffers[view.buffer];
    if (buffer.data.empty())
    {
        Logger::error("Error: Buffer %i for attribute '%s' is empty.\n", __FUNCTION__, view.buffer,
                      attributeName.c_str());
        return std::nullopt;
    }

    const uint8_t *ptr = buffer.data.data() + view.byteOffset + acc.byteOffset;
    size_t stride = view.byteStride ? view.byteStride : defaultSize;
    return AttributeInfo{ptr, stride, static_cast<size_t>(acc.count)};
}

std::optional<VMesh> GLTFLoader::LoadGLTF(const std::string &filepath)
{
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string error;
    std::string warning;
    loader.LoadASCIIFromFile(&model, &error, &warning, filepath);

    if (!error.empty())
    {
        Logger::error("Failed to load gltf file '%s'.", __FUNCTION__, filepath.c_str());
        return {};
    }

    const tinygltf::Mesh &mesh = model.meshes[1];
    const tinygltf::Primitive &primitive = mesh.primitives[0];

    std::optional<AttributeInfo> posInfo = GetAttributeInfo(model, primitive, "POSITION", sizeof(glm::vec3));
    std::optional<AttributeInfo> normInfo = GetAttributeInfo(model, primitive, "NORMAL", sizeof(glm::vec3));
    std::optional<AttributeInfo> texInfo = GetAttributeInfo(model, primitive, "TEXCOORD_0", sizeof(glm::vec2));

    if (posInfo.has_value() && normInfo.has_value() && texInfo.has_value())
    {
        std::vector<VVertex> vertices;
        vertices.resize(posInfo->count);

        for (size_t i = 0; i < posInfo->count; ++i)
        {
            VVertex &v = vertices[i];
            v.position = *reinterpret_cast<const glm::vec3 *>(posInfo->ptr + i * posInfo->stride);
            v.normal = *reinterpret_cast<const glm::vec3 *>(normInfo->ptr + i * normInfo->stride);
            v.texcoord = *reinterpret_cast<const glm::vec2 *>(texInfo->ptr + i * texInfo->stride);
        }

        // Indices
        const tinygltf::Accessor &acc = model.accessors[primitive.indices];
        std::optional<IndexInfo> oIndexInfo = GetIndexInfo(acc);
        if (!oIndexInfo.has_value())
        {
            Logger::error("Unknown index size.\n", __FUNCTION__);
            return {};
        }
        const IndexInfo& indexInfo = oIndexInfo.value();
        const tinygltf::BufferView &view = model.bufferViews[acc.bufferView];
        const tinygltf::Buffer &buffer = model.buffers[view.buffer];
        const uint8_t* indexData = buffer.data.data() + view.byteOffset + acc.byteOffset;
        size_t indexBufferSize = acc.count * indexInfo.componentSize;
        std::vector<uint8_t> indices(indexBufferSize);
        memcpy(indices.data(), indexData, indexBufferSize);

        return VMesh{std::move(vertices), indexInfo.indexType, std::move(indices)};
    }
    return {};
}

std::optional<uint32_t> GLTFLoader::GetElementByteSize(const tinygltf::Accessor &accessor)
{
    const std::optional<int> oElementSize = ExtractElementSize(accessor.type);
    if (!oElementSize)
    {
        Logger::error("accessor uses unknown element size %i\n", __FUNCTION__, accessor.type);
        return {};
    }
    const int elementSize = oElementSize.value();

    std::optional<int> oComponentSize = ExtractComponentSize(accessor.componentType);
    if (!oComponentSize)
    {
        Logger::error("accessor uses unknown component size %i\n", __FUNCTION__,
                      accessor.componentType);
        return {};
    }
    const int componentSize = oComponentSize.value();

    return componentSize * elementSize;
}

std::optional<int> GLTFLoader::ExtractComponentSize(int componentType)
{
    switch (componentType)
    {
        case TINYGLTF_COMPONENT_TYPE_BYTE:
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            return 1;
        case TINYGLTF_COMPONENT_TYPE_SHORT:
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            return 2;
        case TINYGLTF_COMPONENT_TYPE_INT:
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            return 4;
        case TINYGLTF_COMPONENT_TYPE_DOUBLE:
            return 8;
        default:
            return std::nullopt;
    }
}

std::optional<int> GLTFLoader::ExtractElementSize(const int accessorType)
{
    switch (accessorType)
    {
        case TINYGLTF_TYPE_SCALAR:
            return 1;
        case TINYGLTF_TYPE_VEC2:
            return 2;
        case TINYGLTF_TYPE_VEC3:
            return 3;
        case TINYGLTF_TYPE_VEC4:
            return 4;
        default:
            break;
    }
    return {};
}

std::optional<GLTFLoader::IndexInfo> GLTFLoader::GetIndexInfo(const tinygltf::Accessor &accessor)
{
    switch (accessor.componentType) {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            return IndexInfo{VK_INDEX_TYPE_UINT8_EXT, sizeof(uint8_t)};
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            return IndexInfo{VK_INDEX_TYPE_UINT16, sizeof(uint16_t)};
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            return IndexInfo{VK_INDEX_TYPE_UINT32, sizeof(uint32_t)};
        default:
            return {};
    }
    return {};
}
