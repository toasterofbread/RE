#ifndef INCLUDED_SCENE_NODE
#define INCLUDED_SCENE_NODE

#include "engine/src/core/node/node.h"

// Forward declarations
class Engine;

// !todo Remove or rewrite
class SceneNode: public Node {
    protected:
        virtual void init_scenenode(string scene_path, string scene_type);
        SceneNode(): Node() {};
    public:
        static string getTypeName() {return "SceneNode";}
};

#endif