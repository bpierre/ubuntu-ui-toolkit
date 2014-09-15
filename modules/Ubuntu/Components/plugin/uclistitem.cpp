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
#include "uclistitemoptions.h"
#include "uclistitemoptions_p.h"
#include "ucubuntuanimation.h"
#include "propertychange_p.h"
#include "i18n.h"
#include "quickutils.h"
#include "plugin.h"
#include <QtQml/QQmlInfo>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickpositioners_p.h>

#include <QtQml/private/qqmlabstractbinding_p.h>
#define foreach Q_FOREACH //workaround to fix private includes
#include <QtQml/private/qqmlbinding_p.h>     // for QmlBinding
#undef foreach


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
        m_leftMargin = UCUnits::instance().gu(2);
    }
    if (!m_rightMarginChanged) {
        m_rightMargin = UCUnits::instance().gu(2);
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

QSGNode *UCListItemDivider::paint(QSGNode *paintNode, const QRectF &rect)
{
    if (m_visible && !m_lastItem && (m_gradient.size() > 0)) {
        QSGRectangleNode *rectNode = static_cast<QSGRectangleNode *>(paintNode);
        if (!rectNode) {
            rectNode = m_listItem->sceneGraphContext()->createRectangleNode();
        }
        rectNode->setRect(QRectF(m_leftMargin, rect.height() - m_thickness,
                                 rect.width() - m_leftMargin - m_rightMargin, m_thickness));
        rectNode->setGradientStops(m_gradient);
        rectNode->update();
        return rectNode;
    } else {
        delete paintNode;
        return 0;
    }
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
 * ListItemContent
 */
UCListItemContent::UCListItemContent(QQuickItem *parent)
    : QQuickItem(parent)
    , m_color(Qt::transparent)
    , m_pressedColor(Qt::yellow)
    , m_item(0)
{
    setFlag(QQuickItem::ItemHasContents);
    // catch theme palette changes
    connect(&UCTheme::instance(), &UCTheme::paletteChanged, this, &UCListItemContent::updateColors);
    updateColors();
}

UCListItemContent::~UCListItemContent()
{
}

void UCListItemContent::setColor(const QColor &color)
{
    if (m_color == color) {
        return;
    }
    m_color = color;
    update();
    Q_EMIT colorChanged();
}

void UCListItemContent::setPressedColor(const QColor &color)
{
    if (m_pressedColor == color) {
        return;
    }
    m_pressedColor = color;
    // no more theme change watch
    disconnect(&UCTheme::instance(), &UCTheme::paletteChanged, this, &UCListItemContent::updateColors);
    update();
    Q_EMIT pressedColorChanged();
}

void UCListItemContent::updateColors()
{
    m_pressedColor = getPaletteColor("selected", "background");
    update();
}


void UCListItemContent::itemChange(ItemChange change, const ItemChangeData &data)
{
    if (change == ItemParentHasChanged) {
        m_item = qobject_cast<UCListItem*>(data.item);
    }
    QQuickItem::itemChange(change, data);
}

QSGNode *UCListItemContent::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    UCListItemPrivate *dd = UCListItemPrivate::get(m_item);
    bool pressed = (dd && dd->pressed);
    bool selected = (dd && dd->selectable && dd->selected);
    QColor color = (pressed || selected) ? m_pressedColor : m_color;

    delete oldNode;
    if (width() <= 0 || height() <= 0 || (color.alpha() == 0)) {
        return 0;
    }

    QSGRectangleNode *rectNode = QQuickItemPrivate::get(this)->sceneGraphContext()->createRectangleNode();
    rectNode->setColor(color);
    rectNode->setRect(boundingRect());
    rectNode->update();
    return rectNode;
}


/******************************************************************************
 * ListItemBasePrivate
 */
UCListItemPrivate::UCListItemPrivate()
    : UCStyledItemBasePrivate()
    , pressed(false)
    , moved(false)
    , ready(false)
    , selectable(false)
    , selected(false)
    , index(-1)
    , xAxisMoveThresholdGU(1.5)
    , reboundAnimation(0)
    , flickableInteractive(0)
    , disabledOpacity(0)
    , contentItem(new UCListItemContent)
    , divider(new UCListItemDivider)
    , leadingOptions(0)
    , trailingOptions(0)
    , selectionPanel(0)
{
}
UCListItemPrivate::~UCListItemPrivate()
{
    delete flickableInteractive;
    delete disabledOpacity;
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
    QObject::connect(contentItem, &UCListItemContent::childrenChanged,
                     q, &UCListItem::childrenChanged);
    q->setFlag(QQuickItem::ItemHasContents);
    // turn activeFocusOnPress on
    activeFocusOnPress = true;
    setFocusable();

    // watch size change and set implicit size;
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(_q_updateSize()));
    _q_updateSize();

    // watch enabledChanged()
    QObject::connect(q, SIGNAL(enabledChanged()), q, SLOT(_q_dimmDisabled()), Qt::DirectConnection);

    // create rebound animation
    UCUbuntuAnimation animationCodes;
    reboundAnimation = new QQuickPropertyAnimation(q);
    reboundAnimation->setEasing(animationCodes.StandardEasing());
    reboundAnimation->setDuration(animationCodes.SnapDuration());
    reboundAnimation->setTargetObject(contentItem);
    reboundAnimation->setProperty("x");
    reboundAnimation->setAlwaysRunToEnd(true);
}

void UCListItemPrivate::setFocusable()
{
    // alsways accept mouse events
    Q_Q(UCListItem);
    q->setAcceptedMouseButtons(Qt::LeftButton | Qt::MiddleButton | Qt::RightButton);
    q->setFiltersChildMouseEvents(true);
}

void UCListItemPrivate::_q_dimmDisabled()
{
    Q_Q(UCListItem);
    if (q->isEnabled()) {
        PropertyChange::restore(disabledOpacity);
    } else if (opacity() != 0.5) {
        // this is the first time we need to create the property change
        if (!disabledOpacity) {
            disabledOpacity = new PropertyChange(q, "opacity");
        }
        PropertyChange::setValue(disabledOpacity, 0.5);
    }
}

void UCListItemPrivate::_q_rebound()
{
    setPressed(false);
    // initiate rebinding only if there were options tugged
    Q_Q(UCListItem);
    if (!UCListItemOptionsPrivate::isConnectedTo(leadingOptions, q) && !UCListItemOptionsPrivate::isConnectedTo(trailingOptions, q)) {
        return;
    }
    setMoved(false);
    //connect rebound completion so we can disconnect
    QObject::connect(reboundAnimation, SIGNAL(stopped()), q, SLOT(_q_completeRebinding()));
    // then rebound to zero
    reboundTo(0);
}
void UCListItemPrivate::_q_completeRebinding()
{
    Q_Q(UCListItem);
    // disconnect animation, otherwise snapping will disconnect the panel
    QObject::disconnect(reboundAnimation, SIGNAL(stopped()), q, SLOT(_q_completeRebinding()));
    // restore flickable's interactive and cleanup
    PropertyChange::restore(flickableInteractive);
    // disconnect options
    grabPanel(leadingOptions, false);
    grabPanel(trailingOptions, false);
}

void UCListItemPrivate::_q_updateIndex(QObject *ownerItem)
{
    Q_Q(UCListItem);
    if (!ownerItem) {
        ownerItem = q->sender();
    }
    Q_ASSERT(ownerItem);
    // update the index as well
    QQmlContext *context = qmlContext(q);
    if (context) {
        QVariant indexProperty = context->contextProperty("index");
        index = indexProperty.isValid() ? indexProperty.toInt() : -1;
    }
    divider->m_lastItem = ready && index == (ownerItem->property("count").toInt() - 1);
}

void UCListItemPrivate::_q_updateSelected()
{
    Q_Q(UCListItem);
    bool checked = selectionPanel->property("checked").toBool();
    q->setSelected(checked);
    contentItem->update();
}

// the function performs a cleanup on mouse release without any rebound animation
void UCListItemPrivate::cleanup()
{
    setPressed(false);
    setMoved(false);
    _q_completeRebinding();
}

void UCListItemPrivate::reboundTo(qreal x)
{
    reboundAnimation->setFrom(contentItem->x());
    reboundAnimation->setTo(x);
    reboundAnimation->restart();
}

// set pressed flag and update background
// called when units size changes
void UCListItemPrivate::_q_updateSize()
{
    Q_Q(UCListItem);
    QQuickItem *owner = flickable ? flickable : parentItem;
    q->setImplicitWidth(owner ? owner->width() : UCUnits::instance().gu(40));
    q->setImplicitHeight(UCUnits::instance().gu(6));
}

// set pressed flag and update contentItem
void UCListItemPrivate::setPressed(bool pressed)
{
    if (this->pressed != pressed) {
        this->pressed = pressed;
        suppressClick = false;
        contentItem->update();
        Q_Q(UCListItem);
        Q_EMIT q->pressedChanged();
    }
}
// toggles the moved flag and installs/removes event filter
void UCListItemPrivate::setMoved(bool moved)
{
    suppressClick = moved;
    if (this->moved == moved) {
        return;
    }
    this->moved = moved;
    Q_Q(UCListItem);
    QQuickWindow *window = q->window();
    if (!window) {
        return;
    }
    if (moved) {
        window->installEventFilter(q);
    } else {
        window->removeEventFilter(q);
    }
}

// sets the moved flag but also grabs the panels from the leading/trailing options
bool UCListItemPrivate::grabPanel(UCListItemOptions *optionsList, bool isMoved)
{
    Q_Q(UCListItem);
    if (isMoved) {
        bool grab = UCListItemOptionsPrivate::connectToListItem(optionsList, q, (optionsList == leadingOptions));
        if (grab) {
            PropertyChange::setValue(flickableInteractive, false);
        }
        return grab;
    } else {
        UCListItemOptionsPrivate::disconnectFromListItem(optionsList);
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
    UCListItemOptionsPrivate *leading = UCListItemOptionsPrivate::get(leadingOptions);
    UCListItemOptionsPrivate *trailing = UCListItemOptionsPrivate::get(trailingOptions);
    x += dx;
    // min cannot be less than the trailing's panel width
    qreal min = (trailing && trailing->panelItem) ? -trailing->panelItem->width() : 0;
    // max cannot be bigger than 0 or the leading's width in case we have leading panel
    qreal max = (leading && leading->panelItem) ? leading->panelItem->width() : 0;
    x = CLAMP(x, min, max);
}

void UCListItemPrivate::autoLeadingOptions()
{
    Q_Q(UCListItem);
    if (flickable && flickable->property("leadingOptions").isValid() && !leadingOptions) {
        QQmlProperty prop(flickable, "leadingOptions", qmlContext(flickable));
        QQmlAbstractBinding *binding = QQmlPropertyPrivate::binding(prop);
        // set the value first, then bind
        q->setLeadingOptions(prop.read().value<UCListItemOptions*>());
        if (binding) {
            // set the binding to our leadingOptions property
            QQmlProperty leadingOptionsProperty(q, "leadingOptions", qmlContext(q));
            QQmlAbstractBinding *oldBinding = QQmlPropertyPrivate::setBinding(leadingOptionsProperty, binding);
            if (oldBinding && (oldBinding != binding)) {
                oldBinding->destroy();
            }
        }
    }
}

QQuickItem *UCListItemPrivate::createSelectionPanel()
{
    Q_Q(UCListItem);
    if (!selectionPanel) {
        QUrl panelDocument = UbuntuComponentsPlugin::pluginUrl().
                resolved(QUrl::fromLocalFile("ListItemSelectablePanel.qml"));
        QQmlComponent component(qmlEngine(q), panelDocument);
        if (!component.isError()) {
            selectionPanel = qobject_cast<QQuickItem*>(component.beginCreate(qmlContext(q)));
            if (selectionPanel) {
                QQml_setParent_noEvent(selectionPanel, q);
                selectionPanel->setParentItem(q);
                selectionPanel->setVisible(false);
                selectionPanel->setProperty("checked", selected);
                // complete component creation
                component.completeCreate();
            }
        } else {
            qmlInfo(q) << component.errorString();
        }
    }
    return selectionPanel;
}
void UCListItemPrivate::toggleSelectionMode()
{
    if (!createSelectionPanel()) {
        return;
    }
    Q_Q(UCListItem);
    if (selectable) {
        // move and dimm content item
        selectionPanel->setVisible(true);
        reboundTo(selectionPanel->width());
        QObject::connect(selectionPanel, SIGNAL(checkedChanged()), q, SLOT(_q_updateSelected()));
    } else {
        // remove content item dimming and destroy selection panel as well
        reboundTo(0.0);
        selectionPanel->setVisible(false);
        QObject::disconnect(selectionPanel, SIGNAL(checkedChanged()), q, SLOT(_q_updateSelected()));
    }
    _q_updateSelected();
}

/*!
 * \qmltype ListItem
 * \instantiates UCListItem
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
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
 * \c contentItem is an essential part of the component. Beside the fact that it
 * holds all components and resources declared as child to ListItem, it can also
 * configure the color of the background when in normal mode or when pressed. Being
 * an item, all other properties can be accessed or altered, with the exception
 * of some:
 * \list A
 * \li do not alter \c x, \c y, \c width or \c height properties as those are
 *      controlled by the ListItem itself when leading or trailing options are
 *      revealed and thus will destroy your logic
 * \li never anchor left or right anchor lines as it will block revealing the options.
 * \endlist
 *
 * Each ListItem has a thin divider shown on the bottom of the component. This
 * divider can be configured through the \l divider grouped property, which can
 * configure its margins from the edges of the ListItem as well as its visibility.
 * When used in \c ListView or \l UbuntuListView, the last list item will not
 * show the divider no matter of the visible property value set. In other
 * circumstances declaring a \c count property in the ListItem's parent item
 * or Flickable can help applying the last item detection logic.
 * \qml
 * Column {
 *     width: units.gu(40)
 *     // bring count to Column from Repeater
 *     property alias count: repeater.count
 *     Repeater {
 *         model: 10
 *         ListItem {
 *             Label {
 *                 anchors.fill: parent
 *                 horizontalCenter: Text.AlignHCenter
 *                 verticalCenter: Text.AlignVCenter
 *                 text: "Item #" + modelData
 *             }
 *         }
 *     }
 * }
 * \endqml
 *
 * ListItem can handle options that can ge tugged from front ot right of the item.
 * These options are Action elements visualized in panels attached to the front
 * or to the end of the item, and are revealed by swiping the item horizontally.
 * The tug is started only after the mouse/touch move had passed a given threshold.
 * These options are configured through the \l leadingOptions as well as \l
 * trailingOptions properties.
 * \qml
 * ListItem {
 *     id: listItem
 *     leadingOptions: ListItemOptions {
 *         Action {
 *             iconName: "delete"
 *             onTriggered: listItem.destroy()
 *         }
 *     }
 *     trailingOptions: ListItemOptions {
 *         Action {
 *             iconName: "search"
 *             onTriggered: {
 *                 // do some search
 *             }
 *         }
 *     }
 * }
 * \endqml
 * \note A ListItem cannot use the same ListItemOption instance for both leading or
 * trailing options. If it is desired to have the same action present in both leading
 * and trailing options, one of the ListItemOption options list can use the other's
 * list. In the following example the list item can be deleted through both option
 * leading and trailing options:
 * \qml
 * ListItem {
 *     id: listItem
 *     leadingOptions: ListItemOptions {
 *         Action {
 *             iconName: "delete"
 *             onTriggered: listItem.destroy()
 *         }
 *     }
 *     trailingOptions: ListItemOptions {
 *         options: leadingOptions.options
 *     }
 * }
 * \endqml
 * \sa ListItemOptions
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

    // check if the parent ListView has leadingOptions defined, if yes, use it!
    d->autoLeadingOptions();
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
    if (width() <= 0 || height() <= 0 || !d->divider) {
        delete oldNode;
        return 0;
    }
    // paint divider
    return d->divider->paint(oldNode, boundingRect());
}

void UCListItem::mousePressEvent(QMouseEvent *event)
{
    UCStyledItemBase::mousePressEvent(event);
    Q_D(UCListItem);
    if (d->selectable || (!d->flickable.isNull() && d->flickable->isMoving())) {
        // while moving, we cannot select or tug any items
        return;
    }
    d->setPressed(true);
    d->lastPos = d->pressedPos = event->localPos();
    // connect the Flickable to know when to rebound
    d->listenToRebind(true);
    // accept the event so we get the rest of the events as well
    event->setAccepted(true);
}

void UCListItem::mouseReleaseEvent(QMouseEvent *event)
{
    UCStyledItemBase::mouseReleaseEvent(event);
    Q_D(UCListItem);
    if (d->selectable) {
        // no move is allowed while selectable mode is on
        return;
    }
    // set released
    if (d->pressed) {
        // disconnect the flickable
        d->listenToRebind(false);

        if (!d->suppressClick) {
            Q_EMIT clicked();
            d->_q_rebound();
        } else {
            // snap
            qreal snapPosition = 0.0;
            if (d->contentItem->x() < 0) {
                snapPosition = UCListItemOptionsPrivate::snap(d->trailingOptions);
            } else if (d->contentItem->x() > 0) {
                snapPosition = UCListItemOptionsPrivate::snap(d->leadingOptions);
            }
            if (d->contentItem->x() == 0.0) {
                // do a cleanup, no need to rebound, the item has been dragged back to 0
                d->cleanup();
            } else if (snapPosition == 0.0){
                d->_q_rebound();
            } else {
                d->reboundTo(snapPosition);
            }
        }
    }
    d->setPressed(false);
}

void UCListItem::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(UCListItem);
    UCStyledItemBase::mouseMoveEvent(event);

    if (d->selectable) {
        // no move is allowed while selectable mode is on
        return;
    }

    // accept the tugging only if the move is within the threshold
    bool leadingAttached = UCListItemOptionsPrivate::isConnectedTo(d->leadingOptions, this);
    bool trailingAttached = UCListItemOptionsPrivate::isConnectedTo(d->trailingOptions, this);
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
            leadingAttached = d->grabPanel(d->leadingOptions, true);
            trailingAttached = d->grabPanel(d->trailingOptions, true);
        }
    }

    if (leadingAttached || trailingAttached) {
        qreal x = d->contentItem->x();
        qreal dx = event->localPos().x() - d->lastPos.x();
        d->lastPos = event->localPos();

        if (dx) {
            // clamp X into allowed dragging area
            d->clampX(x, dx);
            // block flickable
            d->setMoved(true);
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
 * \qmlproperty ListItemOptions ListItem::leadingOptions
 *
 * The property holds the options and its configuration to be revealed when swiped
 * from left to right.
 *
 * \sa trailingOptions
 */
UCListItemOptions *UCListItem::leadingOptions() const
{
    Q_D(const UCListItem);
    return d->leadingOptions;
}
void UCListItem::setLeadingOptions(UCListItemOptions *options)
{
    Q_D(UCListItem);
    if (d->leadingOptions == options) {
        return;
    }
    d->leadingOptions = options;
    if (d->leadingOptions == d->trailingOptions && d->leadingOptions) {
        qmlInfo(this) << UbuntuI18n::tr("leadingOptions and trailingOptions cannot share the same object!");
    }
    Q_EMIT leadingOptionsChanged();
}

/*!
 * \qmlproperty ListItemOptions ListItem::trailingOptions
 *
 * The property holds the options and its configuration to be revealed when swiped
 * from right to left.
 *
 * \sa leadingOptions
 */
UCListItemOptions *UCListItem::trailingOptions() const
{
    Q_D(const UCListItem);
    return d->trailingOptions;
}
void UCListItem::setTrailingOptions(UCListItemOptions *options)
{
    Q_D(UCListItem);
    if (d->trailingOptions == options) {
        return;
    }
    d->trailingOptions = options;
    if (d->leadingOptions == d->trailingOptions && d->trailingOptions) {
        qmlInfo(this) << UbuntuI18n::tr("leadingOptions and trailingOptions cannot share the same object!");
    }
    Q_EMIT trailingOptionsChanged();
}

/*!
 * \qmlpropertygroup ::ListItem::contentItem
 * \qmlproperty color ListItem::contentItem.color
 * \qmlproperty color ListItem::contentItem.pressedColor
 *
 * contentItem holds the components placed on a ListItem. \c color configures
 * the color of the normal contentItem, and \c pressedColor configures the color
 * when pressed.
 */
UCListItemContent* UCListItem::contentItem() const
{
    Q_D(const UCListItem);
    return d->contentItem;
}

/*!
 * \qmlpropertygroup ::ListItem::divider
 * \qmlproperty bool ListItem::divider.visible
 * \qmlproperty real ListItem::divider.leftMargin
 * \qmlproperty real ListItem::divider.rightMargin
 *
 * This grouped property configures the thin divider shown in the bottom of the
 * component. Configures the visibility and the margins from the left and right
 * of the ListItem. When tugged (swiped left or right to reveal the options),
 * it is not moved together with the content.
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
 * \qmlproperty bool ListItem::selectable
 * The property drives whether a list item is selectable or not. When set, the item
 * will show a check box on the leading side hanving the content item pushed towards
 * trailing side and dimmed. The checkbox which will reflect and drive the \l selected
 * state.
 * Defaults to false.
 * \note it is recommended to be used with UbuntuListView which can drive all the
 * ListItem's selectable.
 */
bool UCListItem::selectable() const
{
    Q_D(const UCListItem);
    return d->selectable;
}
void UCListItem::setSelectable(bool selectable)
{
    Q_D(UCListItem);
    if (d->selectable == selectable) {
        return;
    }
    d->selectable = selectable;
    d->toggleSelectionMode();
    Q_EMIT selectableChanged();
}

/*!
 * \qmlproperty bool ListItem::selected
 * The property drives whether a list item is selected or not. While selected, the
 * ListItem is dimmed and cannot be tugged. The default value is false.
 */
bool UCListItem::selected() const
{
    Q_D(const UCListItem);
    return d->selected;
}
void UCListItem::setSelected(bool selected)
{
    Q_D(UCListItem);
    if (d->selected == selected) {
        return;
    }
    d->selected = selected;
    // update panel as well
    if (d->selectionPanel) {
        d->selectionPanel->setProperty("checked", d->selected);
    }
    Q_EMIT selectedChanged();
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
