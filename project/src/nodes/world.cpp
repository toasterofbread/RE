#include "world.h"

#include "player.h"

void World::ready() {
    super::ready();

    player = new Player;
    addChild(player);
    player->setPosition(Vector3(0.0f, 1.5f, 0.0f));

    // Create initial chunks
    chunks = new Chunk;
    Chunk* chunk = NULL;

    for (int x = 0; x <= RENDER_DISTANCE * 2; x++) {
        for (int y = 0; y <= RENDER_DISTANCE * 2; y++) {
            if (chunk == NULL) {
                chunk = chunks;
            }
            else {
                Chunk* new_chunk = new Chunk;
                new_chunk->previous = chunk;
                chunk->next = new_chunk;
                chunk = new_chunk;
            }

            chunk->setup(Vector2(x - RENDER_DISTANCE, y - RENDER_DISTANCE));
            addChild(chunk);
        }
    }
}

void World::loadChunk(Vector2 grid_position) {

}

void World::unloadChunk(Vector2 grid_position) {

}
