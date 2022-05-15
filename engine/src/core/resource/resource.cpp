#include "resource.h"

#include "common/utils.h"
#include "engine/src/node/node.h"
#include "engine/src/node/scene_tree.h"
#include "engine/src/engine.h"
#include "engine/src/core/signal.h"

Resource::Resource() {
    Engine::getSingleton()->resourceCreated(this);
}

Resource::~Resource() {
    SIGNAL_DELETED.emit();
    Engine::getSingleton()->resourceDeleted(this);
}