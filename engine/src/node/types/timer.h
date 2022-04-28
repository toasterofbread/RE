#include "engine/src/node/node.h"
#include "engine/src/core/signal.h"

class Timer: public Node {

    public:

        REGISTER_NODE(Timer, Node, {
            c->template registerProperty<MODE>("mode", &NodeType::setMode);
        });
        
        Signal<> SIGNAL_TIMEOUT;

        enum class MODE { ONE_SHOT, LOOP };

        void process(float delta);

        float duration = 0.0f; // If duration == 0.0, timeout instantly (doesn't loop even if mode == LOOP).
        bool running = false;

        void start(float start_duration = -1.0f);

        void setMode(MODE _mode) { mode = _mode; }
        MODE getMode() { return mode; }

    private:

        float current_duration = 0.0f;
        void timeout();
        MODE mode = MODE::ONE_SHOT;

};