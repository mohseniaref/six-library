/* =========================================================================
 * This file is part of scene-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
 *
 * scene-c++ is free software; you can redistribute it and/or modify
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
#include "scene/CoordinateTransform.h"

scene::CoordinateTransform::CoordinateTransform()
{
    model = new WGS84EllipsoidModel();
}

scene::CoordinateTransform::CoordinateTransform(
        const scene::EllipsoidModel *initVals)
{
    model = new EllipsoidModel();

    if(initVals != NULL)
    {
        setEllipsoidModel(*initVals);
    }
}

scene::CoordinateTransform::~CoordinateTransform()
{
    if(model != NULL)
    {
        delete model;
    }
}

scene::CoordinateTransform* scene::CoordinateTransform::clone() const
{
  scene::CoordinateTransform *newTransform = 
          new CoordinateTransform(this->getEllipsoidModel());
  return newTransform;
}


void scene::CoordinateTransform::setEllipsoidModel(
        const scene::EllipsoidModel & initVals)
{
    *model = initVals;
}

scene::EllipsoidModel* scene::CoordinateTransform::getEllipsoidModel()
{
    return model;
}

const scene::EllipsoidModel* 
scene::CoordinateTransform::getEllipsoidModel() const
{
    return model;
}


