#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <QCoreApplication>
#include <iostream>

int main(int /*argc*/, char** /*argv*/)
{

    int argc = 1;
    char *argv[] = {(char*)"pelican-lofar"};
    QCoreApplication* app = new QCoreApplication(argc, argv);

    // Get the top level suite from the registry
    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    // Adds the test to the list of test to run
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( suite );

    // Change the default outputter to a compiler error format outputter
    runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(),
                std::cerr));
    // Run the tests.
    bool wasSucessful = runner.run();

    // Return error code 1 if the one of test failed.

    delete app;
    return wasSucessful ? 0 : 1;
}
