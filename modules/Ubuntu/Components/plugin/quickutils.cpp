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
 */

#include "quickutils.h"

#include <QGuiApplication>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlContext>
#include <QtCore/QAbstractListModel>
#include <QtCore/QAbstractProxyModel>
#include <QtQml/QQmlPropertyMap>
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlEngine>

#include <private/qquicktextinput_p.h>
#include <private/qquicktextedit_p.h>

QuickUtils::QuickUtils(QObject *parent) :
    QObject(parent),
    m_rootView(0)
{
    QGuiApplication::instance()->installEventFilter(this);
    // connect to focusObjectChanged() to get the latest active focus object
    QObject::connect(QGuiApplication::instance(), SIGNAL(focusObjectChanged(QObject*)), this, SLOT(activeFocus(QObject*)));
}

/*!
 * \internal
 * Filter events to catch ChildAdded, when the the application gets the topmost
 * window assigned. Need to check the topmost windows each time as widgets added
 * to the application are not signaled in any other way.
 */
bool QuickUtils::eventFilter(QObject *obj, QEvent *event)
{
    if (!m_rootView && (event->type() == QEvent::ApplicationActivate))
        lookupQuickView();
    if (event->type() == QEvent::ApplicationDeactivate) {
        Q_EMIT deactivated();
    }

    return QObject::eventFilter(obj, event);
}

/*!
 * \internal
 * Catch active focus object change to detecte whether we need to remove OSK or not.
 */
void QuickUtils::activeFocus(QObject *active)
{
    // FIXME: workaround for bug https://bugreports.qt-project.org/browse/QTBUG-30729
    // input panel does not get removed when no input is active
    // remove input panel if there's no more active object or the new active object
    // is not a text input
    // workaround for bug https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1163371
    if (QGuiApplication::inputMethod()->isVisible() && (!active || (active &&
                    !qobject_cast<QQuickTextInput*>(active) &&
                    !qobject_cast<QQuickTextEdit*>(active)))) {
        QGuiApplication::inputMethod()->hide();
    }
}

/*!
 * \internal
 * \deprecated
 * Returns the current root object.
 */
QQuickItem *QuickUtils::rootObject()
{
    qmlInfo(this) << "WARNING: QuickUtils.rootObject property is deprecated: Use QuickUtils::rootItem() function instead.";
    if (!m_rootView)
        lookupQuickView();
    return (m_rootView) ? m_rootView->rootObject() : 0;
}

/*!
 * \internal
 * Returns the root item of a given item. In case there is a QQuickWindow (Window)
 * found in the hierarchy, the function will return the contentItem of the window.
 */
QQuickItem *QuickUtils::rootItem(QObject *object)
{
    // make sure we have the m_rootView updated
    lookupQuickView();
    if (!object) {
        return (m_rootView) ? m_rootView->rootObject() : 0;
    }

    QQuickItem *item = qobject_cast<QQuickItem*>(object);
    // the given object may be a non-visual element (QtObject or QQmlComponent)
    // therefore those objects' parent object should be considered
    QQuickItem *parentItem = item ? item->parentItem() : qobject_cast<QQuickItem*>(object->parent());
    while (parentItem && parentItem->parentItem()) {
        parentItem = parentItem->parentItem();
    }

    if (m_rootView && (m_rootView->contentItem() == parentItem)) {
        // when traversing visual parents of an element from the application,
        // we reach QQuickView's contentItem, whose size is invalid. Therefore
        // we need to return the QQuickView's rootObject() instead of the topmost
        // item found
        return m_rootView->rootObject();
    }
    return parentItem;
}


QString QuickUtils::inputMethodProvider() const
{
    return QString(getenv("QT_IM_MODULE"));
}

/*!
 * \internal
 * Returns the class name (type) of a QtQuick item.
 */
QString QuickUtils::className(QObject *item)
{
    QString result = item->metaObject()->className();
    return result.left(result.indexOf("_QML"));
}

/*!
 * \internal
 * Moves a given item to the specified index in its parent's child list.
 */
void QuickUtils::moveItemBefore(QQuickItem *item, QQuickItem *before)
{
    Q_ASSERT(item);
//    QQuickItem *parentItem = item->parentItem();
    Q_ASSERT(item->parentItem());
    if (before) {
        Q_ASSERT(before->parentItem() == item->parentItem());
        item->stackBefore(before);
    }
}


/*!
 * \internal
 * Get QQuickView from the application's window list and connect its status change
 * signal as the root element is set after the root element completion.
 */
void QuickUtils::lookupQuickView()
{
    if (m_rootView)
        return;
    Q_FOREACH (QWindow *w, QGuiApplication::topLevelWindows()) {
        m_rootView = qobject_cast<QQuickView*>(w);
        if (m_rootView) {
            // connect in case we get the root object changed
            QObject::connect(m_rootView, SIGNAL(statusChanged(QQuickView::Status)),
                             this, SIGNAL(rootObjectChanged()));
            // emit changed signal so we update the eventual bindings
            if (m_rootView->rootObject())
                Q_EMIT rootObjectChanged();
            break;
        }
    }
}

QObject* QuickUtils::createQmlObject(const QUrl &url, QQmlEngine *engine)
{
    /* FIXME: if the directory pointed to by url contains a qmldir file that
       declares a JavaScript module then QQmlComponent::create() fails with
       the error "QQmlComponent: Component is not ready".
    */
    QQmlComponent *component = new QQmlComponent(engine, url, QQmlComponent::PreferSynchronous);
    QObject* result = component->create();
    delete component;
    return result;
}
