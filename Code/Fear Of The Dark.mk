##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=Fear Of The Dark
ConfigurationName      :=Debug
WorkspacePath          :=/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code
ProjectPath            :=/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=UndistinguishedProgrammer
Date                   :=19/06/20
CodeLitePath           :=/home/davis-dev/.codelite
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=bin/Debug/Fear Of The Dark
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="Fear Of The Dark.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -Wl,-Bstatic /usr/lib/x86_64-linux-gnu/libboost_thread.a /usr/lib/x86_64-linux-gnu/libboost_filesystem.a /usr/lib/x86_64-linux-gnu/libboost_system.a -llua5.3 -Wl,-Bdynamic ../../GLEngine/bin/Release/libGLEngine.a -lCEGUIBase-0 -lCEGUIOpenGLRenderer-0 -lSDL2 -lSDL2_mixer -lSDL2_ttf -lGL -lGLU -lGLEW -lpthread 
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)Items $(IncludeSwitch)Code $(IncludeSwitch)Code/Entities $(IncludeSwitch)Code/QuestSystem $(IncludeSwitch)include $(IncludeSwitch)Code/ $(IncludeSwitch)Code/Scripting $(IncludeSwitch)../../GLEngine 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)../../GLEngine 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS := -std=c++17 -g3 -g -DDEV_CONTROLS -DDEBUG -DLINUX -fexceptions -DLUA_APICHECK  $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/Entities_EntityFunctions.cpp$(ObjectSuffix) $(IntermediateDirectory)/ScriptingHelperFunctions.cpp$(ObjectSuffix) $(IntermediateDirectory)/EntityPlayer.cpp$(ObjectSuffix) $(IntermediateDirectory)/EntityProjectile.cpp$(ObjectSuffix) $(IntermediateDirectory)/EntityNPC.cpp$(ObjectSuffix) $(IntermediateDirectory)/GameplayScreen.cpp$(ObjectSuffix) $(IntermediateDirectory)/World.cpp$(ObjectSuffix) $(IntermediateDirectory)/Entity.cpp$(ObjectSuffix) $(IntermediateDirectory)/Logging.cpp$(ObjectSuffix) $(IntermediateDirectory)/LuaScript.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/XMLData.cpp$(ObjectSuffix) $(IntermediateDirectory)/OptionsMenuScreen.cpp$(ObjectSuffix) $(IntermediateDirectory)/PerlinNoise_PerlinNoise.cpp$(ObjectSuffix) $(IntermediateDirectory)/PerlinNoise_ppm.cpp$(ObjectSuffix) $(IntermediateDirectory)/ScriptQueue.cpp$(ObjectSuffix) $(IntermediateDirectory)/Tile.cpp$(ObjectSuffix) $(IntermediateDirectory)/Inventory.cpp$(ObjectSuffix) $(IntermediateDirectory)/XMLDataTypes.cpp$(ObjectSuffix) $(IntermediateDirectory)/QuestTypes.cpp$(ObjectSuffix) $(IntermediateDirectory)/Console.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/ScripterMain.cpp$(ObjectSuffix) $(IntermediateDirectory)/QuestManager.cpp$(ObjectSuffix) $(IntermediateDirectory)/Main.cpp$(ObjectSuffix) $(IntermediateDirectory)/Item.cpp$(ObjectSuffix) $(IntermediateDirectory)/EntityItem.cpp$(ObjectSuffix) $(IntermediateDirectory)/StartupScreen.cpp$(ObjectSuffix) $(IntermediateDirectory)/WaitScreen.cpp$(ObjectSuffix) $(IntermediateDirectory)/MainMenuScreen.cpp$(ObjectSuffix) $(IntermediateDirectory)/WorldIOManager.cpp$(ObjectSuffix) $(IntermediateDirectory)/AudioManager.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/App.cpp$(ObjectSuffix) $(IntermediateDirectory)/SaveDataTypes.cpp$(ObjectSuffix) $(IntermediateDirectory)/EventQueue.cpp$(ObjectSuffix) $(IntermediateDirectory)/LoadScreen.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/Entities_EntityFunctions.cpp$(ObjectSuffix): Entities/EntityFunctions.cpp $(IntermediateDirectory)/Entities_EntityFunctions.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/Entities/EntityFunctions.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Entities_EntityFunctions.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Entities_EntityFunctions.cpp$(DependSuffix): Entities/EntityFunctions.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Entities_EntityFunctions.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Entities_EntityFunctions.cpp$(DependSuffix) -MM Entities/EntityFunctions.cpp

$(IntermediateDirectory)/Entities_EntityFunctions.cpp$(PreprocessSuffix): Entities/EntityFunctions.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Entities_EntityFunctions.cpp$(PreprocessSuffix) Entities/EntityFunctions.cpp

$(IntermediateDirectory)/ScriptingHelperFunctions.cpp$(ObjectSuffix): ScriptingHelperFunctions.cpp $(IntermediateDirectory)/ScriptingHelperFunctions.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/ScriptingHelperFunctions.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ScriptingHelperFunctions.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ScriptingHelperFunctions.cpp$(DependSuffix): ScriptingHelperFunctions.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ScriptingHelperFunctions.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ScriptingHelperFunctions.cpp$(DependSuffix) -MM ScriptingHelperFunctions.cpp

$(IntermediateDirectory)/ScriptingHelperFunctions.cpp$(PreprocessSuffix): ScriptingHelperFunctions.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ScriptingHelperFunctions.cpp$(PreprocessSuffix) ScriptingHelperFunctions.cpp

$(IntermediateDirectory)/EntityPlayer.cpp$(ObjectSuffix): EntityPlayer.cpp $(IntermediateDirectory)/EntityPlayer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/EntityPlayer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/EntityPlayer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/EntityPlayer.cpp$(DependSuffix): EntityPlayer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/EntityPlayer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/EntityPlayer.cpp$(DependSuffix) -MM EntityPlayer.cpp

$(IntermediateDirectory)/EntityPlayer.cpp$(PreprocessSuffix): EntityPlayer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/EntityPlayer.cpp$(PreprocessSuffix) EntityPlayer.cpp

$(IntermediateDirectory)/EntityProjectile.cpp$(ObjectSuffix): EntityProjectile.cpp $(IntermediateDirectory)/EntityProjectile.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/EntityProjectile.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/EntityProjectile.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/EntityProjectile.cpp$(DependSuffix): EntityProjectile.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/EntityProjectile.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/EntityProjectile.cpp$(DependSuffix) -MM EntityProjectile.cpp

$(IntermediateDirectory)/EntityProjectile.cpp$(PreprocessSuffix): EntityProjectile.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/EntityProjectile.cpp$(PreprocessSuffix) EntityProjectile.cpp

$(IntermediateDirectory)/EntityNPC.cpp$(ObjectSuffix): EntityNPC.cpp $(IntermediateDirectory)/EntityNPC.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/EntityNPC.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/EntityNPC.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/EntityNPC.cpp$(DependSuffix): EntityNPC.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/EntityNPC.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/EntityNPC.cpp$(DependSuffix) -MM EntityNPC.cpp

$(IntermediateDirectory)/EntityNPC.cpp$(PreprocessSuffix): EntityNPC.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/EntityNPC.cpp$(PreprocessSuffix) EntityNPC.cpp

$(IntermediateDirectory)/GameplayScreen.cpp$(ObjectSuffix): GameplayScreen.cpp $(IntermediateDirectory)/GameplayScreen.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/GameplayScreen.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GameplayScreen.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GameplayScreen.cpp$(DependSuffix): GameplayScreen.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GameplayScreen.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GameplayScreen.cpp$(DependSuffix) -MM GameplayScreen.cpp

$(IntermediateDirectory)/GameplayScreen.cpp$(PreprocessSuffix): GameplayScreen.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GameplayScreen.cpp$(PreprocessSuffix) GameplayScreen.cpp

$(IntermediateDirectory)/World.cpp$(ObjectSuffix): World.cpp $(IntermediateDirectory)/World.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/World.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/World.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/World.cpp$(DependSuffix): World.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/World.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/World.cpp$(DependSuffix) -MM World.cpp

$(IntermediateDirectory)/World.cpp$(PreprocessSuffix): World.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/World.cpp$(PreprocessSuffix) World.cpp

$(IntermediateDirectory)/Entity.cpp$(ObjectSuffix): Entity.cpp $(IntermediateDirectory)/Entity.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/Entity.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Entity.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Entity.cpp$(DependSuffix): Entity.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Entity.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Entity.cpp$(DependSuffix) -MM Entity.cpp

$(IntermediateDirectory)/Entity.cpp$(PreprocessSuffix): Entity.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Entity.cpp$(PreprocessSuffix) Entity.cpp

$(IntermediateDirectory)/Logging.cpp$(ObjectSuffix): Logging.cpp $(IntermediateDirectory)/Logging.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/Logging.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Logging.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Logging.cpp$(DependSuffix): Logging.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Logging.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Logging.cpp$(DependSuffix) -MM Logging.cpp

$(IntermediateDirectory)/Logging.cpp$(PreprocessSuffix): Logging.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Logging.cpp$(PreprocessSuffix) Logging.cpp

$(IntermediateDirectory)/LuaScript.cpp$(ObjectSuffix): LuaScript.cpp $(IntermediateDirectory)/LuaScript.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/LuaScript.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/LuaScript.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/LuaScript.cpp$(DependSuffix): LuaScript.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/LuaScript.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/LuaScript.cpp$(DependSuffix) -MM LuaScript.cpp

$(IntermediateDirectory)/LuaScript.cpp$(PreprocessSuffix): LuaScript.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/LuaScript.cpp$(PreprocessSuffix) LuaScript.cpp

$(IntermediateDirectory)/XMLData.cpp$(ObjectSuffix): XMLData.cpp $(IntermediateDirectory)/XMLData.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/XMLData.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/XMLData.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/XMLData.cpp$(DependSuffix): XMLData.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/XMLData.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/XMLData.cpp$(DependSuffix) -MM XMLData.cpp

$(IntermediateDirectory)/XMLData.cpp$(PreprocessSuffix): XMLData.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/XMLData.cpp$(PreprocessSuffix) XMLData.cpp

$(IntermediateDirectory)/OptionsMenuScreen.cpp$(ObjectSuffix): OptionsMenuScreen.cpp $(IntermediateDirectory)/OptionsMenuScreen.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/OptionsMenuScreen.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/OptionsMenuScreen.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/OptionsMenuScreen.cpp$(DependSuffix): OptionsMenuScreen.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/OptionsMenuScreen.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/OptionsMenuScreen.cpp$(DependSuffix) -MM OptionsMenuScreen.cpp

$(IntermediateDirectory)/OptionsMenuScreen.cpp$(PreprocessSuffix): OptionsMenuScreen.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/OptionsMenuScreen.cpp$(PreprocessSuffix) OptionsMenuScreen.cpp

$(IntermediateDirectory)/PerlinNoise_PerlinNoise.cpp$(ObjectSuffix): PerlinNoise/PerlinNoise.cpp $(IntermediateDirectory)/PerlinNoise_PerlinNoise.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/PerlinNoise/PerlinNoise.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PerlinNoise_PerlinNoise.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PerlinNoise_PerlinNoise.cpp$(DependSuffix): PerlinNoise/PerlinNoise.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PerlinNoise_PerlinNoise.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PerlinNoise_PerlinNoise.cpp$(DependSuffix) -MM PerlinNoise/PerlinNoise.cpp

$(IntermediateDirectory)/PerlinNoise_PerlinNoise.cpp$(PreprocessSuffix): PerlinNoise/PerlinNoise.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PerlinNoise_PerlinNoise.cpp$(PreprocessSuffix) PerlinNoise/PerlinNoise.cpp

$(IntermediateDirectory)/PerlinNoise_ppm.cpp$(ObjectSuffix): PerlinNoise/ppm.cpp $(IntermediateDirectory)/PerlinNoise_ppm.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/PerlinNoise/ppm.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PerlinNoise_ppm.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PerlinNoise_ppm.cpp$(DependSuffix): PerlinNoise/ppm.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PerlinNoise_ppm.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PerlinNoise_ppm.cpp$(DependSuffix) -MM PerlinNoise/ppm.cpp

$(IntermediateDirectory)/PerlinNoise_ppm.cpp$(PreprocessSuffix): PerlinNoise/ppm.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PerlinNoise_ppm.cpp$(PreprocessSuffix) PerlinNoise/ppm.cpp

$(IntermediateDirectory)/ScriptQueue.cpp$(ObjectSuffix): ScriptQueue.cpp $(IntermediateDirectory)/ScriptQueue.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/ScriptQueue.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ScriptQueue.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ScriptQueue.cpp$(DependSuffix): ScriptQueue.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ScriptQueue.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ScriptQueue.cpp$(DependSuffix) -MM ScriptQueue.cpp

$(IntermediateDirectory)/ScriptQueue.cpp$(PreprocessSuffix): ScriptQueue.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ScriptQueue.cpp$(PreprocessSuffix) ScriptQueue.cpp

$(IntermediateDirectory)/Tile.cpp$(ObjectSuffix): Tile.cpp $(IntermediateDirectory)/Tile.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/Tile.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Tile.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Tile.cpp$(DependSuffix): Tile.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Tile.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Tile.cpp$(DependSuffix) -MM Tile.cpp

$(IntermediateDirectory)/Tile.cpp$(PreprocessSuffix): Tile.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Tile.cpp$(PreprocessSuffix) Tile.cpp

$(IntermediateDirectory)/Inventory.cpp$(ObjectSuffix): Inventory.cpp $(IntermediateDirectory)/Inventory.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/Inventory.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Inventory.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Inventory.cpp$(DependSuffix): Inventory.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Inventory.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Inventory.cpp$(DependSuffix) -MM Inventory.cpp

$(IntermediateDirectory)/Inventory.cpp$(PreprocessSuffix): Inventory.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Inventory.cpp$(PreprocessSuffix) Inventory.cpp

$(IntermediateDirectory)/XMLDataTypes.cpp$(ObjectSuffix): XMLDataTypes.cpp $(IntermediateDirectory)/XMLDataTypes.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/XMLDataTypes.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/XMLDataTypes.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/XMLDataTypes.cpp$(DependSuffix): XMLDataTypes.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/XMLDataTypes.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/XMLDataTypes.cpp$(DependSuffix) -MM XMLDataTypes.cpp

$(IntermediateDirectory)/XMLDataTypes.cpp$(PreprocessSuffix): XMLDataTypes.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/XMLDataTypes.cpp$(PreprocessSuffix) XMLDataTypes.cpp

$(IntermediateDirectory)/QuestTypes.cpp$(ObjectSuffix): QuestTypes.cpp $(IntermediateDirectory)/QuestTypes.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/QuestTypes.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/QuestTypes.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/QuestTypes.cpp$(DependSuffix): QuestTypes.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/QuestTypes.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/QuestTypes.cpp$(DependSuffix) -MM QuestTypes.cpp

$(IntermediateDirectory)/QuestTypes.cpp$(PreprocessSuffix): QuestTypes.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/QuestTypes.cpp$(PreprocessSuffix) QuestTypes.cpp

$(IntermediateDirectory)/Console.cpp$(ObjectSuffix): Console.cpp $(IntermediateDirectory)/Console.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/Console.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Console.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Console.cpp$(DependSuffix): Console.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Console.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Console.cpp$(DependSuffix) -MM Console.cpp

$(IntermediateDirectory)/Console.cpp$(PreprocessSuffix): Console.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Console.cpp$(PreprocessSuffix) Console.cpp

$(IntermediateDirectory)/ScripterMain.cpp$(ObjectSuffix): ScripterMain.cpp $(IntermediateDirectory)/ScripterMain.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/ScripterMain.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ScripterMain.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ScripterMain.cpp$(DependSuffix): ScripterMain.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ScripterMain.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ScripterMain.cpp$(DependSuffix) -MM ScripterMain.cpp

$(IntermediateDirectory)/ScripterMain.cpp$(PreprocessSuffix): ScripterMain.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ScripterMain.cpp$(PreprocessSuffix) ScripterMain.cpp

$(IntermediateDirectory)/QuestManager.cpp$(ObjectSuffix): QuestManager.cpp $(IntermediateDirectory)/QuestManager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/QuestManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/QuestManager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/QuestManager.cpp$(DependSuffix): QuestManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/QuestManager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/QuestManager.cpp$(DependSuffix) -MM QuestManager.cpp

$(IntermediateDirectory)/QuestManager.cpp$(PreprocessSuffix): QuestManager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/QuestManager.cpp$(PreprocessSuffix) QuestManager.cpp

$(IntermediateDirectory)/Main.cpp$(ObjectSuffix): Main.cpp $(IntermediateDirectory)/Main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/Main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Main.cpp$(DependSuffix): Main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Main.cpp$(DependSuffix) -MM Main.cpp

$(IntermediateDirectory)/Main.cpp$(PreprocessSuffix): Main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Main.cpp$(PreprocessSuffix) Main.cpp

$(IntermediateDirectory)/Item.cpp$(ObjectSuffix): Item.cpp $(IntermediateDirectory)/Item.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/Item.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Item.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Item.cpp$(DependSuffix): Item.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Item.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Item.cpp$(DependSuffix) -MM Item.cpp

$(IntermediateDirectory)/Item.cpp$(PreprocessSuffix): Item.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Item.cpp$(PreprocessSuffix) Item.cpp

$(IntermediateDirectory)/EntityItem.cpp$(ObjectSuffix): EntityItem.cpp $(IntermediateDirectory)/EntityItem.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/EntityItem.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/EntityItem.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/EntityItem.cpp$(DependSuffix): EntityItem.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/EntityItem.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/EntityItem.cpp$(DependSuffix) -MM EntityItem.cpp

$(IntermediateDirectory)/EntityItem.cpp$(PreprocessSuffix): EntityItem.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/EntityItem.cpp$(PreprocessSuffix) EntityItem.cpp

$(IntermediateDirectory)/StartupScreen.cpp$(ObjectSuffix): StartupScreen.cpp $(IntermediateDirectory)/StartupScreen.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/StartupScreen.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/StartupScreen.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/StartupScreen.cpp$(DependSuffix): StartupScreen.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/StartupScreen.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/StartupScreen.cpp$(DependSuffix) -MM StartupScreen.cpp

$(IntermediateDirectory)/StartupScreen.cpp$(PreprocessSuffix): StartupScreen.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/StartupScreen.cpp$(PreprocessSuffix) StartupScreen.cpp

$(IntermediateDirectory)/WaitScreen.cpp$(ObjectSuffix): WaitScreen.cpp $(IntermediateDirectory)/WaitScreen.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/WaitScreen.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/WaitScreen.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/WaitScreen.cpp$(DependSuffix): WaitScreen.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/WaitScreen.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/WaitScreen.cpp$(DependSuffix) -MM WaitScreen.cpp

$(IntermediateDirectory)/WaitScreen.cpp$(PreprocessSuffix): WaitScreen.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/WaitScreen.cpp$(PreprocessSuffix) WaitScreen.cpp

$(IntermediateDirectory)/MainMenuScreen.cpp$(ObjectSuffix): MainMenuScreen.cpp $(IntermediateDirectory)/MainMenuScreen.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/MainMenuScreen.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/MainMenuScreen.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/MainMenuScreen.cpp$(DependSuffix): MainMenuScreen.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/MainMenuScreen.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/MainMenuScreen.cpp$(DependSuffix) -MM MainMenuScreen.cpp

$(IntermediateDirectory)/MainMenuScreen.cpp$(PreprocessSuffix): MainMenuScreen.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/MainMenuScreen.cpp$(PreprocessSuffix) MainMenuScreen.cpp

$(IntermediateDirectory)/WorldIOManager.cpp$(ObjectSuffix): WorldIOManager.cpp $(IntermediateDirectory)/WorldIOManager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/WorldIOManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/WorldIOManager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/WorldIOManager.cpp$(DependSuffix): WorldIOManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/WorldIOManager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/WorldIOManager.cpp$(DependSuffix) -MM WorldIOManager.cpp

$(IntermediateDirectory)/WorldIOManager.cpp$(PreprocessSuffix): WorldIOManager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/WorldIOManager.cpp$(PreprocessSuffix) WorldIOManager.cpp

$(IntermediateDirectory)/AudioManager.cpp$(ObjectSuffix): AudioManager.cpp $(IntermediateDirectory)/AudioManager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/AudioManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/AudioManager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/AudioManager.cpp$(DependSuffix): AudioManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/AudioManager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/AudioManager.cpp$(DependSuffix) -MM AudioManager.cpp

$(IntermediateDirectory)/AudioManager.cpp$(PreprocessSuffix): AudioManager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/AudioManager.cpp$(PreprocessSuffix) AudioManager.cpp

$(IntermediateDirectory)/App.cpp$(ObjectSuffix): App.cpp $(IntermediateDirectory)/App.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/App.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/App.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/App.cpp$(DependSuffix): App.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/App.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/App.cpp$(DependSuffix) -MM App.cpp

$(IntermediateDirectory)/App.cpp$(PreprocessSuffix): App.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/App.cpp$(PreprocessSuffix) App.cpp

$(IntermediateDirectory)/SaveDataTypes.cpp$(ObjectSuffix): SaveDataTypes.cpp $(IntermediateDirectory)/SaveDataTypes.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/SaveDataTypes.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SaveDataTypes.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SaveDataTypes.cpp$(DependSuffix): SaveDataTypes.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SaveDataTypes.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SaveDataTypes.cpp$(DependSuffix) -MM SaveDataTypes.cpp

$(IntermediateDirectory)/SaveDataTypes.cpp$(PreprocessSuffix): SaveDataTypes.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SaveDataTypes.cpp$(PreprocessSuffix) SaveDataTypes.cpp

$(IntermediateDirectory)/EventQueue.cpp$(ObjectSuffix): EventQueue.cpp $(IntermediateDirectory)/EventQueue.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/EventQueue.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/EventQueue.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/EventQueue.cpp$(DependSuffix): EventQueue.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/EventQueue.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/EventQueue.cpp$(DependSuffix) -MM EventQueue.cpp

$(IntermediateDirectory)/EventQueue.cpp$(PreprocessSuffix): EventQueue.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/EventQueue.cpp$(PreprocessSuffix) EventQueue.cpp

$(IntermediateDirectory)/LoadScreen.cpp$(ObjectSuffix): LoadScreen.cpp $(IntermediateDirectory)/LoadScreen.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/LoadScreen.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/LoadScreen.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/LoadScreen.cpp$(DependSuffix): LoadScreen.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/LoadScreen.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/LoadScreen.cpp$(DependSuffix) -MM LoadScreen.cpp

$(IntermediateDirectory)/LoadScreen.cpp$(PreprocessSuffix): LoadScreen.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/LoadScreen.cpp$(PreprocessSuffix) LoadScreen.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


