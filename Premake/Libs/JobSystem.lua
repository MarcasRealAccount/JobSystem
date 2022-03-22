if not libs then libs = {} end
if not libs.jobsystem then
	libs.jobsystem = {
		name       = "",
		location   = ""
	}
end

require("../../ThirdParty/Fibers/Premake/Libs/Fibers")

local jobsystem = libs.jobsystem

function jobsystem:setup()
	self.name     = common:projectName()
	self.location = common:projectLocation()

	kind("StaticLib")
	common:outDirs(false)
	
	common:addPCH(self.location .. "/Source/PCH.cpp", self.location .. "/Source/PCH.h")

	includedirs({
		self.location .. "/Include/",
		self.location .. "/Source/"
	})

	libs.fibers:setupDep()

	files({
		self.location .. "/Include/**",
		self.location .. "/Source/**"
	})
	removefiles({ "*.DS_Store" })
end

function jobsystem:setupDep()
	links({ self.name })
	sysincludedirs({ self.location .. "/include/" })

	libs.fibers:setupDep()
end
