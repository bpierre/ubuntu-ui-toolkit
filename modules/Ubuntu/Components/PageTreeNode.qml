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
 */

import QtQuick 2.0

/*!
    \internal
    \qmltype PageTreeNode
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The common parent of \l Page, \l MainView, \l PageStack and \l Tabs.

    It is used to propagate properties such as \l header and \l toolbar from a
    \l MainView (the root node) to each \l Page (leaf node) in the tree.
*/
Item {
    id: node

    /*!
      \internal
      Used to determine whether an Item is a PageTreeNode
     */
    property bool __isPageTreeNode: true

    /*! \internal */
    onParentChanged: internal.updatePageTree()
    /*! \internal */
    Component.onCompleted: internal.updatePageTree()

    /*!
      The header of the node. Propagates down from the root node.
     */
    property Header header: node.parentNode ? node.parentNode.header : null

    /*!
      The toolbar of the node. Propagates down from the root node.
     */
    property Toolbar toolbar: node.parentNode ? node.parentNode.toolbar : null

    /*!
      At any time, there is exactly one path from the root node to a Page leaf node
      where all nodes are active. All other nodes are not active. This is used by
      \l Tabs and \l PageStack to determine which of multiple nodes in the Tabs or
      PageStack is the currently active one.
     */
    property bool active: node.parentNode ? node.parentNode.active : false

    /*!
      The \l PageStack that this Page has been pushed on, or null if it is not
      part of a PageStack. This value is automatically set for pages that are pushed
      on a PageStack, and propagates to its child nodes.
     */
    property Item pageStack: node.parentNode ? node.parentNode.pageStack : null

    /*!
      The parent node of the current node in the page tree.
     */
    property Item parentNode: null

    Item {
        id: internal
        function isPageTreeNode(object) {
            // FIXME: Use QuickUtils.className() when it becomes available.
            return (object && object.hasOwnProperty("__isPageTreeNode") && object.__isPageTreeNode);
        }

        /*!
          Return the parent node in the page tree, or null if the item is the root node or invalid.
         */
        function getParentPageTreeNode(item) {
            var node = null;
            if (item) {
                var i = item.parent;
                while (i) {
                    if (internal.isPageTreeNode(i)) {
                        node = i;
                        break;
                    }
                    i = i.parent;
                }
            }
            return node;
        }

        /*!
          Find the parent node.
         */
        function updatePageTree() {
            node.parentNode = internal.getParentPageTreeNode(node);
        }
    }
}
