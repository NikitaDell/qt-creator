// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

import QtQuick
import StudioControls as StudioControls
import StudioTheme as StudioTheme
import EffectMakerBackend

StudioControls.CheckBox {
    actionIndicatorVisible: false
    checked: uniformValue
    onToggled: uniformValue = checked
    width: height // limit hovering and clicking the CheckBox to its area
}
