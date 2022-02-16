#include "engine/src/core/node/node.h"

class SceneNode: public Node {
    protected:
        virtual void init_scenenode(string scene_path, string scene_type);
        SceneNode(NodeManager* node_manager): Node(node_manager) {};
    public:
        virtual string getTypeName() {return "SceneNode";}
};