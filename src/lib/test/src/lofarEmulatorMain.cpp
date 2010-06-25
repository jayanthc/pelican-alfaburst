#include <QCoreApplication>

#include "pelican/emulator/EmulatorDriver.h"
#include "LofarUdpEmulator.h"

using namespace pelican;
using namespace pelican::lofar;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    int samplesPerPacket  = 32;   // Number of block per frame (for a 32 MHz beam)
    int nrPolarisations   = 2;    // Number of polarization in the data
    int numPackets        = 10000;   // Number of packet to send
    int clock             = 200;  // Rounded up clock station clock speed
    int subbandsPerPacket = (clock == 200) ? 42 : 54;  //  Number of block per frame

    // Set up LOFAR data emulator configuration.
    ConfigNode emulatorConfig(
            "<LofarUdpEmulator>"
            "    <connection host=\"127.0.0.1\" port=\"8090\"/>"
            "    <params clock=\""         + QString::number(clock)             + "\"/>"
            "    <packet interval=\"800\""
            "            startDelay=\"1\""
            "            sampleSize=\"8\""
            "            samples=\""       + QString::number(samplesPerPacket)  + "\""
            "            polarisations=\"" + QString::number(nrPolarisations)   + "\""
            "            subbands=\""      + QString::number(subbandsPerPacket) + "\"/>"
//            "            nPackets=\""      + QString::number(numPackets + 10)   + "\"/>"
            "</LofarUdpEmulator>");

    EmulatorDriver emulator(new LofarUdpEmulator(emulatorConfig));

    return app.exec();
}