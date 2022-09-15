// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0+ OR GPL-3.0 WITH Qt-GPL-exception-1.0

#pragma once

constexpr auto armgcc_stm32f769i_discovery_baremetal_json = R"(
{
    "qulVersion": "2.3.0",
    "compatVersion": "1",
    "platform": {
        "id": "STM32F769I-DISCOVERY-BAREMETAL",
        "vendor": "ST",
        "colorDepths": [
            32
        ],
        "cmakeEntries": [
            {
                "id": "STM32CubeProgrammer_PATH",
                "label": "STM32CubeProgrammer",
                "type": "path",
                "setting": "Stm32CubeProgrammer",
                "defaultValue": {
                    "windows": "%{Env:PROGRAMSANDFILES}/STMicroelectronics/STM32Cube/STM32CubeProgrammer/",
                    "unix": "%{Env:HOME}/STMicroelectronics/STM32Cube/STM32CubeProgrammer/"
                },
                "optional": false
            }
        ]
    },
    "toolchain": {
        "id": "armgcc",
        "versions": [
            "10.3.1"
        ],
        "compiler": {
            "id": "ARMGCC_DIR",
            "label": "GNU Arm Embedded Toolchain",
            "cmakeVar": "QUL_TARGET_TOOLCHAIN_DIR",
            "envVar": "ARMGCC_DIR",
            "setting": "GNUArmEmbeddedToolchain",
            "type": "path",
            "optional": false,
            "versionDetection": {
                "filePattern": "bin/arm-none-eabi-g++",
                "executableArgs": "--version",
                "regex": "\\bv(\\d+\\.\\d+\\.\\d+)\\b"
            }
        },
        "file": {
            "id": "ARMGCC_CMAKE_TOOLCHAIN_FILE",
            "cmakeVar": "CMAKE_TOOLCHAIN_FILE",
            "type": "file",
            "defaultValue": "%{Qul_ROOT}//lib/cmake/Qul/toolchain/armgcc.cmake",
            "visible": false,
            "optional": false
        }
    },
    "boardSdk": {
        "envVar": "STM32Cube_FW_F7_SDK_PATH",
        "setting": "STM32Cube_FW_F7_SDK_PATH",
        "versions": [
            "1.17.0"
        ],
        "versionDetection": {
            "filePattern": "package.xml",
            "xmlElement": "PackDescription",
            "xmlAttribute": "Release",
            "regex": "\\b(\\d+\\.\\d+\\.\\d+)\\b"
        },
        "label": "Board SDK for STM32F769I-Discovery",
        "cmakeVar": "QUL_BOARD_SDK_DIR",
        "type": "path",
        "optional": false
    }
}
)";
