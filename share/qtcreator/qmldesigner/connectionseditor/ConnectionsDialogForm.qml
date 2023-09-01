// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

import QtQuick
import QtQuick.Controls
import HelperWidgets as HelperWidgets
import StudioControls as StudioControls
import StudioTheme as StudioTheme

Column {
    id: root

    readonly property real horizontalSpacing: 10
    readonly property real verticalSpacing: 16
    readonly property real columnWidth: (root.width - root.horizontalSpacing) / 2

    property var backend


    /* replaced by ConnectionModelStatementDelegate defined in C++
    enum ActionType {
        CallFunction,
        Assign,
        ChangeState,
        SetProperty,
        PrintMessage,
        Custom
    } */

    y: StudioTheme.Values.popupMargin
    width: parent.width
    spacing: root.verticalSpacing

    Row {
        spacing: root.horizontalSpacing

        PopupLabel { text: qsTr("Signal"); tooltip: qsTr("The name of the signal.") }
        PopupLabel { text: qsTr("Action"); tooltip: qsTr("The type of the action.") }
    }

    Row {
        spacing: root.horizontalSpacing

        StudioControls.TopLevelComboBox {
            id: signal
            style: StudioTheme.Values.connectionPopupControlStyle
            width: root.columnWidth

            model: backend.signal.name.model ?? 0

            onActivated: backend.signal.name.activateIndex(signal.currentIndex)
            property int currentTypeIndex: backend.signal.name.currentIndex ?? 0
            onCurrentTypeIndexChanged: signal.currentIndex = signal.currentTypeIndex
        }

        StudioControls.TopLevelComboBox {
            id: action
            style: StudioTheme.Values.connectionPopupControlStyle
            width: root.columnWidth
            textRole: "text"
            valueRole: "value"
            ///model.getData(currentIndex, "role")
            property int indexFromBackend: indexOfValue(backend.actionType)
            onIndexFromBackendChanged: action.currentIndex = action.indexFromBackend
            onActivated: backend.changeActionType(action.currentValue)

            model: [
                { value: ConnectionModelStatementDelegate.CallFunction, text: qsTr("Call Function") },
                { value: ConnectionModelStatementDelegate.Assign, text: qsTr("Assign") },
                { value: ConnectionModelStatementDelegate.ChangeState, text: qsTr("Change State") },
                { value: ConnectionModelStatementDelegate.SetProperty, text: qsTr("Set Property") },
                { value: ConnectionModelStatementDelegate.PrintMessage, text: qsTr("Print Message") },
                { value: ConnectionModelStatementDelegate.Custom, text: qsTr("Unknown") }
            ]
        }
    }

    StatementEditor {
        actionType: action.currentValue ?? ConnectionModelStatementDelegate.Custom
        horizontalSpacing: root.horizontalSpacing
        columnWidth: root.columnWidth
        statement: backend.okStatement
        spacing: root.verticalSpacing
    }

    HelperWidgets.AbstractButton {
        style: StudioTheme.Values.connectionPopupButtonStyle
        width: 160
        buttonIcon: qsTr("Add Condition")
        iconSize: StudioTheme.Values.baseFontSize
        iconFont: StudioTheme.Constants.font
        anchors.horizontalCenter: parent.horizontalCenter
        visible: action.currentValue !== ConnectionModelStatementDelegate.Custom && !backend.hasCondition

        onClicked: backend.addCondition()
    }

    HelperWidgets.AbstractButton {
        style: StudioTheme.Values.connectionPopupButtonStyle
        width: 160
        buttonIcon: qsTr("Remove Condition")
        iconSize: StudioTheme.Values.baseFontSize
        iconFont: StudioTheme.Constants.font
        anchors.horizontalCenter: parent.horizontalCenter
        visible: action.currentValue !== ConnectionModelStatementDelegate.Custom && backend.hasCondition

        onClicked: backend.removeCondition()
    }

    Flow {
        spacing: root.horizontalSpacing
        width: root.width
        Repeater {

            model: backend.conditionListModel
            Text {
                text: value
                color: "white"
                Rectangle {
                    z: -1
                    opacity: 0.2
                    color: {
                        if (type === ConditionListModel.Invalid)
                            return "red"
                        if (type === ConditionListModel.Operator)
                            return "blue"
                        if (type === ConditionListModel.Literal)
                            return "green"
                        if (type === ConditionListModel.Variable)
                            return "yellow"
                    }
                    anchors.fill: parent
                }
            }
        }
    }

    TextInput {
        id: commandInput
        width: root.width
        onAccepted:  backend.conditionListModel.command(commandInput.text)
    }

    Text {
        text: "invalid " + backend.conditionListModel.error
        visible: !backend.conditionListModel.valid
    }

    HelperWidgets.AbstractButton {
        style: StudioTheme.Values.connectionPopupButtonStyle
        width: 160
        buttonIcon: qsTr("Add Else Statement")
        iconSize: StudioTheme.Values.baseFontSize
        iconFont: StudioTheme.Constants.font
        anchors.horizontalCenter: parent.horizontalCenter
        visible: action.currentValue !== ConnectionModelStatementDelegate.Custom && backend.hasCondition && !backend.hasElse

        onClicked: backend.addElse()
    }

    HelperWidgets.AbstractButton {
        style: StudioTheme.Values.connectionPopupButtonStyle
        width: 160
        buttonIcon: qsTr("Remove Else Statement")
        iconSize: StudioTheme.Values.baseFontSize
        iconFont: StudioTheme.Constants.font
        anchors.horizontalCenter: parent.horizontalCenter
        visible: action.currentValue !== ConnectionModelStatementDelegate.Custom && backend.hasCondition && backend.hasElse

        onClicked: backend.removeElse()
    }

    //Else Statement
    StatementEditor {
        actionType: action.currentValue ?? ConnectionModelStatementDelegate.Custom
        horizontalSpacing: root.horizontalSpacing
        columnWidth: root.columnWidth
        statement: backend.koStatement
        spacing: root.verticalSpacing
        visible: action.currentValue !== ConnectionModelStatementDelegate.Custom && backend.hasCondition && backend.hasElse
    }

    // Editor
    Rectangle {
        id: editor
        width: parent.width
        height: 150
        color: StudioTheme.Values.themeToolbarBackground

        Text {
            anchors.centerIn: parent
            text: backend.source
            color: StudioTheme.Values.themeTextColor
            font.pixelSize: StudioTheme.Values.myFontSize
        }

        HelperWidgets.AbstractButton {
            id: editorButton

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 4

            style: StudioTheme.Values.viewBarButtonStyle
            buttonIcon: StudioTheme.Constants.edit_medium
            tooltip: qsTr("Add something.")
            onClicked: console.log("OPEN EDITOR")
        }
    }
}
