#define CATCH_CONFIG_MAIN
#include <include/catch.hpp>
#include "common/utils.h"

#include "engine/src/raylib_include.h"
#include "engine/src/core/signal.h"

TEST_CASE( "Signal" ) {
    
    #define SignalType Signal<int&, int>

    SignalType SIGNAL;
    REQUIRE(SIGNAL.getConnectionCount() == 0);

    struct TestClass {
        void signalCallback(int& emit_reference, int emit_value, int& connection_reference, int connection_value) {
            emit_reference = emit_value;
            connection_reference = connection_value;
        }

        void signalCallbackNoArgs(int& reference, int value) {
            reference = value;
        }

    };

    int connection_value = 0;
    int& connection_reference = connection_value;

    TestClass object;

    // Connect signal to object method
    SIGNAL.connect<TestClass, int&, int>(&TestClass::signalCallback, &object, connection_reference, 2);
    REQUIRE(SIGNAL.getConnectionCount() == 1);

    // Test signal emission with arguments and binds
    int emit_value = 0;
    int& emit_reference = emit_value;
    SIGNAL.emit(emit_reference, 1);
    REQUIRE(emit_reference == 1);
    REQUIRE(connection_reference == 2);

    // Disconnect signal
    SIGNAL.disconnect(&object);
    REQUIRE(SIGNAL.getConnectionCount() == 0);
    
    // Connect signal to object method (without arguments)
    int value = 0;
    int& reference = value;
    SIGNAL.connectWithoutArgs<TestClass, int&, int>(&TestClass::signalCallbackNoArgs, &object, reference, 3);
    REQUIRE(SIGNAL.getConnectionCount() == 1);

    // Test signal emission with binds, without arguments
    SIGNAL.emit(reference, 4);
    REQUIRE(reference == 3);

    // Disconnect signal
    SIGNAL.disconnect(&object);
    REQUIRE(SIGNAL.getConnectionCount() == 0);

}