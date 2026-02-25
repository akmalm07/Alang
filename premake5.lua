workspace "Alang"
    architecture "x64"
    startproject "alang-seed-compiler"

    configurations { "Debug", "Release" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder
IncludeDir = {}
-- IncludeDir["item"] = "vendors/item/include"


LibDir = {}
-- LibDir["item"] = "vendors/item/lib"



group "alang-seed-compiler"
    include "alang-seed-compiler/alang-seed-compiler.lua"