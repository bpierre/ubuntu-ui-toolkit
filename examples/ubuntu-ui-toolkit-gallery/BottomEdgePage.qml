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
import Ubuntu.Components 1.3

Template {
    id: page
    title: i18n.tr("Bottom Edge")

    TemplateSection {
        title: "BottomEdgeHint"
        className: "BottomEdgeHint"

        TemplateRow {
            title: i18n.tr("On clicked")
            Row {
                spacing: units.gu(1)
                CheckBox {
                    id: contentToLayout
                    text: i18n.tr("push content into the layout")
                    enabled: bottomEdge.hint.status >= BottomEdgeHint.Active
                }
                Label {
                    text: contentToLayout.text
                    anchors.verticalCenter: contentToLayout.verticalCenter
                }
            }
        }
    }

    TemplateSection {
        title: "BottomEdge"
        className: "BottomEdge"

        TemplateRow {
            title: i18n.tr("Top")
            Slider {
                id: bottomEdgeHeight
                maximumValue: page.height
                value: bottomEdge.height
                onValueChanged: bottomEdge.height = value
            }
        }

        TemplateRow {
            title: i18n.tr("Hint")
            Row {
                spacing: units.gu(1)
                CheckBox {
                    id: attachHintToContent
                    text: i18n.tr("attach hint to content")
                }
                Label {
                    text: attachHintToContent.text
                    anchors.verticalCenter: attachHintToContent.verticalCenter
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Ranges")
            Slider {
                id: rangeCount
                width: units.gu(20)
                maximumValue: 3.0
                live: true
            }
        }
        Repeater {
            id: rangeConfig
            model: rangeCount.value.toFixed(0)
            TemplateRow {
                title: i18n.tr("Range #%1").arg(index)
                property int rangeIndex: index
                Repeater {
                    model: ["from", "to"]
                    Row {
                        spacing: units.gu(2)
                        Label {
                            text: i18n.tr(modelData)
                        }
                        TextField {
                            id: rangeFrom
                            text: bottomEdge.ranges[rangeIndex][modelData]
                            inputMask: "0.0"
                            validator: DoubleValidator {bottom: 0.0; top: 1.0; decimals: 2}
                            inputMethodHints: Qt.ImhPreferNumbers | Qt.ImhFormattedNumbersOnly
                            width: units.gu(7)
                            hasClearButton: false
                            errorHighlight: true
                            onAccepted: bottomEdge.ranges[rangeIndex][modelData] = text
                            onTextChanged: if (acceptableInput) bottomEdge.ranges[rangeIndex][modelData] = text
                        }
                    }
                }
            }
        }
    }

    BottomEdge {
        id: bottomEdge
        // make sure it doesn't land inside the flickable
        parent: page
        height: page.height
        // hint
        hint {
            action: Action {
                text: "Demo content"
                iconName: "stock_message"
                onTriggered: bottomEdge.commit()
            }
            flickable: page.flickable
        }
        contentComponent: bottomEdgeContent

        StyleHints {
//            backgroundColor: Qt.rgba(theme.palette.normal.background.r,
//                                     theme.palette.normal.background.g,
//                                     theme.palette.normal.background.b,
//                                     bottomEdge.dragProgress)
            attachHintToContent: attachHintToContent.checked
        }

        onCommitCompleted: {
            if (contentToLayout.checked && contentToLayout.enabled) {
                page.pageStack.addPageToCurrentColumn(page, contentComponent);
                collapse();
            }
        }

        ranges: [
            BottomEdgeRange {
                objectName: "CustomRange1"
                enabled: rangeConfig.model >= 1
                property color baseColor: UbuntuColors.lightGrey
                onFromChanged: print(objectName, "from", from)
                onToChanged: print(objectName, "to", to)
            },
            BottomEdgeRange {
                objectName: "CustomRange2"
                enabled: rangeConfig.model >= 2
            },
            BottomEdgeRange {
                objectName: "CustomRange3"
                enabled: rangeConfig.model >= 3
            },
            // default range, mimics the default setup
            BottomEdgeRange {
                objectName: "DefaultRange"
                enabled: rangeConfig.model <= 0
                from: 0.3
            }
        ]

        Component {
            id: bottomEdgeContent
            Page {
                height: bottomEdge.height
                header: PageHeader {
                    title: {
                        var state = "UNDEFINED";
                        switch (bottomEdge.state) {
                        case BottomEdge.Hidden: state = "Hidden"; break;
                        case BottomEdge.Revealed: state = "Revealed"; break;
                        case BottomEdge.Committed: state = "Hidden"; break;
                        }
                        return bottomEdge.activeRange
                          ? i18n.tr("Within range '%1', state: %2").arg(bottomEdge.activeRange.objectName).arg(state)
                          : i18n.tr("Not in any active range, state: %1").arg(state);
                    }
                }
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: units.gu(1)
                    color: bottomEdge.activeRange && bottomEdge.activeRange.hasOwnProperty("baseColor") ?
                               bottomEdge.activeRange.baseColor : Qt.rgba(0.5, 1, bottomEdge.dragProgress, 1)
                }
            }
        }
    }
}
