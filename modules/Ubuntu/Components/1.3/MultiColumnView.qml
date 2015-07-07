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

import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import "stack.js" as Stack

/*!
  \qmltype MultiColumnView
  \inqmlmodule Ubuntu.Components 1.3
  \since Ubuntu.Components 1.3
  \ingroup ubuntu
  \brief View with multiple columns of Pages.

  The component provides a flexible way of viewing a stack of pages in one or
  more columns. Unlike in PageStack, there can be more than one Page active at
  a time, depending on the number of the columns in the view.

  MultiColumnView stores pages added in a tree. Pages are added relative to a
  given page, either as sibling (\l addPageToCurrentColumn) or as child
  (\l addPageToNextColumn). This means that removing a non-leaf page from the Page
  tree will remove all its children from the page tree.

  The columns are populated from left to right. The column a page is added to is
  detected based on the source page that is given to the functions adding the page.
  The pages can be added either to the same column the source page resides or to
  the column next to the source page. Giving a null value to the source page will
  add the page to the leftmost column of the view.

  The primary page, the very first page must be specified through the \l primaryPage
  property. The property cannot be changed after component completion and can hold
  a Page instance, a Component or a url to a document defining a Page. The page
  cannot be removed from the view.

  \note Unlike PageStack or Page the component does not fill its parent by default.

  \qml
  import QtQuick 2.4
  import Ubuntu.Components 1.3

  MainView {
      width: units.gu(80)
      height: units.gu(71)

      MultiColumnView {
          anchors.fill: parent
          primaryPage: page1
          Page {
              id: page1
              title: "Main page"
              Column {
                  Button {
                      text: "Add Page2 above " + title
                      onClicked: page1.pageStack.addPageToCurrentColumn(page1, page2)
                  }
                  Button {
                      text: "Add Page3 next to " + title
                      onClicked: page1.pageStack.addPageToNextColumn(page1, page3)
                  }
              }
          }
          Page {
              id: page2
              title: "Page #2"
          }
          Page {
              id: page3
              title: "Page #3"
          }
      }
  }
  \endqml

  MultiColumnView supports adaptive column handling. When the number of columns changes at
  runtime the pages are automatically rearranged. To understand it better, let's take the following example:
  \qml
  import QtQuick 2.4
  import Ubuntu.Components 1.3

  MainView {
      width: units.gu(120)
      height: units.gu(71)

      MultiColumnView {
          anchors.fill: parent
          primaryPage: page1
          Page {
              id: page1
              title: "Main page"
              Button {
                  text: "Add Page2 next to " + title
                  onClicked: page1.pageStack.addPageToNextColumn(page1, page2)
              }
          }
          Page {
              id: page2
              title: "Page #2"
              Button {
                  text: "Add Page3 next to " + title
                  onClicked: page2.pageStack.addPageToNextColumn(page2, page3)
              }
          }
          Page {
              id: page3
              title: "Page #3"
          }
      }
  }
  \endqml

  When the code is run on sufficiently wide screen, like a desktop or TV,
  it will launch with multiple columns.

  \c page1 is set to be the primary page, \c page2 will be added to column next to
  \c page1 (to column 2) and \c page3 next to \c page2 (column 3). When the window
  is resized to have its size below 80 GU, the component will switch to 1 column
  mode, and \c page3 will be placed in the last column, and the header for \c page2
  will have a back button, indicating that there is a page below it. If the window
  is resized to contain only one column, all pages will be shown in that column, so
  the component will act as PageStack. Resizing the window back to 2 columns will place the pages side-by-side.

  \note In the above example if \c page2 is removed, that will remove all its child
  pages, meaning \c page3 will also be removed.

  \sa PageStack
*/

PageTreeNode {
    id: multiColumnView

    Page {
        // MultiColumnView has its own split headers, so
        //  disable the application header.
        id: appHeaderControlPage
        head {
            locked: true
            visible: false
        }
        // title is set in attachPage() when the attached Page.column === 0
    }

    /*!
      The property holds the first Page which will be added to the view. If the
      view has more than one column, the page will be added to the leftmost column.
      The property can hold either a Page instance, a component holding a Page
      or a QML document defining the Page. The property cannot be changed after
      component completion.
      */
    property var primaryPage

    /*!
      \qmlmethod Item addPageToCurrentColumn(Item sourcePage, var page[, var properties])
      Adds a \c page to the column the \c sourcePage resides in. \c page can be a
      Component or a file. \c properties is a JSON object containing properties
      to be set when page is created. \c sourcePage must be active. Returns the
      instance of the page created.
      */
    function addPageToCurrentColumn(sourcePage, page, properties) {
        return d.addPageToColumn(d.columnForPage(sourcePage), sourcePage, page, properties);
    }

    /*!
      \qmlmethod Item addPageToNextColumn(Item sourcePage, var page[, var properties])
      Same as \l addPageToCurrentColumn except that the \c page is added to the column
      next to the one the \c sourcePage resides. If \c sourcePage is null, the new
      page will be added to the leftmost column. If \c sourcePage is located in the
      rightmost column, the new page will be pushed to the same column as \c sourcePage.
      */
    function addPageToNextColumn(sourcePage, page, properties) {
        return d.addPageToColumn(d.columnForPage(sourcePage) + 1, sourcePage, page, properties);
    }

    /*!
      \qmlmethod void removePages(Item page)
      The function removes and deletes all pages up to and including \c page
      is reached. If the \a page is the same as the \l primaryPage, only its child
      pages will be removed.
      */
    function removePages(page) {
        // remove nodes which have page as ascendant
        var node = d.stack.top();
        while (node && node.childOf(page)) {
            d.popAndSetPageForColumn(node);
            node = d.stack.top();
        }
        while (node && node.object == page && node != d.stack.first()) {
            d.popAndSetPageForColumn(node);
            node = d.stack.top();
        }
    }

    /*
      internals
      */

    Component.onCompleted: {
        d.relayout();
        d.completed = true;
        if (primaryPage) {
            var wrapper = d.createWrapper(primaryPage);
            d.addWrappedPage(wrapper);
        } else {
            console.warn("No primary page set. No pages can be added without a primary page.");
        }
    }
    onPrimaryPageChanged: {
        if (d.completed) {
            console.warn("Cannot change primaryPage after completion.");
            return;
        }
    }

    QtObject {
        id: d

        property bool completed: false
        property var stack: new Stack.Stack()

        property int columns: multiColumnView.width >= units.gu(80) ? 2 : 1
        /*! internal */
        onColumnsChanged: {
            if (columns <= 0) {
                console.warn("There must be a minimum of one column set.");
                columns = 1;
            }
            d.relayout();
        }
        property real defaultColumnWidth: units.gu(40)
        onDefaultColumnWidthChanged: body.applyMetrics()
        property list<ColumnMetrics> columnMetrics

        function createWrapper(page, properties) {
            var wrapperComponent = Qt.createComponent("PageWrapper.qml");
            var wrapperObject = wrapperComponent.createObject(hiddenPages);
            wrapperObject.pageStack = multiColumnView;
            wrapperObject.properties = properties;
            // set reference last because it will trigger creation of the object
            //  with specified properties.
            wrapperObject.reference = page;
            return wrapperObject;
        }

        function addWrappedPage(pageWrapper) {
            stack.push(pageWrapper);
            pageWrapper.parentWrapper = stack.find(pageWrapper.parentPage);
            var targetColumn = MathUtils.clamp(pageWrapper.column, 0, d.columns - 1);
            // replace page holder's child
            var holder = body.children[targetColumn];
            holder.detachCurrentPage();
            holder.attachPage(pageWrapper)
        }

        function columnForPage(page) {
            var wrapper = stack.find(page);
            return wrapper ? wrapper.column : 0;
        }

        function addPageToColumn(column, sourcePage, page, properties) {
            if (column < 0) {
                console.warn("Column must be >= 0.");
                return;
            }
            if (!sourcePage) {
                console.warn("No sourcePage specified. Page will not be added.");
                return;
            }
            if (!d.stack.find(sourcePage)) {
                console.warn("sourcePage must be added to the view to add new page.");
                return;
            }

            var wrapper = d.createWrapper(page, properties);
            wrapper.parentPage = sourcePage;
            wrapper.column = column;
            d.addWrappedPage(wrapper);
            return wrapper.object;
        }

        // node is a triplet of {page, column, parentPage}
        function popAndSetPageForColumn(node) {
            stack.pop();
            var effectiveColumn = MathUtils.clamp(node.column, 0, d.columns - 1);
            var columnHolder = body.children[effectiveColumn];
            // is the page in a column?
            if (node == columnHolder.pageWrapper) {
                // detach page from column
                columnHolder.detachCurrentPage();
            }
            node.parent = null;
            var prevPage = stack.topForColumn(effectiveColumn, effectiveColumn < d.columns - 1);
            if (prevPage) {
                columnHolder.attachPage(prevPage);
            }
            if (node.canDestroy) {
                node.destroyObject();
            }
        }

        // relayouts when column count changes
        function relayout() {
            if (body.children.length == d.columns) return;
            if (body.children.length > d.columns) {
                // need to remove few columns, the last ones
                while (body.children.length > d.columns) {
                    var holder = body.children[body.children.length - 1];
                    holder.detachCurrentPage();
                    holder.parent = null;
                    holder.destroy();
                }
            } else {
                var prevColumns = body.children.length;

                // add columns
                for (var i = 0; i < d.columns - prevColumns; i++) {
                    pageHolderComponent.createObject(body);
                }
            }
            rearrangePages();
        }

        function rearrangePages() {
            for (var column = d.columns - 1; column >= 0; column--) {
                var holder = body.children[column];
                var pageWrapper = stack.topForColumn(column, column < (d.columns - 1));
                if (!pageWrapper) {
                    continue;
                }
                if (!pageWrapper.parent) {
                    // this should never happen, so if it does, we have a bug!
                    console.error("Found a page which wasn't parented anywhere!", pageWrapper.object.title);
                    continue;
                }
                // detach current page from holder if differs
                if (holder.pageWrapper != pageWrapper) {
                    holder.detachCurrentPage();
                }
                if (pageWrapper.parent == hiddenPages) {
                    // add the page to the column
                    holder.attachPage(pageWrapper);
                } else if (pageWrapper.pageHolder != holder) {
                    // detach the pageWrapper from its holder
                    if (pageWrapper.pageHolder) {
                        pageWrapper.pageHolder.detachCurrentPage();
                    }
                    // then attach to this holder
                    holder.attachPage(pageWrapper);
                }
            }
        }
    }

    // default metrics
    Component {
        id: defaultMetrics
        ColumnMetrics {
            fillWidth: column == d.columns
            minimumWidth: d.defaultColumnWidth
        }
    }

    // Page holder component, can have only one Page as child at a time, all stacked pages
    // will be parented into hiddenPages
    Component {
        id: pageHolderComponent
        // Page uses the height of the parentNode for its height, so make
        //  the holder a PageTreeNode that determines the Page height.
        PageTreeNode {
            id: holder
            active: false
            objectName: "ColumnHolder" + column
            property PageWrapper pageWrapper
            property int column
            property alias config: subHeader.config
            property ColumnMetrics metrics: setDefaultMetrics()

            Layout.fillWidth: metrics.fillWidth
            Layout.fillHeight: true
            Layout.preferredWidth: metrics.maximumWidth > 0 ?
                                       MathUtils.clamp(d.defaultColumnWidth, metrics.minimumWidth, metrics.maximumWidth) :
                                       d.defaultColumnWidth
            Layout.minimumWidth: metrics.minimumWidth
            Layout.maximumWidth: metrics.maximumWidth

            // prevent the pages from taking the app header height into account.
            __propagated: null
            Item {
                id: holderBody
                objectName: parent.objectName + "Body"
                anchors {                    
                    top: subHeader.bottom
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                    rightMargin: verticalDivider.width
                }
                // we need to clip because the header does not have a background
                clip: true
            }

            property alias head: subHeader
            StyledItem {
                id: subHeader
                anchors {
                    left: parent.left
                    top: parent.top
                    right: parent.right
                }
                height: body.headerHeight

                styleName: config ? "PageHeadStyle" : ""
                theme.version: Ubuntu.toolkitVersion
                objectName: "Header" + column

                property real preferredHeight: subHeader.__styleInstance ?
                                                   subHeader.__styleInstance.implicitHeight :
                                                   0
                onPreferredHeightChanged: {
                    body.updateHeaderHeight(preferredHeight);
                }

                property PageHeadConfiguration config: null
                property Item contents: null

                property color dividerColor: multiColumnView.__propagated.header.dividerColor
                property color panelColor: multiColumnView.__propagated.header.panelColor

                visible: holder.pageWrapper && holder.pageWrapper.active
            }

            Rectangle {
                id: verticalDivider
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                }
                width: (column == (d.columns - 1)) || !pageWrapper ? 0 : units.dp(1)
                color: subHeader.dividerColor
            }

            function attachPage(page) {
                pageWrapper = page;
                pageWrapper.parent = holderBody;
                pageWrapper.pageHolder = holder;
                pageWrapper.active = true;

                if (pageWrapper.object.hasOwnProperty("head")) {
                    subHeader.config = pageWrapper.object.head;
                }
                if (pageWrapper.column === 0 && pageWrapper.object.hasOwnProperty("title")) {
                    // set the application title
                    appHeaderControlPage.title = pageWrapper.object.title;
                }
            }
            function detachCurrentPage() {
                if (!pageWrapper) return undefined;
                var wrapper = pageWrapper;
                // remove header
                wrapper.active = false;
                subHeader.config = null;
                pageWrapper = null;
                wrapper.parent = hiddenPages;
                wrapper.pageHolder = null;
                return wrapper;
            }

            function setDefaultMetrics() {
                var result = defaultMetrics.createObject(holder);
                result.column = Qt.binding(function() { return holder.column + 1; });
                return result;
            }
        }
    }

    /*! \internal */
    // Pages declared as children will be placed directly into hiddenPages
    default property alias data: hiddenPages.data
    Item {
        id: hiddenPages
        objectName: "HiddenPagePool"
        visible: false
        // make sure nothing is shown eventually
        clip: true
    }

    // Holds the columns holding the pages visible. Each column has only one page
    // as child, the invisible stacked ones are all stored in the hiddenPages
    // component. The stack keeps the column index onto which those should be moved
    // once they become visible.
    RowLayout {
        id: body
        objectName: "body"
        anchors.fill: parent
        spacing: 0

        property real headerHeight: 0

        function updateHeaderHeight(newHeight) {
            if (newHeight > body.headerHeight) {
                body.headerHeight = newHeight;
            } else {
                var h = 0;
                var subHeight = 0;
                for (var i = 0; i < children.length; i++) {
                    subHeight = children[i].head.preferredHeight;
                    if (subHeight > h) h = subHeight;
                }
                body.headerHeight = h;
            }
        }

        onChildrenChanged: {
            // all children should have Layout.fillWidth false, except the last one
            for (var i = 0; i < children.length; i++) {
                children[i].column = i;
            }
            applyMetrics();
        }

        function applyMetrics() {
            for (var i = 0; i < children.length; i++) {
                var holder = children[i];
                // search for the column metrics
                var metrics = null;
                for (var j = 0; j < d.columnMetrics.length; j++) {
                    if (d.columnMetrics[j].column == (i + 1)) {
                        metrics = d.columnMetrics[j];
                        break;
                    }
                }
                if (!metrics) {
                    metrics = holder.setDefaultMetrics();
                }
                holder.metrics = metrics;
                updateHeaderHeight(0);
            }
        }
    }
}
