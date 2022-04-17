#include "resource.h"

#include "common/utils.h"
#include "engine/src/core/node/node.h"
#include "engine/src/core/node/scene_tree.h"
#include "engine/src/engine.h"
#include "engine/src/core/signal.h"

Resource::ResourcePool* Resource::resource_pool = NULL;

Resource::Resource() {
    Engine::getSingleton()->resourceCreated(this);
    assert(registered());
}

Resource::~Resource() {
    SIGNAL_DELETED.emit();
}