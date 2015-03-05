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
 *
 * Authors: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ucdeprecatedtheme.h"
#include "uctheme.h"
#include "listener.h"
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlInfo>

/*!
    \qmltype Theme
    \instantiates UCTheme
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup theming
    \brief The Theme class provides facilities to interact with the current theme.
    \note \B {This class is deprecated. Use StyleSet with StyledItem for styling your
    components.}

    A global instance is exposed as the \b Theme context property.

    The theme defines the visual aspect of the Ubuntu components.

    Example changing the current theme:

    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.2

    Item {
        Button {
            onClicked: Theme.name = "Ubuntu.Components.Themes.Ambiance"
        }
    }
    \endqml

    Example creating a style component:

    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.2

    StyledItem {
        id: myItem
        style: Theme.createStyleComponent("MyItemStyle.qml", myItem)
    }
    \endqml

    \sa {StyledItem}
*/
UCDeprecatedTheme::UCDeprecatedTheme(QObject *parent)
    : QObject(parent)
    , m_theme(new UCTheme(true, this))
{
    connect(m_theme, &UCTheme::nameChanged, this, &UCDeprecatedTheme::nameChanged);
    connect(m_theme, &UCTheme::paletteChanged, this, &UCDeprecatedTheme::paletteChanged);
}

/*!
    \qmlproperty string Theme::name

    The name of the current theme.
*/
QString UCDeprecatedTheme::name() const
{
//    qmlInfo(this) << "Theme.name is deprecated. Use StyleSet instead.";
    return m_theme->name();
}
void UCDeprecatedTheme::setName(const QString& name)
{
//    qmlInfo(this) << "Theme.name is deprecated. Use StyleSet instead.";
    m_theme->setName(name);
}

/*!
    \qmlproperty Palette Theme::palette

    The palette of the current theme.
*/
QObject* UCDeprecatedTheme::palette()
{
//    qmlInfo(this) << "Theme.palette is deprecated. Use StyleSet instead.";
    return m_theme->palette();
}

/*!
    \qmlmethod Component Theme::createStyleComponent(string styleName, object parent)

    Returns an instance of the style component named \a styleName.
*/
QQmlComponent* UCDeprecatedTheme::createStyleComponent(const QString& styleName, QObject* parent)
{
//    qmlInfo(this) << "Theme.createStyleComponent is deprecated. Use StyleSet instead.";
    return m_theme->createStyleComponent(styleName, parent);
}

void UCDeprecatedTheme::registerToContext(QQmlContext* context)
{
    // add paths to engine search folder
    m_theme->m_engine = context->engine();
    m_theme->updateEnginePaths();

    // register deprecated Theme property
    context->setContextProperty("Theme", this);

    ContextPropertyChangeListener *themeChangeListener =
        new ContextPropertyChangeListener(context, "Theme");
    QObject::connect(this, SIGNAL(nameChanged()),
                     themeChangeListener, SLOT(updateContextProperty()));
}
