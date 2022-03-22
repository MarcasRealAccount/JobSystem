require("Premake/Common")

require("Premake/Libs/JobSystem")

require("ThirdParty/Fibers/Premake/Libs/Fibers")

workspace("JobSystem")
	common:setConfigsAndPlatforms()

	common:addCoreDefines()

	cppdialect("C++20")
	rtti("Off")
	exceptionhandling("On")
	flags("MultiProcessorCompile")

	startproject("Tests")

	group("Dependencies")
	project("Fibers")
		location("ThirdParty/Fibers/Fibers/")
		warnings("Off")
		libs.fibers:setup()
		location("ThirdParty/")

	project("JobSystem")
	    location("JobSystem/")
	    warnings("Extra")
		libs.jobsystem:setup()
		common:addActions()

	project("Tests")
		location("Tests/")
		warnings("Extra")

		kind("ConsoleApp")
		common:outDirs()
		common:debugDir()

		common:addPCH("%{prj.location}/Source/PCH.cpp", "%{prj.location}/Source/PCH.h")

		includedirs({ "%{prj.location}/Source/" })

		if common.host == "windows" then
			linkoptions({ "/IGNORE:4099" })
		elseif common.host == "macosx" then
			linkoptions({ "-Wl,-rpath,'@executable_path'" })
		end

		libs.jobsystem:setupDep()

		files({ "%{prj.location}/Source/**" })
		removefiles({ "*.DS_Store" })

		common:addActions()
