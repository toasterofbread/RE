#include "resource.h"

#include "common/utils.h"
#include "engine/src/core/node/node.h"
#include "engine/src/core/node/scene_tree.h"
#include "engine/src/engine.h"
#include "engine/src/core/signal.h"

Resource::Resource() {
    Engine::getSingleton()->resourceCreated(this);
}

Resource::~Resource() {
    SIGNAL_DELETED.emit();
}