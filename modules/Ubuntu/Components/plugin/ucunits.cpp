/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#include "ucunits.h"

#include <QtQml/QQmlContext>
#include <QtQml/QQmlFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#define ENV_GRID_UNIT_PX "GRID_UNIT_PX"
#define DEFAULT_GRID_UNIT_PX 8
#define DEFAULT_RESOURCES_UNIT_PX 8

static float getenvFloat(const char* name, float defaultValue)
{
    QByteArray stringValue = qgetenv(name);
    bool ok;
    float value = stringValue.toFloat(&ok);
    return ok ? value : defaultValue;
}


/*!
    \qmltype Units
    \instantiates UCUnits
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup resolution-independence
    \brief Units of measurement for sizes, spacing, margin, etc.

    Units provides facilities for measuring UI elements in a variety
    of units other than just pixels.

    A global instance of Units is exposed as the \b{units} context property.
    Example usage:

    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1

    Item {
        width: units.gu(2)
        height: units.gu(5)
    }
    \endqml

    \sa {Resolution Independence}
*/
UCUnits::UCUnits(QObject *parent) :
    QObject(parent)
{
    m_gridUnit = getenvFloat(ENV_GRID_UNIT_PX, DEFAULT_GRID_UNIT_PX);
    m_resourcesUnit = DEFAULT_RESOURCES_UNIT_PX;
}

/* A file named fileName can be provided by the application
 * and contains the grid unit the resources have been designed to. */
bool UCUnits::loadResourcesUnitFile(const QUrl& baseUrl, QString fileName)
{
    QUrl unresolved = QUrl::fromLocalFile(fileName);
    fileName = baseUrl.resolved(unresolved).toLocalFile();

    QFile resourcesUnitFile(fileName);
    if (resourcesUnitFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        bool valid;
        int unit = resourcesUnitFile.readLine().trimmed().toInt(&valid);
        if (valid) {
            m_resourcesUnit = (float)unit;
        }
        return valid;
    }
    return false;
}

/*!
    \qmlproperty real Units::gridUnit

    The number of pixels 1 grid unit corresponds to.
*/
float UCUnits::gridUnit()
{
    return m_gridUnit;
}

void UCUnits::setGridUnit(float gridUnit)
{
    m_gridUnit = gridUnit;
    Q_EMIT gridUnitChanged();
}

/*!
    \qmlmethod real Units::dp(real value)

    Returns the number of pixels \a value density independent pixels correspond to.
*/
float UCUnits::dp(float value)
{
    return qRound(value * m_gridUnit / DEFAULT_GRID_UNIT_PX);
}

/*!
    \qmlmethod real Units::gu(real value)

    Returns the number of pixels \a value grid units correspond to.
*/
float UCUnits::gu(float value)
{
    return qRound(value * m_gridUnit);
}

QString UCUnits::resolveResource(const QUrl& url)
{
    if (url.isEmpty()) {
        return "";
    }

    QString path = QQmlFile::urlToLocalFileOrQrc(url);

    if (path.isEmpty()) {
        return "";
    }

    QFileInfo fileInfo(path);
    if (fileInfo.exists() && !fileInfo.isFile()) {
        return "";
    }

    QString prefix = fileInfo.dir().absolutePath() + QDir::separator() + fileInfo.baseName();
    QString suffix = "." + fileInfo.completeSuffix();

    path = prefix + suffixForGridUnit(m_gridUnit) + suffix;
    float scaleFactor = m_gridUnit / m_resourcesUnit;

    if (QFile::exists(path)) {
        return QString("1") + "/" + path;
    }

    path = prefix + suffix;
    if (QFile::exists(path)) {
        return QString::number(scaleFactor) + "/" + path;
    }

    return "";
}

QString UCUnits::suffixForGridUnit(float gridUnit)
{
    return "@" + QString::number(gridUnit);
}
