/*
 * Copyright (C) 2015 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef UCSWIPEAREA_H
#define UCSWIPEAREA_H

#include <QtQuick/QQuickItem>
#include "ubuntugesturesqmlglobal.h"
#include "damper.h"

// lib UbuntuGestures
#include <UbuntuGestures/Pool>
#include <UbuntuGestures/Timer>

// logging
#include <QtCore/QLoggingCategory>

class TouchOwnershipEvent;
class UnownedTouchEvent;
class UCSwipeAreaPrivate;

class UBUNTUGESTURESQML_EXPORT UCSwipeArea : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(Direction direction READ direction WRITE setDirection NOTIFY directionChanged)
    Q_PROPERTY(qreal distance READ distance NOTIFY distanceChanged)
    Q_PROPERTY(QPointF touchPosition READ touchPosition NOTIFY touchPositionChanged)
    Q_PROPERTY(bool dragging READ dragging NOTIFY draggingChanged)
    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
    Q_PROPERTY(bool immediateRecognition
            READ immediateRecognition
            WRITE setImmediateRecognition
            NOTIFY immediateRecognitionChanged)

    Q_ENUMS(Direction)
public:
    enum Direction {
        Rightwards,
        Leftwards,
        Downwards,
        Upwards,
        Horizontal,
        Vertical
    };

    UCSwipeArea(QQuickItem *parent = 0);

    Direction direction() const;
    void setDirection(Direction);

    qreal distance() const;

    QPointF touchPosition() const;

    bool dragging() const;

    bool pressed() const;

    bool immediateRecognition() const;
    void setImmediateRecognition(bool enabled);

Q_SIGNALS:
    void directionChanged(Direction direction);
    void draggingChanged(bool dragging);
    void pressedChanged(bool pressed);
    void distanceChanged(qreal distance);
    void touchPositionChanged(const QPointF &position);
    void immediateRecognitionChanged(bool immediateRecognition);

protected:
    bool event(QEvent *e) override;

    void touchEvent(QTouchEvent *event) override;
    void itemChange(ItemChange change, const ItemChangeData &value) override;

public: // so tests can access it
    UCSwipeAreaPrivate *d;
};

Q_DECLARE_LOGGING_CATEGORY(ucSwipeArea)
Q_DECLARE_LOGGING_CATEGORY(ucActiveTouchInfo)

#endif // UCSWIPEAREA_H
