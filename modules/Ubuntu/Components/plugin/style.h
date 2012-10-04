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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef STYLE_H
#define STYLE_H

#include <QObject>
#include <QStringList>
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlParserStatus>

//#include <QLibrary>

class StyleRulePrivate;
class QQmlComponent;
class StyleRule : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(QString selector READ selector WRITE setSelector)
    Q_PROPERTY(QQmlComponent *style READ style WRITE setStyle)
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate)

public:
    StyleRule(QObject *parent = 0);
    ~StyleRule();

    void classBegin();
    void componentComplete();

Q_SIGNALS:
    // the signal is emitted once the rule is completed
    void ruleChanged();

public Q_SLOTS:

public: //getter/setters
    QString selector() const;
    void setSelector(const QString &selector);
    QQmlComponent *style();
    void setStyle(QQmlComponent *style);
    QQmlComponent *delegate();
    void setDelegate(QQmlComponent *delegate);

private:
    Q_DISABLE_COPY(StyleRule)
    Q_DECLARE_PRIVATE(StyleRule)
    QScopedPointer<StyleRulePrivate> d_ptr;
};

QML_DECLARE_TYPE(StyleRule)

#endif // STYLE_H
