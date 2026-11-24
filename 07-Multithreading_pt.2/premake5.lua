local function vendorRoot()
  if os.isdir("../00-Dependencies/raylib") then
    return path.getabsolute("../00-Dependencies")
  end
  error("Dossier dependencies introuvable.")
end

local vendor = vendorRoot()
local pthreadDll = vendor .. "/pthread/bin/pthreadVC2.dll"

print(vendor .. "/pthread/include")

workspace "cups"
  configurations { "Release", "Debug" }
  location "build"

  language "C++"
  cppdialect "C++17"
  pic "On"

  filter "system:linux"
    links { "pthread", "m" }
  filter "system:windows"
    architecture "x86_64"
    defines { "HAVE_STRUCT_TIMESPEC" }
    includedirs { vendor .. "/pthread/include" }
    libdirs { vendor .. "/pthread/lib" }
    links { "ws2_32" }
  filter { "system:windows" }
    links { "pthreadVC2" }
  filter { "configurations:Release", "toolset:msc" }
    optimize "Size"
  filter { "configurations:Release", "toolset:not msc" }
    optimize "Size"
  filter { "configurations:Debug", "toolset:msc" }
    symbols "On"
    defines { "__DEBUG" }
  filter { "configurations:Debug", "toolset:not msc" }
    symbols "On"
    defines { "__DEBUG" }
  filter {}

  local core = {
    "src/network/address.cpp",
    "src/network/file_descriptor.cpp",
    "src/network/poller.cpp",
    "src/network/socket.cpp",
    "src/network/timestamp.cpp",
    "src/cups/state/IsPrintingState.cpp",
    "src/cups/state/OffState.cpp",
    "src/cups/state/OnState.cpp",
    "src/cups/state/SuspendState.cpp",
    "src/cups/User.cpp",
    "src/cups/Spooler.cpp",
    "src/cups/Printer.cpp",
  }

project "cups_core"
  kind "StaticLib"
  files(core)
  includedirs { "src" }

  local apps = { "server", "client", "printer" }
  for _, app in ipairs(apps) do
    project(app)
      kind "ConsoleApp"
      files { "src/" .. app .. ".cpp" }
      includedirs { "src" }
      links { "cups_core" }
      filter "configurations:Debug"
        targetname(app .. "d")
      filter "configurations:Release"
        targetname(app)
      --filter { "system:windows", "toolset:msc", "configurations:Release" }
        --postbuildcommands { '{COPYFILE} "' .. pthreadDll .. '" " ./bin/Release"' }
      --filter { "system:windows", "toolset:msc", "configurations:Debug" }
        --postbuildcommands { '{COPYFILE} "' .. pthreadDll .. '" " ./bin/Debug"' }
      --filter { "system:windows", "toolset:not msc", "configurations:Release" }
        --postbuildcommands { '{COPYFILE} "' .. pthreadDll .. '" " ./bin/Release"' }
      --filter { "system:windows", "toolset:not msc", "configurations:Debug" }
        --postbuildcommands { '{COPYFILE} "' .. pthreadDll .. '" " ./bin/Debug"' }
      filter {}
  end
