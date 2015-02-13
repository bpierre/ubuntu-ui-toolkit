/*
 * Copyright 2014-2015 Canonical Ltd.
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

#include "ucunits.h"
#include "uctheme.h"
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "propertychange_p.h"
#include "uclistitemstyle.h"
#include <QtQuick/private/qquickflickable_p.h>

/*
 * The properties are attached to the ListItem's parent item or to its closest
 * Flickable parent, when embedded in ListView or Flickable. There will be only
 * one attached property per Flickable for all embedded child ListItems, enabling
 * in this way the controlling of the interactive flag of the Flickable and all
 * its ascendant Flickables.
 */
UCViewItemsAttachedPrivate::UCViewItemsAttachedPrivate(UCViewItemsAttached *qq)
    : q_ptr(qq)
    , globalDisabled(false)
    , selectable(false)
{
}

UCViewItemsAttachedPrivate::~UCViewItemsAttachedPrivate()
{
    clearChangesList();
    clearFlickablesList();
}

// disconnect all flickables
void UCViewItemsAttachedPrivate::clearFlickablesList()
{
    Q_Q(UCViewItemsAttached);
    Q_FOREACH(const QPointer<QQuickFlickable> &flickable, flickables) {
        if (flickable.data())
        QObject::disconnect(flickable.data(), &QQuickFlickable::movementStarted,
                            q, &UCViewItemsAttached::unbindItem);
    }
    flickables.clear();
}

// connect all flickables
void UCViewItemsAttachedPrivate::buildFlickablesList()
{
    Q_Q(UCViewItemsAttached);
    QQuickItem *item = qobject_cast<QQuickItem*>(q->parent());
    if (!item) {
        return;
    }
    clearFlickablesList();
    while (item) {
        QQuickFlickable *flickable = qobject_cast<QQuickFlickable*>(item);
        if (flickable) {
            QObject::connect(flickable, &QQuickFlickable::movementStarted,
                             q, &UCViewItemsAttached::unbindItem);
            flickables << flickable;
        }
        item = item->parentItem();
    }
}

void UCViewItemsAttachedPrivate::clearChangesList()
{
    // clear property change objects
    qDeleteAll(changes);
    changes.clear();
}

void UCViewItemsAttachedPrivate::buildChangesList(const QVariant &newValue)
{
    // collect all ascendant flickables
    Q_Q(UCViewItemsAttached);
    QQuickItem *item = qobject_cast<QQuickItem*>(q->parent());
    if (!item) {
        return;
    }
    clearChangesList();
    while (item) {
        QQuickFlickable *flickable = qobject_cast<QQuickFlickable*>(item);
        if (flickable) {
            PropertyChange *change = new PropertyChange(item, "interactive");
            PropertyChange::setValue(change, newValue);
            changes << change;
        }
        item = item->parentItem();
    }
}

/*!
 * \qmltype ViewItems
 * \instantiates UCViewItemsAttached
 * \inqmlmodule Ubuntu.Components 1.2
 * \ingroup unstable-ubuntu-listitems
 * \since Ubuntu.Components 1.2
 * \brief A set of properties attached to the ListItem's parent item or ListView.
 *
 * These properties are attached to the parent item of the ListItem, or to
 * ListView, when the component is used as delegate.
 */
UCViewItemsAttached::UCViewItemsAttached(QObject *owner)
    : QObject(owner)
    , d_ptr(new UCViewItemsAttachedPrivate(this))
{
}

UCViewItemsAttached::~UCViewItemsAttached()
{
}

UCViewItemsAttached *UCViewItemsAttached::qmlAttachedProperties(QObject *owner)
{
    return new UCViewItemsAttached(owner);
}

// register item to be rebound
bool UCViewItemsAttached::listenToRebind(UCListItem *item, bool listen)
{
    // we cannot bind the item until we have an other one bound
    bool result = false;
    Q_D(UCViewItemsAttached);
    if (listen) {
        if (d->boundItem.isNull() || (d->boundItem == item)) {
            d->boundItem = item;
            // rebuild flickable list
            d->buildFlickablesList();
            result = true;
        }
    } else if (d->boundItem == item) {
        d->boundItem.clear();
        result = true;
    }
    return result;
}

// reports true if any of the ascendant flickables is moving
bool UCViewItemsAttached::isMoving()
{
    Q_D(UCViewItemsAttached);
    Q_FOREACH(const QPointer<QQuickFlickable> &flickable, d->flickables) {
        if (flickable && flickable->isMoving()) {
            return true;
        }
    }
    return false;
}

// returns true if the given ListItem is bound to listen on moving changes
bool UCViewItemsAttached::isBoundTo(UCListItem *item)
{
    Q_D(UCViewItemsAttached);
    return d->boundItem == item;
}

/*
 * Disable/enable interactive flag for the ascendant flickables. The item is used
 * to detect whether the same item is trying to enable the flickables which disabled
 * it before. The enabled/disabled states are not equivalent to the enabled/disabled
 * state of the interactive flag.
 * When disabled, always the last item disabling will be kept as active disabler,
 * and only the active disabler can enable (restore) the interactive flag state.
 */
void UCViewItemsAttached::disableInteractive(UCListItem *item, bool disable)
{
    Q_D(UCViewItemsAttached);
    if (disable) {
        // disabling or re-disabling
        d->disablerItem = item;
        if (d->globalDisabled == disable) {
            // was already disabled, leave
            return;
        }
        d->globalDisabled = true;
    } else if (d->globalDisabled && d->disablerItem == item) {
        // the one disabled it will enable
        d->globalDisabled = false;
        d->disablerItem.clear();
    } else {
        // !disabled && (!globalDisabled || item != d->disablerItem)
        return;
    }
    if (disable) {
        // (re)build changes list with disabling the interactive value
        d->buildChangesList(false);
    } else {
        d->clearChangesList();
    }
}

void UCViewItemsAttached::unbindItem()
{
    Q_D(UCViewItemsAttached);
    if (d->boundItem) {
        // snap out before we unbind

        UCListItemPrivate::get(d->boundItem)->snapOut();
        d->boundItem.clear();
    }
    // clear binding list
    d->clearFlickablesList();
}

/*!
 * \qmlattachedproperty bool ViewItems::selectMode
 * The property drives whether list items are selectable or not.
 *
 * When set, the ListItems of the Item the property is attached to will enter into
 * selection state. ListItems provide a visual clue which can be used to toggle
 * the selection state of each, which in order will be reflected in the
 * \l {ViewItems::selectedIndices}{ViewItems.selectedIndices} list.
 */
bool UCViewItemsAttached::selectMode() const
{
    Q_D(const UCViewItemsAttached);
    return d->selectable;
}
void UCViewItemsAttached::setSelectMode(bool value)
{
    Q_D(UCViewItemsAttached);
    if (d->selectable == value) {
        return;
    }
    d->selectable = value;
    Q_EMIT selectModeChanged();
}

/*!
 * \qmlattachedproperty list<int> ViewItems::selectedIndices
 * The property contains the indexes of the ListItems marked as selected. The
 * indexes are model indexes when used in ListView, and child indexes in other
 * components. The property being writable, initial selection configuration
 * can be provided for a view, and provides ability to save the selection state.
 */
QList<int> UCViewItemsAttached::selectedIndices() const
{
    Q_D(const UCViewItemsAttached);
    return d->selectedList.toList();
}
void UCViewItemsAttached::setSelectedIndices(const QList<int> &list)
{
    Q_D(UCViewItemsAttached);
    if (d->selectedList.toList() == list) {
        return;
    }
    d->selectedList = QSet<int>::fromList(list);
    Q_EMIT selectedIndicesChanged();
}

bool UCViewItemsAttachedPrivate::addSelectedItem(UCListItem *item)
{
    int index = UCListItemPrivate::get(item)->index();
    if (!selectedList.contains(index)) {
        selectedList.insert(index);
        Q_EMIT q_ptr->selectedIndicesChanged();
        return true;
    }
    return false;
}
bool UCViewItemsAttachedPrivate::removeSelectedItem(UCListItem *item)
{
    if (selectedList.remove(UCListItemPrivate::get(item)->index()) > 0) {
        Q_EMIT q_ptr->selectedIndicesChanged();
        return true;
    }
    return false;
}

bool UCViewItemsAttachedPrivate::isItemSelected(UCListItem *item)
{
    return selectedList.contains(UCListItemPrivate::get(item)->index());
}
