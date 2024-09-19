// Copyright (C) The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "../luaengine.h"

#include <projectexplorer/buildmanager.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectmanager.h>
#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/runcontrol.h>
#include <projectexplorer/target.h>

#include <utils/commandline.h>
#include <utils/processinterface.h>

using namespace ProjectExplorer;
using namespace Utils;

namespace Lua::Internal {

void setupProjectModule()
{
    registerProvider("Project", [](sol::state_view lua) -> sol::object {
        const ScriptPluginSpec *pluginSpec = lua.get<ScriptPluginSpec *>("PluginSpec");
        QObject *guard = pluginSpec->connectionGuard.get();

        sol::table result = lua.create_table();

        result.new_usertype<RunConfiguration>(
            "RunConfiguration",
            sol::no_constructor,
            "runnable",
            sol::property(&RunConfiguration::runnable));

        result.new_usertype<Project>(
            "Project",
            sol::no_constructor,
            "directory",
            sol::property(&Project::projectDirectory),
            "activeRunConfiguration",
            [](Project *project) { return project->activeTarget()->activeRunConfiguration(); });

        result["startupProject"] = [] { return ProjectManager::instance()->startupProject(); };

        result["canRunStartupProject"] =
            [](const QString &mode) -> std::pair<bool, std::variant<QString, sol::lua_nil_t>> {
            auto result = ProjectExplorerPlugin::canRunStartupProject(Id::fromString(mode));
            if (result)
                return std::make_pair(true, sol::lua_nil);
            return std::make_pair(false, result.error());
        };

        result["runStartupProject"] =
            [guard](const sol::optional<ProcessRunData> &runnable) {
                auto project = ProjectManager::instance()->startupProject();
                if (!project)
                    throw sol::error("No startup project");

                auto runConfiguration = project->activeTarget()->activeRunConfiguration();

                if (!runConfiguration)
                    throw sol::error("No active run configuration");

                auto rc = std::make_unique<RunControl>(ProjectExplorer::Constants::NORMAL_RUN_MODE);
                rc->copyDataFromRunConfiguration(runConfiguration);

                if (runnable) {
                    rc->setCommandLine(runnable->command);
                    rc->setWorkingDirectory(runnable->workingDirectory);
                    rc->setEnvironment(runnable->environment);
                }

                BuildForRunConfigStatus status = BuildManager::potentiallyBuildForRunConfig(
                    runConfiguration);

                auto startRun = [rc = std::move(rc)]() mutable {
                    if (!rc->createMainWorker())
                        return;
                    ProjectExplorerPlugin::startRunControl(rc.release());
                };

                if (status == BuildForRunConfigStatus::Building) {
                    QObject::connect(
                        BuildManager::instance(),
                        &BuildManager::buildQueueFinished,
                        guard,
                        [startRun = std::move(startRun)](bool success) mutable {
                            if (success)
                                startRun();
                        },
                        Qt::SingleShotConnection);
                } else {
                    startRun();
                }
            };

        result["RunMode"] = lua.create_table_with(
            "Normal", Constants::NORMAL_RUN_MODE, "Debug", Constants::DEBUG_RUN_MODE);

        return result;
    });

    // startupProjectChanged
    registerHook("projects.startupProjectChanged", [](sol::function func, QObject *guard) {
        QObject::connect(
            ProjectManager::instance(),
            &ProjectManager::startupProjectChanged,
            guard,
            [func](Project *project) {
                expected_str<void> res = void_safe_call(func, project);
                QTC_CHECK_EXPECTED(res);
            });
    });

    // projectAdded
    registerHook("projects.projectAdded", [](sol::function func, QObject *guard) {
        QObject::connect(
            ProjectManager::instance(),
            &ProjectManager::projectAdded,
            guard,
            [func](Project *project) {
                expected_str<void> res = void_safe_call(func, project);
                QTC_CHECK_EXPECTED(res);
            });
    });

    // projectRemoved
    registerHook("projects.projectRemoved", [](sol::function func, QObject *guard) {
        QObject::connect(
            ProjectManager::instance(),
            &ProjectManager::projectRemoved,
            guard,
            [func](Project *project) {
                expected_str<void> res = void_safe_call(func, project);
                QTC_CHECK_EXPECTED(res);
            });
    });

    // aboutToRemoveProject
    registerHook("projects.aboutToRemoveProject", [](sol::function func, QObject *guard) {
        QObject::connect(
            ProjectManager::instance(),
            &ProjectManager::aboutToRemoveProject,
            guard,
            [func](Project *project) {
                expected_str<void> res = void_safe_call(func, project);
                QTC_CHECK_EXPECTED(res);
            });
    });

    // runActionsUpdated
    registerHook("projects.runActionsUpdated", [](sol::function func, QObject *guard) {
        QObject::connect(
            ProjectExplorerPlugin::instance(),
            &ProjectExplorerPlugin::runActionsUpdated,
            guard,
            [func]() {
                expected_str<void> res = void_safe_call(func);
                QTC_CHECK_EXPECTED(res);
            });
    });
}

} // namespace Lua::Internal
