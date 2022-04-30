#include "world.h"

#include "player.h"
#include "common/draw.h"
#include "physics/physics_server.h"

#include "raylib/raylib.h"

void World::init() {
    player = new Player;
    player->world = this;

    addChild(player);
    player->setPosition(Vector3(CHUNK_AMOUNT * 0.5f * CHUNK_SIZE, SUBCHUNK_COUNT * SUBCHUNK_HEIGHT, CHUNK_AMOUNT * 0.5f * CHUNK_SIZE));

    material = LoadMaterialDefault();
    material.shader = LoadShader(0, 0);
    material.maps[MATERIAL_MAP_DIFFUSE].color = Colour(0.8, 0.8, 0.8);
    material.maps[MATERIAL_MAP_DIFFUSE].texture = OS::getRaylibTexture(OS::loadTexture("project/resources/terrain.png"));

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

void World::addSubChunk(SubChunk* chunk) {

}

void World::loadChunk(Vector2 grid_position) {

}

void World::unloadChunk(Vector2 grid_position) {

}
