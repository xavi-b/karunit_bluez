import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Karunit 1.0
import KarunitPlugins 1.0

Item {
    anchors.fill: parent

    function connectToDevice(deviceInfo) {
        KUPBluezPluginConnector.connectToDevice(deviceInfo)
    }

    function disconnectFromDevice(deviceInfo) {
        KUPBluezPluginConnector.disconnectFromDevice(deviceInfo)
    }

    onVisibleChanged: {
        KUPBluezPluginConnector.stopScanning()
    }

    ListView {
        anchors.fill: parent
        id: listview
        model: KUPBluezPluginConnector.devices
        spacing: 5

        delegate: ItemDelegate {
            width: listview.width

            spacing: 5

            contentItem: Label {
                text: modelData.name + " - " + modelData.address
                color: modelData.connected ? "green" : "red"
            }

            background: Rectangle {
                anchors.fill: parent
                color: "lightgrey"
            }

            onDoubleClicked: {
                if (!modelData.connected)
                    connectToDevice(modelData)
                else
                    disconnectFromDevice(modelData)
            }
        }

        RoundButton {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 5
            icon.name: KUPBluezPluginConnector.discovering ? "process-stop" : "view-refresh"
            icon.color: "transparent"
            onClicked: {
                if (KUPBluezPluginConnector.discovering)
                    KUPBluezPluginConnector.stopScanning()
                else
                    KUPBluezPluginConnector.startScanning()
            }
        }
    }
}
