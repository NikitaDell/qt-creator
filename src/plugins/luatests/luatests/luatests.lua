-- Copyright (C) 2024 The Qt Company Ltd.
-- SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0
return {
    Name = "LuaTests",
    Version = "1.0.0",
    CompatVersion = "1.0.0",
    Vendor = "The Qt Company",
    Category = "Tests",
    DisabledByDefault = true,
    Experimental = true,
    Description = "This plugin tests the Lua API.",
    LongDescription = [[
        It has tests for (almost) all functionality exposed by the API.
    ]],
    Dependencies = {
        { Name = "Core", Version = "13.0.82", Required = true },
        { Name = "Lua",  Version = "13.0.82", Required = true }
    },
    setup = function() require 'tests'.setup() end,
} --[[@as QtcPlugin]]
