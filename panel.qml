import QtQuick 2.0
import Ubuntu.Components 0.1

Rectangle {
    width: units.gu(50)
    height: units.gu(50)

    Label {
        text: "Panels!"
        anchors.centerIn: parent
    }

    Panel {
        align: Qt.AlignLeft

        Rectangle {
            anchors.fill: parent
            color: "black"
            opacity: 1.0
        }

        Slider {
            anchors.centerIn: parent
            width: parent.width / 2
        }
    }
}
