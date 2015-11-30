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

import QtQuick 2.4

/*!
    \qmltype PaletteValues
    \inqmlmodule Ubuntu.Components.Themes 1.0
    \ingroup theme-module
    \brief Color values used for a given widget state.
*/
QtObject {
    /*!
       Color applied to the background of the application.
    */
    property color background
    /*!
       Color applied to elements placed on top of the \l background color.
       Typically used for labels and images.
    */
    property color backgroundText
    /*!
       Color applied to secondary content on the \l background color.
       \since Ubuntu.Components.Themes 1.3
    */
    property color backgroundSecondaryText
    /*!
       Color applied to tertiary content on the \l background color.
       \since Ubuntu.Components.Themes 1.3
    */
    property color backgroundTertiaryText

    property color base
    /*!
       Color applied to elements placed on top of the \l base color.
       Typically used for labels and images.
    */
    property color baseText

    /*!
       Color applied to widgets on top of the base colour.
    */
    property color foreground
    /*!
       Color applied to elements placed on top of the \l foreground color.
       Typically used for labels and images.
    */
    property color foregroundText

    /*!
       Color applied to widgets on top of any layer, background or base.
       \since Ubuntu.Components.Themes 1.3
    */
    property color raised
    /*!
       Color applied to elements placed on top of the \l raised color.
       \since Ubuntu.Components.Themes 1.3
    */
    property color raisedText
    /*!
       Color applied to seconday elements placed on top of the \l raised color.
       \since Ubuntu.Components.Themes 1.3
    */
    property color raisedSecondaryText

    /*!
       Color applied to the background of widgets floating over other widgets.
       For example popovers, menus, dialogs.
    */
    property color overlay
    /*!
       Color applied to elements placed on top of the \l overlay color.
       Typically used for labels and images.
    */
    property color overlayText
    /*!
       Color applied to secondary elements placed on top of the \l overlay color.
       \since Ubuntu.Components.Themes 1.3
    */
    property color overlaySecondaryText

    /*!
       Colour applied to the background of text input fields.
    */
    property color field
    /*!
       Color applied to elements placed on top of the \l field color.
       Typically used for labels and images.
    */
    property color fieldText

    /*!
       Colour applied to the highlights.
       \since Ubuntu.Components.Themes 1.3
    */
    property color highlight
    /*!
       Color applied to elements placed on top of the \l highlight color.
       \since Ubuntu.Components.Themes 1.3
    */
    property color highlightText

    /*!
       Colour applied to the positive actions.
       \since Ubuntu.Components.Themes 1.3
    */
    property color positive
    /*!
       Color applied to elements placed on top of the \l positive color.
       \since Ubuntu.Components.Themes 1.3
    */
    property color positiveText

    /*!
       Colour applied to the negative actions.
       \since Ubuntu.Components.Themes 1.3
    */
    property color negative
    /*!
       Color applied to elements placed on top of the \l negative color.
       \since Ubuntu.Components.Themes 1.3
    */
    property color hegativeText

    /*!
       Colour applied to the activity colors. Typical example is progress indication,
       activity indication.
       \since Ubuntu.Components.Themes 1.3
    */
    property color activity
    /*!
       Color applied to elements placed on top of the \l activity color.
       \since Ubuntu.Components.Themes 1.3
    */
    property color actvityText

    /*!
       Color applied to selected text in editable components.
    */
    property color selection
    /*!
       Color applied to elements placed on top of the \l selection color.
       \since Ubuntu.Components.Themes 1.3
    */
    property color selectionText
}
