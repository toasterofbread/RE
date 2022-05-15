#ifndef INCLUDED_INPUT_EVENT
#define INCLUDED_INPUT_EVENT

#include "engine/src/core/signal.h"
#include "core/resource/resource.h"

#include <vector>
using namespace std;

// Forward declarations
class Macro;

class InputEvent: public Resource {
    public:
        Signal<> SIGNAL_TRIGGERED;
        Signal<> SIGNAL_UNTRIGGERED;

        InputEvent(): Resource() {}
        InputEvent(vector<Macro*> input_macros): Resource() { macros = input_macros; }

        enum class HOLD_TYPE {
            NONE, REPEAT, DELAY
        };
        HOLD_TYPE hold_type = HOLD_TYPE::NONE;
        float hold_interval = -1.0f;

        bool isTriggered();
        bool isJustTriggered();
        bool isJustUntriggered();

        virtual void process(float delta);
        vector<Macro*> getMacros() { return macros; }

    private:
        vector<Macro*> macros;
        bool was_triggered = false;
        float hold_time = 0.0;
        int hold_status = 0;

        bool IisTriggered();
        bool IisJustTriggered();
        bool IisJustUntriggered();


};

#endif
