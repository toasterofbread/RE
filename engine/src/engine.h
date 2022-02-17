#define PONDER_USES_RUNTIME_IMPL

// Forward declarations
class NodeManager;

class Engine {
    private:
        NodeManager* manager;
    public:
        Engine(NodeManager* node_manager);
        void process(float delta);
};