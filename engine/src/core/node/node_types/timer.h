#include "engine/src/core/node/node.h"
#include "engine/src/core/signal.h"

class Timer: public Node {

    public:

        REGISTER_NODE(Timer);
        Signal<> SIGNAL_TIMEOUT;

        enum class MODE { ONE_SHOT, LOOP };
        MODE mode = MODE::ONE_SHOT;

        void process(float delta);

        void addedToNode(Node* parent);

        float duration = 0.0f; // If duration == 0.0, timeout instantly (doesn't loop even if mode == LOOP).
        bool running = false;

        void start(float start_duration = -1.0f);

    private:

        float current_duration = 0.0f;
        void timeout();

};