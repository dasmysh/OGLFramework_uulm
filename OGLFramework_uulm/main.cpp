/**
 * @file    main.cpp
 * @author  Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date    22.06.2012
 *
 * @brief   Implements the applications entry point for windows.
 */

#include "main.h"
#include "app/FWApplication.h"
#include "app/Configuration.h"

#include <fstream>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <glad/glad.h>

/**
 * @brief Window main.
 *
 * @author  Sebastian Maisch
 * @date    22.06.2012
 *
 * @param   hInstance       The instance of the process.
 * @param   hPrevInstance   The previous instance.
 * @param   lpCmdLine       The command line.
 * @param   nCmdShow        Controls how the window is to be shown.
 *
 * @return The applications return code.
 */
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    g2LogWorker g2log("application", "./", LOG_USE_TIMESTAMPS);
    g2::initializeLogging(&g2log);

    LOG(INFO) << L"Log created.";

    LOG(DEBUG) << L"Trying to load configuration.";
    cgu::Configuration config;
    std::ifstream configFile(configFileName, std::ios::in);
    if (configFile.is_open()) {
        boost::archive::xml_iarchive ia(configFile);
        ia >> BOOST_SERIALIZATION_NVP(config);

        // always directly write configuration to update version.
        std::ofstream ofs(configFileName, std::ios::out);
        boost::archive::xml_oarchive oa(ofs);
        oa << BOOST_SERIALIZATION_NVP(config);
    } else {
        LOG(DEBUG) << L"Configuration file not found. Using standard config.";
    }

    LOG(DEBUG) << L"Starting window initialization.";
    cguFrameworkApp::FWApplication app("OGLFramework Dummy Application", config);

    LOG(DEBUG) << L"Starting main loop.";
    app.StartRun();
    auto done = false;
    while (app.IsRunning() && !done) {
        try {
            app.Step();
        } catch (std::runtime_error e) {
            LOG(ERROR) << L"Could not process frame step: " << e.what() << std::endl << L"Exiting.";
            done = true;
        }
    }
    app.EndRun();
    LOG(DEBUG) << L"Main loop ended.";

    LOG(DEBUG) << L"Exiting application. Saving configuration to file.";
    std::ofstream ofs(configFileName, std::ios::out);
    boost::archive::xml_oarchive oa(ofs);
    oa << BOOST_SERIALIZATION_NVP(config);

    return 0;
}
