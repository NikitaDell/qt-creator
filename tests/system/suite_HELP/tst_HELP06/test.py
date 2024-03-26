# Copyright (C) 2016 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

source("../../shared/qtcreator.py")

# test bookmark functionality
def renameBookmarkFolder(view, item, newName):
    invokeContextMenuItemOnBookmarkFolder(view, item, "Rename Folder")
    replaceEditorContent(waitForObject(":Add Bookmark.treeView_QExpandingLineEdit"), newName)
    type(waitForObject(":Add Bookmark.treeView_QExpandingLineEdit"), "<Return>")
    return

def invokeContextMenuItemOnBookmarkFolder(view, item, menuItem):
    mouseClick(waitForObjectItem(view, item))
    openItemContextMenu(view, item, 5, 5, 0)
    activateItem(waitForObjectItem("{type='QMenu' unnamed='1' visible='1' "
                                   "window=':Add Bookmark_BookmarkDialog'}", menuItem))

def textForQtVersion(text):
    suffix = "Qt Creator Manual"
    if text != suffix:
        text += " | " + suffix
    return text

def main():
    startQC()
    if not startedWithoutPluginError():
        return
    # goto help mode and click on topic
    switchViewTo(ViewConstants.HELP)
    manualQModelIndex = getQModelIndexStr("text?='Qt Creator Manual *'",
                                          ":Qt Creator_QHelpContentWidget")
    manualQMIObj = waitForObject(manualQModelIndex)
    doubleClick(manualQMIObj, 5, 5, 0, Qt.LeftButton)
    if not waitFor("not manualQMIObj.collapsed", 2000):
        test.warning("It takes more than two seconds to expand the help content tree.")
    devQModelIndex = getQModelIndexStr("text='Developing with Qt Creator'", manualQModelIndex)
    doubleClick(devQModelIndex)
    gettingStartedQModelIndex = getQModelIndexStr("text='Getting Started'", devQModelIndex)
    doubleClick(gettingStartedQModelIndex)
    pageTitle = "Configuring Qt Creator"
    mouseClick(waitForObject(getQModelIndexStr("text='%s'" % pageTitle,
                                               gettingStartedQModelIndex)))
    helpSelector = waitForObject(":Qt Creator_HelpSelector_QComboBox")
    pageOpened = "str(helpSelector.currentText).startswith('%s')" % pageTitle
    if not waitFor(pageOpened, 10000):
        test.fatal("Help page is not opened after ten seconds. Giving up.")
        invokeMenuItem("File", "Exit")
        return
    # open bookmarks window
    clickButton(waitForObject(":Qt Creator.Add Bookmark_QToolButton"))
    clickButton(waitForObject(":Add Bookmark.ExpandBookmarksList_QToolButton"))
    # create root bookmark directory
    clickButton(waitForObject(":Add Bookmark.New Folder_QPushButton"))
    # rename root bookmark directory
    bookmarkDialog = waitForObject(":Add Bookmark_BookmarkDialog")
    waitFor("bookmarkDialog.isActiveWindow")
    bookmarkView = waitForObject(":Add Bookmark.treeView_QTreeView")
    renameBookmarkFolder(bookmarkView, "New Folder*", "Sample")
    # create two more subfolders
    clickButton(waitForObject(":Add Bookmark.New Folder_QPushButton"))
    renameBookmarkFolder(bookmarkView, "Sample.New Folder*", "Folder 1")
    clickButton(waitForObject(":Add Bookmark.New Folder_QPushButton"))
    renameBookmarkFolder(bookmarkView, "Sample.Folder 1.New Folder*", "Folder 2")
    clickButton(waitForObject(":Add Bookmark.OK_QPushButton"))
    mouseClick(manualQModelIndex)
    type(waitForObject(":Qt Creator_QHelpContentWidget"), "<Down>")
    clickButton(waitForObject(":Qt Creator.Add Bookmark_QToolButton"))
    clickButton(waitForObject(":Add Bookmark.ExpandBookmarksList_QToolButton"))
    # click on "Sample" and create new directory under it
    mouseClick(waitForObject(getQModelIndexStr("text='Sample'", ":Add Bookmark.treeView_QTreeView")))
    clickButton(waitForObject(":Add Bookmark.New Folder_QPushButton"))
    clickButton(waitForObject(":Add Bookmark.OK_QPushButton"))
    # choose bookmarks
    mouseClick(waitForObjectItem(":Qt Creator_Core::Internal::CommandComboBox", "Bookmarks"))
    # verify if all folders are created and bookmarks present
    sampleQModelIndex = getQModelIndexStr("text='Sample'", ":Qt Creator_Bookmarks_TreeView")
    folder1QModelIndex = getQModelIndexStr("text='Folder 1'", sampleQModelIndex)
    folder2QModelIndex = getQModelIndexStr("text='Folder 2'", folder1QModelIndex)
    configQModelIndex = getQModelIndexStr("text?='%s'" % textForQtVersion("%s*" % pageTitle),
                                          folder2QModelIndex)
    newFolderQModelIndex = getQModelIndexStr("text='New Folder'", sampleQModelIndex)
    manualQModelIndex = getQModelIndexStr("text='%s'" % textForQtVersion("Qt Creator Manual"),
                                             newFolderQModelIndex)
    test.verify(checkIfObjectExists(sampleQModelIndex, verboseOnFail = True) and
                checkIfObjectExists(folder1QModelIndex, verboseOnFail = True) and
                checkIfObjectExists(folder2QModelIndex, verboseOnFail = True) and
                checkIfObjectExists(configQModelIndex, verboseOnFail = True) and
                checkIfObjectExists(manualQModelIndex, verboseOnFail = True),
                "Verifying if all folders and bookmarks are present")
    mouseClick(waitForObject(":Qt Creator_Bookmarks_TreeView"), 5, 5, 0, Qt.LeftButton)
    for _ in range(6):
        type(waitForObject(":Qt Creator_Bookmarks_TreeView"), "<Right>")
    type(waitForObject(":Qt Creator_Bookmarks_TreeView"), "<Return>")
    test.verify(textForQtVersion(pageTitle) in getHelpTitle(),
                "Verifying if first bookmark is opened")
    mouseClick(waitForObject(configQModelIndex))
    type(waitForObject(":Qt Creator_Bookmarks_TreeView"), "<Down>")
    type(waitForObject(":Qt Creator_Bookmarks_TreeView"), "<Right>")
    type(waitForObject(":Qt Creator_Bookmarks_TreeView"), "<Down>")
    type(waitForObject(":Qt Creator_Bookmarks_TreeView"), "<Return>")
    test.verify(textForQtVersion("Qt Creator Manual") in getHelpTitle(),
                "Verifying if second bookmark is opened")
    # delete previously created directory
    clickButton(waitForObject(":Qt Creator.Add Bookmark_QToolButton"))
    clickButton(waitForObject(":Add Bookmark.ExpandBookmarksList_QToolButton"))
    invokeContextMenuItemOnBookmarkFolder(":Add Bookmark.treeView_QTreeView", "Sample.Folder 1",
                                          "Delete Folder")
    clickButton(waitForObject("{container=':Add Bookmark.treeView_QTreeView' text='Yes' "
                              "type='QPushButton' unnamed='1' visible='1'}"))
    # close bookmarks
    clickButton(waitForObject(":Add Bookmark.OK_QPushButton"))
    # choose bookmarks from command combobox
    mouseClick(waitForObject(":Qt Creator_Core::Internal::CommandComboBox"))
    mouseClick(waitForObjectItem(":Qt Creator_Core::Internal::CommandComboBox", "Bookmarks"))
    # verify if folders and bookmark deleted
    test.verify(checkIfObjectExists(sampleQModelIndex, verboseOnFail = True) and
                checkIfObjectExists(folder1QModelIndex, shouldExist = False, verboseOnFail = True) and
                checkIfObjectExists(folder2QModelIndex, shouldExist = False, verboseOnFail = True) and
                checkIfObjectExists(configQModelIndex, shouldExist = False, verboseOnFail = True) and
                checkIfObjectExists(manualQModelIndex, verboseOnFail = True),
                "Verifying if folder 1 and folder 2 deleted including their bookmark")
    # exit
    invokeMenuItem("File", "Exit")
