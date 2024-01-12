// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#pragma once

#include "jsonobject.h"

namespace LanguageServerProtocol {

constexpr Key actionsKey{"actions"};
constexpr Key activeParameterKey{"activeParameter"};
constexpr Key activeParameterSupportKey{"activeParameterSupport"};
constexpr Key activeSignatureKey{"activeSignature"};
constexpr Key addedKey{"added"};
constexpr Key additionalTextEditsKey{"additionalTextEdits"};
constexpr Key alphaKey{"alpha"};
constexpr Key appliedKey{"applied"};
constexpr Key applyEditKey{"applyEdit"};
constexpr Key argumentsKey{"arguments"};
constexpr Key blueKey{"blue"};
constexpr Key callHierarchyKey{"callHierarchy"};
constexpr Key callHierarchyProviderKey{"callHierarchyProvider"};
constexpr Key cancellableKey{"cancellable"};
constexpr Key capabilitiesKey{"capabilities"};
constexpr Key chKey{"ch"};
constexpr Key changeKey{"change"};
constexpr Key changeNotificationsKey{"changeNotifications"};
constexpr Key changesKey{"changes"};
constexpr Key characterKey{"character"};
constexpr Key childrenKey{"children"};
constexpr Key clientInfoKey{"clientInfo"};
constexpr Key codeActionKey{"codeAction"};
constexpr Key codeActionKindKey{"codeActionKind"};
constexpr Key codeActionKindsKey{"codeActionKinds"};
constexpr Key codeActionLiteralSupportKey{"codeActionLiteralSupport"};
constexpr Key codeActionProviderKey{"codeActionProvider"};
constexpr Key codeKey{"code"};
constexpr Key codeLensKey{"codeLens"};
constexpr Key codeLensProviderKey{"codeLensProvider"};
constexpr Key colorInfoKey{"colorInfo"};
constexpr Key colorKey{"color"};
constexpr Key colorProviderKey{"colorProvider"};
constexpr Key commandKey{"command"};
constexpr Key commandsKey{"commands"};
constexpr Key commitCharacterSupportKey{"commitCharacterSupport"};
constexpr Key commitCharactersKey{"commitCharacters"};
constexpr Key completionItemKey{"completionItem"};
constexpr Key completionItemKindKey{"completionItemKind"};
constexpr Key completionKey{"completion"};
constexpr Key completionProviderKey{"completionProvider"};
constexpr Key configurationKey{"configuration"};
constexpr Key containerNameKey{"containerName"};
constexpr Key contentChangesKey{"contentChanges"};
constexpr Key contentFormatKey{"contentFormat"};
constexpr Key contentKey{"value"};
constexpr Key contentsKey{"contents"};
constexpr Key contextKey{"context"};
constexpr Key contextSupportKey{"contextSupport"};
constexpr Key dataKey{"data"};
constexpr Key definitionKey{"definition"};
constexpr Key definitionProviderKey{"definitionProvider"};
constexpr Key deleteCountKey{"deleteCount"};
constexpr Key deltaKey{"delta"};
constexpr Key deprecatedKey{"deprecated"};
constexpr Key detailKey{"detail"};
constexpr Key diagnosticsKey{"diagnostics"};
constexpr Key didChangeConfigurationKey{"didChangeConfiguration"};
constexpr Key didChangeWatchedFilesKey{"didChangeWatchedFiles"};
constexpr Key didSaveKey{"didSave"};
constexpr Key documentChangesKey{"documentChanges"};
constexpr Key documentFormattingProviderKey{"documentFormattingProvider"};
constexpr Key documentHighlightKey{"documentHighlight"};
constexpr Key documentHighlightProviderKey{"documentHighlightProvider"};
constexpr Key documentLinkKey{"documentLink"};
constexpr Key documentLinkProviderKey{"documentLinkProvider"};
constexpr Key documentRangeFormattingProviderKey{"documentRangeFormattingProvider"};
constexpr Key documentSelectorKey{"documentSelector"};
constexpr Key documentSymbolKey{"documentSymbol"};
constexpr Key documentSymbolProviderKey{"documentSymbolProvider"};
constexpr Key documentationFormatKey{"documentationFormat"};
constexpr Key documentationKey{"documentation"};
constexpr Key dynamicRegistrationKey{"dynamicRegistration"};
constexpr Key editKey{"edit"};
constexpr Key editsKey{"edits"};
constexpr Key endKey{"end"};
constexpr Key errorKey{"error"};
constexpr Key eventKey{"event"};
constexpr Key executeCommandKey{"executeCommand"};
constexpr Key executeCommandProviderKey{"executeCommandProvider"};
constexpr Key experimentalKey{"experimental"};
constexpr Key filterTextKey{"filterText"};
constexpr Key firstTriggerCharacterKey{"firstTriggerCharacter"};
constexpr Key formatsKey{"formats"};
constexpr Key formattingKey{"formatting"};
constexpr Key fromKey{"from"};
constexpr Key fromRangesKey{"fromRanges"};
constexpr Key fullKey{"full"};
constexpr Key greenKey{"green"};
constexpr Key hierarchicalDocumentSymbolSupportKey{"hierarchicalDocumentSymbolSupport"};
constexpr Key hoverKey{"hover"};
constexpr Key hoverProviderKey{"hoverProvider"};
constexpr Key idKey{"id"};
constexpr Key ignoreIfExistsKey{"ignoreIfExists"};
constexpr Key ignoreIfNotExistsKey{"ignoreIfNotExists"};
constexpr Key implementationKey{"implementation"};
constexpr Key implementationProviderKey{"implementationProvider"};
constexpr Key includeDeclarationKey{"includeDeclaration"};
constexpr Key includeTextKey{"includeText"};
constexpr Key initializationOptionsKey{"initializationOptions"};
constexpr Key insertFinalNewlineKey{"insertFinalNewline"};
constexpr Key insertSpaceKey{"insertSpace"};
constexpr Key insertTextFormatKey{"insertTextFormat"};
constexpr Key insertTextKey{"insertText"};
constexpr Key isIncompleteKey{"isIncomplete"};
constexpr Key itemKey{"item"};
constexpr Key itemsKey{"items"};
constexpr Key jsonRpcVersionKey{"jsonrpc"};
constexpr Key kindKey{"kind"};
constexpr Key labelKey{"label"};
constexpr Key languageIdKey{"languageId"};
constexpr Key languageKey{"language"};
constexpr Key legendKey{"legend"};
constexpr Key limitKey{"limit"};
constexpr Key lineKey{"line"};
constexpr Key linesKey{"lines"};
constexpr Key locationKey{"location"};
constexpr Key messageKey{"message"};
constexpr Key methodKey{"method"};
constexpr Key moreTriggerCharacterKey{"moreTriggerCharacter"};
constexpr Key multiLineTokenSupportKey{"multiLineTokenSupport"};
constexpr Key nameKey{"name"};
constexpr Key newNameKey{"newName"};
constexpr Key newTextKey{"newText"};
constexpr Key newUriKey{"newUri"};
constexpr Key oldUriKey{"oldUri"};
constexpr Key onTypeFormattingKey{"onTypeFormatting"};
constexpr Key onlyKey{"only"};
constexpr Key openCloseKey{"openClose"};
constexpr Key optionsKey{"options"};
constexpr Key overlappingTokenSupportKey{"overlappingTokenSupport"};
constexpr Key overwriteKey{"overwrite"};
constexpr Key parametersKey{"parameters"};
constexpr Key paramsKey{"params"};
constexpr Key patternKey{"pattern"};
constexpr Key percentageKey{"percentage"};
constexpr Key placeHolderKey{"placeHolder"};
constexpr Key positionKey{"position"};
constexpr Key prepareProviderKey{"prepareProvider"};
constexpr Key prepareSupportKey{"prepareSupport"};
constexpr Key previousResultIdKey{"previousResultId"};
constexpr Key processIdKey{"processId"};
constexpr Key queryKey{"query"};
constexpr Key rangeFormattingKey{"rangeFormatting"};
constexpr Key rangeKey{"range"};
constexpr Key rangeLengthKey{"rangeLength"};
constexpr Key reasonKey{"reason"};
constexpr Key recursiveKey{"recursive"};
constexpr Key redKey{"red"};
constexpr Key referencesKey{"references"};
constexpr Key referencesProviderKey{"referencesProvider"};
constexpr Key refreshSupportKey{"refreshSupport"};
constexpr Key registerOptionsKey{"registerOptions"};
constexpr Key registrationsKey{"registrations"};
constexpr Key removedKey{"removed"};
constexpr Key renameKey{"rename"};
constexpr Key renameProviderKey{"renameProvider"};
constexpr Key requestsKey{"requests"};
constexpr Key resolveProviderKey{"resolveProvider"};
constexpr Key resourceOperationsKey{"resourceOperations"};
constexpr Key resultIdKey{"resultId"};
constexpr Key resultKey{"result"};
constexpr Key retryKey{"retry"};
constexpr Key rootPathKey{"rootPath"};
constexpr Key rootUriKey{"rootUri"};
constexpr Key saveKey{"save"};
constexpr Key schemeKey{"scheme"};
constexpr Key scopeUriKey{"scopeUri"};
constexpr Key sectionKey{"section"};
constexpr Key selectionRangeKey{"selectionRange"};
constexpr Key semanticTokensKey{"semanticTokens"};
constexpr Key semanticTokensProviderKey{"semanticTokensProvider"};
constexpr Key serverInfoKey{"serverInfo"};
constexpr Key settingsKey{"settings"};
constexpr Key severityKey{"severity"};
constexpr Key signatureHelpKey{"signatureHelp"};
constexpr Key signatureHelpProviderKey{"signatureHelpProvider"};
constexpr Key signatureInformationKey{"signatureInformation"};
constexpr Key signaturesKey{"signatures"};
constexpr Key snippetSupportKey{"snippetSupport"};
constexpr Key sortTextKey{"sortText"};
constexpr Key sourceKey{"source"};
constexpr Key startKey{"start"};
constexpr Key supportedKey{"supported"};
constexpr Key symbolKey{"symbol"};
constexpr Key symbolKindKey{"symbolKind"};
constexpr Key syncKindKey{"syncKind"};
constexpr Key synchronizationKey{"synchronization"};
constexpr Key tabSizeKey{"tabSize"};
constexpr Key tagsKey{"tags"};
constexpr Key targetKey{"target"};
constexpr Key textDocumentKey{"textDocument"};
constexpr Key textDocumentSyncKey{"textDocumentSync"};
constexpr Key textEditKey{"textEdit"};
constexpr Key textKey{"text"};
constexpr Key titleKey{"title"};
constexpr Key toKey{"to"};
constexpr Key tokenKey{"token"};
constexpr Key tokenModifiersKey{"tokenModifiers"};
constexpr Key tokenTypesKey{"tokenTypes"};
constexpr Key traceKey{"trace"};
constexpr Key triggerCharacterKey{"triggerCharacter"};
constexpr Key triggerCharactersKey{"triggerCharacters"};
constexpr Key triggerKindKey{"triggerKind"};
constexpr Key trimFinalNewlinesKey{"trimFinalNewlines"};
constexpr Key trimTrailingWhitespaceKey{"trimTrailingWhitespace"};
constexpr Key typeDefinitionKey{"typeDefinition"};
constexpr Key typeDefinitionProviderKey{"typeDefinitionProvider"};
constexpr Key typeKey{"type"};
constexpr Key unregistrationsKey{"unregistrations"};
constexpr Key uriKey{"uri"};
constexpr Key valueKey{"value"};
constexpr Key valueSetKey{"valueSet"};
constexpr Key versionKey{"version"};
constexpr Key willSaveKey{"willSave"};
constexpr Key willSaveWaitUntilKey{"willSaveWaitUntil"};
constexpr Key windowKey{"window"};
constexpr Key workDoneProgressKey{"workDoneProgress"};
constexpr Key workspaceEditKey{"workspaceEdit"};
constexpr Key workspaceFoldersKey{"workspaceFolders"};
constexpr Key workspaceKey{"workspace"};
constexpr Key workspaceSymbolProviderKey{"workspaceSymbolProvider"};

} // namespace LanguageServerProtocol
