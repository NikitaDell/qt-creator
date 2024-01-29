// Copyright (C) 2020 Alexis Jeandet.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "mesonactionsmanager.h"
#include "mesonbuildconfiguration.h"
#include "mesonbuildsystem.h"
#include "mesonproject.h"
#include "mesonrunconfiguration.h"
#include "ninjabuildstep.h"
#include "toolssettingsaccessor.h"
#include "toolssettingspage.h"

#include <extensionsystem/iplugin.h>

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectmanager.h>

#include <utils/fsengine/fileiconprovider.h>

using namespace ProjectExplorer;
using namespace Utils;

namespace MesonProjectManager::Internal {

class MesonProjectPluginPrivate
{
public:
    MesonActionsManager m_actions;
};

class MesonProjectPlugin final : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "MesonProjectManager.json")

public:
    ~MesonProjectPlugin() final
    {
        delete d;
    }

private:
    void initialize() final
    {
        d = new MesonProjectPluginPrivate;

        setupToolsSettingsPage();
        setupToolsSettingsAccessor();

        setupMesonBuildSystem();
        setupMesonBuildConfiguration();
        setupNinjaBuildStep();

        setupMesonRunConfiguration();
        setupMesonRunAndDebugWorkers();

        ProjectManager::registerProjectType<MesonProject>(Constants::Project::MIMETYPE);

        FileIconProvider::registerIconOverlayForFilename(Constants::Icons::MESON, "meson.build");
        FileIconProvider::registerIconOverlayForFilename(Constants::Icons::MESON, "meson_options.txt");
    }

    class MesonProjectPluginPrivate *d = nullptr;
};

} // MesonProjectManager::Internal

#include "mesonprojectplugin.moc"
