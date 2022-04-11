#include "scene_node.h"

#include <yaml-cpp/yaml.h>
#include <map>

#include "common/utils.h"
#include "engine/src/core/node/scene_loader.h"

void SceneNode::init_scenenode(string scene_path, string scene_type) {
    SceneLoader::loadSceneFromFile(scene_path, this);
}