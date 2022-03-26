#include "engine/src/core/signal.h"

#include "engine/src/engine.h"
#include "engine/src/core/node/scene_tree.h"
 
//!todo make this less awful
template<typename CallbackReturnType, typename... CallbackArgs>
void Signal<CallbackReturnType, CallbackArgs...>::await() {
    Engine::getSingleton()->ensureAsync();
    double start_time = GetTime();
    while (start_time >= last_emission_time) {
        // Pass
    };
}

// template<typename CallbackReturnType, typename... CallbackArgs>
// bool Signal<CallbackReturnType, CallbackArgs...>::connected() {
    // return callbacks.size() > 0 || member_callbacks.size() > 0;
// }