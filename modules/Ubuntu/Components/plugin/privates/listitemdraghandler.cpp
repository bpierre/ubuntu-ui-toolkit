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

#include "listitemdraghandler.h"
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "propertychange_p.h"
#include <QtQuick/private/qquickanimation_p.h>

ListItemDragHandler::ListItemDragHandler(UCListItem *baseItem, UCListItem *listItem)
    : QObject(listItem)
    , listItem(listItem)
    , baseItem(baseItem)
    , baseVisible(0)
{
    // set the object name for testing purposes
    listItem->setObjectName("DraggedListItem");
    targetPos = baseItem->position();
    baseVisible = new PropertyChange(baseItem, "visible");
}

ListItemDragHandler::~ListItemDragHandler()
{
    delete baseVisible;
}

void ListItemDragHandler::init()
{
    PropertyChange::setValue(baseVisible, false);
    // position the item and show it
    listItem->setPosition(baseItem->position());
    listItem->setZ(2);
    listItem->setOpacity(0.5);
    listItem->setVisible(true);
    // emit draggingChanged() signal
    Q_EMIT listItem->draggingChanged();
}

void ListItemDragHandler::drop()
{
    QQuickPropertyAnimation *animation = UCListItemPrivate::get(listItem)->styleItem->m_dropAnimation;
    if (animation) {
        // complete any previous animation
        animation->complete();

        connect(animation, &QQuickAbstractAnimation::stop,
                this, &ListItemDragHandler::dropItem, Qt::DirectConnection);
        // force properties to contain only the 'y' coordinate
        animation->setProperties("y");
        animation->setTargetObject(listItem);
        animation->setFrom(listItem->y());
        animation->setTo(targetPos.y());
        animation->start();
    } else {
        dropItem();
    }
}

// private slot connected to the reposition animation to drop item
void ListItemDragHandler::dropItem()
{
    listItem->setVisible(false);
    delete baseVisible;
    baseVisible = 0;
    listItem->deleteLater();
}

// update dragged item with the new target item the dragging is hovered over
void ListItemDragHandler::update(UCListItem *hoveredItem)
{
    if (hoveredItem) {
        targetPos = hoveredItem->position();
    }
}
