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

#ifndef UCPAGEWRAPPER_P_H
#define UCPAGEWRAPPER_P_H

#include "ucpagetreenode_p.h"

class UCPageWrapper;
class UCPageWrapperIncubator;
class UCPageWrapperPrivate : public UCPageTreeNodePrivate
{
    Q_DECLARE_PUBLIC(UCPageWrapper)

public:
    UCPageWrapperPrivate ();
    ~UCPageWrapperPrivate ();
    void init();

    enum PropertyFlags {
        CustomPageWrapperActive = LastPageTreeNodeFlag
    };

    void initPage();
    void reset ();
    void activate   ();
    void deactivate ();
    void copyProperties (QObject *target);

    void setIncubator(UCPageWrapperIncubator* incubator);
    void onComponentStatusChanged(QQmlComponent *pageComponent);
    void onIncubatorStatusChanged();
    void onVisibleChanged();

    QVariant m_reference;
    QVariant m_properties;
    QObject* m_object;
    QQuickItem* m_parentPage;
    QQuickItem* m_parentWrapper;
    QQuickItem* m_pageHolder;
    UCPageWrapperIncubator* m_incubator;
    QQmlComponent *m_localComponent; //component was created by us
    int m_column;
    bool m_canDestroy:1;
    bool m_synchronous:1;
};




#endif // UCPAGEWRAPPER_P_H

