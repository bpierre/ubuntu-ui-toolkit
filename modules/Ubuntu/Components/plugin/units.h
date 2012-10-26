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

#ifndef UBUNTU_COMPONENTS_UNITS_H
#define UBUNTU_COMPONENTS_UNITS_H

#include <QObject>
#include <QtCore/QHash>
#include <QtCore/QUrl>

class Units : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float gridUnit READ gridUnit WRITE setGridUnit NOTIFY gridUnitChanged)

public:
    static Units& instance() {
        static Units instance;
        return instance;
    }

    explicit Units(QObject *parent = 0);
    Q_INVOKABLE float dp(float value);
    Q_INVOKABLE float gu(float value);
    Q_INVOKABLE QString resolveResource(const QUrl& url);

    // getters
    float gridUnit();

    // setters
    void setGridUnit(float gridUnit);
    void setBaseUrl(const QUrl& baseUrl);

Q_SIGNALS:
    void gridUnitChanged();

protected:
    QString suffixForGridUnit(float gridUnit);
    bool loadResourcesUnitFile(QString fileName);

private:
    float m_gridUnit;
    float m_resourcesUnit;
    QUrl m_baseUrl;
};

class QQmlContext;

class ContextPropertyChangeListener : public QObject
{
    Q_OBJECT
public:
    explicit ContextPropertyChangeListener(QQmlContext* context, QString contextProperty);
    Q_SLOT void updateContextProperty();
    QQmlContext* m_context;
    QString m_contextProperty;
};

#endif // UBUNTU_COMPONENTS_UNITS_H
