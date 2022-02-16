#include "scene_node.h"

#include <yaml-cpp/yaml.h>
#include <map>

#include "engine/src/utils.h"
#include "engine/src/core/node/node_manager.h"

void SceneNode::init_scenenode(string scene_path, string scene_type) {
    getManager()->loadScene(scene_path, this);
}