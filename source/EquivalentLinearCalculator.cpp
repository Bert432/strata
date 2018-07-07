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

#include "EquivalentLinearCalculator.h"

#include "SoilProfile.h"
#include "SubLayer.h"
#include "Units.h"

EquivalentLinearCalculator::EquivalentLinearCalculator(QObject *parent)
    : AbstractIterativeCalculator(parent)
{
    _strainRatio = 0.65;
}

QString EquivalentLinearCalculator::toHtml() const
{
    return tr(
            "<li>Equivalent Linear Parameters"
            "<table border=\"0\">"
            "<tr><th>Effective strain ratio:</th><td>%1 Hz</td></tr>"
            "<tr><th>Error tolerance:</th><td>%2</td></tr>"
            "<tr><th>Maximum number of iterations:</th><td>%3</td></tr>"
            "</table>"
            "</li>"
            )
            .arg(_strainRatio)
            .arg(_errorTolerance)
            .arg(_maxIterations);
}

double EquivalentLinearCalculator::strainRatio() const
{
    return _strainRatio;
}

void EquivalentLinearCalculator::setStrainRatio(double strainRatio)
{
    if (_strainRatio != strainRatio) {
        _strainRatio = strainRatio;

        emit strainRatioChanged(_strainRatio);
        emit wasModified();
    }
}

bool EquivalentLinearCalculator::updateSubLayer(
        int index,
        const QVector<std::complex<double> > &strainTf)
{
    const double strainMax = 100 * _motion->calcMaxStrain(strainTf);

    if (strainMax <= 0)
        return false;

    _site->subLayers()[index].setStrain(_strainRatio * strainMax, strainMax);

    // Compute the complex shear modulus and complex shear-wave velocity
    // for each soil layer -- these change because the damping and shear
    // modulus change.
    _shearMod[index].fill(calcCompShearMod(
            _site->subLayers().at(index).shearMod(),
            _site->subLayers().at(index).damping() / 100.));

    return true;
}

void EquivalentLinearCalculator::estimateInitialStrains()
{
    // Estimate the intial strain from the ratio of peak ground velocity of the
    //  motion and the shear-wave velocity of the layer.
    double estimatedStrain = 0;
    for (int i = 0; i < _nsl; ++i) {
        estimatedStrain = _motion->pgv() / _site->subLayers().at(i).shearVel();
        _site->subLayers()[i].setInitialStrain(estimatedStrain);
    }

    // Compute the complex shear modulus and complex shear-wave velocity for
    // each soil layer -- initially this is assumed to be frequency independent
    for (int i = 0; i < _nsl; ++i ) {
        _shearMod[i].fill(calcCompShearMod(
                _site->shearMod(i), _site->damping(i) / 100.));
    }
}

void EquivalentLinearCalculator::fromJson(const QJsonObject &json)
{
    AbstractIterativeCalculator::fromJson(json);
    _strainRatio = json["strainRatio"].toDouble();
}

QJsonObject EquivalentLinearCalculator::toJson() const
{
    QJsonObject json = AbstractIterativeCalculator ::toJson();
    json["strainRatio"] = _strainRatio;
    return json;
}

QDataStream & operator<< (QDataStream & out,
                                 const EquivalentLinearCalculator* elc)
{
    out << (quint8)1;
    out << qobject_cast<const AbstractIterativeCalculator*>(elc)
            << elc->_strainRatio;

    return out;
}

QDataStream & operator>> (QDataStream & in,
                                 EquivalentLinearCalculator* elc)
{
    quint8 ver;
    in >> ver;

    in >> qobject_cast<AbstractIterativeCalculator*>(elc);
    in >> elc->_strainRatio;

    return in;
}
