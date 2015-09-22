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
 * Author: Andrea Bernabei <andrea.bernabei@canonical.com>
 */

#ifndef UCSLOTSLAYOUT_H
#define UCSLOTSLAYOUT_H

#include <QtQuick/QQuickItem>
#include <QtGui/QPixmap>
#include <private/qquicktext_p.h>

class UCSlotsAttached;
class UCSlotsLayoutPadding;
class UCSlotsLayoutPrivate;
class UCSlotsLayout : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QQuickItem *mainSlot READ mainSlot WRITE setMainSlot NOTIFY mainSlotChanged)
    Q_PROPERTY(UCSlotsLayoutPadding *padding READ padding CONSTANT FINAL)

    Q_ENUMS(UCSlotPosition)

public:
    explicit UCSlotsLayout(QQuickItem *parent = 0);

    QQuickItem *mainSlot() const;
    void setMainSlot(QQuickItem *item);

    UCSlotsLayoutPadding *padding();

    enum UCSlotPosition {
        First = INT_MIN,
        Leading = INT_MIN/2,
        Trailing = INT_MAX/2,
        Last = INT_MAX
    };

    static UCSlotsAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void mainSlotChanged();

protected:
    Q_DECLARE_PRIVATE(UCSlotsLayout)
    void componentComplete();
    void itemChange(ItemChange change, const ItemChangeData &data);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_onGuValueChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_updateCachedHeight())
    Q_PRIVATE_SLOT(d_func(), void _q_updateGuValues())
    Q_PRIVATE_SLOT(d_func(), void _q_onMainSlotHeightChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_updateSlotsBBoxHeight())
    Q_PRIVATE_SLOT(d_func(), void _q_updateSize())
    Q_PRIVATE_SLOT(d_func(), void _q_onSlotPositionChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_relayout())
};
QML_DECLARE_TYPEINFO(UCSlotsLayout, QML_HAS_ATTACHED_PROPERTIES)

class UCSlotsAttachedPrivate;
class UCSlotsAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(UCSlotsLayout::UCSlotPosition position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(UCSlotsLayoutPadding *padding READ padding CONSTANT FINAL)
    Q_PROPERTY(bool overrideVerticalPositioning READ overrideVerticalPositioning WRITE setOverrideVerticalPositioning NOTIFY overrideVerticalPositioningChanged)

public:
    UCSlotsAttached(QObject *object = 0);

    UCSlotsLayout::UCSlotPosition position() const;
    void setPosition(UCSlotsLayout::UCSlotPosition pos);

    UCSlotsLayoutPadding *padding();

    bool overrideVerticalPositioning() const;
    void setOverrideVerticalPositioning(bool val);

Q_SIGNALS:
    void positionChanged();
    void overrideVerticalPositioningChanged();

protected:
    Q_DECLARE_PRIVATE(UCSlotsAttached)

private:
    Q_PRIVATE_SLOT(d_func(), void _q_onGuValueChanged())
};

class UCSlotsLayoutPadding : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal leading READ leading WRITE setLeadingQml NOTIFY leadingChanged FINAL)
    Q_PROPERTY(qreal trailing READ trailing WRITE setTrailingQml NOTIFY trailingChanged FINAL)
    Q_PROPERTY(qreal top READ top WRITE setTopQml NOTIFY topChanged FINAL)
    Q_PROPERTY(qreal bottom READ bottom WRITE setBottomQml NOTIFY bottomChanged FINAL)

public:
    explicit UCSlotsLayoutPadding(QObject *parent = 0);

    qreal leading() const;
    void setLeading(qreal val);
    void setLeadingQml(qreal val);

    qreal trailing() const;
    void setTrailing(qreal val);
    void setTrailingQml(qreal val);

    qreal top() const;
    void setTop(qreal val);
    void setTopQml(qreal val);

    qreal bottom() const;
    void setBottom(qreal val);
    void setBottomQml(qreal val);

    //once the dev tries to change the offsets (and he does so via QML) we'll stop
    //updating offset's value, for instance when gu value changes or when the
    //positioning mode changes
    bool leadingWasSetFromQml : 1;
    bool trailingWasSetFromQml : 1;
    bool topWasSetFromQml : 1;
    bool bottomWasSetFromQml : 1;

Q_SIGNALS:
    void leadingChanged();
    void trailingChanged();
    void topChanged();
    void bottomChanged();

private:
    //similar to anchors.margins, but we don't use a contentItem so we handle this ourselves
    qreal m_leading;
    qreal m_trailing;
    qreal m_top;
    qreal m_bottom;
};

class UCThreeLabelsSlotPrivate;
class UCThreeLabelsSlot : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickText *title READ title CONSTANT FINAL)
    Q_PROPERTY(QQuickText *subtitle READ subtitle CONSTANT FINAL)
    Q_PROPERTY(QQuickText *summary READ summary CONSTANT FINAL)

public:
    explicit UCThreeLabelsSlot(QQuickItem *parent = 0);

    QQuickText *title();
    QQuickText *subtitle();
    QQuickText *summary();

protected:
    Q_DECLARE_PRIVATE(UCThreeLabelsSlot)
    void componentComplete();

private:
    Q_PRIVATE_SLOT(d_func(), void _q_onThemeChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_onGuValueChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_updateLabelsAnchorsAndBBoxHeight())
};

#endif // UCSLOTSLAYOUT_H
