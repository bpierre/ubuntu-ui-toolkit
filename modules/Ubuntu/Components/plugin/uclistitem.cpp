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

#include "ucunits.h"
#include "uctheme.h"
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "uclistitemactions.h"
#include "uclistitemactions_p.h"
#include "ucubuntuanimation.h"
#include "propertychange_p.h"
#include "i18n.h"
#include "quickutils.h"
#include <QtQml/QQmlInfo>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickpositioners_p.h>

#define MIN(x, y)           ((x < y) ? x : y)
#define MAX(x, y)           ((x > y) ? x : y)
#define CLAMP(v, min, max)  (min <= max) ? MAX(min, MIN(v, max)) : MAX(max, MIN(v, min))

QColor getPaletteColor(const char *profile, const char *color)
{
    QColor result;
    QObject *palette = UCTheme::instance().palette();
    if (palette) {
        QObject *paletteProfile = palette->property(profile).value<QObject*>();
        if (paletteProfile) {
            result = paletteProfile->property(color).value<QColor>();
        }
    }
    return result;
}
/******************************************************************************
 * Divider
 */
UCListItemDivider::UCListItemDivider(QObject *parent)
    : QObject(parent)
    , m_visible(true)
    , m_lastItem(false)
    , m_leftMarginChanged(false)
    , m_rightMarginChanged(false)
    , m_colorFromChanged(false)
    , m_colorToChanged(false)
    , m_thickness(0)
    , m_leftMargin(0)
    , m_rightMargin(0)
    , m_listItem(0)
{
    connect(&UCUnits::instance(), &UCUnits::gridUnitChanged, this, &UCListItemDivider::unitsChanged);
    connect(&UCTheme::instance(), &UCTheme::paletteChanged, this, &UCListItemDivider::paletteChanged);
    unitsChanged();
    paletteChanged();
}
UCListItemDivider::~UCListItemDivider()
{
}

void UCListItemDivider::init(UCListItem *listItem)
{
    QQml_setParent_noEvent(this, listItem);
    m_listItem = UCListItemPrivate::get(listItem);
}

void UCListItemDivider::unitsChanged()
{
    m_thickness = UCUnits::instance().dp(2);
    if (!m_leftMarginChanged) {
        m_leftMargin = UCUnits::instance().dp(2);
    }
    if (!m_rightMarginChanged) {
        m_rightMargin = UCUnits::instance().dp(2);
    }
    if (m_listItem) {
        m_listItem->update();
    }
}

void UCListItemDivider::paletteChanged()
{
    QColor background = getPaletteColor("normal", "background");
    if (!background.isValid()) {
        return;
    }
    // FIXME: we need a palette value for divider colors, till then base on the background
    // luminance
    if (!m_colorFromChanged || !m_colorToChanged) {
        qreal luminance = (background.red()*212 + background.green()*715 + background.blue()*73)/1000.0/255.0;
        bool lightBackground = (luminance > 0.85);
        if (!m_colorFromChanged) {
            m_colorFrom = lightBackground ? QColor("#26000000") : QColor("#26FFFFFF");
        }
        if (!m_colorToChanged) {
            m_colorTo = lightBackground ? QColor("#14FFFFFF") : QColor("#14000000");
        }
        updateGradient();
    }
}

void UCListItemDivider::updateGradient()
{
    m_gradient.clear();
    m_gradient.append(QGradientStop(0.0, m_colorFrom));
    m_gradient.append(QGradientStop(0.49, m_colorFrom));
    m_gradient.append(QGradientStop(0.5, m_colorTo));
    m_gradient.append(QGradientStop(1.0, m_colorTo));
    if (m_listItem) {
        m_listItem->update();
    }
}

QSGNode *UCListItemDivider::paint(QSGNode *node, const QRectF &rect)
{
    QSGRectangleNode *dividerNode = static_cast<QSGRectangleNode*>(node);
    if (m_visible && !m_lastItem && (m_gradient.size() > 0) && ((m_colorFrom.alphaF() >= (1.0f / 255.0f)) || (m_colorTo.alphaF() >= (1.0f / 255.0f)))) {
        if (!dividerNode) {
            dividerNode = m_listItem->sceneGraphContext()->createRectangleNode();
        }
        QRectF divider(m_leftMargin, rect.height() - m_thickness, rect.width() - m_leftMargin - m_rightMargin, m_thickness);
        dividerNode->setRect(divider);
        dividerNode->setGradientStops(m_gradient);
        dividerNode->update();
        return dividerNode;
    } else if (node) {
        // delete the node
        delete node;
    }
    return 0;
}

void UCListItemDivider::setVisible(bool visible)
{
    if (m_visible == visible) {
        return;
    }
    m_visible = visible;
    m_listItem->resize();
    m_listItem->update();
    Q_EMIT visibleChanged();
}

void UCListItemDivider::setLeftMargin(qreal leftMargin)
{
    if (m_leftMargin == leftMargin) {
        return;
    }
    m_leftMargin = leftMargin;
    m_leftMarginChanged = true;
    m_listItem->update();
    Q_EMIT leftMarginChanged();
}

void UCListItemDivider::setRightMargin(qreal rightMargin)
{
    if (m_rightMargin == rightMargin) {
        return;
    }
    m_rightMargin = rightMargin;
    m_rightMarginChanged = true;
    m_listItem->update();
    Q_EMIT rightMarginChanged();
}

void UCListItemDivider::setColorFrom(const QColor &color)
{
    if (m_colorFrom == color) {
        return;
    }
    m_colorFrom = color;
    m_colorFromChanged = true;
    updateGradient();
    Q_EMIT colorFromChanged();
}

void UCListItemDivider::setColorTo(const QColor &color)
{
    if (m_colorTo == color) {
        return;
    }
    m_colorTo = color;
    m_colorToChanged = true;
    updateGradient();
    Q_EMIT colorToChanged();
}

/******************************************************************************
 * ListItemPrivate
 */
UCListItemPrivate::UCListItemPrivate()
    : UCStyledItemBasePrivate()
    , pressed(false)
    , highlightColorChanged(false)
    , tugged(false)
    , ready(false)
    , contentMoving(false)
    , xAxisMoveThresholdGU(1.5)
    , overshootGU(2)
    , color(Qt::transparent)
    , highlightColor(Qt::transparent)
    , reboundAnimation(0)
    , flickableInteractive(0)
    , contentItem(new QQuickItem)
    , divider(new UCListItemDivider)
    , leadingActions(0)
    , trailingActions(0)
{
}
UCListItemPrivate::~UCListItemPrivate()
{
    delete flickableInteractive;
}

void UCListItemPrivate::init()
{
    Q_Q(UCListItem);
    contentItem->setObjectName("ListItemHolder");
    QQml_setParent_noEvent(contentItem, q);
    contentItem->setParentItem(q);
    divider->init(q);
    // content will be redirected to the contentItem, therefore we must report
    // children changes as it would come from the main component
    QObject::connect(contentItem, &QQuickItem::childrenChanged,
                     q, &UCListItem::childrenChanged);
    q->setFlag(QQuickItem::ItemHasContents);
    // turn activeFocusOnPress on
    q->setActiveFocusOnPress(true);

    // catch theme palette changes
    QObject::connect(&UCTheme::instance(), SIGNAL(paletteChanged()), q, SLOT(_q_updateColors()));
    _q_updateColors();

    // watch size change and set implicit size;
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(_q_updateSize()));
    _q_updateSize();

    // create rebound animation
    UCUbuntuAnimation animationCodes;
    reboundAnimation = new QQuickPropertyAnimation(q);
    QEasingCurve easing(QEasingCurve::OutElastic);
    easing.setPeriod(0.5);
    reboundAnimation->setEasing(easing);
    reboundAnimation->setDuration(animationCodes.BriskDuration());
    reboundAnimation->setTargetObject(contentItem);
    reboundAnimation->setProperty("x");
    reboundAnimation->setAlwaysRunToEnd(true);
}

void UCListItemPrivate::_q_updateColors()
{
    Q_Q(UCListItem);
    highlightColor = getPaletteColor("selected", "background");
    q->update();
}

void UCListItemPrivate::_q_rebound()
{
    setPressed(false);
    // initiate rebinding only if there were actions tugged
    Q_Q(UCListItem);
    if (!UCListItemActionsPrivate::isConnectedTo(leadingActions, q) && !UCListItemActionsPrivate::isConnectedTo(trailingActions, q)) {
        return;
    }
    setTugged(false);
    // connect rebound completion so we can disconnect the action lists
    // then rebound to zero
    reboundTo(0, "_q_completeRebinding()");
}
void UCListItemPrivate::_q_completeRebinding()
{
    // disconnect animation, otherwise snapping will disconnect the panel
    QObject::disconnect(reboundAnimation, 0, 0, 0);
    // restore flickable's interactive and cleanup
    PropertyChange::restore(flickableInteractive);
    // disconnect actions
    grabPanel(leadingActions, false);
    grabPanel(trailingActions, false);
    // set contentMoved to false
    setContentMoved(false);
}
void UCListItemPrivate::_q_completeSnapping()
{
    QObject::disconnect(reboundAnimation, 0, 0, 0);
    setContentMoved(false);
}

void UCListItemPrivate::_q_updateIndex(QObject *ownerItem)
{
    Q_Q(UCListItem);
    if (!ownerItem) {
        ownerItem = q->sender();
    }
    Q_ASSERT(ownerItem);
    // update the index as well
    int index = -1;
    QQmlContext *context = qmlContext(q);
    if (context) {
        QVariant indexProperty = context->contextProperty("index");
        index = indexProperty.isValid() ? indexProperty.toInt() : -1;
    }
    divider->m_lastItem = ready && index == (ownerItem->property("count").toInt() - 1);
}

int UCListItemPrivate::index()
{
    Q_Q(UCListItem);
    // is there an index context property?
    QQmlContext *context = qmlContext(q);
    QVariant index = context->contextProperty("index");
    return index.isValid() ? index.toInt() : -1;
}

/*!
 * \qmlproperty bool ListItem::moved
 * The property signals the move of the list item's content. It is set whenever
 * the content is tugged and reset when the snapping and rebounding animations
 * complete.
 *
 * \sa movingStarted, movingEnded
 */

/*!
 * \qmlsignal ListItem::movingStarted
 * Signal emitted when the moving of the list item content is started.
 */
/*!
 * \qmlsignal ListItem::movingEnded
 * Signal emitted when the moving of the list item content is ended.
 */
bool UCListItemPrivate::isMoving() const
{
    return contentMoving;
}
// the function drives the moving property
void UCListItemPrivate::setContentMoved(bool move)
{
    if (contentMoving == move) {
        return;
    }
    contentMoving = move;
    Q_Q(UCListItem);
    if (move) {
        Q_EMIT q->movingStarted();
    } else {
        Q_EMIT q->movingEnded();
    }
    Q_EMIT q->movingChanged();
}

// the function performs a prompt rebound on mouse release without any animation
void UCListItemPrivate::promptRebound()
{
    setPressed(false);
    setTugged(false);
    _q_completeRebinding();
}
// rebounds to a given x position, connecting a slot signature
void UCListItemPrivate::reboundTo(qreal x, const char *signature)
{
    if (signature) {
        const QMetaObject *moListItem = q_ptr->metaObject();
        const QMetaMethod slot = moListItem->method(moListItem->indexOfMethod(signature));
        const QMetaObject *moAnimation = reboundAnimation->metaObject();
        const QMetaMethod signal = moAnimation->method(moAnimation->indexOfSignal("stopped()"));
        QObject::connect(reboundAnimation, signal, q_ptr, slot);
    }
    reboundAnimation->setFrom(contentItem->x());
    reboundAnimation->setTo(x);
    reboundAnimation->restart();
    setContentMoved(true);
}

// set pressed flag and update background
// called when units size changes
void UCListItemPrivate::_q_updateSize()
{
    Q_Q(UCListItem);
    QQuickItem *owner = flickable ? flickable : parentItem;
    q->setImplicitWidth(owner ? owner->width() : UCUnits::instance().gu(40));
    q->setImplicitHeight(UCUnits::instance().gu(7));
}

// set pressed flag and update contentItem
void UCListItemPrivate::setPressed(bool pressed)
{
    if (this->pressed != pressed) {
        this->pressed = pressed;
        suppressClick = false;
        Q_Q(UCListItem);
        q->update();
        Q_EMIT q->pressedChanged();
    }
}
// toggles the tugged flag and installs/removes event filter
void UCListItemPrivate::setTugged(bool tugged)
{
    suppressClick = tugged;
    if (this->tugged == tugged) {
        return;
    }
    this->tugged = tugged;
    Q_Q(UCListItem);
    QQuickWindow *window = q->window();
    if (tugged) {
        window->installEventFilter(q);
    } else {
        window->removeEventFilter(q);
    }
}

// sets the tugged flag but also grabs the panels from the leading/trailing actions
bool UCListItemPrivate::grabPanel(UCListItemActions *actionsList, bool isTugged)
{
    Q_Q(UCListItem);
    if (isTugged) {
        bool grab = UCListItemActionsPrivate::connectToListItem(actionsList, q, (actionsList == leadingActions));
        if (grab) {
            PropertyChange::setValue(flickableInteractive, false);
        }
        return grab;
    } else {
        UCListItemActionsPrivate::disconnectFromListItem(actionsList);
        return false;
    }
}


// connects/disconnects from the Flickable anchestor to get notified when to do rebound
void UCListItemPrivate::listenToRebind(bool listen)
{
    if (flickable.isNull()) {
        return;
    }
    Q_Q(UCListItem);
    if (listen) {
        QObject::connect(flickable.data(), SIGNAL(movementStarted()), q, SLOT(_q_rebound()));
    } else {
        QObject::disconnect(flickable.data(), SIGNAL(movementStarted()), q, SLOT(_q_rebound()));
    }
}

void UCListItemPrivate::resize()
{
    Q_Q(UCListItem);
    QRectF rect(q->boundingRect());
    if (divider && divider->m_visible) {
        rect.setHeight(rect.height() - divider->m_thickness);
    }
    contentItem->setSize(rect.size());
}

void UCListItemPrivate::update()
{
    if (!ready) {
        return;
    }
    Q_Q(UCListItem);
    q->update();
}

void UCListItemPrivate::clampX(qreal &x, qreal dx)
{
    UCListItemActionsPrivate *leading = UCListItemActionsPrivate::get(leadingActions);
    UCListItemActionsPrivate *trailing = UCListItemActionsPrivate::get(trailingActions);
    x += dx;
    // min cannot be less than the trailing's panel width
    qreal min = (trailing && trailing->panelItem) ? -trailing->panelItem->width() - UCUnits::instance().gu(overshootGU): 0;
    // max cannot be bigger than 0 or the leading's width in case we have leading panel
    qreal max = (leading && leading->panelItem) ? leading->panelItem->width() + UCUnits::instance().gu(overshootGU): 0;
    x = CLAMP(x, min, max);
}

/*!
 * \qmltype ListItem
 * \instantiates UCListItem
 * \inqmlmodule Ubuntu.Components 1.2
 * \ingroup unstable-ubuntu-listitems
 * \since Ubuntu.Components 1.2
 * \brief The ListItem element provides Ubuntu design standards for list or grid
 * views.
 *
 * The component is dedicated to be used in designs with static or dynamic lists
 * (i.e. list views where each item's layout differs or in lists where the content
 * is determined by a given model, thus each element has the same layout). The
 * element does not define any specific layout, components can be placed in any
 * ways on it. However, when used in list views, the content must be carefully
 * chosen to in order to keep the kinetic behavior and the highest FPS possible.
 *
 * The component provides two color properties which configures the item's background
 * when normal or pressed. This can be configures through \l color and \l highlightColor
 * properties.
 *
 * \c contentItem holds all components and resources declared as child to ListItem.
 * Being an Item, all other properties can be accessed or altered, with the exception
 * of some:
 * \list A
 * \li do not alter \c x, \c y, \c width or \c height properties as those are
 *      controlled by the ListItem itself when leading or trailing actions are
 *      revealed and thus will destroy your logic
 * \li never anchor left or right anchor lines as it will block revealing the actions.
 * \endlist
 *
 * Each ListItem has a thin divider shown on the bottom of the component. This
 * divider can be configured through the \l divider grouped property, which can
 * configure its margins from the edges of the ListItem as well as its visibility.
 * When used in \c ListView or \l UbuntuListView, the last list item will not
 * show the divider no matter of the visible property value set.
 *
 * ListItem can handle actions that can get tugged from front to back of the item.
 * These actions are Action elements visualized in panels attached to the front
 * or to the back of the item, and are revealed by swiping the item horizontally.
 * The tug is started only after the mouse/touch move had passed a given threshold.
 * These actions are configured through the \l leadingActions as well as \l
 * trailingActions properties.
 * \qml
 * ListItem {
 *     id: listItem
 *     leadingActions: ListItemActions {
 *         actions: [
 *             Action {
 *                 iconName: "delete"
 *                 onTriggered: listItem.destroy()
 *             }
 *         ]
 *     }
 *     trailingActions: ListItemActions {
 *         actions: [
 *             Action {
 *                 iconName: "search"
 *                 onTriggered: {
 *                     // do some search
 *                 }
 *             }
 *         ]
 *     }
 * }
 * \endqml
 * \note When a list item is tugged, it automatically connects both leading and
 * trailing actions to the list item. This implies that a ListItem cannot use
 * the same ListItemActions instance for both leading and trailing actions. If
 * it is desired to have the same action present in both leading and trailing
 * actions, one of the ListItemActions actions list can use the other's list. In
 * the following example the list item can be deleted through both leading and
 * trailing actions:
 * \qml
 * ListItem {
 *     id: listItem
 *     leadingActions: ListItemActions {
 *         actions: [
 *             Action {
 *                 iconName: "delete"
 *                 onTriggered: listItem.destroy()
 *             }
 *         ]
 *     }
 *     trailingActions: ListItemActions {
 *         actions: leadingActions.actions
 *     }
 * }
 * \endqml
 * \sa ListItemActions
 */

/*!
 * \qmlsignal ListItem::clicked()
 *
 * The signal is emitted when the component gets released while the \l pressed property
 * is set. The signal is not emitted if the ListItem content is tugged or when used in
 * Flickable (or ListView, GridView) and the Flickable gets moved.
 */
UCListItem::UCListItem(QQuickItem *parent)
    : UCStyledItemBase(*(new UCListItemPrivate), parent)
{
    Q_D(UCListItem);
    d->init();
}

UCListItem::~UCListItem()
{
}

void UCListItem::componentComplete()
{
    UCStyledItemBase::componentComplete();
    Q_D(UCListItem);
    d->ready = true;
    /* We only deal with ListView, as for other cases we would need to check the children
     * changes, which would have an enormous impact on performance in case of huge amount
     * of items. However, if the parent item, or Flickable declares a "count" property,
     * the ListItem will take use of it!
     */
    QQuickItem *countOwner = (d->flickable && d->flickable->property("count").isValid()) ?
                d->flickable :
                (d->parentItem && d->parentItem->property("count").isValid()) ? d->parentItem : 0;
    if (countOwner) {
        QObject::connect(countOwner, SIGNAL(countChanged()),
                         this, SLOT(_q_updateIndex()), Qt::DirectConnection);
        d->_q_updateIndex(countOwner);
    }
}

void UCListItem::itemChange(ItemChange change, const ItemChangeData &data)
{
    UCStyledItemBase::itemChange(change, data);
    if (change == ItemParentHasChanged) {
        Q_D(UCListItem);
        // make sure we are not connected to the previous Flickable
        d->listenToRebind(false);
        // check if we are in a positioner, and if that positioner is in a Flickable
        QQuickBasePositioner *positioner = qobject_cast<QQuickBasePositioner*>(data.item);
        if (positioner && positioner->parentItem()) {
            d->flickable = qobject_cast<QQuickFlickable*>(positioner->parentItem()->parentItem());
        } else if (data.item && data.item->parentItem()){
            // check if we are in a Flickable then
            d->flickable = qobject_cast<QQuickFlickable*>(data.item->parentItem());
        }

        if (d->flickable) {
            // create the flickableInteractive property change now
            d->flickableInteractive = new PropertyChange(d->flickable, "interactive");
            // connect to flickable to get width changes
            QObject::connect(d->flickable, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()));
        } else if (data.item) {
            QObject::connect(data.item, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()));
        } else {
            // in case we had a flickableInteractive property change active, destroy it
            if (d->flickableInteractive) {
                delete d->flickableInteractive;
                d->flickableInteractive = 0;
            }
            // mar as not ready, so no action should be performed which depends on readyness
            d->ready = false;
        }

        // update size
        d->_q_updateSize();
    }
}

void UCListItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    UCStyledItemBase::geometryChanged(newGeometry, oldGeometry);
    // resize contentItem item
    Q_D(UCListItem);
    d->resize();
}

QSGNode *UCListItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    Q_D(UCListItem);
    QColor color = d->pressed ? d->highlightColor : d->color;

    if (width() <= 0 || height() <= 0) {
        delete oldNode;
        return 0;
    }

    QSGRectangleNode *rectNode = 0;
    rectNode = static_cast<QSGRectangleNode*>(oldNode);
    if (!rectNode) {
        rectNode = QQuickItemPrivate::get(this)->sceneGraphContext()->createRectangleNode();
    }
    if (color.alphaF() >= (1.0f / 255.0f)) {
        rectNode->setColor(color);
        // cover only the area of the contentItem
        rectNode->setRect(d->contentItem->boundingRect());
        rectNode->setGradientStops(QGradientStops());
        rectNode->setAntialiasing(true);
        rectNode->setAntialiasing(false);
        rectNode->update();
    } else {
        // delete node, this will delete the divider node as well
        delete rectNode;
        rectNode = 0;
    }
    oldNode = rectNode;
    QSGNode *dividerNode = oldNode ? oldNode->childAtIndex(0) : 0;
    if (d->divider && d->divider->m_visible) {
        QSGNode *newNode = d->divider->paint(dividerNode, boundingRect());
        if (newNode != dividerNode && oldNode) {
            if (dividerNode) {
                oldNode->removeChildNode(dividerNode);
            }
            if (newNode) {
                oldNode->appendChildNode(newNode);
            }
        }
        if (!oldNode) {
            oldNode = newNode;
        }
    } else if (dividerNode) {
        // the divider painter node may be still added as child, so remove it
        oldNode->removeChildNode(dividerNode);
    }
    return oldNode;
}

void UCListItem::mousePressEvent(QMouseEvent *event)
{
    UCStyledItemBase::mousePressEvent(event);
    Q_D(UCListItem);
    if (!d->flickable.isNull() && d->flickable->isMoving()) {
        // while moving, we cannot select or tug any items
        return;
    }
    if (event->button() == Qt::LeftButton) {
        d->setPressed(true);
        d->lastPos = d->pressedPos = event->localPos();
        // connect the Flickable to know when to rebound
        d->listenToRebind(true);
        // accept the event so we get the rest of the events as well
        event->setAccepted(true);
    }
}

void UCListItem::mouseReleaseEvent(QMouseEvent *event)
{
    UCStyledItemBase::mouseReleaseEvent(event);
    Q_D(UCListItem);
    // set released
    if (d->pressed) {
        // disconnect the flickable
        d->listenToRebind(false);

        d->setContentMoved(true);
        if (!d->suppressClick) {
            Q_EMIT clicked();
            d->_q_rebound();
        } else {
            // snap
            qreal snapPosition = 0.0;
            if (d->contentItem->x() < 0) {
                snapPosition = UCListItemActionsPrivate::snap(d->trailingActions);
            } else if (d->contentItem->x() > 0) {
                snapPosition = UCListItemActionsPrivate::snap(d->leadingActions);
            }
            if (d->contentItem->x() == 0.0) {
                // do a cleanup, no need to rebound, the item has been dragged back to 0
                d->promptRebound();
            } else if (snapPosition == 0.0){
                d->_q_rebound();
            } else {
                d->reboundTo(snapPosition, "_q_completeSnapping()");
            }
        }
    }
    d->setPressed(false);
}

void UCListItem::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(UCListItem);
    UCStyledItemBase::mouseMoveEvent(event);

    // accept the tugging only if the move is within the threshold
    bool leadingAttached = UCListItemActionsPrivate::isConnectedTo(d->leadingActions, this);
    bool trailingAttached = UCListItemActionsPrivate::isConnectedTo(d->trailingActions, this);
    if (d->pressed && !(leadingAttached || trailingAttached)) {
        // check if we can initiate the drag at all
        // only X direction matters, if Y-direction leaves the threshold, but X not, the tug is not valid
        qreal threshold = UCUnits::instance().gu(d->xAxisMoveThresholdGU);
        qreal mouseX = event->localPos().x();
        qreal pressedX = d->pressedPos.x();

        if ((mouseX < (pressedX - threshold)) || (mouseX > (pressedX + threshold))) {
            // the press went out of the threshold area, enable move, if the direction allows it
            d->lastPos = event->localPos();
            // connect both panels
            leadingAttached = d->grabPanel(d->leadingActions, true);
            trailingAttached = d->grabPanel(d->trailingActions, true);
        }
    }

    if (leadingAttached || trailingAttached) {
        qreal x = d->contentItem->x();
        qreal dx = event->localPos().x() - d->lastPos.x();
        d->lastPos = event->localPos();

        if (dx) {
            d->setContentMoved(true);
            // clamp X into allowed dragging area
            d->clampX(x, dx);
            // block flickable
            d->setTugged(true);
            d->contentItem->setX(x);
        }
    }
}

bool UCListItem::eventFilter(QObject *target, QEvent *event)
{
    QPointF myPos;
    // only filter press events, and rebound when pressed outside
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        QQuickWindow *window = qobject_cast<QQuickWindow*>(target);
        if (window) {
            myPos = window->contentItem()->mapToItem(this, mouse->localPos());
        }
    } else if (event->type() == QEvent::TouchBegin) {
        QTouchEvent *touch = static_cast<QTouchEvent*>(event);
        QQuickWindow *window = qobject_cast<QQuickWindow*>(target);
        if (window) {
            myPos = window->contentItem()->mapToItem(this, touch->touchPoints()[0].pos());
        }
    }
    if (!myPos.isNull() && !contains(myPos)) {
        Q_D(UCListItem);
        d->_q_rebound();
        // only accept event, but let it be handled by the underlying or surrounding Flickables
        event->accept();
    }
    return UCStyledItemBase::eventFilter(target, event);
}

/*!
 * \qmlproperty ListItemActions ListItem::leadingActions
 *
 * The property holds the actions and its configuration to be revealed when swiped
 * from left to right.
 *
 * \sa trailingActions
 */
UCListItemActions *UCListItem::leadingActions() const
{
    Q_D(const UCListItem);
    return d->leadingActions;
}
void UCListItem::setLeadingActions(UCListItemActions *actions)
{
    Q_D(UCListItem);
    if (d->leadingActions == actions) {
        return;
    }
    // snap out before we change the actions
    d->promptRebound();
    // then delete panelItem
    if (d->leadingActions) {
        UCListItemActionsPrivate *list = UCListItemActionsPrivate::get(d->leadingActions);
        delete list->panelItem;
        list->panelItem = 0;
    }
    d->leadingActions = actions;
    if (d->leadingActions == d->trailingActions && d->leadingActions) {
        qmlInfo(this) << UbuntuI18n::tr("leadingActions and trailingActions cannot share the same object!");
    }
    Q_EMIT leadingActionsChanged();
}

/*!
 * \qmlproperty ListItemActions ListItem::trailingActions
 *
 * The property holds the actions and its configuration to be revealed when swiped
 * from right to left.
 *
 * \sa leadingActions
 */
UCListItemActions *UCListItem::trailingActions() const
{
    Q_D(const UCListItem);
    return d->trailingActions;
}
void UCListItem::setTrailingActions(UCListItemActions *actions)
{
    Q_D(UCListItem);
    if (d->trailingActions == actions) {
        return;
    }
    // snap out before we change the actions
    d->promptRebound();
    // then delete panelItem
    if (d->trailingActions) {
        UCListItemActionsPrivate *list = UCListItemActionsPrivate::get(d->trailingActions);
        delete list->panelItem;
        list->panelItem = 0;
    }
    d->trailingActions = actions;
    if (d->leadingActions == d->trailingActions && d->trailingActions) {
        qmlInfo(this) << UbuntuI18n::tr("leadingActions and trailingActions cannot share the same object!");
    }
    Q_EMIT trailingActionsChanged();
}

/*!
 * \qmlproperty Item ListItem::contentItem
 *
 * contentItem holds the components placed on a ListItem.
 */
QQuickItem* UCListItem::contentItem() const
{
    Q_D(const UCListItem);
    return d->contentItem;
}

/*!
 * \qmlpropertygroup ::ListItem::divider
 * \qmlproperty bool ListItem::divider.visible
 * \qmlproperty real ListItem::divider.leftMargin
 * \qmlproperty real ListItem::divider.rightMargin
 * \qmlproperty real ListItem::divider.colorFrom
 * \qmlproperty real ListItem::divider.colorTo
 *
 * This grouped property configures the thin divider shown in the bottom of the
 * component. Configures the visibility and the margins from the left and right
 * of the ListItem. When tugged (swiped left or right to reveal the actions),
 * it is not moved together with the content. \c colorFrom and \c colorTo configure
 * the starting and ending colors of the divider.
 *
 * When \c visible is true, the ListItem's content size gets thinner with the
 * divider's \c thickness.
 *
 * The default values for the properties are:
 * \list
 * \li \c visible: true
 * \li \c leftMargin: 2 GU
 * \li \c rightMargin: 2 GU
 * \endlist
 */
UCListItemDivider* UCListItem::divider() const
{
    Q_D(const UCListItem);
    return d->divider;
}

/*!
 * \qmlproperty bool ListItem::pressed
 * True when the item is pressed. The items stays pressed when the mouse or touch
 * is moved horizontally. When in Flickable (or ListView), the item gets un-pressed
 * (false) when the mouse or touch is moved towards the vertical direction causing
 * the flickable to move.
 */
bool UCListItem::pressed() const
{
    Q_D(const UCListItem);
    return d->pressed;
}

/*!
 * \qmlproperty color ListItem::color
 * Configures the color of the normal background. The default value is transparent.
 */
QColor UCListItem::color() const
{
    Q_D(const UCListItem);
    return d->color;
}
void UCListItem::setColor(const QColor &color)
{
    Q_D(UCListItem);
    if (d->color == color) {
        return;
    }
    d->color = color;
    update();
    Q_EMIT colorChanged();
}

/*!
 * \qmlproperty color ListItem::highlightColor
 * Configures the color when pressed. Defaults to the theme palette's background color.
 */
QColor UCListItem::highlightColor() const
{
    Q_D(const UCListItem);
    return d->highlightColor;
}
void UCListItem::setHighlightColor(const QColor &color)
{
    Q_D(UCListItem);
    if (d->highlightColor == color) {
        return;
    }
    d->highlightColor = color;
    // no more theme change watch
    disconnect(&UCTheme::instance(), SIGNAL(paletteChanged()), this, SLOT(_q_updateColors()));
    update();
    Q_EMIT highlightColorChanged();
}

/*!
 * \qmlproperty list<Object> ListItem::data
 * \default
 * Overloaded default property containing all the children and resources.
 */
QQmlListProperty<QObject> UCListItem::data()
{
    Q_D(UCListItem);
    return QQuickItemPrivate::get(d->contentItem)->data();
}

/*!
 * \qmlproperty list<Item> ListItem::children
 * Overloaded default property containing all the visible children of the item.
 */
QQmlListProperty<QQuickItem> UCListItem::children()
{
    Q_D(UCListItem);
    return QQuickItemPrivate::get(d->contentItem)->children();
}

#include "moc_uclistitem.cpp"
