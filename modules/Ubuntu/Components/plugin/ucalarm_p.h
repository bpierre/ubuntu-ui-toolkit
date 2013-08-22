/*
 * Copyright 2013 Canonical Ltd.
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

#ifndef UCALARM_P_H
#define UCALARM_P_H

#include "ucalarm.h"
#include "alarmmanager_p.h"
//#include "alarmmanager_p_p.h"
#include <QtCore/QDateTime>
#include <QtQml/QQmlListProperty>

class UCAlarmPrivate
{
    Q_DECLARE_PUBLIC(UCAlarm)
public:
    UCAlarmPrivate(UCAlarm *qq);
    ~UCAlarmPrivate();

    static UCAlarmPrivate *get(UCAlarm *alarm) {
        return alarm->d_func();
    }

    void setDefaults();

    UCAlarm* q_ptr;
    AlarmRequest *request;
    AlarmData rawData;
    int error;
    UCAlarm::Status status;

    void createRequest();

    void _q_syncStatus(int status, int error) {
        UCAlarm::Status alarmStatus = static_cast<UCAlarm::Status>(status);
        if (this->status != alarmStatus || this->error != error) {
            this->status = alarmStatus;
            this->error = error;

            Q_EMIT q_func()->statusChanged();
            Q_EMIT q_func()->errorChanged();
        }
    }
};

#endif // UUALARM_P_H
