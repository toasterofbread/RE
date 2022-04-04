#include "timer.h"

void Timer::addedToNode(Node* parent) {
    Node::addedToNode(parent);

    assert(duration >= 0.0f);

    if (duration == 0.0f) {
        timeout();
        running = false;
    }
}

void Timer::process(float delta) {
    if (!running && duration > 0.0f) {
        return;
    }

    current_duration += delta;
    if (current_duration >= duration) {
        timeout();
        current_duration = 0.0f;

        if (mode == MODE::ONE_SHOT) {
            running = false;
        }
    }
}

void Timer::timeout() {
    SIGNAL_TIMEOUT.emit();
}

void Timer::start(float start_duration) {
    if (start_duration >= 0.0f) {
        duration = start_duration;
    }
    
    if (duration == 0.0f) {
        timeout();
        running = false;
        return;
    }

    running = true;
    current_duration = 0.0f;
}