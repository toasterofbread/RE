#ifndef INCLUDED_SCENE_NODE
#define INCLUDED_SCENE_NODE

#include "engine/src/node/node.h"

// Forward declarations
class Engine;

// TODO Remove or rewrite
class SceneNode: public Node {
    public:
        REGISTER_NODE(SceneNode, Node, {});
    protected:
        virtual void init_scenenode(string scene_path, string scene_type);
};

#endif