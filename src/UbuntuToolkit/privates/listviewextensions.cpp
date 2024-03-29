/*
 * Copyright 2016 Canonical Ltd.
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
 * Author Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "privates/listviewextensions_p.h"

#include <QtQuick/QQuickItem>
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickitemview_p.h> // for QQuickItemView::BottomToTop

#include "uclistitem_p_p.h"
#include "quickutils_p.h"

UT_NAMESPACE_BEGIN

ListViewProxy::ListViewProxy(QQuickFlickable *listView, QObject *parent)
    : QObject(parent)
    , listView(listView)
    , _currentItem(Q_NULLPTR)
{
    connect(listView, SIGNAL(currentItemChanged()), this, SLOT(onCurrentItemChanged()), Qt::DirectConnection);
    onCurrentItemChanged();
}
ListViewProxy::~ListViewProxy()
{
    if (isEventFilter) {
        listView->removeEventFilter(this);
    }
}

// proxy methods

Qt::Orientation ListViewProxy::orientation()
{
    return (Qt::Orientation)listView->property("orientation").toInt();
}

int ListViewProxy::count()
{
    return listView->property("count").toInt();
}

QQuickItem *ListViewProxy::currentItem()
{
    return _currentItem;
}

int ListViewProxy::currentIndex()
{
    return listView->property("currentIndex").toInt();
}

void ListViewProxy::setCurrentIndex(int index)
{
    listView->setProperty("currentIndex", index);
}

QVariant ListViewProxy::model()
{
    return listView->property("model");
}

/*********************************************************************
 * Additional functionality used in different places in toolkit
 *********************************************************************/

// Navigation override used by ListItems
void ListViewProxy::overrideItemNavigation(bool override)
{
    if (override) {
        listView->installEventFilter(this);
    } else {
        listView->removeEventFilter(this);
    }
    isEventFilter = override;
}

bool ListViewProxy::eventFilter(QObject *, QEvent *event)
{
    switch (event->type()) {
        case QEvent::FocusIn:
            return focusInEvent(static_cast<QFocusEvent*>(event));
        case QEvent::KeyPress:
            return keyPressEvent(static_cast<QKeyEvent*>(event));
        default:
            break;
    }

    return false;
}

void ListViewProxy::setKeyNavigationForListView(bool value)
{
    UCListItem *listItem = qobject_cast<UCListItem*>(_currentItem);
    if (listItem) {
        UCListItemPrivate::get(listItem)->setListViewKeyNavigation(value);
        listItem->update();
    }
}

// grab focusIn event
bool ListViewProxy::focusInEvent(QFocusEvent *event)
{
    switch (event->reason()) {
        case Qt::TabFocusReason:
        case Qt::BacktabFocusReason:
        {
            if (!_currentItem && count() > 0) {
                // set the first one to be the focus
                setCurrentIndex(0);
                setKeyNavigationForListView(true);
            }
            break;
        }
        default:
            break;
    }
    return false;
}

// override cursor key presses for ListView; returns true if the key event is consumed
// in which case ListView won't get it.
bool ListViewProxy::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    Qt::Orientation orientation = this->orientation();

    if ((orientation == Qt::Vertical && (key == Qt::Key_Up || key == Qt::Key_Down))
        || (orientation == Qt::Horizontal && (key == Qt::Key_Left || key == Qt::Key_Right))) {
        keyNavigation = true;
    }

    return false;
}

void ListViewProxy::onCurrentItemChanged()
{
    setKeyNavigationForListView(false);
    _currentItem = listView->property("currentItem").value<QQuickItem*>();
    if (_currentItem && _currentItem->isEnabled()) {
        setKeyNavigationForListView(keyNavigation);
        keyNavigation = false;
    }
}

UT_NAMESPACE_END
