/*
 * Copyright 2012-2016 Canonical Ltd.
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

#include "ucunits_p.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QRegularExpression>
#include <QtCore/QtMath>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QtGui/qpa/qplatformwindow.h>
#include <QtGui/qpa/qplatformscreen.h>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlFile>

#define ENV_GRID_UNIT_PX "GRID_UNIT_PX"
#define DEFAULT_GRID_UNIT_PX 8

UT_NAMESPACE_BEGIN

static float getenvFloat(const char* name, float defaultValue)
{
    QByteArray stringValue = qgetenv(name);
    bool ok;
    float value = stringValue.toFloat(&ok);
    return ok ? value : defaultValue;
}


/*!
    \qmltype Units
    \inqmlmodule Ubuntu.Components
    \ingroup resolution-independence
    \brief Units of measurement for sizes, spacing, margin, etc.

    Units provides facilities for measuring UI elements in a variety
    of units other than just pixels.

    A global instance of Units is exposed as the \b{units} context property.
    Example usage:

    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.2

    Item {
        width: units.gu(2)
        height: units.gu(5)
    }
    \endqml

    \sa {Resolution Independence}
*/

/*
 * In Qt5.6 the following variables affect scaling in QWidget and QML:
 *   1. QT_SCALE_FACTOR: global integer scale factor, including point-sized fonts
 *   2. QT_SCREEN_SCALE_FACTORS: a list of scale factors
 *   3. QT_AUTO_SCREEN_SCALE_FACTOR: enables automatic scaling heuristics
 *
 * In Qt5.4 there is a single means to scale the UI: the QT_DEVICE_PIXEL_RATIO environment
 * variable. This accepts only integer values, thus allowing a x2 or x3 scaling of any
 * Qt-based UI, that includes QWidget as well as any QML UI.
 *
 * The Ubuntu UI Toolkit has solved the scaling problem with the GRID_UNIT_PX variable.
 * It offers more flexibility, but only scales QML applications written to use the UITK
 * (since it uses this Units class) as it is built on top of QML.
 *
 * There are additional areas in Qt where QT_DEVICE_PIXEL_RATIO causes correct scaling which
 * GRID_UNIT_PX cannot, for example:
 *   1. cacheBuffer for ListView/GridViews - specified in density-independent pixels
 *   2. gesture recognition  matches what is on screen better, as it is density-independent
 *      pixel aware
 *
 * In order to get the best of both worlds, Ubuntu will set both GRID_UNIT_PX and
 * QT_DEVICE_PIXEL_RATIO. Thus all Qt apps will scale reasonably well, with UITK-based apps
 * scaling perfectly for any desired scale (i.e. non-integer scales).
 *
 * However UITK developers can just use this Units class as usual, and will be almost totally
 * isolated from Qt's own scaling concept.
 */

UCUnits *UCUnits::m_units = nullptr;

UCUnits::UCUnits(QObject *parent) :
    QObject(parent),
    m_devicePixelRatio(qGuiApp->devicePixelRatio())
{
    // If GRID_UNIT_PX set, always use it. If not, 1GU := DEFAULT_GRID_UNIT_PX * m_devicePixelRatio
    if (qEnvironmentVariableIsSet(ENV_GRID_UNIT_PX)) {
        m_gridUnit = getenvFloat(ENV_GRID_UNIT_PX, DEFAULT_GRID_UNIT_PX);
    } else {
        m_gridUnit = DEFAULT_GRID_UNIT_PX * m_devicePixelRatio;
    }

    auto nativeInterface = qGuiApp->platformNativeInterface();
    if (nativeInterface) {
        QObject::connect(nativeInterface, &QPlatformNativeInterface::windowPropertyChanged,
                         this, &UCUnits::windowPropertyChanged);
    }
}

UCUnits::~UCUnits()
{
    m_units = nullptr;
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
    if (qFuzzyCompare(gridUnit, m_gridUnit)) {
        return;
    }
    m_gridUnit = gridUnit;
    Q_EMIT gridUnitChanged();
}

/*!
    \qmlmethod real Units::dp(real value)

    Returns the number of pixels \a value density independent pixels correspond to.
*/
// Density-independent pixels (and not physical pixels) because Qt sizes in terms of density-independent pixels.
float UCUnits::dp(float value)
{
    const float ratio = m_gridUnit / DEFAULT_GRID_UNIT_PX;
    if (value <= 2.0) {
        // for values under 2dp, return only multiples of the value
        return qRound(value * qFloor(ratio)) / m_devicePixelRatio;
    } else {
        return qRound(value * ratio) / m_devicePixelRatio;
    }
}

/*!
    \qmlmethod real Units::gu(real value)

    Returns the number of pixels \a value grid units correspond to.
*/
// Density-independent pixels (and not physical pixels) because Qt sizes in terms of density-independent pixels.

float UCUnits::gu(float value)
{
    return qRound(value * m_gridUnit) / m_devicePixelRatio;
}

QString UCUnits::resolveResource(const QUrl& url)
{
    if (url.isEmpty()) {
        return QString();
    }

    QString path = QQmlFile::urlToLocalFileOrQrc(url);

    if (path.isEmpty()) {
        return QString();
    }

    const QFileInfo fileInfo(path);
    if (fileInfo.exists()) {
        if (fileInfo.isFile()) {
            return QStringLiteral("1/") + path;
        } else {
            return QString();
        }
    }

    const QString prefix = fileInfo.dir().absolutePath() + "/" + fileInfo.baseName();
    const QString suffix = "." + fileInfo.completeSuffix();

    /* Use file with expected grid unit suffix if it exists.
       For example, if m_gridUnit = 10, look for resource@10.png.
    */

    path = prefix + suffixForGridUnit(m_gridUnit) + suffix;
    if (QFile::exists(path)) {
        return QStringLiteral("1/") + path;
    }

    /* No file with expected grid unit suffix exists.
       List all the files of the form fileBaseName@[0-9]*.fileSuffix and select
       the most appropriate one privileging downscaling high resolution assets
       over upscaling low resolution assets.

       The most appropriate file has a grid unit suffix greater than the target
       grid unit (m_gridUnit) yet as small as possible.
       If no file with a grid unit suffix greater than the target grid unit
       exists, then select one with a grid unit suffix as close as possible to
       the target grid unit.

       For example, if m_gridUnit = 10 and the available files are
       resource@9.png, resource@14.png and resource@18.png, the most appropriate
       file would be resource@14.png since it is above 10 and smaller
       than resource@18.png.
    */
    QStringList nameFilters;
    nameFilters << fileInfo.baseName() + "@[0-9]*" + suffix;
    const QStringList files = fileInfo.dir().entryList(nameFilters, QDir::Files);

    if (!files.empty()) {
        float selectedGridUnitSuffix = gridUnitSuffixFromFileName(files.first());

        Q_FOREACH (const QString& fileName, files) {
            float gridUnitSuffix = gridUnitSuffixFromFileName(fileName);
            if ((selectedGridUnitSuffix >= m_gridUnit && gridUnitSuffix >= m_gridUnit && gridUnitSuffix < selectedGridUnitSuffix)
                || (selectedGridUnitSuffix < m_gridUnit && gridUnitSuffix > selectedGridUnitSuffix)) {
                selectedGridUnitSuffix = gridUnitSuffix;
            }
        }

        path = prefix + suffixForGridUnit(selectedGridUnitSuffix) + suffix;
        float scaleFactor = m_gridUnit / selectedGridUnitSuffix;
        return QString::number(scaleFactor) + "/" + path;
    }

    return QString();
}

QString UCUnits::suffixForGridUnit(float gridUnit)
{
    return "@" + QString::number(gridUnit);
}

float UCUnits::gridUnitSuffixFromFileName(const QString& fileName)
{
    QRegularExpression re(QStringLiteral("^.*@([0-9]*).*$"));
    QRegularExpressionMatch match = re.match(fileName);
    if (match.hasMatch()) {
        return match.captured(1).toFloat();
    } else {
        return 0;
    }
}

void UCUnits::windowPropertyChanged(QPlatformWindow *window, const QString &propertyName)
{
    if (propertyName != QStringLiteral("scale")) { //don't care otherwise
        return;
    }

    /*
     * FIXME - hack to work around LP bug #1573118.
     *
     * The "units" context property has no idea about which screen it is on, making it impossible
     * to have different grid unit values per window in a single app. This would be the case, for
     * example, when one app has multiple windows on different screens.
     *
     * The below ensures that the internal screen (LVDS for all our devices today) scale is ignored
     * when an external screen is connected, for which a different scale is required. Otherwise
     * the two screens would fight for which scale gets applied.
     *
     * This is only currently supported for unity8, which has a window per screen, but we want the
     * external (non-LVDS) screen's scale applied across the shell.
     *
     */
    if (qGuiApp->allWindows().count() > 1) {
        if (window && window->screen()
            && window->screen()->name().contains(QStringLiteral("LVDS"))) {
            return;
        }
    }

    auto nativeInterface = qGuiApp->platformNativeInterface();
    QVariant scaleVal = nativeInterface->windowProperty(window, QStringLiteral("scale"));
    if (!scaleVal.isValid()) {
        return;
    }
    bool ok;
    float scale = scaleVal.toFloat(&ok);
    if (!ok || scale <= 0) {
        return;
    }
    // choose integral grid unit value closest to requested scale
    setGridUnit(qCeil(scale * DEFAULT_GRID_UNIT_PX) * m_devicePixelRatio);
}

UT_NAMESPACE_END
