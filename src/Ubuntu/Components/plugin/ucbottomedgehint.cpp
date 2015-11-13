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
 * Authors: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ucbottomedgehint.h"
#include "ucstyleditembase_p.h"
#include "quickutils.h"
#include "ucunits.h"
#include <QtQml/private/qqmlproperty_p.h>
#include <QtQuick/private/qquickflickable_p.h>

/*!
    \qmltype BottomEdgeHint
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    \inherits StyledItem
    \brief The BottomEdgeHint shows the availability of extra features
    available from the bottom edge of the application.

    It displays a label and/or an icon at the bottom of the component it is
    attached to.

    When used with a mouse it acts like a button. The typical action associated
    with clicking on it should be revealing the extra features provided by the
    bottom edge.

    Example:
    \qml
    BottomEdgeHint {
        id: bottomEdgeHint
        text: i18n.tr("Favorites")
        onClicked: revealBottomEdge()
    }
    \endqml

    The component is styled through \b BottomEdgeHintStyle.
*/
UCBottomEdgeHint::UCBottomEdgeHint(QQuickItem *parent)
    : UCStyledItemBase(parent)
    , m_gestureDetector(this)
    , m_flickable(Q_NULLPTR)
    , m_deactivateTimeout(800)
    // FIXME: we need QInputDeviceInfo to be complete with the locked!!
    , m_status(QuickUtils::instance().mouseAttached() ? Locked : Inactive)
    , m_pressed(false)
{
    /*
     * we cannot use setStyleName as that will trigger style loading
     * and the qmlEngine is not known at this phase of the of the initialization
     * Therefore we simply set the style name default. Style loading will
     * happen during component completion.
     */
    UCStyledItemBasePrivate::get(this)->styleDocument = "BottomEdgeHintStyle";

    // connect old stateChanged
    connect(this, &QQuickItem::stateChanged, this, &UCBottomEdgeHint::stateChanged);

    // connect to gesture detection
    connect(&m_gestureDetector, &GestureDetector::bottomUpSwipeDetected,
            this, &UCBottomEdgeHint::onBottomUpSwipeDetected);
    connect(&m_gestureDetector, &GestureDetector::statusChanged,
            this, &UCBottomEdgeHint::onGestureStatusChanged);

    // FIXME: use QInputDeviceInfo once available
    connect(&QuickUtils::instance(), &QuickUtils::mouseAttachedChanged, [this]() {
        setStatus(QuickUtils::instance().mouseAttached() ? Locked : Active);
        if (m_status == Active) {
            m_deactivationTimer.start(m_deactivateTimeout, this);
        }
    });

    // accept mouse events
    setAcceptedMouseButtons(Qt::LeftButton);
}

void UCBottomEdgeHint::itemChange(ItemChange change, const ItemChangeData &data)
{
    UCStyledItemBase::itemChange(change, data);
    if (change == ItemParentHasChanged) {
        QQmlProperty bottomAnchors(this, "anchors.bottom", qmlContext(this));
        if (data.item && !QQmlPropertyPrivate::binding(bottomAnchors)) {
            QQuickAnchors *anchors = QQuickItemPrivate::get(this)->anchors();
            anchors->setBottom(QQuickItemPrivate::get(data.item)->bottom());
        }
    }
}

void UCBottomEdgeHint::timerEvent(QTimerEvent *event)
{
    UCStyledItemBase::timerEvent(event);
    if (event->timerId() == m_deactivationTimer.timerId()) {
        setStatus(Inactive);
        m_deactivationTimer.stop();
    }
}

// handle clicked event when locked and enter or return is pressed
void UCBottomEdgeHint::keyPressEvent(QKeyEvent *event)
{
    UCStyledItemBase::keyPressEvent(event);
    if ((status() >= Active) && (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {
        Q_EMIT clicked();
    }
}

// handle gesture detection
void UCBottomEdgeHint::touchEvent(QTouchEvent *event)
{
    UCStyledItemBase::touchEvent(event);
    m_gestureDetector.handleTouchEvent(this, event);
}

// handle click event
void UCBottomEdgeHint::mousePressEvent(QMouseEvent *event)
{
    if (contains(event->localPos()) && (m_status >= Active)) {
        m_pressed = true;
    } else {
        UCStyledItemBase::mousePressEvent(event);
    }
}
void UCBottomEdgeHint::mouseReleaseEvent(QMouseEvent *event)
{
    UCStyledItemBase::mouseReleaseEvent(event);
    if (m_pressed && (m_status >= Active)) {
        Q_EMIT clicked();
    }
}

// watch gesture detection status changes
void UCBottomEdgeHint::onBottomUpSwipeDetected()
{
    m_deactivationTimer.stop();
    setStatus(Active);
}

void UCBottomEdgeHint::onGestureStatusChanged(GestureDetector::Status status)
{
    if (status == GestureDetector::Completed) {
        if (m_status == Active) {
            m_deactivationTimer.start(m_deactivateTimeout, this);
        }
    }
}

/*!
  \qmlsignal void BottomEdgeHint::clicked()
   This handler is called when there is a mouse click on the BottomEdgeHint
   and the BottomEdgeHint is not disabled.
*/

/*!
  \qmlproperty string BottomEdgeHint::text
  The label displayed by the BottomEdgeHint.
 */

/*!
  \qmlproperty url BottomEdgeHint::iconSource
  The icon displayed by the BottomEdgeHint.

  This is the URL of any image file.
  If both iconSource and \l iconName are defined, \l iconName will be ignored.
 */

/*!
  \qmlproperty string BottomEdgeHint::iconName
  The icon associated with the BottomEdgeHint in the icon theme.

  If both \l iconSource and iconName are defined, iconName will be ignored.
 */

/*!
  \qmlproperty Flickable BottomEdgeHint::flickable
  The property holds the flickable, which when flicked hides the hint.
  \e Hidden state is reached when this property is set to a Flickable
  which is flicking or moving. It is recommended to set the property
  when the hint is placed above a flickable content. Defaults to null.
  */
void UCBottomEdgeHint::setFlickable(QQuickFlickable *flickable)
{
    if (flickable == m_flickable) {
        return;
    }
    if (m_flickable) {
        disconnect(m_flickable, &QQuickFlickable::flickingChanged,
                   this, &UCBottomEdgeHint::handleFlickableActivation);
        disconnect(m_flickable, &QQuickFlickable::movingChanged,
                   this, &UCBottomEdgeHint::handleFlickableActivation);
        m_gestureDetector.removeItemFilter(m_flickable);
    }
    m_flickable = flickable;
    if (m_flickable) {
        connect(m_flickable, &QQuickFlickable::flickingChanged,
                this, &UCBottomEdgeHint::handleFlickableActivation, Qt::DirectConnection);
        connect(m_flickable, &QQuickFlickable::movingChanged,
                this, &UCBottomEdgeHint::handleFlickableActivation, Qt::DirectConnection);
        m_gestureDetector.setItemFilter(m_flickable);
    }
    Q_EMIT flickableChanged();
}

// flickable moves hide the hint only if the current status is not Locked
void UCBottomEdgeHint::handleFlickableActivation()
{
    if (m_status < Locked && !m_gestureDetector.isDetecting() && !m_deactivationTimer.isActive()) {
        bool moving = m_flickable->isFlicking() || m_flickable->isMoving();
        if (moving) {
            setStatus(Hidden);
        } else if (m_status == Hidden) {
            setStatus(Inactive);
        }
    }
}

/*!
  \qmlproperty string BottomEdgeHint::state
  \deprecated
  BottomEdgeHint can take 2 states of visibility: \e Hidden, \e Visible.
  \table
  \header
    \li State
    \li Description
  \row
    \li Hidden
    \li The hint is not shown at all and cannot be activated.
  \row
    \li Visible
    \li The hint is in a state where it is visible but not active. \l clicked
        signal is not emitted.
  \endtable

  Defaults to \e Visible.
 */
QString UCBottomEdgeHint::state() const
{
    return QQuickItem::state();
}
void UCBottomEdgeHint::setState(const QString &state)
{
    QQuickItem::setState(state);

    qmlInfo(this) << "Overloaded 'state' property deprecated, will be removed from 1.3 release. Use 'status' instead.";
    QQuickItem *style = UCStyledItemBasePrivate::get(this)->styleItem;
    if (!style) {
        return;
    }
    if (state == "Hidden") {
        setStatus(Hidden);
    }
    if (state == "Visible") {
        setStatus(Inactive);
    }
}

/*!
  \qmlproperty Status BottomEdgeHint::status
  The property represents the status of the hint. The property is writable so it
  can be set to any of the following values programatically:
  \table
  \header
    \li Status
    \li Description
  \row
    \li Hidden
    \li The hint is not shown. Equivalent with setting \e visible to \c false,
        however visuals may do animations when altering this property. It can
        only be set if the current status is not \e Locked.
  \row
    \li Inactive
    \li The hint is shown and inactive. Styles can represent this state with
        different visuals. When inactive, \l clicked signal cannot be emitted.
  \row
    \li Active
    \li The hint is shown and active, meaning \l clicked signal is emitted when
        clicked with mouse.
  \row
    \li Locked
    \li Similar to \e Active the hint is shown and active, but no automatic transition
        to any other state is allowed. This is relevant for style implementations.
  \endtable
  \note \e Locked status value is set automatically when the system detects a
  mouse attached. In this case any change into other state value than \e Locked
  is rejected.
  Defaults to
  \list
    \li Inactive if no mouse is attached or
    \li Locked if there is a mouse detected.
  \endlist
  */
UCBottomEdgeHint::Status UCBottomEdgeHint::status()
{
    // FIXME: we won't need this once we get the QInputDeviceInfo reporting mouse attach/detach
    if (QuickUtils::instance().mouseAttached()) {
        m_status = Locked;
    }
    return m_status;
}

void UCBottomEdgeHint::setStatus(Status status)
{
    // FIXME: we need QInputDeviceInfo to complete this!
    // cannot unlock if mouse is attached or we don't have touch screen available
    if (status == m_status || (status != Locked && QuickUtils::instance().mouseAttached())) {
        return;
    }
    m_status = status;
    // make sure we stop the deactivation timer if Inactive or Locked
    if (status != Active && m_deactivationTimer.isActive()) {
        m_deactivationTimer.stop();
    }
    Q_EMIT statusChanged();
}

/*!
 * \qmlproperty int BottomEdgeHint::deactivateTimeout
 * The property specifies the timeout interval in milliseconds the \l status
 * is set to \e Inactive after a gesture based activation. Gesture based activation
 * is only possible when mouse is not attached to the device. Defaults to 800
 * milliseconds.
 */

void UCBottomEdgeHint::setDeactivateTimeout(int timeout)
{
    if (timeout == m_deactivateTimeout || timeout < 0) {
        return;
    }
    m_deactivateTimeout = timeout;
    if (m_deactivationTimer.isActive()) {
        m_deactivationTimer.stop();
        m_deactivationTimer.start(m_deactivateTimeout, this);
    }
    Q_EMIT deactivateTimeoutChanged();
}