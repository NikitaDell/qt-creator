// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0
#pragma once

#include "utils/filepath.h"

#include <QString>

namespace QmlProjectManager {

class QmlProject;
class QmlBuildSystem;

namespace QmlProjectExporter {

struct Node
{
    enum class Type {
        App,
        Module,
        Library,
        Folder,
        MockModule
    };

    std::shared_ptr<Node> parent = nullptr;
    Type type = Type::Folder;

    QString uri;
    QString name;
    Utils::FilePath dir;

    std::vector<std::shared_ptr<Node>> subdirs;
    std::vector<Utils::FilePath> files;
    std::vector<Utils::FilePath> singletons;
    std::vector<Utils::FilePath> assets;
    std::vector<Utils::FilePath> sources;
};

using NodePtr = std::shared_ptr<Node>;
using FileGetter = std::function<std::vector<Utils::FilePath>(const NodePtr &)>;

class CMakeGenerator;

const char ENV_VARIABLE_CONTROLCONF[] =
    "QT_QUICK_CONTROLS_CONF";

const char DO_NOT_EDIT_FILE[] =
    "### This file is automatically generated by Qt Design Studio.\n"
    "### Do not change\n\n";

const char TEMPLATE_LINK_LIBRARIES[] =
    "target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE\n"
    "%3"
    ")";

class CMakeWriter
{
public:
    using Ptr = std::shared_ptr<CMakeWriter>;

    static Ptr create(CMakeGenerator *parent);
    static QString readTemplate(const QString &templatePath);
    static void writeFile(const Utils::FilePath &path, const QString &content);

    CMakeWriter(CMakeGenerator *parent);
    virtual ~CMakeWriter() = default;

    const CMakeGenerator *parent() const;

    virtual bool isPlugin(const NodePtr &node) const;
    virtual QString sourceDirName() const;
    virtual void transformNode(NodePtr &) const;

    virtual void writeRootCMakeFile(const NodePtr &node) const = 0;
    virtual void writeModuleCMakeFile(const NodePtr &node, const NodePtr &root) const = 0;
    virtual void writeSourceFiles(const NodePtr &node, const NodePtr &root) const = 0;

protected:
    std::vector<Utils::FilePath> files(const NodePtr &node, const FileGetter &getter) const;
    std::vector<Utils::FilePath> qmlFiles(const NodePtr &node) const;
    std::vector<Utils::FilePath> singletons(const NodePtr &node) const;
    std::vector<Utils::FilePath> assets(const NodePtr &node) const;
    std::vector<Utils::FilePath> sources(const NodePtr &node) const;
    std::vector<QString> plugins(const NodePtr &node) const;

    QString getEnvironmentVariable(const QString &key) const;

    QString makeFindPackageBlock(const QmlBuildSystem* buildSystem) const;
    QString makeRelative(const NodePtr &node, const Utils::FilePath &path) const;
    QString makeQmlFilesBlock(const NodePtr &node) const;
    QString makeSingletonBlock(const NodePtr &node) const;
    QString makeSubdirectoriesBlock(const NodePtr &node) const;
    QString makeSetEnvironmentFn() const;
    std::tuple<QString, QString> makeResourcesBlocksRoot(const NodePtr &node) const;
    std::tuple<QString, QString> makeResourcesBlocksModule(const NodePtr &node) const;

private:
    void collectPlugins(const NodePtr &node, std::vector<QString> &out) const;
    void collectResources(const NodePtr &node, QStringList &res, QStringList &bigRes) const;
    const CMakeGenerator *m_parent = nullptr;
};

} // End namespace QmlProjectExporter.

} // End namespace QmlProjectManager.
