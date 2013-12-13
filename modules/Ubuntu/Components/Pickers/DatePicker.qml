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
import Ubuntu.Components 0.1

/*!
    \qmltype DatePicker
    \inqmlmodule Ubuntu.Components.Pickers 0.1
    \ingroup ubuntu-pickers
    \brief DatePicker component provides date and time value picking functionality.

    DatePicker combines up to three Picker elements providing different date or time
    value selection possibilities. It can be used to select full date (year, month,
    day), full time (hours, minutes, seconds) as well as to select a combination of
    year and month, month and day, hours and minutes, minutes and seconds or individual
    time units (i.e. year, month or day as well as hours, minutes or seconds). The
    selected date as well as the initial one is provided by the \l date property.
    For convenience the component provides also the \a year, \a month, \a day,
    \a week, \a hours, \a minutes and \a seconds values as separate properties,
    however these properties are not writable, and their initialization can happen
    only through the \l date property.

    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1
    import Ubuntu.Components.Pickers 0.1

    Column {
        Label {
            text: "Selected date: W" + datePicker.week + " - " +
                    Qt.formatDate(datePicker.date, "dddd, dd-mmmm-yyyy")
        }
        DatePicker {
            id: datePicker
        }
    }
    \endqml

    The \l mode property specifies what time units should be shown by the picker.
    The property holds a string, combining \b Year, \b Month, \b Day, \b Hours,
    \b Minutes and \b Seconds strings sepatared with '|' character. A DatePicker
    which shows only year and month date units would look as follows:
    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1
    import Ubuntu.Components.Pickers 0.1

    Column {
        Label {
            text: "Selected month: " + Qt.formatDate(datePicker.date, "mmmm-yyyy")
        }
        DatePicker {
            id: datePicker
            mode: "Years|Months"
        }
    }
    \endqml

    The \b mode of the DatePicker is set to date picking. In case tiume picking
    is needed, the model should be set to contain the time specific mode flags.
    The following example demonstrates how to use DatePicker for time picking.
    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1
    import Ubuntu.Components.Pickers 0.1

    Column {
        Label {
            text: "Selected time: " + Qt.formatTime(datePicker.date, "hh:mm:ss")
        }
        DatePicker {
            id: datePicker
            mode: "Hours|Minutes|Seconds"
        }
    }
    \endqml
    Note that the order the flags are specified does not influence the order the
    pickers are arranged. That is driven by the date format of the \l locale used
    in the picker. Also, date and time unit flags cannot be combined, therefore
    the component can be used either for date units or for time units.

    The default interval the date values are chosen is a window starting at
    the current date ending 50 years later. This window is defined by the
    \a minimum and \a maximum properties. The interval can be altered considering
    the following rules:
    \list
        \li - \a minimum must be less or equal than the \l date; if the \a date
                value is less than the given \a minimum, the date will be set to
                the minimum's value
        \li - \a maximum value must be greater than the \a minimum, or invalid.
                When the maximum is smaller than the \l date, the \l date property
                will be updated to get the maximum value.
                When set to invalid date (see DateUtils getInvalidDate()), the
                upper limit of the date interval becomes infinite, meaning the
                year picker will extend infinitelly. This leads to increased
                memory use and should be avoided if possible. Invalid date will
                make hours picker presenting 24 hours.
    \endlist
    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1
    import Ubuntu.Components.Pickers 0.1

    Column {
        Label {
            text: "Selected date: " + Qt.formatDate(datePicker.date, "dddd, dd-mmmm-yyyy")
        }
        DatePicker {
            id: datePicker
            minimum: {
                var d = new Date();
                d.setFullYear(d.getFullYear() - 1);
                return d;
            }
            maximum: Date.prototype.getInvalidDate.call()
        }
    }
    \endqml
    \b Note: do not use the \l date property when initializing minimum and maximum
    as it will lead in binding loops.

    \section2 Layout
    As mentioned earlier, DatePicker combines up to three Picker tumblers depending
    on the mode requested. These tumblers are laid out in a row in the order the
    default date format of the \l locale is.

    \section3 Date picker layout rules
    The date picker consist of three pickers: year, month, and date. The exact
    contents of the month and date pickers depends on the available width:
    \list
        \li full name for month, number and full day for date (“August” “28 Wednesday”)
        \li otherwise full name for month, number and abbreviated day
            for date (“August” “28 Wed”);
        \li otherwise full name for month, number for date (“August” “28”);
        \li otherwise abbreviated name for month, number for date (“Aug” “28”).
        \li otherwise number for month, number for date (“08” “28”).
    \endlist

    \a{If the currently selected date becomes impossible due to year change (from a
    leap to a non-leap year when the date is set to February 29) or month change
    (e.g. from a month that has 31 days to one that has fewer when the date is
    set to 31), the date reduces automatically to the last day of the month (i.e
    February 28 or 30th day of the month).}

    \section3 Time picker layout rules
    Time units are shown in fixed width picker tumblers, numbers padded with
    leading zeroes. There is no other special rule on the formatting of the time
    unit numbers.

    \section3 How minimum/maximum affects the tumblers

    If minimum and maximum are within the same year, the year picker will be
    insensitive. If minimum and maximum are within the same month, the month picker
    will also be insensitive.
  */
StyledItem {
    id: datePicker

    /*!
      Specifies what kind of date value selectors should be shown by the picker.
      This is a string of 'flags' separated by '|' separator, where flags are:
      \table
        \header
        \li {2, 1} Date picker modes
            \header
                \li Value
                \li Description
            \row
                \li Year
                \li Specifies to show the year picker
            \row
                \li Month
                \li Specifies to show the month picker
            \row
                \li Day
                \li Specifies to show the day picker
        \header
        \li {2, 1} Time picker modes
            \header
                \li Value
                \li Description
            \row
                \li Hours
                \li Specifies to show the hours picker
            \row
                \li Minutes
                \li Specifies to show the minutes picker
            \row
                \li Seconds
                \li Specifies to show the seconds picker
      \endtable
      With some exceptions, any combination of these flags is allowed within the
      same group. Date and timme picker modes cannot be combined.
      <br />
      When date picker modes are used, the combination of "Year|Day" is not allowed.
      In time picker modes case the "Hours|Seconds" combination is forbidden.
      <br />
      The default value is "Years|Months|Days".
      */
    property string mode: "Years|Months|Days"

    /*!
      The date chosen by the DatePicker. The default value is the date at the
      component creation time. The property automatically updates year, month
      and day properties.
      */
    property date date: Date.prototype.midnight.call(new Date())

    /*!
      \qmlproperty int minimum
      \qmlproperty int maximum
      The minimum and maximum dates (inclusive) to be shown in the picker.
      Both year and month values will be considered from the properties.

      The year and month picker values are filled based on these values. The
      year picker will be infinite (extending infinitely) if the maximum is
      an invalid date. If the distance between maximum and minimum is less than
      a year, the year picker will be shown disabled.

      The month picker will be circular if the distance between maximum and minimum
      is at least one year, or if the maximum date is invalid.

      The default values are the current date for the minimum, and 50 year distance
      value for maximum.
      */
    property date minimum: Date.prototype.midnight.call(new Date())
    /*! \internal */
    property date maximum: {
        var d = Date.prototype.midnight.call(new Date());
        d.setFullYear(d.getFullYear() + 50);
        return d;
    }

    /*!
      \qmlproperty int year
      \readonly
      \qmlproperty int month
      \readonly
      \qmlproperty int day
      \readonly
      \qmlproperty int week
      \readonly
      \qmlproperty int hours
      \readonly
      \qmlproperty int minutes
      \readonly
      \qmlproperty int seconds
      \readonly
      Properties declared for convenience, representing the \b year, \b month,
      \b day, \b week as well as \b hours, \b minutes and \b seconds values of
      the \l date property.
      */
    readonly property int year: datePicker.date.getFullYear()
    /*! \internal */
    readonly property int month: datePicker.date.getMonth()
    /*! \internal */
    readonly property int day: datePicker.date.getDate()
    /*! \internal */
    readonly property int week: datePicker.date.getWeek()
    /*! \internal */
    readonly property int hours: datePicker.date.getHours()
    /*! \internal */
    readonly property int minutes: datePicker.date.getMinutes()
    /*! \internal */
    readonly property int seconds: datePicker.date.getSeconds()

    /*!
      The property defines the locale used in the picker. The default value is
      the system locale.
      \qml
      DatePicker {
           locale: Qt.locale("hu_HU")
      }
      \endqml
      */
    property var locale: Qt.locale()

    implicitWidth: units.gu(36)
    implicitHeight: {
        var h = Qt.inputMethod.keyboardRectangle.height;
        return (h > 0) ? h : units.gu(20);
    }

    /*! \internal */
    onMinimumChanged: {
        if (internals.completed && !minimum.isValid()) {
            // set the minimum to the date
            minimum = date;
        }

        // adjust date
        if (date !== undefined && date < minimum && minimum.isValid() && internals.completed) {
            date = minimum;
        }
    }
    /*! \internal */
    onMaximumChanged: {
        // adjust date
        if (date !== undefined && date > maximum && maximum.isValid() && maximum > minimum  && internals.completed) {
            date = maximum;
        }
    }
    /*! \internal */
    onWidthChanged: {
        // use dayPicker narrowFormatLimit even if the dayPicker is hidden
        // and clamp the width so it cannot have less width that the sum of
        // the three tumblers' narrowFormatLimit
        var minWidth = 0.0;
        for (var i = 0; i < tumblerModel.count; i++) {
            minWidth += tumblerModel.get(i).pickerModel.narrowFormatLimit;
        }
        width = Math.max(width, minWidth);
    }
    /*! \internal */
    onModeChanged: internals.updatePickers()
    /*! \internal */
    onLocaleChanged: internals.updatePickers()

    Component.onCompleted: {
        if (minimum === undefined) {
            minimum = date;
        }
        internals.completed = true;
        internals.updatePickers();
    }

    // models
    YearModel {
        id: yearModel
        mainComponent: datePicker
        pickerCompleted: internals.completed && internals.showYearPicker
        pickerWidth: (!pickerItem) ? 0 : narrowFormatLimit
        function syncModels() {
            dayModel.syncModels();
        }
    }
    MonthModel {
        id: monthModel
        mainComponent: datePicker
        pickerCompleted: internals.completed && internals.showMonthPicker
        pickerWidth: {
            if (!pickerItem) {
                return 0;
            }
            return MathUtils.clamp(datePicker.width - yearModel.pickerWidth - dayModel.pickerWidth, narrowFormatLimit, longFormatLimit);
        }
        function syncModels() {
            dayModel.syncModels();
        }
    }
    DayModel {
        id: dayModel
        mainComponent: datePicker
        pickerCompleted: internals.completed && internals.showDayPicker
        pickerWidth: {
            if (!pickerItem) {
                return 0;
            }
            var w = Math.max((datePicker.width - yearModel.pickerWidth) * internals.dayPickerRatio, narrowFormatLimit);
            if (w < longFormatLimit && w >= shortFormatLimit) {
                return shortFormatLimit;
            }
            return w;
        }
    }
    HoursModel {
        id: hoursModel
        mainComponent: datePicker
        pickerCompleted: internals.completed && internals.showHoursPicker
        pickerWidth: {
            if (!pickerItem) {
                return 0;
            }
            return narrowFormatLimit;
        }
    }
    MinutesModel {
        id: minutesModel
        mainComponent: datePicker
        pickerCompleted: internals.completed && internals.showMinutesPicker
        pickerWidth: {
            if (!pickerItem) {
                return 0;
            }
            return narrowFormatLimit;
        }
    }
    SecondsModel {
        id: secondsModel
        mainComponent: datePicker
        pickerCompleted: internals.completed && internals.showSecondsPicker
        pickerWidth: {
            if (!pickerItem) {
                return 0;
            }
            return narrowFormatLimit;
        }
    }

    style: Theme.createStyleComponent("DatePickerStyle.qml", datePicker)
    Binding {
        target: __styleInstance
        property: "view"
        value: positioner
    }
    Binding {
        target: __styleInstance
        property: "pickerModels"
        value: tumblerModel
    }
    Binding {
        target: __styleInstance
        property: "unitSeparator"
        value: (internals.showHoursPicker || internals.showMinutesPicker || internals.showSecondsPicker) ?
                   ":" : ""
    }

    // tumbler positioner
    PickerRow {
        id: positioner
        parent: (datePicker.__styleInstance && datePicker.__styleInstance.hasOwnProperty("tumblerHolder")) ?
                    datePicker.__styleInstance.tumblerHolder : datePicker
        mainComponent: datePicker
        model: tumblerModel
        margins: internals.margin
        anchors {
            top: parent.top
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }
    // tumbler model
    ListModel {
        /*
              Model to hold tumbler order for repeaters.
              Roles:
              - pickerModel
              - pickerName
              */
        id: tumblerModel

        // the function checks whether a pickerModel was added or not
        // returns the index of the model object the pickerModel was found
        // or -1 on error.
        function pickerModelIndex(name) {
            for (var i = 0; i < count; i++) {
                if (get(i).pickerName === name) {
                    return i;
                }
            }
            return -1;
        }

        // the function checks whether a pickerModel is present in the list;
        // moves the existing one to the given index or inserts it if not present
        function setPickerModel(model, name, index) {
            var idx = pickerModelIndex(name);
            if (idx >= 0) {
                move(idx, index, 1);
            } else {
                append({"pickerModel": model, "pickerName": name});
            }
        }

        // removes the given picker
        function removePicker(name) {
            var idx = pickerModelIndex(name);
            if (idx >= 0) {
                remove(idx);
            }
        }
    }

    // component to calculate text fitting
    Label { id: textSizer; visible: false }
    QtObject {
        id: internals
        property bool completed: false
        property real margin: units.gu(1.5)
        property real dayPickerRatio: 0.1

        property bool showYearPicker: true
        property bool showMonthPicker: true
        property bool showDayPicker: true

        property bool showHoursPicker: false
        property bool showMinutesPicker: false
        property bool showSecondsPicker: false

        /*
          Update pickers.
          */
        function updatePickers() {
            if (completed) {
                // check mode flags first
                var modes = datePicker.mode.split(/\W/g);

                showYearPicker = (modes.indexOf("Years") >= 0);
                showMonthPicker = (modes.indexOf("Months") >= 0);
                showDayPicker = (modes.indexOf("Days") >= 0);
                // filter unaccepted date picking mode
                if (!showMonthPicker && showYearPicker && showDayPicker) {
                    console.error("Invalid DatePicker mode: " + datePicker.mode);
                    return;
                }

                showHoursPicker = (modes.indexOf("Hours") >= 0);
                showMinutesPicker = (modes.indexOf("Minutes") >= 0);
                showSecondsPicker = (modes.indexOf("Seconds") >= 0);
                // filter unaccepted time picking mode
                if (showHoursPicker && showSecondsPicker && !showMinutesPicker) {
                    console.error("Invalid DatePicker mode: " + datePicker.mode);
                    return;
                }

                // date and time picking not allowed at the same time
                if ((showYearPicker || showMonthPicker || showDayPicker) &&
                        (showHoursPicker || showMinutesPicker || showSecondsPicker)) {
                    console.error("Date and Time picking not allowed at the same time.");
                    return;
                }

                arrangeTumblers();
                resetPickers();
            }
        }

        /*
          Resets the pickers. Pickers will update their models with the given date,
          minimum and maximum values.
          */
        function resetPickers() {
            if (!completed) return;
            for (var i = 0; i < tumblerModel.count; i++) {
                var pickerItem = tumblerModel.get(i).pickerModel.pickerItem;
                pickerItem.resetPicker();
            }

            // calculate the ratio for the dayPicker
            var width = datePicker.width - yearModel.pickerWidth;
            dayPickerRatio = (dayModel.longFormatLimit / width).toPrecision(3);
        }

        /*
            Detects the tumbler order from the date format of the locale
          */
        function arrangeTumblers() {
            // disable completion so avoid accidental date changes
            completed = false;

            // use short format to exclude any extra characters
            var format = datePicker.locale.dateFormat(Locale.ShortFormat).split(/\W/g);
            // loop through the format to decide the position of the tumbler
            var formatIndex = 0;
            for (var i in format) {
                if (!format[i].length) continue;
                // check the first two characters
                switch (format[i].substr(0, 1).toLowerCase()) {
                case 'y':
                    if (showYearPicker) {
                        tumblerModel.setPickerModel(yearModel, "YearPicker", formatIndex);
                        formatIndex++;
                    } else {
                        tumblerModel.removePicker("YearPicker");
                    }

                    break;
                case 'm':
                    if (showMonthPicker) {
                        tumblerModel.setPickerModel(monthModel, "MonthPicker", formatIndex);
                        formatIndex++;
                    } else {
                        tumblerModel.removePicker("MonthPicker");
                    }

                    break;
                case 'd':
                    if (showDayPicker) {
                        tumblerModel.setPickerModel(dayModel, "DayPicker", formatIndex);
                        formatIndex++;
                    } else {
                        tumblerModel.removePicker("DayPicker");
                    }
                    break;
                }
            }
            // check hms
            if (showHoursPicker) {
                tumblerModel.setPickerModel(hoursModel, "HoursPicker", formatIndex);
                formatIndex++;
            } else {
                tumblerModel.removePicker("HoursPicker");
            }
            if (showMinutesPicker) {
                tumblerModel.setPickerModel(minutesModel, "MinutesPicker", formatIndex);
                formatIndex++;
            } else {
                tumblerModel.removePicker("MinutesPicker");
            }
            if (showSecondsPicker) {
                tumblerModel.setPickerModel(secondsModel, "SecondsPicker", formatIndex);
                formatIndex++;
            } else {
                tumblerModel.removePicker("SecondsPicker");
            }

            // re-enable completion
            completed = true;
        }
    }
}


