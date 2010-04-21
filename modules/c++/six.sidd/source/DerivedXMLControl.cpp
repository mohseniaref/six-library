/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#include "six/Types.h"
#include "six/Utilities.h"
#include <import/str.h>

#include "six/sidd/DerivedXMLControl.h"
#include "six/sidd/DerivedData.h"
#include "six/sidd/DerivedDataBuilder.h"

using namespace six;
using namespace six::sidd;

void DerivedXMLControl::xmlToProductCreation(
        xml::lite::Element* productCreationXML,
        ProductCreation* productCreation)
{
    xml::lite::Element* informationXML = getFirstAndOnly(productCreationXML,
            "ProcessorInformation");
    productCreation->processorInformation->application = getFirstAndOnly(
            informationXML, "Application")->getCharacterData();

    parseDateTime(getFirstAndOnly(informationXML, "ProcessingDateTime"),
            productCreation->processorInformation->processingDateTime);

    productCreation->processorInformation->site = getFirstAndOnly(
            informationXML, "Site")->getCharacterData();

    xml::lite::Element* profileXML = getOptional(informationXML, "Profile");
    if (profileXML)
    {
        productCreation->processorInformation->profile
                = profileXML->getCharacterData();
    }

    //Classification
    xml::lite::Element* classificationXML = getFirstAndOnly(productCreationXML,
            "Classification");
    productCreation->classification.level = getFirstAndOnly(classificationXML,
            "Level")->getCharacterData();

    std::vector<xml::lite::Element*> secMarkingsXML;
    classificationXML->getElementsByTagName("SecurityMarkings", secMarkingsXML);
    for (std::vector<xml::lite::Element*>::iterator it = secMarkingsXML.begin(); it
            != secMarkingsXML.end(); ++it)
    {
        productCreation->classification.securityMarkings. push_back(
                (*it)->getCharacterData());
    }

    xml::lite::Element* classGuidanceXML = getOptional(
            classificationXML, "Guidance");
    if (classGuidanceXML)
    {
        ClassificationGuidance* classGuidance = new ClassificationGuidance();
        productCreation->classification.guidance = classGuidance;

        parseDateTime(getFirstAndOnly(classGuidanceXML, "Date"),
                productCreation->classification.guidance->date);
        productCreation->classification.guidance->authority = getFirstAndOnly(
                classGuidanceXML, "Authority")->getCharacterData();
    }

    productCreation->productName = getFirstAndOnly(productCreationXML,
            "ProductName")->getCharacterData();
    productCreation->productClass = getFirstAndOnly(productCreationXML,
            "ProductClass")->getCharacterData();

    xml::lite::Element* productTypeXML = getOptional(productCreationXML,
            "ProductType");
    if (productTypeXML)
    {
        productCreation->productType = productTypeXML->getCharacterData();
    }

    try
    {
        parseParameters(productCreationXML, "ProductCreationExtension",
                productCreation->productCreationExtensions);
    }
    catch (except::Exception&)
    {
    }
}

void DerivedXMLControl::xmlToDisplay(xml::lite::Element* displayXML,
        Display* display)
{
    display->pixelType = str::toType<PixelType>(getFirstAndOnly(displayXML,
            "PixelType")->getCharacterData());

    //RemapInformation
    xml::lite::Element* remapInformationXML = getOptional(displayXML,
            "RemapInformation");
    if (remapInformationXML)
    {
        if (display->remapInformation->displayType == DISPLAY_COLOR)
        {

            xml::lite::Element* remapXML = getFirstAndOnly(
                    remapInformationXML, "ColorDisplayRemap");

            xml::lite::Element* remapLUTXML = getFirstAndOnly(remapXML,
                    "RemapLUT");

            //get size attribute
            int size = str::toType<int>(
                    remapLUTXML->getAttributes().getValue("size"));
            display->remapInformation->remapLUT = new LUT(size, 3);

            std::string
                    lutStr =
                            getFirstAndOnly(remapXML, "RemapLUT")->getCharacterData();
            std::vector<std::string> lutVals = str::split(lutStr, " ");

            int k = 0;
            for (unsigned int i = 0; i < lutVals.size(); i++)
            {
                std::vector<std::string> rgb = str::split(lutVals[i], ",");
                for (unsigned int j = 0; j < rgb.size(); j++)
                {
                    display->remapInformation->remapLUT->table[k++]
                            = rgb[j].at(0);
                }
            }
        }
        else if (display->remapInformation->displayType == DISPLAY_MONO)
        {
            xml::lite::Element* remapXML = getFirstAndOnly(
                    remapInformationXML, "MonochromeDisplayRemap");

            xml::lite::Element* remapLUTXML = getFirstAndOnly(remapXML,
                    "RemapLUT");

            //get size attribute
            int size = str::toType<int>(
                    remapLUTXML->getAttributes().getValue("size"));
            display->remapInformation->remapLUT = new LUT(size, 1);

            std::string
                    lutStr =
                            getFirstAndOnly(remapXML, "RemapLUT")->getCharacterData();
            std::vector<std::string> lutVals = str::split(lutStr, " ");
            for (unsigned int i = 0; i < lutVals.size(); i++)
            {
                display->remapInformation->remapLUT->table[i]
                        = lutVals[i].at(0);
            }

            ((MonochromeDisplayRemap*) display->remapInformation)->remapType
                    = getFirstAndOnly(remapXML, "RemapType")->getCharacterData();

            //RemapParameters
            try
            {
                parseParameters(
                        remapXML,
                        "RemapParameter",
                        ((MonochromeDisplayRemap*) display-> remapInformation)->remapParameters);
            }
            catch (except::Exception&)
            {
            }
        }
    }

    display->magnificationMethod
            = str::toType<MagnificationMethod>(getFirstAndOnly(displayXML,
                    "MagnificationMethod")->getCharacterData());
    display->decimationMethod = str::toType<DecimationMethod>(getFirstAndOnly(
            displayXML, "DecimationMethod")->getCharacterData());

    xml::lite::Element* histogramOverridesXML = getOptional(displayXML,
            "DRAHistogramOverrides");
    if (histogramOverridesXML)
    {
        display->histogramOverrides = new DRAHistogramOverrides();
        parseInt(getFirstAndOnly(histogramOverridesXML, "ClipMin"),
                display->histogramOverrides->clipMin);
        parseInt(getFirstAndOnly(histogramOverridesXML, "ClipMax"),
                display->histogramOverrides->clipMax);
    }

    //MonitorCompensationApplied
    xml::lite::Element* monitorCompensationXML = getOptional(
            displayXML, "MonitorCompensationApplied");
    if (monitorCompensationXML)
    {
        display->monitorCompensationApplied = new MonitorCompensationApplied();
        parseDouble(getFirstAndOnly(monitorCompensationXML, "Gamma"),
                display->monitorCompensationApplied->gamma);
        parseDouble(getFirstAndOnly(monitorCompensationXML, "XMin"),
                display->monitorCompensationApplied->xMin);
    }

    //DisplayExtension
    try
    {
        parseParameters(displayXML, "DisplayExtension",
                display->displayExtensions);
    }
    catch (except::Exception&)
    {
    }

}

void DerivedXMLControl::xmlToGeographicAndTarget(
        xml::lite::Element* geographicAndTargetXML,
        GeographicAndTarget* geographicAndTarget)
{
    xml::lite::Element* geographicCoverageXML = getFirstAndOnly(
            geographicAndTargetXML, "GeographicCoverage");

    xmlToGeographicCoverage(geographicCoverageXML,
            geographicAndTarget->geographicCoverage);

    //TargetInformation
    std::vector<xml::lite::Element*> targetInfosXML;
    geographicAndTargetXML->getElementsByTagName("TargetInformation",
            targetInfosXML);

    for (std::vector<xml::lite::Element*>::iterator it = targetInfosXML.begin(); it
            != targetInfosXML.end(); ++it)
    {
        TargetInformation* ti = new TargetInformation();
        geographicAndTarget->targetInformation.push_back(ti);

        parseParameters(*it, "Identifier", ti->identifiers);

        //Footprint
        std::vector<xml::lite::Element*> footprintsXML;
        (*it)->getElementsByTagName("Footprint", footprintsXML);
        for (std::vector<xml::lite::Element*>::iterator it2 =
                footprintsXML.begin(); it2 != footprintsXML.end(); ++it2)
        {
            std::vector<LatLon> fp;
            parseFootprint(*it2, "Vertex", fp, false);
            ti->footprints.push_back(fp);
        }

        //TargetInformationExtension
        try
        {
            parseParameters(*it, "TargetInformationExtension",
                    ti->targetInformationExtensions);
        }
        catch (except::Exception&)
        {
        }

    }

}

void DerivedXMLControl::xmlToGeographicCoverage(
        xml::lite::Element* geographicCoverageXML,
        GeographicCoverage* geographicCoverage)
{
    std::vector<xml::lite::Element*> georegionIdsXML;
    geographicCoverageXML->getElementsByTagName("GeoregionIdentifier",
            georegionIdsXML);
    for (std::vector<xml::lite::Element*>::iterator it =
            georegionIdsXML.begin(); it != georegionIdsXML.end(); ++it)
    {
        geographicCoverage->georegionIdentifiers.push_back(
                (*it)->getCharacterData());
    }

    xml::lite::Element* footprintXML = getFirstAndOnly(geographicCoverageXML,
                                                       "Footprint");

    //Footprint
    parseFootprint(footprintXML, "Vertex", geographicCoverage->footprint,
           false);

    //If there are subregions, recurse
    std::vector<xml::lite::Element*> subRegionsXML;
    geographicCoverageXML->getElementsByTagName("SubRegion", subRegionsXML);

    int i = 0;
    for (std::vector<xml::lite::Element*>::iterator it = subRegionsXML.begin(); it
            != subRegionsXML.end(); ++it)
    {
        geographicCoverage->subRegion.push_back(new GeographicCoverage(
                REGION_SUB_REGION));
        xmlToGeographicCoverage(*it, geographicCoverage->subRegion[i++]);
    }

    //Otherwise read the GeographicInfo
    if (subRegionsXML.size() == 0)
    {
        xml::lite::Element* geographicInfoXML = getFirstAndOnly(
                geographicCoverageXML, "GeographicInfo");

        geographicCoverage->geographicInformation = new GeographicInformation();

        std::vector<xml::lite::Element*> countryCodes;
        geographicInfoXML->getElementsByTagName("CountryCode", countryCodes);
        for (std::vector<xml::lite::Element*>::iterator it =
                countryCodes.begin(); it != countryCodes.end(); ++it)
        {
            xml::lite::Element* countryCode = *it;
            geographicCoverage->geographicInformation->countryCodes.push_back(
                    countryCode->getCharacterData());
        }

        xml::lite::Element* securityInformationXML = getOptional(
            geographicInfoXML, "SecurityInformation");
        if (securityInformationXML)
        {
            geographicCoverage->geographicInformation->securityInformation =
                    securityInformationXML->getCharacterData();
        }

        //GeographicInfoExtension
        try
        {
            parseParameters(
                    geographicInfoXML,
                    "GeographicInfoExtension",
                    geographicCoverage->geographicInformation-> geographicInformationExtensions);
        }
        catch (except::Exception&)
        {
        }

    }
}

void DerivedXMLControl::xmlToMeasurement(xml::lite::Element* measurementXML,
        Measurement* measurement)
{
    parseRowColInt(getFirstAndOnly(measurementXML, "PixelFootprint"),
            measurement->pixelFootprint);

    xml::lite::Element* projXML;
    if (measurement->projection->projectionType == PROJECTION_PLANE)
    {
        projXML = getFirstAndOnly(measurementXML, "PlaneProjection");
    }
    else if (measurement->projection->projectionType == PROJECTION_CYLINDRICAL)
    {
        projXML = getFirstAndOnly(measurementXML, "CylindricalProjection");
    }
    else if (measurement->projection->projectionType == PROJECTION_GEOGRAPHIC)
    {
        projXML = getFirstAndOnly(measurementXML, "GeographicProjection");
    }

    xml::lite::Element* refXML = getFirstAndOnly(projXML, "ReferencePoint");

    parseVector3D(getFirstAndOnly(refXML, "ECEF"),
            measurement->projection->referencePoint.ecef);

    parseRowColDouble(getFirstAndOnly(refXML, "Point"),
            measurement->projection->referencePoint.rowCol);

    parseRowColDouble(getFirstAndOnly(projXML, "SampleSpacing"),
            measurement->projection->sampleSpacing);

    if (measurement->projection->projectionType == PROJECTION_PLANE)
    {
        PlaneProjection* planeProj = (PlaneProjection*) measurement->projection;

        xml::lite::Element* prodPlaneXML = getFirstAndOnly(projXML,
                "ProductPlane");

        parseVector3D(getFirstAndOnly(prodPlaneXML, "RowUnitVector"),
                planeProj->productPlane.rowUnitVector);
        parseVector3D(getFirstAndOnly(prodPlaneXML, "ColUnitVector"),
                planeProj->productPlane.colUnitVector);
    }
    else if (measurement->projection->projectionType == PROJECTION_CYLINDRICAL)
    {
        xml::lite::Element* curvRadiusXML = getOptional(projXML, 
                "CurvatureRadius");
        if (curvRadiusXML)
        {
            CylindricalProjection* cylindricalProj = 
                    (CylindricalProjection*) measurement->projection;
            parseDouble(curvRadiusXML, cylindricalProj->curvatureRadius);
        }
    }

    xml::lite::Element* tmpElem = getFirstAndOnly(measurementXML, "ARPPoly");
    parsePolyXYZ(tmpElem, measurement->arpPoly);

    tmpElem = getFirstAndOnly(measurementXML, "TimeCOAPoly");
    parsePoly2D(tmpElem, measurement->timeCOAPoly);
}

void DerivedXMLControl::xmlToExploitationFeatures(
        xml::lite::Element* exploitationFeaturesXML,
        ExploitationFeatures* exploitationFeatures)
{
    xml::lite::Element* tmpElem;

    std::vector<xml::lite::Element*> collectionsXML;
    exploitationFeaturesXML->getElementsByTagName("Collection", collectionsXML);

    unsigned int idx = 0;
    for (std::vector<xml::lite::Element*>::iterator it = collectionsXML.begin(); it
            != collectionsXML.end(); ++it)
    {
        xml::lite::Element* collectionXML = *it;
        Collection* coll;
        if (exploitationFeatures->collections.size() <= idx)
        {
            coll = new Collection();
            exploitationFeatures->collections.push_back(coll);
        }
        else
        {
            coll = exploitationFeatures->collections[idx];
        }

        // Information
        Information* info = coll->information;
        xml::lite::Element* informationXML = getFirstAndOnly(collectionXML,
                "Information");

        info->sensorName
                = getFirstAndOnly(informationXML, "SensorName")->getCharacterData();

        xml::lite::Element* radarModeXML = getFirstAndOnly(informationXML,
                "RadarMode");
        info->radarMode = str::toType<RadarModeType>(getFirstAndOnly(
                radarModeXML, "ModeType")->getCharacterData());
        tmpElem = getOptional(radarModeXML, "ModeID");
        if (tmpElem)
        {
            info->radarModeID = tmpElem->getCharacterData();
        }

        parseDateTime(getFirstAndOnly(informationXML, "CollectionDateTime"),
                info->collectionDateTime);

        tmpElem = getOptional(informationXML, "LocalDateTime");
        if (tmpElem)
        {
            info->localDateTime = tmpElem->getCharacterData();
        }

        parseDouble(getFirstAndOnly(informationXML, "CollectionDuration"),
                info->collectionDuration);

        parseRangeAzimuth(getFirstAndOnly(informationXML, "Resolution"),
                info->resolution);

        xml::lite::Element* roiXML = getOptional(informationXML, "InputROI");
        if (roiXML)
        {
            info->inputROI = new InputROI();

            parseRowColDouble(getFirstAndOnly(roiXML, "Size"),
                    info->inputROI->size);
            parseRowColDouble(getFirstAndOnly(roiXML, "UpperLeft"),
                    info->inputROI->upperLeft);
        }

        xml::lite::Element* polXML = getOptional(informationXML, "Polarization");
        if (polXML)
        {
            info->polarization = new TxRcvPolarization();

            tmpElem = getFirstAndOnly(polXML, "TxPolarization");
            info->polarization->txPolarization = str::toType<PolarizationType>(
                    tmpElem->getCharacterData());

            tmpElem = getFirstAndOnly(polXML, "RcvPolarization");
            parseDouble(tmpElem, info->polarization->rcvPolarization);
        }

        // Geometry
        xml::lite::Element* geometryXML =
                getOptional(collectionXML, "Geometry");

        if (geometryXML)
        {
            Geometry* geom = new Geometry();
            coll->geometry = geom;

            tmpElem = getOptional(geometryXML, "Azimuth");
            if (tmpElem)
            {
                parseDouble(tmpElem, geom->azimuth);
            }

            tmpElem = getOptional(geometryXML, "Slope");
            if (tmpElem)
            {
                parseDouble(tmpElem, geom->slope);
            }

            tmpElem = getOptional(geometryXML, "Squint");
            if (tmpElem)
            {
                parseDouble(tmpElem, geom->squint);
            }

            tmpElem = getOptional(geometryXML, "Graze");
            if (tmpElem)
            {
                parseDouble(tmpElem, geom->graze);
            }

            tmpElem = getOptional(geometryXML, "Tilt");
            if (tmpElem)
            {
                parseDouble(tmpElem, geom->tilt);
            }
        }

        // Phenomenology

        xml::lite::Element* phenomenologyXML = getOptional(collectionXML,
                "Phenomenology");

        if (phenomenologyXML)
        {

            Phenomenology* phenom = new Phenomenology();
            coll->phenomenology = phenom;

            tmpElem = getOptional(phenomenologyXML, "Shadow");
            if (tmpElem)
            {
                parseDouble(getFirstAndOnly(tmpElem, "Angle"),
                        phenom->shadow.angle);
                parseDouble(getFirstAndOnly(tmpElem, "Magnitude"),
                        phenom->shadow.magnitude);
            }

            tmpElem = getOptional(phenomenologyXML, "Layover");
            if (tmpElem)
            {
                parseDouble(getFirstAndOnly(tmpElem, "Angle"),
                        phenom->layover.angle);
                parseDouble(getFirstAndOnly(tmpElem, "Magnitude"),
                        phenom->layover.magnitude);
            }

            tmpElem = getOptional(phenomenologyXML, "MultiPath");
            if (tmpElem)
            {
                parseDouble(tmpElem, phenom->multiPath);
            }

            tmpElem = getOptional(phenomenologyXML, "GroundTrack");
            if (tmpElem)
            {
                parseDouble(tmpElem, phenom->groundTrack);
            }
        }
    }

    xml::lite::Element* productXML = getFirstAndOnly(exploitationFeaturesXML,
            "Product");
    Product prod = exploitationFeatures->product;

    parseRowColDouble(getFirstAndOnly(productXML, "Resolution"),
            exploitationFeatures->product.resolution);

    tmpElem = getOptional(productXML, "North");
    if (tmpElem)
        parseDouble(tmpElem, prod.north);
}

Data* DerivedXMLControl::fromXML(xml::lite::Document* doc)
{
    xml::lite::Element *root = doc->getRootElement();

    xml::lite::Element* productCreationXML = getFirstAndOnly(root,
            "ProductCreation");
    xml::lite::Element* displayXML = getFirstAndOnly(root, "Display");
    xml::lite::Element* measurementXML = getFirstAndOnly(root, "Measurement");
    xml::lite::Element* exploitationFeaturesXML = getFirstAndOnly(root,
            "ExploitationFeatures");
    xml::lite::Element* geographicAndTargetXML = getFirstAndOnly(root,
            "GeographicAndTarget");

    DerivedDataBuilder builder;
    DerivedData *data = builder.steal(); //steal it

    // see if PixelType has MONO or RGB
    PixelType pixelType = str::toType<PixelType>(getFirstAndOnly(displayXML,
            "PixelType")->getCharacterData());
    builder.addDisplay(pixelType);

    RegionType regionType = REGION_SUB_REGION;
    xml::lite::Element* tmpElem = getFirstAndOnly(geographicAndTargetXML,
            "GeographicCoverage");
    // see if GeographicCoverage contains SubRegion or GeographicInfo

    if (getOptional(tmpElem, "SubRegion"))
    {
        regionType = REGION_SUB_REGION;
    }
    else if (getOptional(tmpElem, "GeographicInfo"))
    {
        regionType = REGION_GEOGRAPHIC_INFO;
    }
    builder.addGeographicAndTarget(regionType);

    six::ProjectionType projType = six::PROJECTION_PLANE;
    if (getOptional(measurementXML, "GeographicProjection"))
        projType = six::PROJECTION_GEOGRAPHIC;
    else if (getOptional(measurementXML, "CylindricalProjection"))
        projType = six::PROJECTION_CYLINDRICAL;

    builder.addMeasurement(projType);

    std::vector<xml::lite::Element*> elements;
    exploitationFeaturesXML->getElementsByTagName("ExploitationFeatures",
            elements);
    builder.addExploitationFeatures(elements.size());

    xmlToProductCreation(productCreationXML, data->productCreation);
    xmlToDisplay(displayXML, data->display);
    xmlToGeographicAndTarget(geographicAndTargetXML, data->geographicAndTarget);
    xmlToMeasurement(measurementXML, data->measurement);
    xmlToExploitationFeatures(exploitationFeaturesXML,
            data->exploitationFeatures);

    xml::lite::Element *productProcessingXML = getOptional(root,
            "ProductProcessing");
    if (productProcessingXML)
    {
        builder.addProductProcessing();
        xmlToProductProcessing(productProcessingXML, data->productProcessing);
    }

    xml::lite::Element *downstreamReprocessingXML = getOptional(root,
            "DownstreamReprocessing");
    if (downstreamReprocessingXML)
    {
        builder.addDownstreamReprocessing();
        xmlToDownstreamReprocessing(downstreamReprocessingXML,
                data->downstreamReprocessing);
    }

    xml::lite::Element *errorStatisticsXML = getOptional(root,
            "ErrorStatistics");
    if (errorStatisticsXML)
    {
        builder.addErrorStatistics();
        xmlToErrorStatistics(errorStatisticsXML, data->errorStatistics);
    }

    xml::lite::Element *radiometricXML = getOptional(root, "Radiometric");
    if (radiometricXML)
    {
        builder.addRadiometric();
        xmlToRadiometric(radiometricXML, data->radiometric);
    }

    return data;
}

xml::lite::Element*
DerivedXMLControl::productCreationToXML(xml::lite::Document* doc,
        ProductCreation* productCreation)
{

    // Make the XML node
    xml::lite::Element* productCreationXML = newElement(doc, "ProductCreation");

    // Processor Info
    xml::lite::Element* procInfoXML = newElement(doc, "ProcessorInformation");

    productCreationXML->addChild(procInfoXML);

    procInfoXML->addChild(createString(doc, "Application",
            productCreation->processorInformation->application));

    procInfoXML->addChild(createDateTime(doc, "ProcessingDateTime",
            productCreation->processorInformation->processingDateTime));

    procInfoXML->addChild(createString(doc, "Site",
            productCreation->processorInformation->site));

    //Profile - optional
    if (productCreation->processorInformation->profile != Init::undefined<
            std::string>())
    {
        procInfoXML->addChild(createString(doc, "Profile",
                productCreation->processorInformation->profile));
    }

    //Classification
    xml::lite::Element* classXML = newElement(doc, "Classification");
    productCreationXML->addChild(classXML);
    classXML->addChild(createString(doc, "Level",
            productCreation->classification.level));

    for (std::vector<std::string>::iterator it =
            productCreation->classification.securityMarkings.begin(); it
            != productCreation->classification.securityMarkings.end(); ++it)
    {
        std::string marking = *it;
        str::trim(marking);
        if (!marking.empty())
            classXML->addChild(createString(doc, "SecurityMarkings", *it));
    }

    if (productCreation->classification.guidance)
    {
        xml::lite::Element* guidanceXML = newElement(doc, "Guidance");
        classXML->addChild(guidanceXML);
        guidanceXML->addChild(createString(doc, "Authority",
                productCreation->classification.guidance->authority));
        guidanceXML->addChild(createDate(doc, "Date",
                productCreation->classification.guidance->date));
    }

    //ProductName
    productCreationXML->addChild(createString(doc, "ProductName",
            productCreation->productName));

    //ProductClass
    productCreationXML->addChild(createString(doc, "ProductClass",
            productCreation->productClass));

    //ProductType - optional
    if (productCreation->productType != Init::undefined<std::string>())
    {
        productCreationXML->addChild(createString(doc, "ProductType",
                productCreation->productType));
    }

    //product extensions
    addParameters(doc, productCreationXML, "ProductCreationExtension",
            productCreation->productCreationExtensions);

    return productCreationXML;
}

xml::lite::Element*
DerivedXMLControl::displayToXML(xml::lite::Document* doc, Display* display)
{
    xml::lite::Element* displayXML = newElement(doc, "Display");

    displayXML->addChild(createString(doc, "PixelType", str::toString(
            display->pixelType)));

    //RemapInformation - optional
    if (display->remapInformation && (display->pixelType == RGB8LU
            || display->pixelType == MONO8LU))
    {
        xml::lite::Element* remapInfoXML = newElement(doc, "RemapInformation");
        displayXML->addChild(remapInfoXML);

        xml::lite::Element* remapXML;
        if (display->pixelType == RGB8LU)
        {
            remapXML = newElement(doc, "ColorDisplayRemap");
            remapXML->addChild(createLUT(doc, "RemapLUT",
                    display->remapInformation->remapLUT));
        }
        else
        {
            remapXML = newElement(doc, "MonochromeDisplayRemap");
            remapXML->addChild(
                    createString(
                            doc,
                            "RemapType",
                            str::toString(
                                    ((MonochromeDisplayRemap*) display->remapInformation)->remapType)));
            remapXML->addChild(createLUT(doc, "RemapLUT",
                    display->remapInformation->remapLUT));
            //should we do this cast (i.e. assume the pixelType is correct)?
            addParameters(
                    doc,
                    remapXML,
                    "RemapParameter",
                    ((MonochromeDisplayRemap*) display->remapInformation)->remapParameters);
        }
        remapInfoXML->addChild(remapXML);
    }

    //MagnificationMethod - optional
    if (display->magnificationMethod != MAG_NOT_SET)
    {
        displayXML->addChild(createString(doc, "MagnificationMethod",
                str::toString(display->magnificationMethod)));
    }

    //DecimationMethod - optional
    if (display->decimationMethod != DEC_NOT_SET)
    {
        displayXML->addChild(createString(doc, "DecimationMethod",
                str::toString(display->decimationMethod)));
    }

    //DRAHistogramOverrides - optional
    if (display->histogramOverrides)
    {
        xml::lite::Element *histo = newElement(doc, "DRAHistogramOverrides");
        displayXML->addChild(histo);
        histo->addChild(createInt(doc, "ClipMin",
                display->histogramOverrides->clipMin));
        histo->addChild(createInt(doc, "ClipMax",
                display->histogramOverrides->clipMax));
    }

    if (display->monitorCompensationApplied)
    {
        //MonitorCompensationApplied - optional
        xml::lite::Element *monComp = newElement(doc,
                "MonitorCompensationApplied");
        monComp->addChild(createDouble(doc, "Gamma",
                display->monitorCompensationApplied->gamma));
        monComp->addChild(createDouble(doc, "XMin",
                display->monitorCompensationApplied->xMin));
        displayXML->addChild(monComp);
    }

    addParameters(doc, displayXML, "DisplayExtension",
            display->displayExtensions);
    return displayXML;
}

xml::lite::Element*
DerivedXMLControl::geographicAndTargetToXML(xml::lite::Document* doc,
        GeographicAndTarget* geographicAndTarget)
{
    xml::lite::Element* geographicAndTargetXML = newElement(doc,
            "GeographicAndTarget");

    geographicAndTargetXML->addChild(geographicCoverageToXML(doc,
            geographicAndTarget->geographicCoverage));

    //loop over TargetInformation
    for (std::vector<TargetInformation*>::iterator it =
            geographicAndTarget->targetInformation.begin(); it
            != geographicAndTarget->targetInformation.end(); ++it)
    {
        TargetInformation* ti = *it;
        xml::lite::Element* tiXML = newElement(doc, "TargetInformation");
        geographicAndTargetXML->addChild(tiXML);

        addParameters(doc, tiXML, "Identifier", ti->identifiers);
        for (unsigned int i = 0; i < ti->footprints.size(); i++)
        {
            tiXML->addChild(createFootprint(doc, "Footprint", "Vertex",
                    ti->footprints[i]));
        }
        addParameters(doc, tiXML, "TargetInformationExtension",
                ti->targetInformationExtensions);
    }

    return geographicAndTargetXML;
}

xml::lite::Element* DerivedXMLControl::geographicCoverageToXML(
        xml::lite::Document* doc, GeographicCoverage* geoCoverage)
{
    //GeographicAndTarget
    xml::lite::Element* geoCoverageXML = newElement(doc, "GeographicCoverage");

    //Georegion Identifiers
    addParameters(doc, geoCoverageXML, "GeoregionIdentifier",
            geoCoverage->georegionIdentifiers);

    //Footprint
    geoCoverageXML->addChild(createFootprint(doc, "Footprint", "Vertex",
            geoCoverage->footprint));

    // GeographicInfo
    if (geoCoverage->geographicInformation)
    {
        xml::lite::Element* geoInfoXML = newElement(doc, "GeographicInfo");
        geoCoverageXML->addChild(geoInfoXML);

        for (unsigned int i = 0, numCC =
                geoCoverage->geographicInformation->countryCodes.size(); i
                < numCC; ++i)
        {
            geoInfoXML->addChild(createString(doc, "CountryCode",
                    geoCoverage->geographicInformation->countryCodes[i]));
        }

        geoInfoXML->addChild(createString(doc, "SecurityInfo",
                geoCoverage->geographicInformation->securityInformation));
        addParameters(
                doc,
                geoInfoXML,
                "GeographicInfoExtension",
                geoCoverage->geographicInformation->geographicInformationExtensions);
    }
    else
    {
        //loop over SubRegions
        for (std::vector<GeographicCoverage*>::iterator it =
                geoCoverage->subRegion.begin(); it
                != geoCoverage->subRegion.end(); ++it)
        {
            xml::lite::Element* subRegionXML =
                    geographicCoverageToXML(doc, *it);
            subRegionXML->setQName("SubRegion");
            geoCoverageXML->addChild(subRegionXML);
        }
    }

    return geoCoverageXML;
}

xml::lite::Element*
DerivedXMLControl::measurementToXML(xml::lite::Document* doc,
        Measurement* measurement)
{
    xml::lite::Element* measurementXML = newElement(doc, "Measurement");

    Projection* projection = measurement->projection;

    std::string projectionName =
            projection->projectionType == PROJECTION_PLANE ? "PlaneProjection"
                    : (projection->projectionType == PROJECTION_GEOGRAPHIC ? 
                            "GeographicProjection" : "CylindricalProjection");

    // PlaneProjection
    xml::lite::Element* projectionXML = newElement(doc, projectionName);
    measurementXML->addChild(projectionXML);

    if (projection->projectionType == PROJECTION_PLANE)
    {
        PlaneProjection* planeProjection = (PlaneProjection*) projection;

        xml::lite::Element* productPlaneXML = newElement(doc, "ProductPlane");
        projectionXML->addChild(productPlaneXML);

        //RowBasis
        productPlaneXML->addChild(createVector3D(doc, "RowUnitVector",
                planeProjection->productPlane.rowUnitVector));

        //ColBasis
        productPlaneXML->addChild(createVector3D(doc, "ColUnitVector",
                planeProjection->productPlane.colUnitVector));
    }

    xml::lite::Element* referencePointXML = newElement(doc, "ReferencePoint");
    setAttribute(referencePointXML, "name", projection->referencePoint.name);
    projectionXML->addChild(referencePointXML);

    //ECEF
    referencePointXML->addChild(createVector3D(doc, "ECEF",
            projection->referencePoint.ecef));
    //Point
    referencePointXML->addChild(createRowCol(doc, "Point",
            projection->referencePoint.rowCol));

    projectionXML->addChild(createRowCol(doc, "SampleSpacing",
            projection->sampleSpacing));

    if (projection->projectionType == PROJECTION_CYLINDRICAL)
    {
        CylindricalProjection* cylindricalProj = 
                (CylindricalProjection*) projection;

        if (cylindricalProj->curvatureRadius != Init::undefined<double>())
        {
            projectionXML->addChild(createDouble(doc, "CurvatureRadius",
                    cylindricalProj->curvatureRadius));
        }
    }

    //Pixel Footprint
    measurementXML->addChild(createRowCol(doc, "PixelFootprint",
            measurement->pixelFootprint));

    //ARPPoly
    measurementXML->addChild(
            createPolyXYZ(doc, "ARPPoly", measurement->arpPoly));

    //TimeCOAPoly
    xml::lite::Element* timeCOAPolyXML = createPoly2D(doc, "TimeCOAPoly",
            measurement->timeCOAPoly);
    measurementXML->addChild(timeCOAPolyXML);
    return measurementXML;
}

xml::lite::Element*
DerivedXMLControl::exploitationFeaturesToXML(xml::lite::Document* doc,
        ExploitationFeatures* exploitationFeatures)
{

    xml::lite::Element* exploitationFeaturesXML = newElement(doc,
            "ExploitationFeatures");

    for (unsigned int i = 0; i < exploitationFeatures->collections.size(); ++i)
    {
        Collection* collection = exploitationFeatures->collections[i];
        xml::lite::Element* collectionXML = newElement(doc, "Collection");
        exploitationFeaturesXML->addChild(collectionXML);
        setAttribute(collectionXML, "identifier", collection->identifier);

        xml::lite::Element* informationXML = newElement(doc, "Information");
        collectionXML->addChild(informationXML);

        informationXML->addChild(createString(doc, "SensorName",
                collection->information->sensorName));

        xml::lite::Element* radarModeXML = newElement(doc, "RadarMode");
        informationXML->addChild(radarModeXML);
        //ModeType - Incomplete?
        radarModeXML->addChild(createString(doc, "ModeType", str::toString(
                collection->information->radarMode)));
        if (collection->information->radarModeID
                != Init::undefined<std::string>())
        {
            radarModeXML->addChild(createString(doc, "ModeID",
                    collection->information->radarModeID));
        }

        //CollectionDateTime
        informationXML->addChild(createDateTime(doc, "CollectionDateTime",
                collection->information->collectionDateTime));

        //LocalDateTime
        if (collection->information->localDateTime
                != Init::undefined<std::string>())
        {
            informationXML->addChild(createDateTime(doc, "LocalDateTime",
                    collection->information->localDateTime));
        }

        informationXML->addChild(createDouble(doc, "CollectionDuration",
                collection->information->collectionDuration));

        //Resolution
        informationXML->addChild(createRangeAzimuth(doc, "Resolution",
                collection->information->resolution));

        //InputROI
        if (collection->information->inputROI != NULL)
        {
            xml::lite::Element* roiXML = newElement(doc, "InputROI");
            roiXML->addChild(createRowCol(doc, "Size",
                    collection->information->inputROI->size));
            roiXML->addChild(createRowCol(doc, "UpperLeft",
                    collection->information->inputROI->upperLeft));
            informationXML->addChild(roiXML);
        }

        if (collection->information->polarization != NULL)
        {
            xml::lite::Element* polXML = newElement(doc, "Polarization");
            polXML->addChild(createString(doc, "TxPolarization", str::toString(
                    collection->information->polarization->txPolarization)));
            polXML->addChild(createDouble(doc, "RcvPolarization",
                    collection->information->polarization->rcvPolarization));
        }

        //Geometry
        Geometry* geom = collection->geometry;

        if (geom != NULL)
        {
            xml::lite::Element* geometryXML = newElement(doc, "Geometry");

            if (geom->azimuth != Init::undefined<double>())
            {
                geometryXML->addChild(createDouble(doc, "Azimuth",
                        geom->azimuth));
            }

            if (geom->slope != Init::undefined<double>())
            {
                geometryXML->addChild(createDouble(doc, "Slope", geom->slope));
            }

            if (geom->squint != Init::undefined<double>())
            {
                geometryXML->addChild(createDouble(doc, "Squint", geom->squint));
            }

            if (geom->graze != Init::undefined<double>())
            {
                geometryXML->addChild(createDouble(doc, "Graze", geom->graze));
            }

            if (geom->tilt != Init::undefined<double>())
            {
                geometryXML->addChild(createDouble(doc, "Tilt", geom->tilt));
            }

            collectionXML->addChild(geometryXML);
        }

        //Phenomenology
        Phenomenology* phenom = collection->phenomenology;
        if (phenom != NULL)
        {
            xml::lite::Element* phenomenologyXML = newElement(doc,
                    "Phenomenology");

            if (phenom->shadow != Init::undefined<AngleMagnitude>())
            {
                xml::lite::Element* shadow = newElement(doc, "Shadow");
                shadow->addChild(createDouble(doc, "Angle",
                        phenom->shadow.angle));
                shadow->addChild(createDouble(doc, "Magnitude",
                        phenom->shadow.magnitude));
                phenomenologyXML->addChild(shadow);
            }

            if (phenom->layover != Init::undefined<AngleMagnitude>())
            {
                xml::lite::Element* layover = newElement(doc, "Layover");
                layover->addChild(createDouble(doc, "Angle",
                        phenom->layover.angle));
                layover->addChild(createDouble(doc, "Magnitude",
                        phenom->layover.magnitude));
                phenomenologyXML->addChild(layover);
            }

            if (phenom->multiPath != Init::undefined<double>())
            {
                phenomenologyXML->addChild(createDouble(doc, "MultiPath",
                        phenom->multiPath));
            }

            if (phenom->groundTrack != Init::undefined<double>())
            {
                phenomenologyXML->addChild(createDouble(doc, "GroundTrack",
                        phenom->groundTrack));
            }

            collectionXML->addChild(phenomenologyXML);
        }
    }

    //Product
    xml::lite::Element* productXML = newElement(doc, "Product");
    exploitationFeaturesXML->addChild(productXML);

    //Resolution
    productXML->addChild(createRowCol(doc, "Resolution",
            exploitationFeatures->product.resolution));

    //North - optional

    if (exploitationFeatures->product.north != Init::undefined<double>())
    {
        productXML->addChild(createDouble(doc, "North",
                exploitationFeatures->product.north));
    }

    return exploitationFeaturesXML;
}

xml::lite::Document* DerivedXMLControl::toXML(Data* data)
{
    if (data->getDataClass() != DATA_DERIVED)
    {
        throw except::Exception("Data must be derived");
    }
    xml::lite::Document* doc = new xml::lite::Document();

    xml::lite::Element* root = newElement(doc, "SIDD");
    doc->setRootElement(root);

    DerivedData *derived = (DerivedData*) data;

    doc->insert(productCreationToXML(doc, derived->productCreation), root);

    doc->insert(displayToXML(doc, derived->display), root);

    doc->insert(geographicAndTargetToXML(doc, derived->geographicAndTarget),
            root);

    doc->insert(measurementToXML(doc, derived->measurement), root);

    doc->insert(exploitationFeaturesToXML(doc, derived->exploitationFeatures),
            root);

    if (derived->productProcessing)
    {
        doc->insert(productProcessingToXML(doc, derived->productProcessing),
                root);
    }
    if (derived->errorStatistics)
    {
        doc->insert(errorStatisticsToXML(doc, derived->errorStatistics), root);
    }
    if (derived->radiometric)
    {
        doc->insert(radiometricToXML(doc, derived->radiometric), root);
    }
    return doc;
}

xml::lite::Element* DerivedXMLControl::createLUT(xml::lite::Document* doc,
        std::string name, LUT *lut)
{
    //     unsigned char* table;
    //     unsigned int numEntries;
    //     unsigned int elementSize;

    xml::lite::Element* lutElement = newElement(doc, name);
    setAttribute(lutElement, "size", str::toString(lut->numEntries));

    std::ostringstream oss;
    for (unsigned int i = 0; i < lut->numEntries; ++i)
    {
        if (lut->elementSize == 2)
        {
            short* idx = (short*) (*lut)[i];
            oss << *idx;
        }
        else if (lut->elementSize == 3)
        {
            //must add 128 to print ASCII
            oss << (unsigned int) (*lut)[i][0] << ','
                    << (unsigned int) (*lut)[i][1] << ','
                    << (unsigned int) (*lut)[i][2];
        }
        else
        {
            throw except::Exception(Ctxt(FmtX("Invalid element size [%d]",
                    lut->elementSize)));
        }
        if ((lut->numEntries - 1) != i)
            oss << ' ';
    }
    lutElement->setCharacterData(oss.str());
    return lutElement;
}

xml::lite::Element* DerivedXMLControl::productProcessingToXML(
        xml::lite::Document* doc, ProductProcessing* productProcessing)
{
    xml::lite::Element* productProcessingXML = newElement(doc,
            "ProductProcessing");

    for (std::vector<ProcessingModule*>::iterator it =
            productProcessing->processingModules.begin(); it
            != productProcessing->processingModules.end(); ++it)
    {
        productProcessingXML->addChild(processingModuleToXML(doc, *it));
    }
    return productProcessingXML;
}

xml::lite::Element* DerivedXMLControl::processingModuleToXML(
        xml::lite::Document* doc, ProcessingModule* procMod)
{
    xml::lite::Element* procModXML = newElement(doc, "ProcessingModule");
    xml::lite::Element *modNameXML = createString(doc, "ModuleName",
            procMod->moduleName.str());
    setAttribute(modNameXML, "name", procMod->moduleName.getName());
    procModXML->addChild(modNameXML);

    if (!procMod->processingModules.empty())
    {
        for (std::vector<ProcessingModule*>::iterator it =
                procMod->processingModules.begin(); it
                != procMod->processingModules.end(); ++it)
        {
            procModXML->addChild(processingModuleToXML(doc, *it));
        }
    }
    else
    {
        addParameters(doc, procModXML, "ModuleParameter",
                procMod->moduleParameters);
    }
    return procModXML;

}

xml::lite::Element* DerivedXMLControl::downstreamReprocessingToXML(
        xml::lite::Document* doc, DownstreamReprocessing* downstreamReproc)
{
    xml::lite::Element* epXML = newElement(doc, "DownstreamReprocessing");

    GeometricChip *geoChip = downstreamReproc->geometricChip;
    if (geoChip)
    {
        xml::lite::Element* geoChipXML = newElement(doc, "GeometricChip");
        epXML->addChild(geoChipXML);

        geoChipXML->addChild(createRowCol(doc, "ChipSize", geoChip->chipSize));
        geoChipXML->addChild(createRowCol(doc, "OriginalUpperLeftCoordinate",
                geoChip->originalUpperLeftCoordinate));
        geoChipXML->addChild(createRowCol(doc, "OriginalUpperRightCoordinate",
                geoChip->originalUpperRightCoordinate));
        geoChipXML->addChild(createRowCol(doc, "OriginalLowerLeftCoordinate",
                geoChip->originalLowerLeftCoordinate));
        geoChipXML->addChild(createRowCol(doc, "OriginalLowerRightCoordinate",
                geoChip->originalLowerRightCoordinate));
    }
    if (!downstreamReproc->processingEvents.empty())
    {
        for (std::vector<ProcessingEvent*>::iterator it =
                downstreamReproc->processingEvents.begin(); it
                != downstreamReproc->processingEvents.end(); ++it)
        {
            ProcessingEvent *procEvent = *it;
            xml::lite::Element* procEventXML = newElement(doc,
                    "ProcessingEvent");
            epXML->addChild(procEventXML);

            procEventXML->addChild(createString(doc, "ApplicationName",
                    procEvent->applicationName));
            procEventXML->addChild(createDateTime(doc, "AppliedDateTime",
                    procEvent->appliedDateTime));

            if (procEvent->interpolationMethod.empty())
            {
                procEventXML->addChild(createString(doc, "InterpolationMethod",
                        procEvent->interpolationMethod));
            }
            addParameters(doc, procEventXML, "Descriptor",
                    procEvent->descriptor);
        }
    }
    return epXML;
}

void DerivedXMLControl::xmlToProcessingModule(xml::lite::Element* procXML,
        ProcessingModule* procMod)
{
    xml::lite::Element *moduleName = getFirstAndOnly(procXML, "ModuleName");
    procMod->moduleName = Parameter(moduleName->getCharacterData());
    procMod->moduleName.setName(moduleName->getAttributes().getValue("name"));

    parseParameters(procXML, "ModuleParameter", procMod->moduleParameters);

    std::vector<xml::lite::Element*> procModuleXML;
    procXML->getElementsByTagName("ProcessingModule", procModuleXML);

    for (unsigned int i = 0, size = procModuleXML.size(); i < size; ++i)
    {
        ProcessingModule *pm = new ProcessingModule();
        xmlToProcessingModule(procModuleXML[i], pm);
        procMod->processingModules.push_back(pm);
    }
}

void DerivedXMLControl::xmlToProductProcessing(xml::lite::Element* elem,
        ProductProcessing* productProcessing)
{
    std::vector<xml::lite::Element*> procModuleXML;
    elem->getElementsByTagName("ProcessingModule", procModuleXML);

    for (unsigned int i = 0, size = procModuleXML.size(); i < size; ++i)
    {
        ProcessingModule *procMod = new ProcessingModule();
        xmlToProcessingModule(procModuleXML[i], procMod);
        productProcessing->processingModules.push_back(procMod);
    }
}

void DerivedXMLControl::xmlToDownstreamReprocessing(xml::lite::Element* elem,
        DownstreamReprocessing* downstreamReproc)
{
    xml::lite::Element *geometricChipXML = getOptional(elem, "GeometricChip");
    std::vector<xml::lite::Element*> procEventXML;
    elem->getElementsByTagName("ProcessingEvent", procEventXML);
    xml::lite::Element *processingEventXML = getOptional(elem,
            "ProcessingEvent");

    if (geometricChipXML)
    {
        GeometricChip *chip = downstreamReproc->geometricChip
                = new GeometricChip();

        parseRowColInt(getFirstAndOnly(geometricChipXML, "ChipSize"),
                chip->chipSize);

        parseRowColDouble(getFirstAndOnly(geometricChipXML,
                "OriginalUpperLeftCoordinate"),
                chip->originalUpperLeftCoordinate);

        parseRowColDouble(getFirstAndOnly(geometricChipXML,
                "OriginalUpperRightCoordinate"),
                chip->originalUpperRightCoordinate);

        parseRowColDouble(getFirstAndOnly(geometricChipXML,
                "OriginalLowerLeftCoordinate"),
                chip->originalLowerLeftCoordinate);

        parseRowColDouble(getFirstAndOnly(geometricChipXML,
                "OriginalLowerRightCoordinate"),
                chip->originalLowerRightCoordinate);
    }

    for (unsigned int i = 0, size = procEventXML.size(); i < size; ++i)
    {
        ProcessingEvent *procEvent = new ProcessingEvent();
        downstreamReproc->processingEvents.push_back(procEvent);
        xml::lite::Element *peXML = procEventXML[i];

        procEvent->applicationName
                = getFirstAndOnly(peXML, "ApplicationName")->getCharacterData();
        parseDateTime(getFirstAndOnly(peXML, "AppliedDateTime"),
                procEvent->appliedDateTime);

        xml::lite::Element *tmpElem = getOptional(peXML, "InterpolationMethod");
        if (tmpElem)
        {
            procEvent->interpolationMethod = tmpElem->getCharacterData();
        }
        parseParameters(peXML, "Descriptor", procEvent->descriptor);
    }
}
