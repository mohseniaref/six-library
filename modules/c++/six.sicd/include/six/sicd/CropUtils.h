/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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

#ifndef __SIX_SICD_CROP_UTILS_H__
#define __SIX_SICD_CROP_UTILS_H__

#include <string>
#include <vector>

#include <types/RowCol.h>

namespace six
{
namespace sicd
{
/*
 * Reads in an AOI from a SICD and creates a cropped SICD, updating the
 * metadata as appropriate to reflect this
 *
 * \param inPathname Input SICD pathname
 * \param schemaPaths Schema paths to use for reading and writing
 * \param aoiOffset Upper left corner of AOI
 * \param aoiDims Size of AOI
 * \param outPathname Output cropped SICD pathname
 */
void cropSICD(const std::string& inPathname,
              const std::vector<std::string>& schemaPaths,
              const types::RowCol<size_t>& aoiOffset,
              const types::RowCol<size_t>& aoiDims,
              const std::string& outPathname);
}
}

#endif
