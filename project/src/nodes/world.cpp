#include "world.h"

#include "chunk.h"
#include "block.h"
#include "player.h"

#include "common/draw.h"
#include "physics/physics_server.h"

#include "common/raylib.h"
#include <btBulletCollisionCommon.h>
#include <json.hpp>
using json = nlohmann::json;

World* World::singleton = NULL;

void setupChunk(Chunk* chunk, Vector2 grid_pos, Chunk* chunks[CHUNK_AMOUNT][CHUNK_AMOUNT]) {
    chunk->setup(grid_pos, chunks);
}

void World::init() {
    super::init();

    ASSERT(World::singleton == NULL);
    World::singleton = this;

    player = new Player;

    addChild(player);
    player->setPosition(Vector3(CHUNK_AMOUNT * 0.5f * CHUNK_SIZE, SUBCHUNK_COUNT * SUBCHUNK_HEIGHT, CHUNK_AMOUNT * 0.5f * CHUNK_SIZE));

    material = LoadMaterialDefault();
    material.shader = LoadShaderFromMemory(

// Vertex shader
#if PLATFORM != PLATFORM_VITA
R"V0G0N(

#version 330                      
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec2 vertexTexCoord2;
in vec3 vertexNormal;
in vec4 vertexColor;

out vec2 uv;
out vec2 atlasPosition;
out vec2 tileSize;
out vec4 fragColor;
out mat2 rotation;
uniform mat4 mvp;

void main() {
    uv = vertexTexCoord;
    atlasPosition = vertexTexCoord2;
    tileSize = vertexNormal.yx;
    
    float sin_factor = sin(vertexNormal.z);
    float cos_factor = cos(vertexNormal.z);
    rotation = mat2(cos_factor, sin_factor, -sin_factor, cos_factor);

    fragColor = vertexColor;      
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}

)V0G0N",
#else
R"V0G0N(
void main(float3 vertexPosition,             
    float2 vertexTexCoord, float2 vertexTexCoord2,
    float3 vertexNormal,            
    float4 vertexColor,                
    float2 out uv : TEXCOORD0,
    float2 out tileSize: TEXCOORD1,
    float2 out atlasPosition: TEXCOORD2,
    float4 out fragColor : COLOR0,      
    float4 out gl_Position : POSITION, 
    uniform float4x4 mvp) {

    uv = vertexTexCoord;
    atlasPosition = vertexTexCoord2;
    tileSize = vertexNormal.yx;
    fragColor = vertexColor;      
    gl_Position = mul(mvp, float4(vertexPosition, 1.0));
}
)V0G0N",
#endif

// Fragment shader
#if PLATFORM != PLATFORM_VITA
R"V0G0N(

#version 330
in vec2 uv;
in vec4 fragColor;
in vec2 atlasPosition;
in vec2 tileSize;
in mat2 rotation;

out vec4 finalColor;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform vec4 colDiffuse;
uniform vec2 atlasSize;

void main() {

    vec2 coord = mod(uv, 1.0 / tileSize) * tileSize;
    coord = vec2((coord.x - 0.5) * (atlasSize.x / atlasSize.y), coord.y - 0.5) * rotation;
    coord += 0.5;

    vec4 texelColor = texture(texture0, (atlasPosition + coord) / atlasSize);
    finalColor = texelColor * colDiffuse * fragColor; // * texture(texture1, uv);
}

)V0G0N"
#else
R"V0G0N(

float mod(float a, float b) {
    return a * floor(a / b);
}

float2 mod(float2 a, float2 b) {
    return float2(mod(a.x, b.x), mod(a.y, b.y));
}

void main(float2 uv : TEXCOORD0, 
    float2 atlasPosition: TEXCOORD2, float2 tileSize: TEXCOORD1,
    float4 fragColor : COLOR0,                  
    uniform sampler2D texture0,        
    uniform float4 colDiffuse,
    uniform float2 atlasSize,        
    float4 out finalColor) {
    float4 texelColor = tex2D(texture0, (atlasPosition + (mod(uv, 1.0 / tileSize) * tileSize)) / atlasSize);
    finalColor = texelColor * colDiffuse * fragColor;
}
)V0G0N"
#endif
    );

    // material.shader = LoadShader(0, 0);

    material.maps[MATERIAL_MAP_DIFFUSE].color = Colour::WHITE();

    Texture2D texture = OS::getRaylibTexture(OS::loadTexture("project/resources/terrain.png"));
    material.maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    // Image image = GenImageChecked(2, 2, 1, 1, Colour(0.5, 0.5, 1), Colour(1, 1, 1));
    // material.maps[MATERIAL_MAP_SPECULAR].texture = LoadTextureFromImage(image);
    // UnloadImage(image);

    float* atlasSize = (float*)malloc(2 * sizeof(float));
    atlasSize[0] = sqrt(texture.width);
    atlasSize[1] = sqrt(texture.height);

    SetShaderValue(material.shader, GetShaderLocation(material.shader, "atlasSize"), atlasSize, SHADER_UNIFORM_VEC2);
    // SetTextureWrap(material.maps[MATERIAL_MAP_DIFFUSE].texture, TEXTURE_WRAP_REPEAT);

    // Load block data
    Block::loadTypeData();

    // Create initial chunks
    chunks = new Chunk;
    Chunk* chunk = NULL;

    Chunk* chunk_array[CHUNK_AMOUNT][CHUNK_AMOUNT];

    for (int x = 0; x < CHUNK_AMOUNT; x++) {
        for (int y = 0; y < CHUNK_AMOUNT; y++) {
            if (chunk == NULL) {
                chunk = chunks;
            }
            else {
                Chunk* new_chunk = new Chunk;
                new_chunk->previous = chunk;
                chunk->next = new_chunk;
                chunk = new_chunk;
            }

            chunk_array[x][y] = chunk;
            chunk->createBlocks();
            addChild(chunk);
        }
    }

    for (int x = 0; x < CHUNK_AMOUNT; x++) {
        for (int y = 0; y < CHUNK_AMOUNT; y++) {
            chunk_array[x][y]->setup(Vector2(x, y), chunk_array);
        }
    }
}

void World::draw() {
    super::draw();

    if (!isVisible()) {
        return;
    }

    for (Chunk* chunk = chunks; chunk != NULL; chunk = chunk->next ) {
        for (int i = 0; i < SUBCHUNK_COUNT; i++) {
            chunk->sub_chunks[i]->drawn = false;
            chunk->sub_chunks[i]->drawChunk();
        }
    }

    return;

    Chunk* starting_chunk = player->getEnteredChunk();
    if (starting_chunk == NULL) {
        starting_chunk = getChunk((int)(CHUNK_AMOUNT * 0.5) * CHUNK_SIZE, (int)(CHUNK_AMOUNT * 0.5) * CHUNK_SIZE, false);
    }
    
    Chunk* chunk = chunks;
    while (chunk != NULL) {
        for (int i = 0; i < SUBCHUNK_COUNT; i++) {
            chunk->sub_chunks[i]->drawn = false;
        }
        chunk = chunk->next;
    }

    vector<SubChunk*> draw_queue = {starting_chunk->sub_chunks[SUBCHUNK_COUNT - 1]};

    #define ADD(subchunk) \
    if (!subchunk->drawn) { \
        draw_queue.push_back(subchunk); \
    } \

    while (!draw_queue.empty()) {

        SubChunk* chunk = *(draw_queue.end() - 1);
        draw_queue.pop_back();

        chunk->drawChunk();

        if (chunk->index == 0 || chunk->index == SUBCHUNK_COUNT - 1) {
            if (chunk->chunk->getFront())
                ADD(chunk->chunk->getFront()->sub_chunks[chunk->index]);
            if (chunk->chunk->getBack())
                ADD(chunk->chunk->getBack()->sub_chunks[chunk->index]);
            if (chunk->chunk->getLeft())
                ADD(chunk->chunk->getLeft()->sub_chunks[chunk->index]);
            if (chunk->chunk->getRight())
                ADD(chunk->chunk->getRight()->sub_chunks[chunk->index]);
        }
    }
}

Chunk* World::getChunk(int x, int y, bool grid_pos) {
    Chunk* chunk = chunks;
    
    if (x < 0 || y < 0) {
        return NULL;
    }

    while (chunk != NULL) {
        Vector2 position = chunk->getGridPos();
        if (!grid_pos)
            position *= CHUNK_SIZE;
            if (x >= position.x && x < position.x + CHUNK_SIZE && y >= position.y && y < position.y + CHUNK_SIZE) {
                return chunk;
            }
        else if (position.x == x && position.y == y) {
            return chunk;
        }

        chunk = chunk->next;
    }

    return NULL;
}

void World::loadChunk(Vector2 grid_position) {

}

void World::unloadChunk(Vector2 grid_position) {

}

void World::requestSubChunkMeshGeneration(SubChunk* chunk) {

    // if (CHUNKMESH_GENERATION_THREADS == 0) {
    chunk->generateMesh();
        // return;
    // }

    // for (int i = 0; i < CHUNKMESH_GENERATION_THREADS; i++) {
    //     if (!gen_threads[i].isRunning()) {
    //         gen_threads[i].set(chunk, &SubChunk::generateMesh);
    //         gen_threads[i].SIGNAL_FINISHED.connect(chunk, &SubChunk::onMeshGenerationFinished, true);
    //         gen_threads[i].start();
    //     }
    // }
}

Player* World::getPlayer() {
    return player;
}