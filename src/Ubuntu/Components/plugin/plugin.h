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
 * Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
 */

#ifndef UBUNTU_COMPONENTS_PLUGIN_H
#define UBUNTU_COMPONENTS_PLUGIN_H

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExtensionPlugin>
#include <ubuntutoolkitglobal.h>

class QWindow;

UT_NAMESPACE_BEGIN

class UbuntuComponentsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri) override;
    void initializeEngine(QQmlEngine *engine, const char *uri) override;
    static void initializeContextProperties(QQmlEngine*);

private Q_SLOTS:
    void registerWindowContextProperty();
    void setWindowContextProperty(QWindow* focusWindow);

private:
    void initializeBaseUrl(QQmlEngine *engine);
    void registerTypesToVersion(const char *uri, int major, int minor);
};

UT_NAMESPACE_END

#endif // UBUNTU_COMPONENTS_PLUGIN_H

