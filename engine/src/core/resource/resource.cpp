#include "resource.h"

#include "engine/src/utils.h"
#include "engine/src/core/node/node.h"
#include "engine/src/core/node/node_manager.h"
#include "engine/src/engine.h"
#include "engine/src/core/signal.h"

Resource::Resource(Engine* engine_singleton) {
    engine = engine_singleton;
    engine->resourceCreated(this);
}