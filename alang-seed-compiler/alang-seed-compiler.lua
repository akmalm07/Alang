project "alang-seed-compiler"
    location "."
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")


    -- Remember to use -- for single line comments and --[[ Comment --]] for multiline comments

    -- Include directories
    includedirs 
    {
        -- "%{IncludeDir.item}", 
        "global",            
        "include",           
        "src"
    }

    -- Files
    files 
    {
        "src/**.cpp",           
        "include/**.h",     
        "include/**.inl",     

        "global/**.h",
        "global/**.cpp",

    }

    -- Library directories
    libdirs 
    { 
        -- "%{LibDir.item}",
    }

    -- Links
    links 
    { 
        -- "item",
    }

    pchheader "headers.h" -- For precompiled headers
    pchsource "global/headers.cpp" -- For precompiled headers

    defines 
    { 
        -- "ITEM", 
    } -- Nessesary definitions



    -- Toolset and compiler settings
    filter "toolset:msc"
        buildoptions { "/std:c++23" } 
        
    -- GCC/Clang settings
    filter "toolset:gcc or toolset:clang"
        buildoptions { "-std=c++23" }
    
    -- Configuration settings
    filter "configurations:Debug"
        defines "DEBUG"
        symbols "On"
        optimize "Off"
        runtime "Debug"  

    filter "configurations:Release"
        symbols "Off"
        optimize "On"
        defines "NDEBUG"
        runtime "Release"  

    -- Windows system settings
    filter "system:windows"
        systemversion "latest"
        defines "PLATFORM_WINDOWS"
        staticruntime "on"

    -- Linux Settings
    filter "system:linux"
        defines "PLATFORM_LINUX"
        staticruntime "off"
    
    -- Apple Settings
    filter "system:macosx"
        defines "PLATFORM_MACOS"
        buildoptions { "-stdlib=libc++" }

    -- Visual Studio specific settings
    filter "action:vs*"
        defines "_CRT_SECURE_NO_WARNINGS"
    
    filter {}
    