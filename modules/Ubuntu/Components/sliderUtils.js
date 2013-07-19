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

.pragma library

/*!
  The function returns the live value of the slider.
  */
function liveValue(item) {
    return item.__internals.liveValue;
}

/*!
  The function returns the normalized live value of the slider.
  */
function normalizedValue(item) {
    return item.__internals.normalizedValue;
}

/*!
  The function returns whether the slider is being pressed or not.
  */
function isPressed(item) {
    return item.__internals.pressed;
}
