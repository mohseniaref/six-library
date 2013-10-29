/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <memory>
#include <sstream>

#include <sys/Conf.h>
#include <except/Exception.h>
#include <str/Convert.h>
#include <six/NITFImageInfo.h>
#include <scene/Utilities.h>

using namespace six;

//!  File security classification system
const char NITFImageInfo::CLSY[] = "CLSY";
//!  File security codewords
const char NITFImageInfo::CODE[] = "CODE";
//!  File control and handling
const char NITFImageInfo::CTLH[] = "CTLH";
//!  File releasing instructions
const char NITFImageInfo::REL[] = "REL";
//!  File security declassification type
const char NITFImageInfo::DCTP[] = "DCTP";
//!  File security declassification date
const char NITFImageInfo::DCDT[] = "DCDT";
//!  File security declassification exemption
const char NITFImageInfo::DCXM[] = "DCXM";
//!  File security downgrade
const char NITFImageInfo::DG[] = "DG";
//!  File security downgrade date
const char NITFImageInfo::DGDT[] = "DGDT";
//!  File security classification text
const char NITFImageInfo::CLTX[] = "CLTX";
//!  File security classification Authority type
const char NITFImageInfo::CATP[] = "CATP";
//!  File security classification Authority
const char NITFImageInfo::CAUT[] = "CAUT";
//!  File security reason
const char NITFImageInfo::CRSN[] = "CRSN";
//!  File security source date
const char NITFImageInfo::SRDT[] = "SRDT";
//!  File security control number
const char NITFImageInfo::CTLN[] = "CTLN";

void NITFImageInfo::computeImageInfo()
{
    size_t bytesPerRow = data->getNumBytesPerPixel()
            * data->getNumCols();

    // This, to be safe, should be a 64bit number
    sys::Uint64_T limit1 = (sys::Uint64_T) std::floor((double) maxProductSize
            / (double) bytesPerRow);

    if (limit1 == 0)
    {
        std::ostringstream ostr;
        ostr << "maxProductSize [" << maxProductSize << "] < bytesPerRow ["
             << bytesPerRow << "]";

        throw except::Exception(Ctxt(ostr.str()));
    }

    if (limit1 < numRowsLimit)
    {
        numRowsLimit = static_cast<size_t>(limit1);
    }
}

void NITFImageInfo::computeSegmentInfo()
{
    if (productSize <= maxProductSize)
    {
        imageSegments.resize(1);
        imageSegments[0].numRows = data->getNumRows();
        imageSegments[0].firstRow = 0;
        imageSegments[0].rowOffset = 0;
        imageSegments[0].corners = data->getImageCorners();
    }

    else
    {
        // NOTE: See header for why rowOffset is always set to numRowsLimit
        //       for image segments 1 and above
        size_t numIS = (size_t) std::ceil(data->getNumRows()
                / (double) numRowsLimit);
        imageSegments.resize(numIS);
        imageSegments[0].numRows = numRowsLimit;
        imageSegments[0].firstRow = 0;
        imageSegments[0].rowOffset = 0;
        size_t i;
        for (i = 1; i < numIS - 1; i++)
        {
            imageSegments[i].numRows = numRowsLimit;
            imageSegments[i].firstRow = i * numRowsLimit;
            imageSegments[i].rowOffset = numRowsLimit;
        }

        imageSegments[i].firstRow = i * numRowsLimit;
        imageSegments[i].rowOffset = numRowsLimit;
        imageSegments[i].numRows = data->getNumRows() - (numIS - 1)
                * numRowsLimit;

    }

    computeSegmentCorners();
}

void NITFImageInfo::computeSegmentCorners()
{
    const LatLonCorners corners = data->getImageCorners();

    // (0, 0)
    Vector3 icp1 = scene::Utilities::latLonToECEF(corners.upperLeft);
    // (0, N)
    Vector3 icp2 = scene::Utilities::latLonToECEF(corners.upperRight);
    // (M, N)
    Vector3 icp3 = scene::Utilities::latLonToECEF(corners.lowerRight);
    // (M, 0)
    Vector3 icp4 = scene::Utilities::latLonToECEF(corners.lowerLeft);

    size_t numIS = imageSegments.size();
    double total = data->getNumRows() - 1.0;

    Vector3 ecef;
    size_t i;
    for (i = 0; i < numIS; i++)
    {
        size_t firstRow = imageSegments[i].firstRow;
        double wgt1 = (total - firstRow) / total;
        double wgt2 = firstRow / total;

        // This requires an operator overload for scalar * vector
        ecef = wgt1 * icp1 + wgt2 * icp4;

        imageSegments[i].corners.upperLeft =
            scene::Utilities::ecefToLatLon(ecef);

        // Now do it for the first
        ecef = wgt1 * icp2 + wgt2 * icp3;

        imageSegments[i].corners.upperRight =
            scene::Utilities::ecefToLatLon(ecef);
    }

    for (i = 0; i < numIS - 1; i++)
    {
        LatLonCorners& theseCorners(imageSegments[i].corners);
        const LatLonCorners& nextCorners(imageSegments[i + 1].corners);

        theseCorners.lowerRight.setLat(nextCorners.upperRight.getLat());
        theseCorners.lowerRight.setLon(nextCorners.upperRight.getLon());

        theseCorners.lowerLeft.setLat(nextCorners.upperLeft.getLat());
        theseCorners.lowerLeft.setLon(nextCorners.upperLeft.getLon());
    }

    // This last one is cake
    imageSegments[i].corners.lowerRight = corners.lowerRight;
    imageSegments[i].corners.lowerLeft = corners.lowerLeft;

    // SHOULD WE JUST ASSUME THAT WHATEVER IS IN THE XML GeoData is what
    // we want?  For now, this makes sense
}

void NITFImageInfo::compute()
{

    computeImageInfo();
    computeSegmentInfo();
}

// Currently punts on LU
std::vector<nitf::BandInfo> NITFImageInfo::getBandInfo()
{
    std::vector < nitf::BandInfo > bands;

    switch (data->getPixelType())
    {
    case PixelType::RE32F_IM32F:
    case PixelType::RE16I_IM16I:
    {
        nitf::BandInfo band1;
        band1.getSubcategory().set("I");
        nitf::BandInfo band2;
        band2.getSubcategory().set("Q");

        bands.push_back(band1);
        bands.push_back(band2);
    }
        break;
    case PixelType::RGB24I:
    {
        nitf::BandInfo band1;
        band1.getRepresentation().set("R");

        nitf::BandInfo band2;
        band2.getRepresentation().set("G");

        nitf::BandInfo band3;
        band3.getRepresentation().set("B");

        bands.push_back(band1);
        bands.push_back(band2);
        bands.push_back(band3);
    }
        break;

    case PixelType::MONO8I:
    case PixelType::MONO16I:
    {
        nitf::BandInfo band1;
        band1.getRepresentation().set("M");
        bands.push_back(band1);
    }
        break;

    case PixelType::MONO8LU:
    {
        nitf::BandInfo band1;

        // TODO: Why do we need to byte swap here?  If it is required, could
        //       we avoid the clone and byte swap and instead index into
        //       the LUT in the opposite order?
        std::auto_ptr<LUT> lut(data->getDisplayLUT()->clone());
        sys::byteSwap((sys::byte*) lut->table.get(), lut->elementSize,
                      lut->numEntries);

        if (lut->elementSize != sizeof(short))
        {
            throw except::Exception(Ctxt(
                "Unexpected element size: " +
                str::toString(lut->elementSize)));
        }

        nitf::LookupTable lookupTable(lut->elementSize, lut->numEntries);
        unsigned char* const table(lookupTable.getTable());

        for (unsigned int i = 0; i < lut->numEntries; ++i)
        {
            // Need two LUTS in the nitf, with high order
            // bits in the first and low order in the second
            const unsigned char* const entry = (*lut)[i];
            table[i] = entry[0];
            table[lut->numEntries + i] = entry[1];
        }

        //         //I would like to set it this way but it does not seem to work.
        //         //Using the init function instead.
        //         //band1.getRepresentation().set("LU");
        //         //band1.getLookupTable().setTable(table, 2, lut->numEntries);

        band1.init("LU", "", "", "", lut->elementSize, lut->numEntries,
                   lookupTable);
        bands.push_back(band1);
    }
        break;

    case PixelType::RGB8LU:
    {
        nitf::BandInfo band1;

        LUT* lut = data->getDisplayLUT();

        if (lut->elementSize != 3)
        {
            throw except::Exception(Ctxt(
                "Unexpected element size: " +
                str::toString(lut->elementSize)));
        }

        nitf::LookupTable lookupTable(lut->elementSize, lut->numEntries);
        unsigned char* const table(lookupTable.getTable());

        for (unsigned int i = 0, k = 0; i < lut->numEntries; ++i)
        {
            for (unsigned int j = 0; j < lut->elementSize; ++j, ++k) //elementSize=3
            {
                // Need to transpose the lookup table entries
                table[j * lut->numEntries + i] = lut->table[k];
            }
        }

        //I would like to set it this way but it does not seem to work.
        //Using the init function instead.
        //band1.getRepresentation().set("LU");
        //band1.getLookupTable().setTable(table, 3, lut->numEntries);

        band1.init("LU", "", "", "", lut->elementSize, lut->numEntries,
                   lookupTable);
        bands.push_back(band1);
    }
        break;

    default:
        throw except::Exception(Ctxt("Unknown pixel type"));
    }

    for (size_t i = 0; i < bands.size(); ++i)
    {
        bands[i].getImageFilterCondition().set("N");
    }
    return bands;
}

std::string NITFImageInfo::generateFieldKey(const std::string& field,
        std::string prefix, int index)
{
    std::ostringstream s;
    s << prefix << field;
    if (index >= 0)
        s << "[" << str::toString(index) << "]";
    return s.str();
}
