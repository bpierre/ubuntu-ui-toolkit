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

#ifndef UCMARGINS_H
#define UCMARGINS_H

#include <QtCore/QObject>
#include <QtQuick/QQuickItem>

class UCMargins : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal left MEMBER m_left NOTIFY leftChanged FINAL)
    Q_PROPERTY(qreal top MEMBER m_top NOTIFY topChanged FINAL)
    Q_PROPERTY(qreal right MEMBER m_right NOTIFY rightChanged FINAL)
    Q_PROPERTY(qreal bottom MEMBER m_bottom NOTIFY bottomChanged FINAL)
public:
    UCMargins(QObject *parent = 0);

    qreal left() const;
    qreal top() const;
    qreal right() const;
    qreal bottom() const;

Q_SIGNALS:
    void leftChanged();
    void topChanged();
    void rightChanged();
    void bottomChanged();

private:
    qreal m_left = 0.0;
    qreal m_top = 0.0;
    qreal m_right = 0.0;
    qreal m_bottom = 0.0;
};

inline UCMargins::UCMargins(QObject *parent) : QObject(parent) {}

inline qreal UCMargins::left() const
{
    return m_left;
}

inline qreal UCMargins::top() const
{
    return m_top;
}

inline qreal UCMargins::right() const
{
    return m_right;
}

inline qreal UCMargins::bottom() const
{
    return m_bottom;
}

#endif // UCMARGINS_H
