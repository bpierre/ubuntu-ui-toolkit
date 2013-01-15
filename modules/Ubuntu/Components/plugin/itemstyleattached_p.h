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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef ITEMSTYLEATTACHED_P_H
#define ITEMSTYLEATTACHED_P_H

#include "itemstyleattached.h"
#include "suffixtree_p.h"

class QQmlContext;
class ItemStyleAttachedPrivate {
    Q_DECLARE_PUBLIC(ItemStyleAttached)
public:
    ItemStyleAttachedPrivate(ItemStyleAttached *qq, QObject *attached);
    ~ItemStyleAttachedPrivate();

    ItemStyleAttached *q_ptr;
    QQuickItem *attachee;
    QObject *style;
    QQuickItem *delegate;
    SelectorNode styleData;
    Selector styleSelector;

    // internal members
    QQmlContext *componentContext;
    StyleTreeNode *styleRule;
    bool delayApplyingStyle;
    bool customStyle;
    bool customDelegate;
    bool connectedToEngine;

    bool updateStyleSelector();
    bool updateStyle();
    bool updateDelegate();
    void updateCurrentStyle();
    bool registerName(const QString &id);
    void listenThemeEngine();
    void _q_refreshStyle();
    void _q_reapplyStyling(QQuickItem *);

};

#endif // ITEMSTYLEATTACHED_P_H
