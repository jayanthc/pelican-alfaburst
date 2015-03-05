#ifndef ABDATAADAPTER_H
#define ABDATAADAPTER_H

#include "pelican/core/AbstractStreamAdapter.h"
#include "timer.h"

namespace pelican {
namespace ampp {

/*
 * Adapter to convert chunks of signal stream data into a ABData data-blob.
 */
class ABDataAdapter : public AbstractStreamAdapter
{
    public:
        // Constructs the adapter.
        ABDataAdapter(const ConfigNode& config);

        // Method to deserialise chunks of memory provided by the I/O device.
        void deserialise(QIODevice* device);

        static TimerData _adapterTime;

    private:
        static const unsigned _headerSize = 8;
        static const unsigned _footerSize = 8;
        unsigned _pktsPerSpec;
        unsigned _channelsPerPacket;
        unsigned _samplesPerPacket;
        unsigned _packetSize;
        unsigned int _nPolarisations;
        unsigned int _nSubbands;
        unsigned int _nChannels;
        unsigned int _first;
        unsigned int _numMissInst;
        unsigned int _numMissPkts;
        signed int _prevSpecQuart;
        unsigned long int _prevIntegCount;
        float _tStart;
        float _tSamp;
        unsigned long int _integCountStart;
        double _lastTimestamp;
        unsigned int _timestampFirst;
        unsigned int _x;
};

PELICAN_DECLARE_ADAPTER(ABDataAdapter)

} // namespace ampp
} // namespace pelican

#endif // ABDATAADAPTER_H

