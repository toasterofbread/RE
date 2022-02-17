#include "state.h"

#include "project/src/nodes/samus/samus.h"

#include "engine/src/utils.h"
#include "engine/src/core/node/node_manager.h"

SamusState::SamusState(Samus* samus_node) {
    samus = samus_node;
    manager = samus->getManager();
}

void SamusState::process(float delta) {
}