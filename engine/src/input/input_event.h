#ifndef INCLUDED_INPUT_EVENT
#define INCLUDED_INPUT_EVENT

#include <vector>
using namespace std;

// Forward declarations
class Macro;
template<typename A, typename... B>
class Signal;
class NodeManager;

class InputEvent {
    public:
        InputEvent(NodeManager* node_manager) { manager = node_manager; init(); }
        InputEvent(NodeManager* node_manager, vector<Macro*> input_macros) { manager = node_manager; macros = input_macros; init(); }

        bool isTriggered();
        bool isJustTriggered();
        bool isJustUntriggered();

        virtual void process(float delta);
    
        Signal<void>* SIGNAL_TRIGGERED;
        Signal<void>* SIGNAL_UNTRIGGERED;
    private:
        void init();
        vector<Macro*> macros;


        NodeManager* manager;
};

#endif
