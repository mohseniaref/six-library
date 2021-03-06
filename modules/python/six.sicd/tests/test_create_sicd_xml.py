#!/usr/bin/env python

#
# =========================================================================
# This file is part of six.sicd-python 
# =========================================================================
# 
# (C) Copyright 2004 - 2015, MDA Information Systems LLC
#
# six.sicd-python is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public 
# License along with this program; If not, 
# see <http://www.gnu.org/licenses/>.
#

import os
import filecmp

from pysix.scene import *
from pysix.six_sicd import *
from pysix.six_base import *
from coda.xml_lite import *
from coda.coda_io import *
from coda.coda_types import *
from coda.math_poly import *
from coda.math_linear import *

# Build up a giant ComplexData from scratch with everything populated
cmplx = ComplexData()

### Collection Information ###
collectionInfo = makeScopedCloneableCollectionInformation()
collectionInfo.collectorName = 'Some collector'
collectionInfo.illuminatorName = 'Some illuminator'
collectionInfo.coreName = 'Some corename'
collectionInfo.collectType = CollectType('MONOSTATIC')
collectionInfo.radarMode = RadarModeType('SPOTLIGHT')
collectionInfo.radarModeID = 'Some ID'
collectionInfo.classification.level = 'UNCLASSIFIED'
collectionInfo.countryCodes.push_back('US')

param = Parameter()
param.setName('Collection Param')
param.setValue('334')
#pdb.set_trace()
collectionInfo.parameters.push_back(param)
param.setValue('335')
collectionInfo.parameters.push_back(param)

cmplx.collectionInformation = collectionInfo

### Image Creation ###
imageCreation = makeScopedCloneableImageCreation()
imageCreation.application = 'Some application'
imageCreation.dateTime = DateTime()
imageCreation.site = 'Some site'
imageCreation.profile = 'Some profile'

cmplx.imageCreation = imageCreation

### Image Data ###
imageData = makeScopedCloneableImageData()
imageData.pixelType = PixelType('RE32F_IM32F')
# TODO: Fill in amplitudeTable
imageData.numRows = 123
imageData.numCols = 456
imageData.firstRow = 9
imageData.firstCol = 16
imageData.fullImage.row = 1000
imageData.fullImage.col = 2000
imageData.scpPixel.row = 500
imageData.scpPixel.col = 1000
imageData.validData.push_back(RowColInt(10, 20))
imageData.validData.push_back(RowColInt(30, 40))
imageData.validData.push_back(RowColInt(50, 60))

cmplx.imageData = imageData

### GeoData ###
geoData = makeScopedCloneableGeoData()
geoData.earthModel = EarthModelType('WGS84')
geoData.scp.ecf[0] = 100
geoData.scp.ecf[1] = 200
geoData.scp.ecf[2] = 300
geoData.scp.llh = LatLonAlt(10, 20, 30)

geoData.imageCorners.upperLeft = LatLon(11, 22)
geoData.imageCorners.upperRight = LatLon(22, 33)
geoData.imageCorners.lowerRight = LatLon(33, 44)
geoData.imageCorners.lowerLeft = LatLon(44, 55)
for i in range(4):
    geoData.validData.push_back(LatLon(i * 11, i * 11 + 13))

geoInfo = makeScopedCloneableGeoInfo()
geoInfo.name = 'My name'
param.setName('Some GeoData param')
param.setValue('Some GeoData value')
geoInfo.desc.push_back(param)
geoInfo.geometryLatLon.push_back(LatLon(10, 20))
geoInfo.geometryLatLon.push_back(LatLon(30, 40))
geoInfo.geometryLatLon.push_back(LatLon(50, 60))
geoData.geoInfos.push_back(geoInfo)

cmplx.geoData = geoData

### Grid ###
grid = makeScopedCloneableGrid()
grid.imagePlane = ComplexImagePlaneType('SLANT')
grid.type = ComplexImageGridType('RGAZIM')
grid.timeCOAPoly = Poly2D(3, 3)
for i in range(4):
    for j in range(4):
        grid.timeCOAPoly[(i, j)] = i + j

grid.row = makeScopedCloneableDirectionParameters()
for i in range(3):
    grid.row.unitVector[i] = i * 10
grid.row.sampleSpacing = 5
grid.row.impulseResponseWidth = 10
grid.row.sign = FFTSign('NEG')
grid.row.impulseResponseBandwidth = 13
grid.row.kCenter = 5
grid.row.deltaK1 = 9
grid.row.deltaK2 = 13
grid.row.deltaKCOAPoly = Poly2D(3, 3)
for i in range(4):
    for j in range(4):
        grid.row.deltaKCOAPoly[(i, j)] = i + j
grid.row.weightType = makeScopedCopyableWeightType()
grid.row.weightType.windowName = 'UNIFORM'

grid.col = makeScopedCloneableDirectionParameters()
for i in range(3):
    grid.col.unitVector[i] = i * 10
grid.col.sampleSpacing = 5
grid.col.impulseResponseWidth = 10
grid.col.sign = FFTSign('NEG')
grid.col.impulseResponseBandwidth = 13
grid.col.kCenter = 5
grid.col.deltaK1 = 9
grid.col.deltaK2 = 13
grid.col.deltaKCOAPoly = Poly2D(3, 3)
for i in range(4):
    for j in range(4):
        grid.col.deltaKCOAPoly[(i, j)] = i + j
grid.col.weightType = makeScopedCopyableWeightType()
grid.col.weightType.windowName = 'UNIFORM'

cmplx.grid = grid

### Timeline ###
timeline = makeScopedCopyableTimeline()
timeline.collectStart = DateTime()
timeline.collectDuration = 5
timeline.interPulsePeriod = makeScopedCopyableInterPulsePeriod()

timelineSet = TimelineSet()
timelineSet.tStart = 1
timelineSet.tEnd = 2
timelineSet.interPulsePeriodStart = 3
timelineSet.interPulsePeriodEnd = 4
timelineSet.interPulsePeriodPoly = Poly1D(3)
for i in range(4):
    timelineSet.interPulsePeriodPoly[i] = i * 10

timeline.interPulsePeriod.sets.push_back(timelineSet)

cmplx.timeline = timeline

### Position ###
position = makeScopedCopyablePosition()
position.arpPoly = PolyVector3(3)
position.grpPoly = PolyVector3(3)
position.txAPCPoly = PolyVector3(3)
for i in range(4):
    for j in range(3):
        position.arpPoly[i][j] = i + j
        position.grpPoly[i][j] = i + j
        position.txAPCPoly[i][j] = i + j

position.rcvAPC = makeScopedCopyableRcvAPC()
rcvAPCPoly = PolyVector3(3)
for i in range(4):
    for j in range(3):
        rcvAPCPoly[i][j] = i * 5 + j
position.rcvAPC.rcvAPCPolys.push_back(rcvAPCPoly)

cmplx.position = position

### Radar Collection ###
radarCollection = makeScopedCloneableRadarCollection()
radarCollection.refFrequencyIndex = 1
radarCollection.txFrequencyMin = -99
radarCollection.txFrequencyMax = 99
radarCollection.txPolarization = PolarizationSequenceType('V')

txStep = makeScopedCloneableTxStep()
txStep.waveformIndex = 1
txStep.txPolarization = PolarizationType('V')
radarCollection.txSequence.push_back(txStep)

wfParams = makeScopedCloneableWaveformParameters()
wfParams.txPulseLength = 11
wfParams.txRFBandwidth = 22
wfParams.txFrequencyStart = 33
wfParams.txFMRate = 44
wfParams.rcvDemodType = DemodType('STRETCH')
wfParams.rcvWindowLength = 55
wfParams.adcSampleRate = 66
wfParams.rcvIFBandwidth = 77
wfParams.rcvFrequencyStart = 88
wfParams.rcvFMRate = 99
radarCollection.waveform.push_back(wfParams)

chanParams = makeScopedCloneableChannelParameters()
chanParams.txRcvPolarization = DualPolarizationType('V_V')
chanParams.rcvAPCIndex = 47
radarCollection.rcvChannels.push_back(chanParams)

area = makeScopedCloneableArea()
area.acpCorners.upperLeft = LatLonAlt(1, 2, 3)
area.acpCorners.upperRight = LatLonAlt(4, 5, 6)
area.acpCorners.lowerRight = LatLonAlt(7, 8, 9)
area.acpCorners.lowerLeft = LatLonAlt(10, 11, 12)

areaPlane = makeScopedCloneableAreaPlane()
for i in range(3):
    areaPlane.referencePoint.ecef[i] = i * 10
areaPlane.referencePoint.rowCol.row = 12
areaPlane.referencePoint.rowCol.col = 34
areaPlane.referencePoint.name = 'My ref point'

xDir = makeScopedCloneableAreaDirectionParameters()
for i in range(3):
    xDir.unitVector[i] = i * 10
xDir.spacing = 11
xDir.elements = 500
xDir.first = 4
areaPlane.xDirection = xDir

yDir = makeScopedCloneableAreaDirectionParameters()
for i in range(3):
    yDir.unitVector[i] = i * 10
yDir.spacing = 11
yDir.elements = 500
yDir.first = 4
areaPlane.yDirection = yDir

segment = makeScopedCloneableSegment()
segment.startLine = 10
segment.startSample = 20
segment.endLine = 30
segment.endSample = 40
segment.identifier = 'My segment'
areaPlane.segmentList.push_back(segment)

areaPlane.orientation = OrientationType('UP')

area.plane = areaPlane

radarCollection.area = area

param.setName('Radar param')
param.setValue('Some radar val')
radarCollection.parameters.push_back(param)

cmplx.radarCollection = radarCollection

### Image Formation ###
imageFormation = makeScopedCopyableImageFormation()
imageFormation.segmentIdentifier = 'AA'

rcvChannelProcessed = makeScopedCopyableRcvChannelProcessed()
rcvChannelProcessed.numChannelsProcessed = 1
rcvChannelProcessed.prfScaleFactor = 99
rcvChannelProcessed.channelIndex.push_back(123)
imageFormation.rcvChannelProcessed = rcvChannelProcessed

imageFormation.txRcvPolarizationProc = DualPolarizationType('V_V')
imageFormation.imageFormationAlgorithm = ImageFormationType('PFA')
imageFormation.tStartProc = 57
imageFormation.tEndProc = 68
imageFormation.txFrequencyProcMin = 789
imageFormation.txFrequencyProcMax = 1111
imageFormation.slowTimeBeamCompensation = SlowTimeBeamCompensationType('GLOBAL')
imageFormation.imageBeamCompensation = ImageBeamCompensationType('SV')
imageFormation.azimuthAutofocus = AutofocusType('GLOBAL')
imageFormation.rangeAutofocus = AutofocusType('GLOBAL')

processing = Processing()
processing.type = 'Some type'
processing.applied = BooleanType('IS_TRUE')
param.setName('Processing param')
param.setValue('Processing value')
processing.parameters.push_back(param)
imageFormation.processing.push_back(processing)

polCal = makeScopedCopyablePolarizationCalibration()
polCal.hvAngleCompensationApplied = BooleanType('IS_TRUE')
polCal.distortionCorrectionApplied = BooleanType('IS_TRUE')
polCal.distortion = makeScopedCopyableDistortion()
polCal.distortion.calibrationDate = DateTime()
polCal.distortion.a = 55
polCal.distortion.f1 = 1 + 2j
polCal.distortion.q1 = 3 + 4j
polCal.distortion.q2 = 5 + 6j
polCal.distortion.f2 = 7 + 8j
polCal.distortion.q3 = 9 + 10j
polCal.distortion.q4 = 11 + 12j
polCal.distortion.gainErrorA = 9
polCal.distortion.gainErrorF1 = 10
polCal.distortion.gainErrorF2 = 15
polCal.distortion.phaseErrorF1 = 99
polCal.distortion.phaseErrorF2 = 45

imageFormation.polarizationCalibration = polCal

cmplx.imageFormation = imageFormation

### SCPCOA ###

scpcoa = makeScopedCopyableSCPCOA()
scpcoa.scpTime = 123
for i in range(3):
    scpcoa.arpPos[i] = i
    scpcoa.arpVel[i] = i * 2
    scpcoa.arpAcc[i] = i * 3
scpcoa.sideOfTrack = SideOfTrackType('LEFT')
scpcoa.slantRange = 88
scpcoa.groundRange = 77
scpcoa.dopplerConeAngle = 66
scpcoa.grazeAngle = 55
scpcoa.incidenceAngle = 44
scpcoa.twistAngle = 33
scpcoa.slopeAngle = 22
scpcoa.azimAngle = 11
scpcoa.layoverAngle = 14

cmplx.scpcoa = scpcoa

### Radiometric ###

radiometric = makeScopedCopyableRadiometric()
radiometric.noiseLevel.noiseType = 'ABSOLUTE'
radiometric.noiseLevel.noisePoly = Poly2D(3, 3)
radiometric.rcsSFPoly = Poly2D(3, 3)
radiometric.betaZeroSFPoly = Poly2D(3, 3)
radiometric.sigmaZeroSFPoly = Poly2D(3, 3)
radiometric.gammaZeroSFPoly = Poly2D(3, 3)
for i in range(4):
    for j in range(4):
        radiometric.noiseLevel.noisePoly[(i, j)] = i + j
        radiometric.rcsSFPoly[(i, j)] = i + j
        radiometric.betaZeroSFPoly[(i, j)] = i + j
        radiometric.sigmaZeroSFPoly[(i, j)] = i + j
        radiometric.gammaZeroSFPoly[(i, j)] = i + j
radiometric.sigmaZeroSFIncidenceMap = AppliedType('IS_TRUE')
radiometric.gammaZeroSFIncidenceMap = AppliedType('IS_TRUE')

cmplx.radiometric = radiometric

### Antenna ###

antenna = makeScopedCopyableAntenna()

# Tx
antenna.tx = makeScopedCopyableAntennaParameters()
antenna.tx.xAxisPoly = PolyVector3(3)
antenna.tx.yAxisPoly = PolyVector3(3)
for i in range(4):
    for j in range(3):
        antenna.tx.xAxisPoly[i][j] = 10 * i + j
        antenna.tx.yAxisPoly[i][j] = 10 * i + j
antenna.tx.frequencyZero = 97

antenna.tx.electricalBoresight = makeScopedCopyableElectricalBoresight()
antenna.tx.electricalBoresight.dcxPoly = Poly1D(3)
antenna.tx.electricalBoresight.dcyPoly = Poly1D(3)
for i in range(4):
    antenna.tx.electricalBoresight.dcxPoly[i] = 12 * i
    antenna.tx.electricalBoresight.dcyPoly[i] = 34 * i

antenna.tx.halfPowerBeamwidths = makeScopedCopyableHalfPowerBeamwidths()
antenna.tx.halfPowerBeamwidths.dcx = 1885
antenna.tx.halfPowerBeamwidths.dcy = 1889

antenna.tx.array = makeScopedCopyableGainAndPhasePolys()
antenna.tx.array.gainPoly = Poly2D(3, 3)
antenna.tx.array.phasePoly = Poly2D(3, 3)
for i in range(4):
    for j in range(4):
        antenna.tx.array.gainPoly[(i, j)] = i * 10 + j * 5
        antenna.tx.array.phasePoly[(i, j)] = i * 10 + j * 5

antenna.tx.element = makeScopedCopyableGainAndPhasePolys()
antenna.tx.element.gainPoly = Poly2D(3, 3)
antenna.tx.element.phasePoly = Poly2D(3, 3)
for i in range(4):
    for j in range(4):
        antenna.tx.element.gainPoly[(i, j)] = i * 10 + j * 5
        antenna.tx.element.phasePoly[(i, j)] = i * 10 + j * 5

antenna.tx.gainBSPoly = Poly1D(3)
for i in range(4):
    antenna.tx.gainBSPoly[i] = i * 3
antenna.tx.electricalBoresightFrequencyShift = BooleanType('IS_TRUE')
antenna.tx.mainlobeFrequencyDilation = BooleanType('IS_TRUE')

# Rcv
antenna.rcv = makeScopedCopyableAntennaParameters()
antenna.rcv.xAxisPoly = PolyVector3(3)
antenna.rcv.yAxisPoly = PolyVector3(3)
for i in range(4):
    for j in range(3):
        antenna.rcv.xAxisPoly[i][j] = 10 * i + j
        antenna.rcv.yAxisPoly[i][j] = 10 * i + j
antenna.rcv.frequencyZero = 97

antenna.rcv.electricalBoresight = makeScopedCopyableElectricalBoresight()
antenna.rcv.electricalBoresight.dcxPoly = Poly1D(3)
antenna.rcv.electricalBoresight.dcyPoly = Poly1D(3)
for i in range(4):
    antenna.rcv.electricalBoresight.dcxPoly[i] = 12 * i
    antenna.rcv.electricalBoresight.dcyPoly[i] = 34 * i

antenna.rcv.halfPowerBeamwidths = makeScopedCopyableHalfPowerBeamwidths()
antenna.rcv.halfPowerBeamwidths.dcx = 1885
antenna.rcv.halfPowerBeamwidths.dcy = 1889

antenna.rcv.array = makeScopedCopyableGainAndPhasePolys()
antenna.rcv.array.gainPoly = Poly2D(3, 3)
antenna.rcv.array.phasePoly = Poly2D(3, 3)
for i in range(4):
    for j in range(4):
        antenna.rcv.array.gainPoly[(i, j)] = i * 10 + j * 5
        antenna.rcv.array.phasePoly[(i, j)] = i * 10 + j * 5

antenna.rcv.element = makeScopedCopyableGainAndPhasePolys()
antenna.rcv.element.gainPoly = Poly2D(3, 3)
antenna.rcv.element.phasePoly = Poly2D(3, 3)
for i in range(4):
    for j in range(4):
        antenna.rcv.element.gainPoly[(i, j)] = i * 10 + j * 5
        antenna.rcv.element.phasePoly[(i, j)] = i * 10 + j * 5

antenna.rcv.gainBSPoly = Poly1D(3)
for i in range(4):
    antenna.rcv.gainBSPoly[i] = i * 3
antenna.rcv.electricalBoresightFrequencyShift = BooleanType('IS_TRUE')
antenna.rcv.mainlobeFrequencyDilation = BooleanType('IS_TRUE')

# twoWay
antenna.twoWay = makeScopedCopyableAntennaParameters()
antenna.twoWay.xAxisPoly = PolyVector3(3)
antenna.twoWay.yAxisPoly = PolyVector3(3)
for i in range(4):
    for j in range(3):
        antenna.twoWay.xAxisPoly[i][j] = 10 * i + j
        antenna.twoWay.yAxisPoly[i][j] = 10 * i + j
antenna.twoWay.frequencyZero = 97

antenna.twoWay.electricalBoresight = makeScopedCopyableElectricalBoresight()
antenna.twoWay.electricalBoresight.dcxPoly = Poly1D(3)
antenna.twoWay.electricalBoresight.dcyPoly = Poly1D(3)
for i in range(4):
    antenna.twoWay.electricalBoresight.dcxPoly[i] = 12 * i
    antenna.twoWay.electricalBoresight.dcyPoly[i] = 34 * i

antenna.twoWay.halfPowerBeamwidths = makeScopedCopyableHalfPowerBeamwidths()
antenna.twoWay.halfPowerBeamwidths.dcx = 1885
antenna.twoWay.halfPowerBeamwidths.dcy = 1889

antenna.twoWay.array = makeScopedCopyableGainAndPhasePolys()
antenna.twoWay.array.gainPoly = Poly2D(3, 3)
antenna.twoWay.array.phasePoly = Poly2D(3, 3)
for i in range(4):
    for j in range(4):
        antenna.twoWay.array.gainPoly[(i, j)] = i * 10 + j * 5
        antenna.twoWay.array.phasePoly[(i, j)] = i * 10 + j * 5

antenna.twoWay.element = makeScopedCopyableGainAndPhasePolys()
antenna.twoWay.element.gainPoly = Poly2D(3, 3)
antenna.twoWay.element.phasePoly = Poly2D(3, 3)
for i in range(4):
    for j in range(4):
        antenna.twoWay.element.gainPoly[(i, j)] = i * 10 + j * 5
        antenna.twoWay.element.phasePoly[(i, j)] = i * 10 + j * 5

antenna.twoWay.gainBSPoly = Poly1D(3)
for i in range(4):
    antenna.twoWay.gainBSPoly[i] = i * 3
antenna.twoWay.electricalBoresightFrequencyShift = BooleanType('IS_TRUE')
antenna.twoWay.mainlobeFrequencyDilation = BooleanType('IS_TRUE')

cmplx.antenna = antenna

### Error Statistics ###
errorStats = makeScopedCopyableErrorStatistics()
errorStats.compositeSCP = makeScopedCopyableCompositeSCP()
errorStats.compositeSCP.xErr = 12
errorStats.compositeSCP.yErr = 34
errorStats.compositeSCP.xyErr = 56

errorStats.components = makeScopedCopyableComponents()

errorStats.components.posVelError = makeScopedCopyablePosVelError()
errorStats.components.posVelError.frame = FrameType('ECF')

errorStats.components.posVelError.p1 = 10
errorStats.components.posVelError.p2 = 20
errorStats.components.posVelError.p3 = 30
errorStats.components.posVelError.v1 = 40
errorStats.components.posVelError.v2 = 50
errorStats.components.posVelError.v3 = 60

errorStats.components.posVelError.corrCoefs = makeScopedCopyableCorrCoefs()
errorStats.components.posVelError.corrCoefs.p1p2 = 1
errorStats.components.posVelError.corrCoefs.p1p3 = 2
errorStats.components.posVelError.corrCoefs.p1v1 = 3
errorStats.components.posVelError.corrCoefs.p1v2 = 4
errorStats.components.posVelError.corrCoefs.p1v3 = 5
errorStats.components.posVelError.corrCoefs.p2p3 = 6
errorStats.components.posVelError.corrCoefs.p2v1 = 7
errorStats.components.posVelError.corrCoefs.p2v2 = 8
errorStats.components.posVelError.corrCoefs.p2v3 = 9
errorStats.components.posVelError.corrCoefs.p3v1 = 10
errorStats.components.posVelError.corrCoefs.p3v2 = 11
errorStats.components.posVelError.corrCoefs.p3v3 = 12
errorStats.components.posVelError.corrCoefs.v1v2 = 13
errorStats.components.posVelError.corrCoefs.v1v3 = 14
errorStats.components.posVelError.corrCoefs.v2v3 = 15

errorStats.components.posVelError.positionDecorr = DecorrType(20, 30)

errorStats.components.radarSensor = makeScopedCopyableRadarSensor()
errorStats.components.radarSensor.rangeBias = 100
errorStats.components.radarSensor.clockFreqSF = 200
errorStats.components.radarSensor.transmitFreqSF = 300
errorStats.components.radarSensor.rangeBiasDecorr = DecorrType(40, 50)

errorStats.components.tropoError = makeScopedCopyableTropoError()
errorStats.components.tropoError.tropoRangeVertical = 1000
errorStats.components.tropoError.tropoRangeSlant = 2000
errorStats.components.tropoError.tropoRangeDecorr = DecorrType(1, 2)

errorStats.components.ionoError = makeScopedCopyableIonoError()
errorStats.components.ionoError.ionoRangeVertical = 3
errorStats.components.ionoError.ionoRangeRateVertical = 4
errorStats.components.ionoError.ionoRgRgRateCC = 5
errorStats.components.ionoError.ionoRangeVertDecorr = DecorrType(6, 7)

param.setName('Some error param')
param.setValue('Some error value')
errorStats.additionalParameters.push_back(param)

cmplx.errorStatistics = errorStats

### Match Information ###
matchInfo = makeScopedCopyableMatchInformation()

matchType = makeScopedCopyableMatchType()
matchType.collectorName = 'Collector'
matchType.illuminatorName = 'Illuminator'
matchType.matchType.push_back('Some match type')
matchType.typeID = 'COHERENT'
matchType.currentIndex = 9

matchCollect = MatchCollect()
matchCollect.coreName = 'Some name'
matchCollect.matchIndex = 13

param.setName('Match param')
param.setValue('Match val')
matchCollect.parameters.push_back(param)

# MatchType's constructor (perhaps unintuitively) sizes this vector to one
# right off the bat since it's mandatory
matchType.matchCollects[0] = matchCollect

# MatchInformation's constructor (perhaps unintuitively) sizes this vector
# to one right off the bat since it's mandatory
matchInfo.types[0] = matchType

cmplx.matchInformation = matchInfo

### PFA ###

pfa = makeScopedCopyablePFA()
for i in range(3):
    pfa.focusPlaneNormal[i] = i * 2
    pfa.imagePlaneNormal[i] = i * 3
pfa.polarAngleRefTime = 12

pfa.polarAnglePoly = Poly1D(3)
pfa.spatialFrequencyScaleFactorPoly = Poly1D(3)
for i in range(4):
    pfa.polarAnglePoly[i] = i
    pfa.spatialFrequencyScaleFactorPoly[i] = i * i

pfa.krg1 = 1189
pfa.krg2 = 1131
pfa.kaz1 = 120779
pfa.kaz2 = 12681
pfa.slowTimeDeskew = makeScopedCopyableSlowTimeDeskew()
pfa.slowTimeDeskew.applied = BooleanType('IS_TRUE')
pfa.slowTimeDeskew.slowTimeDeskewPhasePoly = Poly2D(3, 3)
for i in range(4):
    for j in range(4):
        pfa.slowTimeDeskew.slowTimeDeskewPhasePoly[(i, j)] = 44 * i + j

cmplx.pfa = pfa

### RMA ###

rma = makeScopedCopyableRMA()
rma.algoType = RMAlgoType('OMEGA_K')
rma.rmat = makeScopedCopyableRMAT()
rma.rmat.refTime = 12
for i in range(3):
    rma.rmat.refPos[i] = i
    rma.rmat.refVel[i] = i + 1
rma.rmat.distRefLinePoly = Poly1D(3)
for i in range(4):
    rma.rmat.distRefLinePoly[i] = i * 5
rma.rmat.cosDCACOAPoly = Poly2D(3, 3)
for i in range(4):
    for j in range(4):
        rma.rmat.cosDCACOAPoly[(i, j)] = i + j
rma.rmat.kx1 = 1
rma.rmat.kx2 = 2
rma.rmat.ky1 = 3
rma.rmat.ky2 = 4
rma.rmat.dopConeAngleRef = 5

# Just make this one anyway to make sure it works
rmcr = makeScopedCopyableRMCR()
for i in range(3):
    rmcr.refPos[i] = i
    rmcr.refVel[i] = i * 2
rmcr.dopConeAngleRef = 9

# Just make this one anyway to make sure it works
inca = makeScopedCopyableINCA()
inca.timeCAPoly = Poly1D(3)
for i in range(3):
    inca.timeCAPoly[i] = i
inca.rangeCA = 5
inca.freqZero = 10
inca.dopplerRateScaleFactorPoly = Poly2D(3, 3)
inca.dopplerCentroidPoly = Poly2D(3, 3)
for i in range(4):
    for j in range(4):
        inca.dopplerRateScaleFactorPoly[(i, j)] = 9 * i
        inca.dopplerCentroidPoly[(i, j)] = 10 * i
inca.dopplerCentroidCOA = BooleanType('IS_TRUE')

# Don't actually assign RMA to cmplx since we're using PFA

### RgAzComp ###

rgAzComp = makeScopedCopyableRgAzComp()
rgAzComp.azSF = 123
rgAzComp.kazPoly = Poly1D(3)
for i in range(4):
    rgAzComp.kazPoly[i] = i * 100

# Don't actually assign RgAzComp to cmplx since we're using PFA

### Now format it as XML and write it out to a file ###
vs = VectorString()
vs.push_back(os.environ['SIX_SCHEMA_PATH'])
  
xml_ctrl = ComplexXMLControl()
origPathname = 'test_create_sicd.xml'
fos = FileOutputStream(origPathname)
out_doc = xml_ctrl.toXML(cmplx, vs)
root = out_doc.getRootElement()
root.prettyPrint(fos)
fos.close()

# If we made it to here, all the Python bindings must be present and what we
# wrote out must have passed schema validation
# We can't tell if for some reason some of the XML just didn't get written
# out though

### Now read it back in again ###

fis = FileInputStream(origPathname)
xmlparser = MinidomParser()
xmlparser.preserveCharacterData(True)
xmlparser.parse(fis)
in_doc = xmlparser.getDocument()

data = xml_ctrl.fromXML(in_doc, vs) 
cmplxReadBackIn = asComplexData(data)

# And then write it out one more time #
newPathname = 'test_create_sicd_rt.xml'
fos = FileOutputStream(newPathname)
out_doc = xml_ctrl.toXML(cmplxReadBackIn, vs)
root = out_doc.getRootElement()
root.prettyPrint(fos)

# These should match #
if filecmp.cmp(origPathname, newPathname):
    print 'Round trip succeeded!'
else:
    print 'Round trip failed'

# If we made it to here, the read side appears to be working properly too
