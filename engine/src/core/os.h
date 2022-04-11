namespace OS {

    // Called once on startup, before the main loop begins
    void initialiseApplication();

    // Closes the application
    void closeApplication();

    // Returns time since application start in seconds
    double time();

}