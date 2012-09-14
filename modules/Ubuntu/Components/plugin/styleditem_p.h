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
 * Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
 */

#ifndef STYLEDITEM_P_H
#define STYLEDITEM_P_H

#include "styleditem.h"

class StyledItemPrivate {

    Q_DECLARE_PUBLIC(StyledItem)

public:
    StyledItemPrivate(StyledItem *qq);
    //~StyledItemPrivate(){}

    StyledItem *q_ptr;
    bool privateStyle;
    bool componentCompleted;
    Style *activeStyle;
    QString styleClass;
    QString localSelector;
    QString instanceId;

    // style identification
    QString hierarchy;

    void _q_updateCurrentStyle(const QString &state);

};

#endif // STYLEDITEM_P_H
