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
 */

import QtQuick 2.0

ListModel {
    property bool circular: (distance >= 11)
    property real narrowFormatLimit: 0.0
    property real shortFormatLimit: 0.0
    property real longFormatLimit: 0.0

    // local properties
    property int from
    property int to
    property int distance

    function reset(date, minimum, maximum) {
        clear();
        var modelDate = new Date(date);
        modelDate.setDate(1);

        // if maximum is invalid, we have full model (12 months to show)
        distance = to = maximum.isValid() ? minimum.monthsTo(maximum) : 11;
        if (to < 0 || to > 11) to = 11;
        from = (to < 11) ? minimum.getMonth() : 0;
        // fill the model
        for (var i = from; i <= from + to; i++) {
            modelDate.setMonth(i);
            append({"month": modelDate.getMonth()});
        }
    }

    function updateLimits(label, margin, locale) {
        label.text = '9999';
        narrowFormatLimit = label.paintedWidth + 2 * margin;
        shortFormatLimit = longFormatLimit = 0.0;
        for (var month = 0; month < 12; month++) {
            label.text = locale.monthName(month, Locale.LongFormat);
            shortFormatLimit = Math.max(label.paintedWidth + 2 * margin, shortFormatLimit);
            label.text = locale.monthName(month, Locale.LongFormat);
            longFormatLimit = Math.max(label.paintedWidth + 2 * margin, longFormatLimit);
        }
    }

    function indexOf(date) {
        return date.getMonth() - from;
    }

    function dateFromIndex(date, index) {
        var newDate = new Date(date);
        // check if the days are in the diff zone (29-31)
        var fromDay = newDate.getDate();
        // move the day to the 1st of the month so we don't overflow when setting the month
        newDate.setDate(1);
        newDate.setMonth(get(index).month);
        var maxDays = newDate.daysInMonth();
        // check whether the original day would overflow
        // and trim to the mont's maximum date
        newDate.setDate((fromDay > maxDays) ? maxDays : fromDay);
        return newDate;
    }

    function text(date, value, width, locale) {
        if (width >= longFormatLimit) {
            return locale.monthName(value, Locale.LongFormat);
        }

        if (width >= shortFormatLimit) {
            return locale.monthName(value, Locale.ShortFormat);
        }

        var thisDate = new Date(date);
        thisDate.setDate(1);
        thisDate.setMonth(value);
        return Qt.formatDate(thisDate, "MM");
    }
}
