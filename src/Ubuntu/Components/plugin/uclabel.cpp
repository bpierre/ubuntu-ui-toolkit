/*
 * Copyright 2015 Canonical Ltd.
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
 */

#include "uclabel.h"
#include "ucfontutils.h"
#include "ucunits.h"

void UCLabel::updatePixelSize()
{
    const float sizes[] = {
        UCFontUtils::xxSmallScale, UCFontUtils::xSmallScale, UCFontUtils::smallScale,
        UCFontUtils::mediumScale, UCFontUtils::largeScale, UCFontUtils::xLargeScale
    };
    QFont textFont = font();
    textFont.setPixelSize(
        qRound(sizes[m_adaptiveSize] * UCUnits::instance().dp(UCFontUtils::fontUnits)));
    setFont(textFont);
}

/*!
 * \qmltype LabelBase
 * \qmlabstract
 * \instantiates UCLabel
 * \inqmlmodule Ubuntu.Components 1.3
 * \ingroup ubuntu
 * \brief Extended Text item with Ubuntu styling.
 *
 * Label is an extended Text item with Ubuntu styling. It exposes an additional property that
 * provides adaptive resizing based on the measurement unit.
 *
 * Example:
 * \qml
 * Rectangle {
 *     color: UbuntuColors.warmGrey
 *     width: units.gu(30)
 *     height: units.gu(30)
 *
 *     Label {
 *         anchors.centerIn: parent
 *         text: "Hello world!"
 *         adaptiveSize: "large"
 *     }
 * }
 * \endqml
 */
UCLabel::UCLabel(QQuickItem* parent)
    : QQuickText(parent)
    , m_adaptiveSize(Medium)
    , m_flags(0)
{
    updatePixelSize();
}

/*!
 * \qmlproperty enumeration LabelBase::adaptiveSize
 * \since Ubuntu.Components 1.3
 *
 * This property holds an abstract size that allows adaptive resizing based on the measurement unit
 * (see Units). The default value is \c Label.Medium.
 *
 * \note Setting this disables support for the deprecated \l fontSize property.
 *
 * \list
 *  \li \b Label.XxSmall - extremely small font size
 *  \li \b Label.XSmall - very small font size
 *  \li \b Label.Small - small font size
 *  \li \b Label.Medium - medium font size
 *  \li \b Label.Large - large font size
 *  \li \b Label.XLarge - very large font size
 *  \endlist
 */
void UCLabel::setAdaptiveSize(AdaptiveSize adaptiveSize)
{
    if (!(m_flags & AdaptiveSizeSet)) {
        Q_EMIT fontSizeChanged();
        m_flags |= AdaptiveSizeSet;
    }

    if (m_adaptiveSize != adaptiveSize) {
        m_adaptiveSize = adaptiveSize;
        updatePixelSize();
        Q_EMIT adaptiveSizeChanged();
    }
}

/*!
 * \qmlproperty string LabelBase::fontSize
 * \deprecated
 *
 * This property holds an abstract size represented as a string that allows adaptive resizing based
 * on the measurement unit (see Units). The default value is \c "medium".
 *
 * \note Use \l adaptiveSize instead.
 *
 * Here is the list of allowed strings from the smallest to the largest:
 * \list
 *  \li \b "xx-small" - extremely small font size
 *  \li \b "x-small" - very small font size
 *  \li \b "small" - small font size
 *  \li \b "medium" - medium font size
 *  \li \b "large" - large font size
 *  \li \b "x-large" - very large font size
 *  \endlist
 */
void UCLabel::setFontSize(const QString& fontSize)
{
    if (m_flags & AdaptiveSizeSet) {
        return;
    }
    if (fontSize.size() < 4) {
        return;
    }

    AdaptiveSize adaptiveSize;
    switch (reinterpret_cast<int*>(fontSize.toLatin1().data())[0]) {
        case /*"medi"*/0x6964656d: { adaptiveSize = Medium; break; }
        case /*"larg"*/0x6772616c: { adaptiveSize = Large; break; }
        case /*"smal"*/0x6c616d73: { adaptiveSize = Small; break; }
        case /*"x-la"*/0x616c2d78: { adaptiveSize = XLarge; break; }
        case /*"x-sm"*/0x6d732d78: { adaptiveSize = XSmall; break; }
        case /*"xx-s"*/0x732d7878: { adaptiveSize = XxSmall; break; }
        default: { return; }
    }

    if (m_adaptiveSize != adaptiveSize) {
        m_adaptiveSize = adaptiveSize;
        updatePixelSize();
        Q_EMIT fontSizeChanged();
    }
}
