### This file is automatically generated by Qt Design Studio.
### Do not change

message("Building designer components.")

set(QT_QDS_COMPONENTS_NOWARN on)
set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

include(FetchContent)
FetchContent_Declare(
    ds
    GIT_TAG qds-4.6
    GIT_REPOSITORY https://code.qt.io/qt-labs/qtquickdesigner-components.git
)

FetchContent_GetProperties(ds)
FetchContent_Populate(ds)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    QuickStudioComponentsplugin
    QuickStudioDesignEffectsplugin
    QuickStudioEffectsplugin
    QuickStudioApplicationplugin
    FlowViewplugin
    QuickStudioLogicHelperplugin
    QuickStudioMultiTextplugin
    QuickStudioEventSimulatorplugin
    QuickStudioEventSystemplugin
    QuickStudioUtilsplugin
)

add_subdirectory(${ds_SOURCE_DIR} ${ds_BINARY_DIR})

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
  BUILD_QDS_COMPONENTS=true
)
