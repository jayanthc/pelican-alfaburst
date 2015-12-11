#include "BinMap.h"
#include <stdlib.h>
#include <iostream>


namespace pelican {
namespace ampp {

QMap< unsigned int, QMap< double, QMap<double, unsigned int> > > BinMap::_unique;
unsigned int BinMap::_uniqueCount = 0;

/**
 *@details BinMap 
 */
BinMap::BinMap()
       : _width(1.0)
{
}

BinMap::BinMap(unsigned int numberOfBins)
        : _nBins(numberOfBins),_start(0.0),_width(1.0),_hash(0)
{
}

/**
 *@details
 */
BinMap::~BinMap()
{
}

void BinMap::reset(unsigned int numberOfBins)
{
     _nBins = numberOfBins; _start=0.0; _width=1.0, _hash=0;
}

unsigned int BinMap::hash() const
{
    if (_hash == 0) {
        if( ! _unique[_nBins][_start].contains(_width) ) {
            _unique[_nBins][_start][_width] = ++_uniqueCount;
        }
        _hash = _unique[_nBins][_start][_width];
    }
    return _hash;
}

// 0 - (_nBins-1) if value is in range
int BinMap::binIndex(double value) const
{
    return (int)(0.5 + ((value - _start)/ _width ));
}

void BinMap::setStart(double start)
{
    _hash = 0;
    _start = start;
}

void BinMap::setRef(double ref)
{
    _hash = 0;
    _ref = ref;
}

void BinMap::setBinWidth(double width)
{
    _hash = 0;
    _width = width;
    _halfwidth = width/2.0;
}
void BinMap::setBinWidthFromEndFreq(double end)
{
  setBinWidth( ( end - _start )/(_nBins-1) );
}
double BinMap::binStart(unsigned int index) const
{
   return binAssignmentNumber(index) - _halfwidth;
}

double BinMap::binEnd(unsigned int index) const
{
   return binAssignmentNumber(index) +_halfwidth;
}

//double BinMap::binAssignmentNumber(int index) const
//{
//    return _start + ( _width * index );
//}

//bool BinMap::equals(const BinMap& map) const
//{
//    return (_start == map._start) && (_width == map._width);
//}

bool operator==(const BinMap& m1, const BinMap& m2)
{
    return (m1._start == m2._start) && (m1._width == m2._width);
    //return m1.equals(m2);
}

bool BinMap::operator<(const BinMap& map) const
{
     return _start < map._start || _nBins < map._nBins || _width < map._width;
}

/**
 * @details
 * Provides a hash value for the BinMap object for use with QHash.
 */
unsigned int qHash(const BinMap& key)
{
    return key.hash();
}


} // namespace ampp
} // namespace pelican
