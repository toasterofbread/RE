#include "engine/src/core/node/node.h"

// Forward declarations
class Engine;

class SceneNode: public Node {
    protected:
        virtual void init_scenenode(string scene_path, string scene_type);
        SceneNode(Engine* engine_singleton): Node(engine_singleton) {};
    public:
        virtual string getTypeName() {return "SceneNode";}
};