// Copyright (C) 2016 BogDan Vatra <bog_dan_ro@yahoo.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "androidavdmanager.h"
#include "androidbuildapkstep.h"
#include "androidconfigurations.h"
#include "androidconstants.h"
#include "androiddeployqtstep.h"
#include "androiddevice.h"
#include "androidglobal.h"
#include "androidmanager.h"
#include "androidqtversion.h"
#include "androidrunconfiguration.h"
#include "androidsdkmanager.h"
#include "androidtoolchain.h"
#include "androidtr.h"

#include <coreplugin/documentmanager.h>
#include <coreplugin/messagemanager.h>
#include <coreplugin/icore.h>

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectnodes.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectmanager.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildsystem.h>

#include <qtsupport/qtkitaspect.h>
#include <qtsupport/qtsupportconstants.h>
#include <qtsupport/baseqtversion.h>

#include <cmakeprojectmanager/cmakeprojectconstants.h>

#include <utils/algorithm.h>
#include <utils/fileutils.h>
#include <utils/process.h>
#include <utils/qtcassert.h>
#include <utils/stringutils.h>

#include <QApplication>
#include <QDomDocument>
#include <QFileSystemWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QLoggingCategory>
#include <QMessageBox>
#include <QProcess>
#include <QRegularExpression>
#include <QVersionNumber>

using namespace ProjectExplorer;
using namespace Utils;

using namespace Android::Internal;

namespace Android {

const char AndroidManifestName[] = "AndroidManifest.xml";
const char AndroidDeviceSn[] = "AndroidDeviceSerialNumber";
const char AndroidDeviceAbis[] = "AndroidDeviceAbis";
const char ApiLevelKey[] = "AndroidVersion.ApiLevel";
const char qtcSignature[] = "This file is generated by QtCreator to be read by "
                            "androiddeployqt and should not be modified by hand.";

static Q_LOGGING_CATEGORY(androidManagerLog, "qtc.android.androidManager", QtWarningMsg)

class Library
{
public:
    int level = -1;
    QStringList dependencies;
    QString name;
};

using LibrariesMap = QMap<QString, Library>;

static bool openXmlFile(QDomDocument &doc, const FilePath &fileName);
static bool openManifest(const Target *target, QDomDocument &doc);
static int parseMinSdk(const QDomElement &manifestElem);

static const ProjectNode *currentProjectNode(const Target *target)
{
    return target->project()->findNodeForBuildKey(target->activeBuildKey());
}

QString AndroidManager::packageName(const Target *target)
{
    QDomDocument doc;
    if (!openManifest(target, doc))
        return {};
    QDomElement manifestElem = doc.documentElement();
    return manifestElem.attribute(QLatin1String("package"));
}

QString AndroidManager::packageName(const FilePath &manifestFile)
{
    QDomDocument doc;
    if (!openXmlFile(doc, manifestFile))
        return {};
    QDomElement manifestElem = doc.documentElement();
    return manifestElem.attribute(QLatin1String("package"));
}

QString AndroidManager::activityName(const Target *target)
{
    QDomDocument doc;
    if (!openManifest(target, doc))
        return {};
    QDomElement activityElem = doc.documentElement().firstChildElement(
                QLatin1String("application")).firstChildElement(QLatin1String("activity"));
    return activityElem.attribute(QLatin1String("android:name"));
}

/*!
    Returns the minimum Android API level set for the APK. Minimum API level
    of the kit is returned if the manifest file of the APK cannot be found
    or parsed.
*/
int AndroidManager::minimumSDK(const Target *target)
{
    QDomDocument doc;
    if (!openXmlFile(doc, AndroidManager::manifestSourcePath(target)))
        return minimumSDK(target->kit());
    const int minSdkVersion = parseMinSdk(doc.documentElement());
    if (minSdkVersion == 0)
        return AndroidManager::defaultMinimumSDK(QtSupport::QtKitAspect::qtVersion(target->kit()));
    return minSdkVersion;
}

/*!
    Returns the minimum Android API level required by the kit to compile. -1 is
    returned if the kit does not support Android.
*/
int AndroidManager::minimumSDK(const Kit *kit)
{
    int minSdkVersion = -1;
    QtSupport::QtVersion *version = QtSupport::QtKitAspect::qtVersion(kit);
    if (version && version->targetDeviceTypes().contains(Constants::ANDROID_DEVICE_TYPE)) {
        FilePath stockManifestFilePath = FilePath::fromUserInput(
            version->prefix().toString() + "/src/android/templates/AndroidManifest.xml");
        QDomDocument doc;
        if (openXmlFile(doc, stockManifestFilePath)) {
            minSdkVersion = parseMinSdk(doc.documentElement());
        }
    }
    if (minSdkVersion == 0)
        return AndroidManager::defaultMinimumSDK(version);
    return minSdkVersion;
}

QString AndroidManager::buildTargetSDK(const Target *target)
{
    if (auto bc = target->activeBuildConfiguration()) {
        if (auto androidBuildApkStep = bc->buildSteps()->firstOfType<AndroidBuildApkStep>())
            return androidBuildApkStep->buildTargetSdk();
    }

    QString fallback = AndroidConfig::apiLevelNameFor(
                AndroidConfigurations::sdkManager()->latestAndroidSdkPlatform());
    return fallback;
}

QStringList AndroidManager::applicationAbis(const Target *target)
{
    auto qt = dynamic_cast<AndroidQtVersion *>(QtSupport::QtKitAspect::qtVersion(target->kit()));
    return qt ? qt->androidAbis() : QStringList();
}

QString AndroidManager::archTriplet(const QString &abi)
{
    if (abi == ProjectExplorer::Constants::ANDROID_ABI_X86) {
        return {"i686-linux-android"};
    } else if (abi == ProjectExplorer::Constants::ANDROID_ABI_X86_64) {
        return {"x86_64-linux-android"};
    } else if (abi == ProjectExplorer::Constants::ANDROID_ABI_ARM64_V8A) {
        return {"aarch64-linux-android"};
    }
    return {"arm-linux-androideabi"};
}

QJsonObject AndroidManager::deploymentSettings(const Target *target)
{
    QtSupport::QtVersion *qt = QtSupport::QtKitAspect::qtVersion(target->kit());
    if (!qt)
        return {};

    auto tc = ToolChainKitAspect::cxxToolChain(target->kit());
    if (!tc || tc->typeId() != Constants::ANDROID_TOOLCHAIN_TYPEID)
        return {};
    QJsonObject settings;
    settings["_description"] = qtcSignature;
    settings["qt"] = qt->prefix().toString();
    settings["ndk"] = AndroidConfigurations::currentConfig().ndkLocation(qt).toString();
    settings["sdk"] = AndroidConfigurations::currentConfig().sdkLocation().toString();
    if (!qt->supportsMultipleQtAbis()) {
        const QStringList abis = applicationAbis(target);
        QTC_ASSERT(abis.size() == 1, return {});
        settings["stdcpp-path"] = (AndroidConfigurations::currentConfig().toolchainPath(qt)
                                      / "sysroot/usr/lib"
                                      / archTriplet(abis.first())
                                      / "libc++_shared.so").toString();
    } else {
        settings["stdcpp-path"] = AndroidConfigurations::currentConfig()
                                      .toolchainPath(qt)
                                      .pathAppended("sysroot/usr/lib")
                                      .toString();
    }
    settings["toolchain-prefix"] =  "llvm";
    settings["tool-prefix"] = "llvm";
    settings["useLLVM"] = true;
    settings["ndk-host"] = AndroidConfigurations::currentConfig().toolchainHost(qt);
    return settings;
}

bool AndroidManager::isQtCreatorGenerated(const FilePath &deploymentFile)
{
    QFile f{deploymentFile.toString()};
    if (!f.open(QIODevice::ReadOnly))
        return false;
    return QJsonDocument::fromJson(f.readAll()).object()["_description"].toString() == qtcSignature;
}

FilePath AndroidManager::androidBuildDirectory(const Target *target)
{
    return buildDirectory(target) / Constants::ANDROID_BUILD_DIRECTORY;
}

FilePath AndroidManager::androidAppProcessDir(const Target *target)
{
    return buildDirectory(target) / Constants::ANDROID_APP_PROCESS_DIRECTORY;
}

bool AndroidManager::isQt5CmakeProject(const ProjectExplorer::Target *target)
{
    const QtSupport::QtVersion *qt = QtSupport::QtKitAspect::qtVersion(target->kit());
    const bool isQt5 = qt && qt->qtVersion() < QVersionNumber(6, 0, 0);
    const Core::Context cmakeCtx = Core::Context(CMakeProjectManager::Constants::CMAKE_PROJECT_ID);
    const bool isCmakeProject = (target->project()->projectContext() == cmakeCtx);
    return isQt5 && isCmakeProject;
}

FilePath AndroidManager::buildDirectory(const Target *target)
{
    if (const BuildSystem *bs = target->buildSystem()) {
        const QString buildKey = target->activeBuildKey();

        // Get the target build dir based on the settings file path
        FilePath buildDir;
        const ProjectNode *node = target->project()->findNodeForBuildKey(buildKey);
        if (node) {
            const QString settingsFile = node->data(Constants::AndroidDeploySettingsFile).toString();
            buildDir = FilePath::fromUserInput(settingsFile).parentDir();
        }

        if (!buildDir.isEmpty())
            return buildDir;

        // Otherwise fallback to target working dir
        buildDir = bs->buildTarget(target->activeBuildKey()).workingDirectory;
        if (isQt5CmakeProject(target)) {
            // Return the main build dir and not the android libs dir
            const QString libsDir = QString(Constants::ANDROID_BUILD_DIRECTORY) + "/libs";
            Utils::FilePath parentDuildDir = buildDir.parentDir();
            if (parentDuildDir.endsWith(libsDir) || libsDir.endsWith(libsDir + "/"))
                return parentDuildDir.parentDir().parentDir();
        }
        return buildDir;
    }
    return {};
}

enum PackageFormat {
    Apk,
    Aab
};

QString packageSubPath(PackageFormat format, BuildConfiguration::BuildType buildType, bool sig)
{
    const bool deb = (buildType == BuildConfiguration::Debug);

    if (format == Apk) {
        if (deb) {
            return sig ? packageSubPath(Apk, BuildConfiguration::Release, true) // Intentional
                       : QLatin1String("apk/debug/android-build-debug.apk");
        }
        return QLatin1String(sig ? "apk/release/android-build-release-signed.apk"
                                 : "apk/release/android-build-release-unsigned.apk");
    }
    return QLatin1String(deb ? "bundle/debug/android-build-debug.aab"
                             : "bundle/release/android-build-release.aab");
}

FilePath AndroidManager::packagePath(const Target *target)
{
    QTC_ASSERT(target, return {});

    auto bc = target->activeBuildConfiguration();
    if (!bc)
        return {};
    auto buildApkStep = bc->buildSteps()->firstOfType<AndroidBuildApkStep>();
    if (!buildApkStep)
        return {};

    const QString subPath = packageSubPath(buildApkStep->buildAAB() ? Aab : Apk,
                                           bc->buildType(), buildApkStep->signPackage());

    return androidBuildDirectory(target) / "build/outputs" / subPath;
}

bool AndroidManager::matchedAbis(const QStringList &deviceAbis, const QStringList &appAbis)
{
    for (const auto &abi : appAbis) {
        if (deviceAbis.contains(abi))
            return true;
    }
    return false;
}

QString AndroidManager::devicePreferredAbi(const QStringList &deviceAbis, const QStringList &appAbis)
{
    for (const auto &abi : appAbis) {
        if (deviceAbis.contains(abi))
            return abi;
    }
    return {};
}

Abi AndroidManager::androidAbi2Abi(const QString &androidAbi)
{
    if (androidAbi == ProjectExplorer::Constants::ANDROID_ABI_ARM64_V8A) {
        return Abi{Abi::Architecture::ArmArchitecture,
                   Abi::OS::LinuxOS,
                   Abi::OSFlavor::AndroidLinuxFlavor,
                   Abi::BinaryFormat::ElfFormat,
                   64, androidAbi};
    } else if (androidAbi == ProjectExplorer::Constants::ANDROID_ABI_ARMEABI_V7A) {
        return Abi{Abi::Architecture::ArmArchitecture,
                   Abi::OS::LinuxOS,
                   Abi::OSFlavor::AndroidLinuxFlavor,
                   Abi::BinaryFormat::ElfFormat,
                   32, androidAbi};
    } else if (androidAbi == ProjectExplorer::Constants::ANDROID_ABI_X86_64) {
        return Abi{Abi::Architecture::X86Architecture,
                   Abi::OS::LinuxOS,
                   Abi::OSFlavor::AndroidLinuxFlavor,
                   Abi::BinaryFormat::ElfFormat,
                   64, androidAbi};
    } else if (androidAbi == ProjectExplorer::Constants::ANDROID_ABI_X86) {
        return Abi{Abi::Architecture::X86Architecture,
                   Abi::OS::LinuxOS,
                   Abi::OSFlavor::AndroidLinuxFlavor,
                   Abi::BinaryFormat::ElfFormat,
                   32, androidAbi};
    } else {
        return Abi{Abi::Architecture::UnknownArchitecture,
                   Abi::OS::LinuxOS,
                   Abi::OSFlavor::AndroidLinuxFlavor,
                   Abi::BinaryFormat::ElfFormat,
                   0, androidAbi};
    }
}

bool AndroidManager::skipInstallationAndPackageSteps(const Target *target)
{
    // For projects using Qt 5.15 and Qt 6, the deployment settings file
    // is generated by CMake/qmake and not Qt Creator, so if such file doesn't exist
    // or it's been generated by Qt Creator, we can assume the project is not
    // an android app.
    const FilePath inputFile = AndroidQtVersion::androidDeploymentSettings(target);
    if (!inputFile.exists() || AndroidManager::isQtCreatorGenerated(inputFile))
        return true;

    const Project *p = target->project();

    const Core::Context cmakeCtx = Core::Context(CMakeProjectManager::Constants::CMAKE_PROJECT_ID);
    const bool isCmakeProject = p->projectContext() == cmakeCtx;
    if (isCmakeProject)
        return false; // CMake reports ProductType::Other for Android Apps

    const ProjectNode *n = p->rootProjectNode()->findProjectNode([] (const ProjectNode *n) {
        return n->productType() == ProductType::App;
    });
    return n == nullptr; // If no Application target found, then skip steps
}

FilePath AndroidManager::manifestSourcePath(const Target *target)
{
    if (const ProjectNode *node = currentProjectNode(target)) {
        const QString packageSource
                = node->data(Android::Constants::AndroidPackageSourceDir).toString();
        if (!packageSource.isEmpty()) {
            const FilePath manifest = FilePath::fromUserInput(packageSource + "/AndroidManifest.xml");
            if (manifest.exists())
                return manifest;
        }
    }
    return manifestPath(target);
}

FilePath AndroidManager::manifestPath(const Target *target)
{
    QVariant manifest = target->namedSettings(AndroidManifestName);
    if (manifest.isValid())
        return manifest.value<FilePath>();
    return androidBuildDirectory(target).pathAppended(AndroidManifestName);
}

void AndroidManager::setManifestPath(Target *target, const FilePath &path)
{
     target->setNamedSettings(AndroidManifestName, QVariant::fromValue(path));
}

QString AndroidManager::deviceSerialNumber(const Target *target)
{
    return target->namedSettings(AndroidDeviceSn).toString();
}

void AndroidManager::setDeviceSerialNumber(Target *target, const QString &deviceSerialNumber)
{
    qCDebug(androidManagerLog) << "Target device serial changed:"
                               << target->displayName() << deviceSerialNumber;
    target->setNamedSettings(AndroidDeviceSn, deviceSerialNumber);
}

static QString preferredAbi(const QStringList &appAbis, const Target *target)
{
    const auto deviceAbis = target->namedSettings(AndroidDeviceAbis).toStringList();
    for (const auto &abi : deviceAbis) {
        if (appAbis.contains(abi))
            return abi;
    }
    return {};
}

QString AndroidManager::apkDevicePreferredAbi(const Target *target)
{
    const FilePath libsPath = androidBuildDirectory(target).pathAppended("libs");
    if (!libsPath.exists()) {
        if (const ProjectNode *node = currentProjectNode(target)) {
            const QString abi = preferredAbi(
                        node->data(Android::Constants::AndroidAbis).toStringList(), target);
            if (abi.isEmpty())
                return node->data(Android::Constants::AndroidAbi).toString();
        }
    }
    QStringList apkAbis;
    const FilePaths libsPaths = libsPath.dirEntries(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const FilePath &abiDir : libsPaths) {
        if (!abiDir.dirEntries({{"*.so"}, QDir::Files | QDir::NoDotAndDotDot}).isEmpty())
            apkAbis << abiDir.fileName();
    }
    return preferredAbi(apkAbis, target);
}

void AndroidManager::setDeviceAbis(Target *target, const QStringList &deviceAbis)
{
    target->setNamedSettings(AndroidDeviceAbis, deviceAbis);
}

int AndroidManager::deviceApiLevel(const Target *target)
{
    return target->namedSettings(ApiLevelKey).toInt();
}

void AndroidManager::setDeviceApiLevel(Target *target, int level)
{
    qCDebug(androidManagerLog) << "Target device API level changed:"
                               << target->displayName() << level;
    target->setNamedSettings(ApiLevelKey, level);
}

int AndroidManager::defaultMinimumSDK(const QtSupport::QtVersion *qtVersion)
{
    if (qtVersion && qtVersion->qtVersion() >= QVersionNumber(6, 0))
        return 23;
    else if (qtVersion && qtVersion->qtVersion() >= QVersionNumber(5, 13))
        return 21;
    else
        return 16;
}

QString AndroidManager::androidNameForApiLevel(int x)
{
    switch (x) {
    case 2:
        return QLatin1String("Android 1.1");
    case 3:
        return QLatin1String("Android 1.5 (Cupcake)");
    case 4:
        return QLatin1String("Android 1.6 (Donut)");
    case 5:
        return QLatin1String("Android 2.0 (Eclair)");
    case 6:
        return QLatin1String("Android 2.0.1 (Eclair)");
    case 7:
        return QLatin1String("Android 2.1 (Eclair)");
    case 8:
        return QLatin1String("Android 2.2 (Froyo)");
    case 9:
        return QLatin1String("Android 2.3 (Gingerbread)");
    case 10:
        return QLatin1String("Android 2.3.3 (Gingerbread)");
    case 11:
        return QLatin1String("Android 3.0 (Honeycomb)");
    case 12:
        return QLatin1String("Android 3.1 (Honeycomb)");
    case 13:
        return QLatin1String("Android 3.2 (Honeycomb)");
    case 14:
        return QLatin1String("Android 4.0 (IceCreamSandwich)");
    case 15:
        return QLatin1String("Android 4.0.3 (IceCreamSandwich)");
    case 16:
        return QLatin1String("Android 4.1 (Jelly Bean)");
    case 17:
        return QLatin1String("Android 4.2 (Jelly Bean)");
    case 18:
        return QLatin1String("Android 4.3 (Jelly Bean)");
    case 19:
        return QLatin1String("Android 4.4 (KitKat)");
    case 20:
        return QLatin1String("Android 4.4W (KitKat Wear)");
    case 21:
        return QLatin1String("Android 5.0 (Lollipop)");
    case 22:
        return QLatin1String("Android 5.1 (Lollipop)");
    case 23:
        return QLatin1String("Android 6.0 (Marshmallow)");
    case 24:
        return QLatin1String("Android 7.0 (Nougat)");
    case 25:
        return QLatin1String("Android 7.1.1 (Nougat)");
    case 26:
        return QLatin1String("Android 8.0 (Oreo)");
    case 27:
        return QLatin1String("Android 8.1 (Oreo)");
    case 28:
        return QLatin1String("Android 9.0 (Pie)");
    case 29:
        return QLatin1String("Android 10.0 (Q)");
    case 30:
        return QLatin1String("Android 11.0 (R)");
    case 31:
        return QLatin1String("Android 12.0 (S)");
    case 32:
        return QLatin1String("Android 12L (Sv2, API 32)");
    case 33:
        return QLatin1String("Android 13.0 (Tiramisu)");
    default:
        return Tr::tr("Unknown Android version. API Level: %1").arg(x);
    }
}

static void raiseError(const QString &reason)
{
    QMessageBox::critical(nullptr, Tr::tr("Error creating Android templates."), reason);
}

static bool openXmlFile(QDomDocument &doc, const FilePath &fileName)
{
    QFile f(fileName.toString());
    if (!f.open(QIODevice::ReadOnly))
        return false;

    if (!doc.setContent(f.readAll())) {
        raiseError(Tr::tr("Cannot parse \"%1\".").arg(fileName.toUserOutput()));
        return false;
    }
    return true;
}

static bool openManifest(const Target *target, QDomDocument &doc)
{
    return openXmlFile(doc, AndroidManager::manifestPath(target));
}

static int parseMinSdk(const QDomElement &manifestElem)
{
    QDomElement usesSdk = manifestElem.firstChildElement(QLatin1String("uses-sdk"));
    if (usesSdk.isNull())
        return 0;
    if (usesSdk.hasAttribute(QLatin1String("android:minSdkVersion"))) {
        bool ok;
        int tmp = usesSdk.attribute(QLatin1String("android:minSdkVersion")).toInt(&ok);
        if (ok)
            return tmp;
    }
    return 0;
}

void AndroidManager::installQASIPackage(Target *target, const FilePath &packagePath)
{
    const QStringList appAbis = AndroidManager::applicationAbis(target);
    if (appAbis.isEmpty())
        return;
    const IDevice::ConstPtr device = DeviceKitAspect::device(target->kit());
    AndroidDeviceInfo info = AndroidDevice::androidDeviceInfoFromIDevice(device.data());
    if (!info.isValid()) // aborted
        return;

    QString deviceSerialNumber = info.serialNumber;
    if (info.type == IDevice::Emulator) {
        deviceSerialNumber = AndroidAvdManager().startAvd(info.avdName);
        if (deviceSerialNumber.isEmpty())
            Core::MessageManager::writeDisrupting(Tr::tr("Starting Android virtual device failed."));
    }

    QStringList arguments = AndroidDeviceInfo::adbSelector(deviceSerialNumber);
    arguments << "install" << "-r " << packagePath.path();
    QString error;
    if (!runAdbCommandDetached(arguments, &error, true))
        Core::MessageManager::writeDisrupting(
            Tr::tr("Android package installation failed.\n%1").arg(error));
}

bool AndroidManager::checkKeystorePassword(const FilePath &keystorePath,
                                           const QString &keystorePasswd)
{
    if (keystorePasswd.isEmpty())
        return false;
    const CommandLine cmd(AndroidConfigurations::currentConfig().keytoolPath(),
                          {"-list", "-keystore", keystorePath.toUserOutput(),
                           "--storepass", keystorePasswd});
    Process proc;
    proc.setTimeoutS(10);
    proc.setCommand(cmd);
    proc.runBlocking(EventLoopMode::On);
    return proc.result() == ProcessResult::FinishedWithSuccess;
}

bool AndroidManager::checkCertificatePassword(const FilePath &keystorePath,
                                              const QString &keystorePasswd,
                                              const QString &alias,
                                              const QString &certificatePasswd)
{
    // assumes that the keystore password is correct
    QStringList arguments = {"-certreq", "-keystore", keystorePath.toUserOutput(),
                             "--storepass", keystorePasswd, "-alias", alias, "-keypass"};
    if (certificatePasswd.isEmpty())
        arguments << keystorePasswd;
    else
        arguments << certificatePasswd;

    Process proc;
    proc.setTimeoutS(10);
    proc.setCommand({AndroidConfigurations::currentConfig().keytoolPath(), arguments});
    proc.runBlocking(EventLoopMode::On);
    return proc.result() == ProcessResult::FinishedWithSuccess;
}

bool AndroidManager::checkCertificateExists(const FilePath &keystorePath,
                                            const QString &keystorePasswd, const QString &alias)
{
    // assumes that the keystore password is correct
    QStringList arguments = { "-list", "-keystore", keystorePath.toUserOutput(),
                              "--storepass", keystorePasswd, "-alias", alias };

    Process proc;
    proc.setTimeoutS(10);
    proc.setCommand({AndroidConfigurations::currentConfig().keytoolPath(), arguments});
    proc.runBlocking(EventLoopMode::On);
    return proc.result() == ProcessResult::FinishedWithSuccess;
}

QProcess *AndroidManager::runAdbCommandDetached(const QStringList &args, QString *err,
                                                bool deleteOnFinish)
{
    std::unique_ptr<QProcess> p(new QProcess);
    const FilePath adb = AndroidConfigurations::currentConfig().adbToolPath();
    qCDebug(androidManagerLog).noquote() << "Running command (async):"
                                         << CommandLine(adb, args).toUserOutput();
    p->start(adb.toString(), args);
    if (p->waitForStarted(500) && p->state() == QProcess::Running) {
        if (deleteOnFinish) {
            connect(p.get(), &QProcess::finished, p.get(), &QObject::deleteLater);
        }
        return p.release();
    }

    QString errorStr = QString::fromUtf8(p->readAllStandardError());
    qCDebug(androidManagerLog).noquote() << "Running command (async) failed:"
                                         << CommandLine(adb, args).toUserOutput()
                                         << "Output:" << errorStr;
    if (err)
        *err = errorStr;
    return nullptr;
}

SdkToolResult AndroidManager::runCommand(const CommandLine &command,
                                         const QByteArray &writeData, int timeoutS)
{
    Android::SdkToolResult cmdResult;
    Process cmdProc;
    cmdProc.setTimeoutS(timeoutS);
    cmdProc.setWriteData(writeData);
    qCDebug(androidManagerLog) << "Running command (sync):" << command.toUserOutput();
    cmdProc.setCommand(command);
    cmdProc.runBlocking(EventLoopMode::On);
    cmdResult.m_stdOut = cmdProc.cleanedStdOut().trimmed();
    cmdResult.m_stdErr = cmdProc.cleanedStdErr().trimmed();
    cmdResult.m_success = cmdProc.result() == ProcessResult::FinishedWithSuccess;
    qCDebug(androidManagerLog) << "Command finshed (sync):" << command.toUserOutput()
                               << "Success:" << cmdResult.m_success
                               << "Output:" << cmdProc.allRawOutput();
    if (!cmdResult.success())
        cmdResult.m_exitMessage = cmdProc.exitMessage();
    return cmdResult;
}

SdkToolResult AndroidManager::runAdbCommand(const QStringList &args,
                                            const QByteArray &writeData, int timeoutS)
{
    return runCommand({AndroidConfigurations::currentConfig().adbToolPath(), args},
                      writeData, timeoutS);
}
} // namespace Android
