#include "test/AdapterTimeStreamTest.h"

#include "AdapterTimeStream.h"
#include "pelican/utility/ConfigNode.h"
#include "TimeStreamData.h"
#include "LofarUdpHeader.h"
#include "LofarTypes.h"

#include <QBuffer>
#include <vector>
#include <complex>
#include <cstdlib>

#include <iostream>

#include "pelican/utility/memCheck.h"

namespace pelican {
namespace lofar {

CPPUNIT_TEST_SUITE_REGISTRATION(AdapterTimeStreamTest);


void AdapterTimeStreamTest::setUp()
{
}

void AdapterTimeStreamTest::tearDown()
{
}

/**
 * @details
 * Method to test the adapter configuration.
 */
void AdapterTimeStreamTest::test_configuration()
{
    // Create configuration node.
    unsigned nPackets = 1;
    unsigned nSubbands = 1;
    unsigned nPolarisations = 1;
    unsigned nSamples = 10;
    unsigned sampleBits = 8;
    QString xml = ""
            "<AdapterTimeStream name=\"test\">"
            "	<packetsPerChunk number=\"" + QString::number(nPackets) + "\"/>"
            "   <subbands number=\"" + QString::number(nSubbands) + "\"/>"
            "	<polarisations number=\"" + QString::number(nPolarisations) + "\"/>"
            "	<samplesPerPacket number=\"" + QString::number(nSamples) + "\"/>"
            "	<sampleSize bits=\"" + QString::number(sampleBits) + "\"/>"
            "</AdapterTimeStream>";
    ConfigNode configNode(xml);

    // Construct the adapter.
    AdapterTimeStream adapter(configNode);

    // Check configuration.
    CPPUNIT_ASSERT_EQUAL(nPackets, adapter._nUDPPackets);
    CPPUNIT_ASSERT_EQUAL(nSubbands, adapter._nSubbands);
    CPPUNIT_ASSERT_EQUAL(nPolarisations, adapter._nPolarisations);
    CPPUNIT_ASSERT_EQUAL(nSamples, adapter._nSamples);
    CPPUNIT_ASSERT_EQUAL(sampleBits, adapter._sampleBits);
}


/**
 * @details
 * Method to test the _checkData() method of the adapter.
 */
void AdapterTimeStreamTest::test_checkDataFixedPacket()
{
    // Create configuration node.
    unsigned nPackets = 2;
    unsigned nSubbands = 4;
    unsigned nPolarisations = 2;
    unsigned nSamples = 10;
    unsigned sampleBits = 8;
    QString fixedPackets = "true";
    QString xml = ""
            "<AdapterTimeStream name=\"test\">"
            "	<packetsPerChunk number=\"" + QString::number(nPackets) + "\"/>"
            "   <subbands number=\"" + QString::number(nSubbands) + "\"/>"
            "	<polarisations number=\"" + QString::number(nPolarisations) + "\"/>"
            "	<samplesPerPacket number=\"" + QString::number(nSamples) + "\"/>"
            "	<sampleSize bits=\"" + QString::number(sampleBits) + "\"/>"
            "   <fixedSizePackets value=\"" + fixedPackets + "\"/>"
            "</AdapterTimeStream>";
    ConfigNode configNode(xml);

    // Construct the adapter.
    AdapterTimeStream adapter(configNode);

    // Construct a data blob to adapt into.
    TimeStreamData data(nSubbands, nPolarisations, nSamples);

    // Set the data blob to be adapted, the input chuck size and associated
    // service data.
    unsigned packetSize = sizeof(UDPPacket);
    size_t chunkSize = packetSize * nPackets;
    adapter.config(&data, chunkSize, QHash<QString, DataBlob*>());

    // Check the adapter.config() method behaved as expected.
    CPPUNIT_ASSERT_EQUAL(chunkSize, adapter._chunkSize);
    CPPUNIT_ASSERT_EQUAL(0, adapter._serviceData.size());
    CPPUNIT_ASSERT_EQUAL(nSamples * nPolarisations * nSubbands,
            static_cast<TimeStreamData*>(adapter._data)->size());

    try {
       adapter._checkData();
    }
    catch (QString err) {
        CPPUNIT_FAIL(err.toStdString().data());
    }
}


/**
 * @details
 * Method to test the _checkData() method of the adapter.
 */
void AdapterTimeStreamTest::test_checkDataVariablePacket()
{
    // Create configuration node.
    unsigned nPackets = 2;
    unsigned nSubbands = 4;
    unsigned nPolarisations = 2;
    unsigned nSamples = 10;
    unsigned sampleBits = 8;
    QString fixedPackets = "false";
    QString xml = ""
            "<AdapterTimeStream name=\"test\">"
            "	<packetsPerChunk number=\"" + QString::number(nPackets) + "\"/>"
            "   <subbands number=\"" + QString::number(nSubbands) + "\"/>"
            "	<polarisations number=\"" + QString::number(nPolarisations) + "\"/>"
            "	<samplesPerPacket number=\"" + QString::number(nSamples) + "\"/>"
            "	<sampleSize bits=\"" + QString::number(sampleBits) + "\"/>"
            "   <fixedSizePackets value=\"" + fixedPackets + "\"/>"
            "</AdapterTimeStream>";
    ConfigNode configNode(xml);

    // Construct the adapter.
    AdapterTimeStream adapter(configNode);

    // Construct a data blob to adapt into.
    TimeStreamData data(nSubbands, nPolarisations, nSamples);

    // Set the data blob to be adapted, the input chuck size and associated
    // service data.
    size_t packetSize = sizeof(UDPPacket::Header) +
            (nSubbands * nPolarisations * nSamples * sampleBits * 2) / 8;
    size_t chunkSize = packetSize * nPackets;
    adapter.config(&data, chunkSize, QHash<QString, DataBlob*>());

    // Check the adapter.config() method behaved as expected.
    CPPUNIT_ASSERT_EQUAL(chunkSize, adapter._chunkSize);
    CPPUNIT_ASSERT_EQUAL(0, adapter._serviceData.size());
    CPPUNIT_ASSERT_EQUAL(nSamples * nPolarisations * nSubbands,
            static_cast<TimeStreamData*>(adapter._data)->size());

    try {
       adapter._checkData();
    }
    catch (QString err) {
        CPPUNIT_FAIL(err.toStdString().data());
    }
}


/**
 * @details
 * Method to test the deserialise() method of the adapter.
 */
void AdapterTimeStreamTest::test_deserialise()
{
    // Create configuration node.
    unsigned nPackets = 2;
    unsigned nSubbands = 2;
    unsigned nPolarisations = 2;
    unsigned nSamples = 10;
    unsigned sampleBits = 8; // (4 = 16 values, 8 = 256 values, 16 = 65536 values.)

    // Make very sure only a supported sample size is used.
    if (sampleBits != 4 && sampleBits != 8 && sampleBits != 16) {
        CPPUNIT_FAIL("BAADD TEST OPTION: Sample bits not suppored.");
    }

    typedef TYPES::i4complex iComplex8;
    typedef TYPES::i8complex iComplex16;
    typedef TYPES::i16complex iComplex32;

    QString xml = ""
            "<AdapterTimeStream name=\"test\">"
            "	<packetsPerChunk number=\"" + QString::number(nPackets) + "\"/>"
            "   <subbands number=\"" + QString::number(nSubbands) + "\"/>"
            "	<polarisations number=\"" + QString::number(nPolarisations) + "\"/>"
            "	<samplesPerPacket number=\"" + QString::number(nSamples) + "\"/>"
            "	<sampleSize bits=\"" + QString::number(sampleBits) + "\"/>"
            "</AdapterTimeStream>";
    ConfigNode configNode(xml);

    // Construct the adapter.
    AdapterTimeStream adapter(configNode);

    // Construct a data blob to adapt into.
    TimeStreamData data(nSubbands, nPolarisations, nSamples);

    unsigned packetSize = sizeof(UDPPacket);
    size_t chunkSize = packetSize * nPackets;

    // Configure the adapter setting the data blob, chunk size and service data.
    adapter.config(&data, chunkSize, QHash<QString, DataBlob*>());

    // Create and fill a UDP packet.
    std::vector<UDPPacket> packets(nPackets);
    for (unsigned i = 0; i < nPackets; ++i) {

        // Fill in the header
        packets[i].header.version             = uint8_t(0 + i);
        packets[i].header.sourceInfo          = uint8_t(1 + i);
        packets[i].header.configuration       = uint16_t(sampleBits);
        packets[i].header.station             = uint16_t(3 + i);
        packets[i].header.nrBeamlets          = uint8_t(4 + i);
        packets[i].header.nrBlocks            = uint8_t(5 + i);
        packets[i].header.timestamp           = 6 + i;
        packets[i].header.blockSequenceNumber = 7 + i;

        // Fill in the data
        for (unsigned ii = 0, t = 0; t < nSamples; ++t) {
            for (unsigned c = 0; c < nSubbands; ++c) {
                for (unsigned p = 0; p < nPolarisations; ++p) {

                    if (sampleBits == 4) {
                        iComplex8* values = reinterpret_cast<iComplex8*>(packets[i].data);
                        int index = nPolarisations * (t * nSubbands + c) + p;
                        values[index] = iComplex8(double(ii), double(i));
                        ++ii;
                    }
                    else if (sampleBits == 8) {
                        iComplex16* values = reinterpret_cast<iComplex16*>(packets[i].data);
                        int index = nPolarisations * (t * nSubbands + c) + p;
                        values[index] = iComplex16(ii, i);
                        ++ii;
                    }
                    else if (sampleBits == 16) {
                        iComplex32* values = reinterpret_cast<iComplex32*>(packets[i].data);
                        int index = nPolarisations * (t * nSubbands + c) + p;
                        values[index] = iComplex32(ii, i);
                        ++ii;
                    }

                }
            }
        }
    }


    // Stick the packet into an QIODevice.
    QBuffer buffer;
    buffer.setData(reinterpret_cast<char*>(&packets[0]), chunkSize);
    buffer.open(QBuffer::ReadOnly);
    //std::cout << "buffer size = " << buffer.size() << std::endl;

    try {
        adapter.deserialise(&buffer);
    }
    catch (QString err) {
        CPPUNIT_FAIL(err.toStdString().data());
    }
}


} // namespace lofar
} // namespace pelican
