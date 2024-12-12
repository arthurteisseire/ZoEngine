#include "Model.h"
#include "Logger.h"

void Model::init()
{
    mVertexData.vertices.resize(36);

    glm::vec3 vertex0(0.f, 0.f, 0.f);
    glm::vec3 vertex1(1.f, 0.f, 0.f);
    glm::vec3 vertex2(1.f, 1.f, 0.f);
    glm::vec3 vertex3 = vertex1;
    glm::vec3 vertex4(1.f, 0.f, -1.f);
    glm::vec3 vertex5(1.f, 1.f, -1.f);
    glm::vec3 vertex6 = vertex4;
    glm::vec3 vertex7 = vertex1;
    glm::vec3 vertex8 = vertex0;
    glm::vec3 vertex9(0.f, 0.f, -1.f);
    glm::vec3 vertex10(0.f, 1.f, -1.f);
    glm::vec3 vertex11 = vertex9;
    glm::vec3 vertex12 = vertex0;
    glm::vec3 vertex13(0.f, 1.f, 0.f);

    // Front quad
    mVertexData.vertices[0].position = vertex0;
    mVertexData.vertices[1].position = vertex1;
    mVertexData.vertices[2].position = vertex2;
    mVertexData.vertices[0].color = glm::vec3(0.0f, 0.0f, 1.0f);
    mVertexData.vertices[1].color = glm::vec3(0.0f, 1.0f, 1.0f);
    mVertexData.vertices[2].color = glm::vec3(1.0f, 1.0f, 0.0f);
    mVertexData.vertices[0].uv = glm::vec2(0.0, 0.0);
    mVertexData.vertices[1].uv = glm::vec2(1.0, 0.0);
    mVertexData.vertices[2].uv = glm::vec2(1.0, 1.0);

    mVertexData.vertices[3].position = vertex0;
    mVertexData.vertices[4].position = vertex2;
    mVertexData.vertices[5].position = vertex13;
    mVertexData.vertices[3].color = glm::vec3(0.0f, 0.0f, 1.0f);
    mVertexData.vertices[4].color = glm::vec3(1.0f, 1.0f, 0.0f);
    mVertexData.vertices[5].color = glm::vec3(0.0f, 1.0f, 1.0f);
    mVertexData.vertices[3].uv = glm::vec2(0.0, 0.0);
    mVertexData.vertices[4].uv = glm::vec2(1.0, 1.0);
    mVertexData.vertices[5].uv = glm::vec2(0.0, 1.0);

    // Right quad
    mVertexData.vertices[6].position = vertex3;
    mVertexData.vertices[7].position = vertex4;
    mVertexData.vertices[8].position = vertex5;
    mVertexData.vertices[6].color = glm::vec3(0.0f, 0.0f, 1.0f);
    mVertexData.vertices[7].color = glm::vec3(0.0f, 1.0f, 1.0f);
    mVertexData.vertices[8].color = glm::vec3(1.0f, 1.0f, 0.0f);
    mVertexData.vertices[6].uv = glm::vec2(0.0, 0.0);
    mVertexData.vertices[7].uv = glm::vec2(1.0, 0.0);
    mVertexData.vertices[8].uv = glm::vec2(1.0, 1.0);

    mVertexData.vertices[9].position = vertex3;
    mVertexData.vertices[10].position = vertex5;
    mVertexData.vertices[11].position = vertex2;
    mVertexData.vertices[9].color = glm::vec3(0.0f, 0.0f, 1.0f);
    mVertexData.vertices[10].color = glm::vec3(1.0f, 1.0f, 0.0f);
    mVertexData.vertices[11].color = glm::vec3(0.0f, 1.0f, 1.0f);
    mVertexData.vertices[9].uv = glm::vec2(0.0, 0.0);
    mVertexData.vertices[10].uv = glm::vec2(1.0, 1.0);
    mVertexData.vertices[11].uv = glm::vec2(0.0, 1.0);

    // Top quad
    mVertexData.vertices[12].position = vertex9;
    mVertexData.vertices[13].position = vertex6;
    mVertexData.vertices[14].position = vertex7;
    mVertexData.vertices[12].color = glm::vec3(0.0f, 0.0f, 1.0f);
    mVertexData.vertices[13].color = glm::vec3(0.0f, 1.0f, 1.0f);
    mVertexData.vertices[14].color = glm::vec3(1.0f, 1.0f, 0.0f);
    mVertexData.vertices[12].uv = glm::vec2(0.0, 0.0);
    mVertexData.vertices[13].uv = glm::vec2(1.0, 0.0);
    mVertexData.vertices[14].uv = glm::vec2(1.0, 1.0);

    mVertexData.vertices[15].position = vertex9;
    mVertexData.vertices[16].position = vertex7;
    mVertexData.vertices[17].position = vertex8;
    mVertexData.vertices[15].color = glm::vec3(0.0f, 0.0f, 1.0f);
    mVertexData.vertices[16].color = glm::vec3(1.0f, 1.0f, 0.0f);
    mVertexData.vertices[17].color = glm::vec3(0.0f, 1.0f, 1.0f);
    mVertexData.vertices[15].uv = glm::vec2(0.0, 0.0);
    mVertexData.vertices[16].uv = glm::vec2(1.0, 1.0);
    mVertexData.vertices[17].uv = glm::vec2(0.0, 1.0);

    // Left quad
    mVertexData.vertices[18].position = vertex11;
    mVertexData.vertices[19].position = vertex12;
    mVertexData.vertices[20].position = vertex13;
    mVertexData.vertices[18].color = glm::vec3(0.0f, 0.0f, 1.0f);
    mVertexData.vertices[19].color = glm::vec3(0.0f, 1.0f, 1.0f);
    mVertexData.vertices[20].color = glm::vec3(1.0f, 1.0f, 0.0f);
    mVertexData.vertices[18].uv = glm::vec2(0.0, 0.0);
    mVertexData.vertices[19].uv = glm::vec2(1.0, 0.0);
    mVertexData.vertices[20].uv = glm::vec2(1.0, 1.0);

    mVertexData.vertices[21].position = vertex11;
    mVertexData.vertices[22].position = vertex13;
    mVertexData.vertices[23].position = vertex10;
    mVertexData.vertices[21].color = glm::vec3(0.0f, 0.0f, 1.0f);
    mVertexData.vertices[22].color = glm::vec3(1.0f, 1.0f, 0.0f);
    mVertexData.vertices[23].color = glm::vec3(0.0f, 1.0f, 1.0f);
    mVertexData.vertices[21].uv = glm::vec2(0.0, 0.0);
    mVertexData.vertices[22].uv = glm::vec2(1.0, 1.0);
    mVertexData.vertices[23].uv = glm::vec2(0.0, 1.0);

    // Bottom quad
    mVertexData.vertices[24].position = vertex13;
    mVertexData.vertices[25].position = vertex2;
    mVertexData.vertices[26].position = vertex5;
    mVertexData.vertices[24].color = glm::vec3(0.0f, 0.0f, 1.0f);
    mVertexData.vertices[25].color = glm::vec3(0.0f, 1.0f, 1.0f);
    mVertexData.vertices[26].color = glm::vec3(1.0f, 1.0f, 0.0f);
    mVertexData.vertices[24].uv = glm::vec2(0.0, 0.0);
    mVertexData.vertices[25].uv = glm::vec2(1.0, 0.0);
    mVertexData.vertices[26].uv = glm::vec2(1.0, 1.0);

    mVertexData.vertices[27].position = vertex13;
    mVertexData.vertices[28].position = vertex5;
    mVertexData.vertices[29].position = vertex10;
    mVertexData.vertices[27].color = glm::vec3(0.0f, 0.0f, 1.0f);
    mVertexData.vertices[28].color = glm::vec3(1.0f, 1.0f, 0.0f);
    mVertexData.vertices[29].color = glm::vec3(0.0f, 1.0f, 1.0f);
    mVertexData.vertices[27].uv = glm::vec2(0.0, 0.0);
    mVertexData.vertices[28].uv = glm::vec2(1.0, 1.0);
    mVertexData.vertices[29].uv = glm::vec2(0.0, 1.0);

    // Back quad
    mVertexData.vertices[30].position = vertex6;
    mVertexData.vertices[31].position = vertex9;
    mVertexData.vertices[32].position = vertex10;
    mVertexData.vertices[30].color = glm::vec3(0.0f, 0.0f, 1.0f);
    mVertexData.vertices[31].color = glm::vec3(0.0f, 1.0f, 1.0f);
    mVertexData.vertices[32].color = glm::vec3(1.0f, 1.0f, 0.0f);
    mVertexData.vertices[30].uv = glm::vec2(0.0, 0.0);
    mVertexData.vertices[31].uv = glm::vec2(1.0, 0.0);
    mVertexData.vertices[32].uv = glm::vec2(1.0, 1.0);

    mVertexData.vertices[33].position = vertex6;
    mVertexData.vertices[34].position = vertex10;
    mVertexData.vertices[35].position = vertex5;
    mVertexData.vertices[33].color = glm::vec3(0.0f, 0.0f, 1.0f);
    mVertexData.vertices[34].color = glm::vec3(1.0f, 1.0f, 0.0f);
    mVertexData.vertices[35].color = glm::vec3(0.0f, 1.0f, 1.0f);
    mVertexData.vertices[33].uv = glm::vec2(0.0, 0.0);
    mVertexData.vertices[34].uv = glm::vec2(1.0, 1.0);
    mVertexData.vertices[35].uv = glm::vec2(0.0, 1.0);

    for (MeshVertex& v : mVertexData.vertices)
    {
        v.position.z -= 1.0f;
    }

    Logger::info("%s: loaded %d vertices\n", __FUNCTION__, mVertexData.vertices.size());
}

BasicMesh Model::getVertexData()
{
    return mVertexData;
}
