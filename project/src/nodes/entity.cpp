#include "entity.h"

#include "world.h"
#include "chunk.h"

void Entity::process(float delta) {
    super::process(delta);

    Chunk* chunk = World::get()->getChunk(getPosition().x, getPosition().z, false);

    if (chunk != entered_chunk) {
        if (entered_chunk != NULL)
            entered_chunk->onEntityExited(this);
        
        entered_chunk = chunk;

        if (entered_chunk != NULL)
            entered_chunk->onEntityEntered(this);
    }

}

Chunk* Entity::getEnteredChunk() {
    return entered_chunk;
}
