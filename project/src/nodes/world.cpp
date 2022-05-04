#include "world.h"

#include "player.h"
#include "common/draw.h"
#include "physics/physics_server.h"

#include "common/raylib.h"
#include <btBulletCollisionCommon.h>
#include <json.hpp>
using json = nlohmann::json;

Block::TypeData Block::type_data[BLOCK_TYPE_COUNT];

void World::init() {
    // setKinematic(true);

    player = new Player;
    player->world = this;

    addChild(player);
    player->setPosition(Vector3(CHUNK_AMOUNT * 0.5f * CHUNK_SIZE, SUBCHUNK_COUNT * SUBCHUNK_HEIGHT, CHUNK_AMOUNT * 0.5f * CHUNK_SIZE));

    material = LoadMaterialDefault();
    material.shader = LoadShader(0, 0);
    material.maps[MATERIAL_MAP_DIFFUSE].color = Colour(0.8, 0.8, 0.8);
    material.maps[MATERIAL_MAP_DIFFUSE].texture = OS::getRaylibTexture(OS::loadTexture("project/resources/terrain.png"));

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
            chunk->world = this;
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

    if (starting_chunk == NULL) {
        starting_chunk = getChunk((int)(CHUNK_AMOUNT * 0.5) * CHUNK_SIZE, (int)(CHUNK_AMOUNT * 0.5) * CHUNK_SIZE, false)->sub_chunks[SUBCHUNK_COUNT - 1];
    }
    
    Chunk* chunk = chunks;
    while (chunk != NULL) {
        for (int i = 0; i < SUBCHUNK_COUNT; i++) {
            chunk->sub_chunks[i]->drawn = false;
        }
        chunk = chunk->next;
    }

    vector<SubChunk*> draw_queue = {starting_chunk};

    #define ADD(subchunk) \
    if (!subchunk->drawn) { \
        draw_queue.push_back(subchunk); \
    } \

    while (!draw_queue.empty()) {

        SubChunk* chunk = *(draw_queue.end() - 1);
        draw_queue.pop_back();

        chunk->draw();

        if (chunk->index == 0 || chunk->index == SUBCHUNK_COUNT - 1) {
            if (chunk->chunk->front)
                ADD(chunk->chunk->front->sub_chunks[chunk->index]);
            if (chunk->chunk->back)
                ADD(chunk->chunk->back->sub_chunks[chunk->index]);
            if (chunk->chunk->left)
                ADD(chunk->chunk->left->sub_chunks[chunk->index]);
            if (chunk->chunk->right)
                ADD(chunk->chunk->right->sub_chunks[chunk->index]);
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

void Block::loadTypeData() {
    json block_data = json::parse(OS::loadFileText(OS::getResPath("project/resources/block_data.json")), nullptr, false, true);
    ASSERT(block_data.is_object());

    for (auto& [name, data] : block_data.items()) {
    
        ASSERT(data.is_object());
        
        // Get block type enum from string name
        Block::TYPE type = Block::getTypeFromString(name);
        ASSERT_MSG((int)type >= 0, "Invalid block name: " + name);

        // Get the data container struct for this type
        Block::TypeData& type_data = Block::type_data[(int)type];
        ASSERT(!type_data.populated);
        type_data.populated = true;

        if (data.contains("tangible")) {
            ASSERT(data["tangible"].is_boolean());
            type_data.tangible = data["tangible"].get<bool>();
        }

        // Get texture coordinates
        if (data.contains("textures") && !data["textures"].empty()) {
            json texture_coords = data["textures"];
            ASSERT_MSG(texture_coords.is_object() || texture_coords.is_array(), texture_coords.type_name());
            
            #define CHECK_COORDS(coords) \
            ASSERT(coords.size() == 2); \
            ASSERT(coords[0].is_number_integer() && coords[0].get<int>() >= 0); \
            ASSERT(coords[1].is_number_integer() && coords[1].get<int>() >= 0);

            // Dictionary of faces
            if (texture_coords.is_object()) {
                json other = json::object();
                for (auto& [_face_name, coords] : texture_coords.items()) {
                    CHECK_COORDS(coords);
                    const string face_name = lowerString(_face_name);

                    // The 'else' coordinates are used for all unspecified faces
                    if (face_name == "e" || face_name == "else") {
                        ASSERT(other.empty());
                        other = coords;
                    }
                    else {
                        // Get face direction enum from string name
                        DIRECTION_3 face = stringToDirection3(face_name);
                        ASSERT(face != DIRECTION_3::NONE);
                        ASSERT(type_data.texcoords[(int)face][0] == -1)

                        type_data.texcoords[(int)face][0] = coords[0].get<int>();
                        type_data.texcoords[(int)face][1] = coords[1].get<int>();
                    }
                }

                if (!other.empty()) {
                    // Fill unspecified faces with the 'else' coordinates
                    bool other_used = false;
                    for (int face = 0; face < DIRECTION_3_COUNT; face++) {
                        if (type_data.texcoords[face][0] < 0) {
                            type_data.texcoords[face][0] = other[0].get<int>();
                            type_data.texcoords[face][1] = other[1].get<int>();
                            other_used = true;
                        }
                    }
                    WARN_IF(!other_used, "An 'else' texcoord was specified for block '" + name + "', but isn't needed");
                }
            }
            // Single coordinate set for all faces
            else {
                CHECK_COORDS(texture_coords);

                for (int face = 0; face < DIRECTION_3_COUNT; face++) {
                    type_data.texcoords[face][0] = texture_coords[0].get<int>();
                    type_data.texcoords[face][1] = texture_coords[1].get<int>();
                }
            }
        }
        // Use the void texture (or transparent if block is intangible) if no texcoords are specified
        else {
            for (int face = 0; face < DIRECTION_3_COUNT; face++) {
                type_data.texcoords[(int)face][0] = TEXTURE_MAP_WIDTH - (type_data.tangible ? 1.0 : 2.0);
                type_data.texcoords[(int)face][1] = TEXTURE_MAP_HEIGHT - 1;
            }
        }
    }
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
