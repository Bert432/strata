////////////////////////////////////////////////////////////////////////////////
//
// This file is part of Strata.
//
// Strata is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// Strata is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// Strata.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2010-2018 Albert Kottke
//
////////////////////////////////////////////////////////////////////////////////

#include "SourceTheoryRvtMotion.h"

#include "CrustalAmplification.h"
#include "Dimension.h"
#include "PathDurationModel.h"

#include <QDebug>

SourceTheoryRvtMotion::SourceTheoryRvtMotion(QObject * parent)
        : AbstractRvtMotion(parent)
{
    _freq = new Dimension(this);
    _freq->setMin(0.05);
    _freq->setMax(50);
    _freq->setSize(1024);
    _freq->setSpacing(Dimension::Log);

    _fourierAcc = QVector<double>(freqCount(), 0.);

    _crustalAmp = new CrustalAmplification;
    connect(_crustalAmp, SIGNAL(wasModified()), this, SIGNAL(wasModified()));
    connect(this, SIGNAL(regionChanged(int)), _crustalAmp, SLOT(setRegion(int)));

    _pathDuration = new PathDurationModel;
    connect(_pathDuration, SIGNAL(wasModified()), this, SIGNAL(wasModified()));
    connect(this, SIGNAL(regionChanged(int)), _pathDuration, SLOT(setRegion(int)));

    // Reset the parameters
    setRegion(_region);
    setMagnitude(_magnitude);
    setDistance(_distance);
    setDepth(8.);

    _name = tr("Source Theory RVT Motion (M $mag @ $dist km)");
}

SourceTheoryRvtMotion::~SourceTheoryRvtMotion()
{
    _crustalAmp->deleteLater();
    _pathDuration->deleteLater();
}

const QVector<double> & SourceTheoryRvtMotion::freq() const
{
    return _freq->data();
}

Dimension*  SourceTheoryRvtMotion::freqDimension()
{
    return _freq;
}

QString SourceTheoryRvtMotion::toHtml() const
{
    //FIXME
    return QString();
}

void SourceTheoryRvtMotion::setRegion(AbstractRvtMotion::Region region)
{
    AbstractRvtMotion::setRegion(region);

    _crustalAmp->setRegion(_region);
    _pathDuration->setRegion(_region);

    switch (_region) {
        case AbstractRvtMotion::WUS:
            setStressDrop(100);
            setPathAttenCoeff(180);
            setPathAttenPower(0.45);
            setShearVelocity(3.5);
            setDensity(2.8);
            setSiteAtten(0.04);
            break;
        case AbstractRvtMotion::CEUS:
            setStressDrop(150);
            setPathAttenCoeff(680);
            setPathAttenPower(0.36);
            setShearVelocity(3.6);
            setDensity(2.8);
            setSiteAtten(0.006);
            break;
        default:
            break;
    }

    // Geometric attenuation may have changed
    calcGeoAtten();
}


//QString SourceTheoryRvtMotion::toHtml() const
//{
//    QString html = QString(tr(
//                "<tr><th>Location:</th><td>%1</td></tr>"
//                "<tr><th>Moment Magnitude:</th><td>%2</td></tr>"
//                "<tr><th>Distance:</th><td>%3 km</td></tr>"
//                "<tr><th>Depth:</th><td>%4 km </td></tr>"
//                "<tr><th>Stress Drop:</th><td>%5 bars</td></tr>"
//                "<tr><th>Geometric Attenuation:</th><td>%6</td></tr>"
//                "<tr><th>Path Atten Coeff.:</th><td>%7</td></tr>"
//                "<tr><th>Path Atten Power.:</th><td>%8</td></tr>"
//                "<tr><th>Shear-wave velocity:</th><td>%9 km/s</td></tr>"
//                "<tr><th>Density:</th><td>%10 gm/cm%11</td></tr>"
//                "<tr><th>Site Attenuation (kappa):</th><td>%12</td></tr>"
//                "<tr><th>Generic Crustal Amplication:</th><td>%13</td></tr>"
//                "</table>"
//               ))
//        .arg(locationList().at(_location))
//        .arg(_momentMag)
//        .arg(_distance)
//        .arg(_depth)
//        .arg(_stressDrop)
//        .arg(_geoAtten)
//        .arg(_pathAttenCoeff)
//        .arg(_pathAttenPower)
//        .arg(_shearVelocity)
//        .arg(_density).arg(QChar(0x00B3))
//        .arg(_CrustalAtten)
//        .arg(_siteSpecificCrustalAmp ? tr("yes") : tr("no"));
//
//    html += "<table><tr>";
//
//    if (_siteSpecificCrustalAmp) {
//        // Add the velocity profile
//        html += "<td><h4>Velocity Profile</h4><table border = \"1\">";
//        html += QString("<tr><th>Thickness (km)</th><th>Shear Velocity (km/s)</th><th>Density (gm/cm%1)</th></tr>").arg(QChar(0x00B3));
//
//        for (int i = 0; i < _crustThickness.size(); ++i) {
//            html += QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>")
//                .arg(_crustThickness.at(i))
//                .arg(_crustVelocity.at(i))
//                .arg(_crustDensity.at(i));
//        }
//
//        html += "</table></td>";
//
//        // Add the crustal amplification
//        // FIXME
////        if (_crustAmpNeedsUpdate) {
////            calcCrustalAmp();
////        }
//    }
//
//    html += "<td><h4>Crustal Amplification</h4><table border = \"1\">";
//    html += "<tr><th>Frequency (Hz)</th><th>Amplification</th></tr>";
//
//    for (int i = 0; i < _freq.size(); ++i) {
//        html += QString("<tr><td>%1</td><td>%2</td></tr>")
//            .arg(freqAt(i))
//            .arg(_crustAmp.at(i));
//    }
//    html += "</table></td></tr></table>";
//
//    return html;
//}

bool SourceTheoryRvtMotion::isCustomized() const {return _isCustomized;}

void SourceTheoryRvtMotion::setIsCustomized(bool b) {
    if (_isCustomized != b) {
        _isCustomized = b;
        emit isCustomizedChanged(b);
        emit wasModified();
    }
}

void SourceTheoryRvtMotion::setMagnitude(double magnitude)
{
    AbstractRvtMotion::setMagnitude(magnitude);
    // Compute seismic moment based on the moment magnitude
    _seismicMoment = pow(10, 1.5 * (_magnitude + 10.7));
    calcCornerFreq();
}


void SourceTheoryRvtMotion::setDistance(double distance)
{
    AbstractRvtMotion::setDistance(distance);
    calcHypoDistance();
}

double SourceTheoryRvtMotion::depth() const
{
    return _depth;
}

void SourceTheoryRvtMotion::setDepth(double depth)
{
    _depth = depth;

    calcHypoDistance();
}

double SourceTheoryRvtMotion::stressDrop() const
{
    return _stressDrop;
}

void SourceTheoryRvtMotion::setStressDrop(double stressDrop)
{
    if (_stressDrop != stressDrop) {
        _stressDrop = stressDrop;

        emit stressDropChanged(stressDrop);
        calcCornerFreq();
    }
}

double SourceTheoryRvtMotion::geoAtten() const
{
    return _geoAtten;
}

void SourceTheoryRvtMotion::setGeoAtten(double geoAtten)
{
    if (_geoAtten != geoAtten) {
        _geoAtten = geoAtten;

        emit geoAttenChanged(geoAtten);
    }
}

double SourceTheoryRvtMotion::pathAttenCoeff() const
{
    return _pathAttenCoeff;
}

void SourceTheoryRvtMotion::setPathAttenCoeff(double pathAttenCoeff)
{
    if (_pathAttenCoeff != pathAttenCoeff) {
        _pathAttenCoeff = pathAttenCoeff;

        emit pathAttenCoeffChanged(pathAttenCoeff);
    }
}

double SourceTheoryRvtMotion::pathAttenPower() const
{
    return _pathAttenPower;
}

void SourceTheoryRvtMotion::setPathAttenPower(double pathAttenPower)
{
    if (_pathAttenPower != pathAttenPower) {
        _pathAttenPower = pathAttenPower;

        emit pathAttenPowerChanged(pathAttenPower);
    }
}

double SourceTheoryRvtMotion::shearVelocity() const
{
    return _shearVelocity;
}

void SourceTheoryRvtMotion::setShearVelocity(double shearVelocity)
{
    if (_shearVelocity != shearVelocity) {
        _shearVelocity = shearVelocity;

        emit shearVelocityChanged(shearVelocity);
        calcCornerFreq();
    }
}

double SourceTheoryRvtMotion::density() const
{
    return _density;
}

void SourceTheoryRvtMotion::setDensity(double density)
{
    if (_density != density) {
        _density = density;

        emit densityChanged(density);
    }
}

double SourceTheoryRvtMotion::siteAtten() const
{
    return _siteAtten;
}

double SourceTheoryRvtMotion::duration() const
{
    return _duration;
}

void SourceTheoryRvtMotion::setSiteAtten(double siteAtten)
{
    if (_siteAtten != siteAtten) {
        _siteAtten = siteAtten;

        emit siteAttenChanged(siteAtten);
    }
}

CrustalAmplification* SourceTheoryRvtMotion::crustalAmp()
{
    return _crustalAmp;
}

PathDurationModel* SourceTheoryRvtMotion::pathDuration()
{
    return _pathDuration;
}

void SourceTheoryRvtMotion::calcHypoDistance()
{
    if (_depth > 0 && _distance > 0) {
        _hypoDistance = sqrt(_depth * _depth + _distance * _distance);

        calcDuration();
        calcGeoAtten();
    }
}

void SourceTheoryRvtMotion::calcCornerFreq()
{
    if (_shearVelocity > 0 && _stressDrop > 0 && _seismicMoment > 0) {
        _cornerFreq = 4.9e6 * _shearVelocity * pow(_stressDrop/_seismicMoment, 1./3.);

        calcDuration();
    }
}

void SourceTheoryRvtMotion::calcDuration()
{
    if (_cornerFreq > 0) {
        // Compute source component
        double sourceDur = 1 / _cornerFreq;
        // Path duration
        double pathDur = _pathDuration->duration(_hypoDistance);

        _duration = sourceDur + pathDur;
        emit durationChanged(_duration);
    }
}

void SourceTheoryRvtMotion::calcGeoAtten()
{
    if (_hypoDistance > 0) {
        // Determine the geometric attenuation based on a piecewise linear
        // calculation
        switch (_region) {
            case AbstractRvtMotion::WUS:
                if (_hypoDistance < 40.) {
                    _geoAtten = 1. / _hypoDistance;
                } else {
                    _geoAtten = 1./40. * sqrt(40./_hypoDistance);
                }
                break;
            case AbstractRvtMotion::CEUS:
                if (_hypoDistance < 70.) {
                    _geoAtten = 1. / _hypoDistance;
                } else if (_hypoDistance < 130.) {
                    _geoAtten = 1. / 70.;
                } else {
                    _geoAtten = 1./70. * sqrt(130./_hypoDistance);
                }
                break;
            default:
                break;
        }

        emit geoAttenChanged(_geoAtten);
    }
}


void SourceTheoryRvtMotion::calculate()
{
    // Conversion factor to convert from dyne-cm into gravity-sec
    const double conv = 1e-20 / 981;

    // Constant term for the model component
    const double C = (0.55 * 2) / (M_SQRT2 * 4 * M_PI * _density * pow(_shearVelocity, 3));

    for (int i = 0; i < _fourierAcc.size(); ++i) {
        // Model component
        const double sourceComp =  C * _seismicMoment / (1 + pow(freqAt(i)/_cornerFreq, 2));

        // Path component
        const double pathAtten = _pathAttenCoeff * pow(freqAt(i), _pathAttenPower);
        const double pathComp = _geoAtten *
                                exp((-M_PI * freqAt(i) * _hypoDistance) / (pathAtten * _shearVelocity));

        // Site component
        const double siteAmp = _crustalAmp->interpAmpAt(freqAt(i));
        const double siteDim = exp(-M_PI * _siteAtten * freqAt(i));
        const double siteComp = siteAmp * siteDim;

        // Combine the three components and convert from displacement to
        // acceleleration
        _fourierAcc[i] = conv * pow(2 * M_PI * freqAt(i), 2) * sourceComp * pathComp * siteComp;
    }

    dataChanged(index(0, AmplitudeColumn), index(_fourierAcc.size(), AmplitudeColumn));

    AbstractRvtMotion::calculate();
}

void SourceTheoryRvtMotion::fromJson(const QJsonObject &json)
{
    AbstractRvtMotion::fromJson(json);

    if (_isCustomized) {
        setDepth(json["depth"].toDouble());
        setStressDrop(json["stressDrop"].toDouble());
        setGeoAtten(json["geoAtten"].toDouble());
        setPathAttenCoeff(json["pathAttenCoeff"].toDouble());
        setPathAttenPower(json["pathAttenPower"].toDouble());
        setShearVelocity(json["shearVelocity"].toDouble());
        setDensity(json["density"].toDouble());
        setSiteAtten(json["siteAtten"].toDouble());

        _crustalAmp->fromJson(json["crustalAmp"].toObject());
        _pathDuration->fromJson(json["pathDuration"].toObject());
    } else {
        setRegion(_region);
    }
    calculate();
}

QJsonObject SourceTheoryRvtMotion::toJson() const
{
    QJsonObject json = AbstractRvtMotion::toJson();
    json["depth"] = _depth;
    json["freq"] = _freq->toJson();

    if (_isCustomized) {
        json["stresDrop"] = _stressDrop;
        json["geoAtten"] = _geoAtten;
        json["pathAttenCoeff"] = _pathAttenCoeff;
        json["pathAttenPower"] = _pathAttenPower;
        json["shearVelocity"] = _shearVelocity;
        json["density"] = _density;
        json["siteAtten"] = _siteAtten;
        json["crustalAmp"] = _crustalAmp->toJson();
        json["pathDuration"] = _pathDuration->toJson();
    }
    return json;
}



QDataStream & operator<< (QDataStream & out, const SourceTheoryRvtMotion* strm)
{
    out << (quint8)3;
    out << qobject_cast<const AbstractRvtMotion*>(strm);
    // Properties of SourceTheoryRvtMotion
    out << strm->_depth << strm->_freq;
    if (strm->_isCustomized) {
        out << strm->_stressDrop
            << strm->_geoAtten
            << strm->_pathAttenCoeff
            << strm->_pathAttenPower
            << strm->_shearVelocity
            << strm->_density
            << strm->_siteAtten
            << strm->_crustalAmp
            << strm->_pathDuration;
    }
    return out;
}

QDataStream & operator>> (QDataStream & in, SourceTheoryRvtMotion* strm)
{
    quint8 ver;
    in >> ver;

    in >> qobject_cast<AbstractRvtMotion*>(strm);

    // Properties of SourceTheoryRvtMotion
    double depth;

    in >> depth
       >> strm->_freq;

    strm->setDepth(depth);

    if (strm->_isCustomized) {
        double stressDrop;
        double geoAtten;
        double pathDurCoeff;
        double pathAttenCoeff;
        double pathAttenPower;
        double shearVelocity;
        double density;
        double siteAtten;

        in >> stressDrop
           >> geoAtten
           >> pathDurCoeff
           >> pathAttenCoeff
           >> pathAttenPower
           >> shearVelocity
           >> density
           >> siteAtten
           >> strm->_crustalAmp
           >> strm->_pathDuration;

        // FIXME: Move to one function
        // Use set methods to calculate dependent parameters
        strm->setStressDrop(stressDrop);
        strm->setGeoAtten(geoAtten);
        strm->setPathAttenCoeff(pathAttenCoeff);
        strm->setPathAttenPower(pathAttenPower);
        strm->setShearVelocity(shearVelocity);
        strm->setDensity(density);
        strm->setSiteAtten(siteAtten);
    }

    strm->setRegion(strm->_region);
    strm->setMagnitude(strm->_magnitude);
    strm->setDistance(strm->_distance);
    // Compute the FAS
    strm->calculate();
    return in;

}
