/*
 * Copyright 2014 Canonical Ltd.
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
#ifndef UCLISTITEMSTYLE_H
#define UCLISTITEMSTYLE_H

#include <QtQuick/QQuickItem>

class QQmlComponent;
class QQuickAbstractAnimation;
class QQuickBehavior;
class UCListItemStyle : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQmlComponent *actionsDelegate MEMBER m_actionsDelegate NOTIFY actionsDelegateChanged)
    Q_PROPERTY(QQmlComponent *selectionDelegate MEMBER m_selectionDelegate NOTIFY selectionDelegateChanged)
    Q_PROPERTY(QQmlComponent *dragHandlerDelegate MEMBER m_dragHandlerDelegate NOTIFY dragHandlerDelegateChanged)
    Q_PROPERTY(qreal swipeOvershoot MEMBER m_swipeOvershoot NOTIFY swipeOvershootChanged)
    Q_PROPERTY(bool attachActionPanelByDirection MEMBER m_attachActionPanelByDirection NOTIFY attachActionPanelByDirectionChanged)
public:
    explicit UCListItemStyle(QQuickItem *parent = 0);

Q_SIGNALS:
    void actionsDelegateChanged();
    void selectionDelegateChanged();
    void dragHandlerDelegateChanged();
    void swipeOvershootChanged();
    void attachActionPanelByDirectionChanged();

private:
    QQmlComponent *m_actionsDelegate;
    QQmlComponent *m_selectionDelegate;
    QQmlComponent *m_dragHandlerDelegate;
    qreal m_swipeOvershoot;
    bool m_attachActionPanelByDirection:1;

    friend class UCListItem;
    friend class UCListItemPrivate;
    friend class UCActionPanel;
    friend class ListItemAnimator;
};

#endif // UCLISTITEMSTYLE_H
