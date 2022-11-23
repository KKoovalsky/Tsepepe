## Initialize request from YCM Completer

    {
      "id": 1,
      "jsonrpc": "2.0",
      "method": "initialize",
      "params": {
        "capabilities": {
          "textDocument": {
            "codeAction": {
              "codeActionLiteralSupport": {
                "codeActionKind": {
                  "valueSet": [
                    "",
                    "quickfix",
                    "refactor",
                    "refactor.extract",
                    "refactor.inline",
                    "refactor.rewrite",
                    "source",
                    "source.organizeImports"
                  ]
                }
              }
            },
            "completion": {
              "completionItem": {
                "documentationFormat": [
                  "plaintext",
                  "markdown"
                ]
              },
              "completionItemKind": {
                "valueSet": [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]
              }
            },
            "documentSymbol": {
              "hierarchicalDocumentSymbolSupport": false,
              "labelSupport": false,
              "symbolKind": {
                "valueSet": [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26 ]
              }
            },
            "hover": {
              "contentFormat": [
                "plaintext",
                "markdown"
              ]
            },
            "inlay_hint": {},
            "semanticTokens": {
              "augmentSyntaxTokens": true,
              "formats": [
                "relative"
              ],
              "requests": {
                "full": {
                  "delta": false
                },
                "range": true
              },
              "tokenModifiers": [],
              "tokenTypes": [
                "namespace",
                "type",
                "class",
                "enum",
                "interface",
                "struct",
                "typeParameter",
                "parameter",
                "variable",
                "property",
                "enumMember",
                "event",
                "function",
                "method",
                "member",
                "macro",
                "keyword",
                "modifier",
                "comment",
                "string",
                "number",
                "regexp",
                "operator"
              ]
            },
            "signatureHelp": {
              "signatureInformation": {
                "documentationFormat": [
                  "plaintext",
                  "markdown"
                ],
                "parameterInformation": {
                  "labelOffsetSupport": true
                }
              }
            },
            "synchronization": {
              "didSave": true
            }
          },
          "workspace": {
            "applyEdit": true,
            "didChangeWatchedFiles": {
              "dynamicRegistration": true
            },
            "symbol": {
              "symbolKind": {
                "valueSet": [
                  1,
                  2,
                  3,
                  4,
                  5,
                  6,
                  7,
                  8,
                  9,
                  10,
                  11,
                  12,
                  13,
                  14,
                  15,
                  16,
                  17,
                  18,
                  19,
                  20,
                  21,
                  22,
                  23,
                  24,
                  25,
                  26
                ]
              }
            },
            "workspaceEdit": {
              "documentChanges": true
            },
            "workspaceFolders": true
          }
        },
        "initializationOptions": {},
        "processId": 52594,
        "rootPath": "/home/kacper/Workspace/Tsepepe",
        "rootUri": "file:///home/kacper/Workspace/Tsepepe",
        "workspaceFolders": [
          {
            "name": "Tsepepe",
            "uri": "file:///home/kacper/Workspace/Tsepepe"
          }
        ]
      }
    }

## Basic initialize request

    Content-Length: 89

    {"jsonrpc":"2.0","id":0,"method":"initialize","params":{"capabilities":{},"trace":"off"}}

## Clangd initialize response

    {
       "id":1,
       "jsonrpc":"2.0",
       "result":{
          "capabilities":{
             "astProvider":true,
             "callHierarchyProvider":true,
             "clangdInlayHintsProvider":true,
             "codeActionProvider":{
                "codeActionKinds":[
                   "quickfix",
                   "refactor",
                   "info"
                ]
             },
             "compilationDatabase":{
                "automaticReload":true
             },
             "completionProvider":{
                "resolveProvider":false,
                "triggerCharacters":[
                   ".",
                   "<",
                   ">",
                   ":",
                   "\"",
                   "/",
                   "*"
                ]
             },
             "declarationProvider":true,
             "definitionProvider":true,
             "documentFormattingProvider":true,
             "documentHighlightProvider":true,
             "documentLinkProvider":{
                "resolveProvider":false
             },
             "documentOnTypeFormattingProvider":{
                "firstTriggerCharacter":"\n",
                "moreTriggerCharacter":[
                   
                ]
             },
             "documentRangeFormattingProvider":true,
             "documentSymbolProvider":true,
             "executeCommandProvider":{
                "commands":[
                   "clangd.applyFix",
                   "clangd.applyTweak"
                ]
             },
             "hoverProvider":true,
             "implementationProvider":true,
             "inlayHintProvider":true,
             "memoryUsageProvider":true,
             "referencesProvider":true,
             "renameProvider":true,
             "selectionRangeProvider":true,
             "semanticTokensProvider":{
                "full":{
                   "delta":true
                },
                "legend":{
                   "tokenModifiers":[
                      "declaration",
                      "deprecated",
                      "deduced",
                      "readonly",
                      "static",
                      "abstract",
                      "virtual",
                      "dependentName",
                      "defaultLibrary",
                      "usedAsMutableReference",
                      "functionScope",
                      "classScope",
                      "fileScope",
                      "globalScope"
                   ],
                   "tokenTypes":[
                      "variable",
                      "variable",
                      "parameter",
                      "function",
                      "method",
                      "function",
                      "property",
                      "variable",
                      "class",
                      "interface",
                      "enum",
                      "enumMember",
                      "type",
                      "type",
                      "unknown",
                      "namespace",
                      "typeParameter",
                      "concept",
                      "type",
                      "macro",
                      "comment"
                   ]
                },
                "range":false
             },
             "signatureHelpProvider":{
                "triggerCharacters":[
                   "(",
                   ")",
                   "{",
                   "}",
                   "<",
                   ">",
                   ","
                ]
             },
             "standardTypeHierarchyProvider":true,
             "textDocumentSync":{
                "change":2,
                "openClose":true,
                "save":true
             },
             "typeDefinitionProvider":true,
             "typeHierarchyProvider":true,
             "workspaceSymbolProvider":true
          },
          "serverInfo":{
             "name":"clangd",
             "version":"clangd version 15.0.1 (https://github.com/ycm-core/llvm 708056a3d8259ce1d9fc0f15676d13b53cc23835) linux x86_64-unknown-linux-gnu"
          }
       }
    }
