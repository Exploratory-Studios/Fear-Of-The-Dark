##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=AnimationTool
ConfigurationName      :=Debug
WorkspacePath          :=/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code
ProjectPath            :=/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/AnimationTool/AnimationTool
IntermediateDirectory  :=/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/AnimationTool/AnimationTool/Debug/obj
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Davis-dev
Date                   :=06/07/20
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
OutputFile             :=/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/AnimationTool/AnimationTool/Debug/bin/Tool
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="AnimationTool.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/AnimationTool/AnimationTool/ $(IncludeSwitch)/home/davis-dev/Documents/Programming/C++/CodingGithub/GLEngine/ $(IncludeSwitch)/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/ $(IncludeSwitch)/usr/include/x86_64-linux-gnu/cegui-0.8.7 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)boost_filesystem $(LibrarySwitch)boost_system $(LibrarySwitch)lua5.3 $(LibrarySwitch)GLEngine $(LibrarySwitch)CEGUIBase-0 $(LibrarySwitch)CEGUIOpenGLRenderer-0 $(LibrarySwitch)SDL2 $(LibrarySwitch)SDL2_mixer $(LibrarySwitch)SDL2_ttf $(LibrarySwitch)GL $(LibrarySwitch)GLU $(LibrarySwitch)GLEW 
ArLibs                 :=  "libboost_filesystem.a" "libboost_system.a" "lua5.3" "libGLEngine.a" "CEGUIBase-0" "CEGUIOpenGLRenderer-0" "SDL2" "SDL2_mixer" "SDL2_ttf" "GL" "GLU" "GLEW" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)/home/davis-dev/Documents/Programming/C++/CodingGithub/GLEngine/bin/Release/ 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -O0 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/Animation.cpp$(ObjectSuffix) $(IntermediateDirectory)/App.cpp$(ObjectSuffix) $(IntermediateDirectory)/MainScreen.cpp$(ObjectSuffix) 



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
	@test -d /home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/AnimationTool/AnimationTool/Debug/obj || $(MakeDirCommand) /home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/AnimationTool/AnimationTool/Debug/obj


$(IntermediateDirectory)/.d:
	@test -d /home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/AnimationTool/AnimationTool/Debug/obj || $(MakeDirCommand) /home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/AnimationTool/AnimationTool/Debug/obj

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/AnimationTool/AnimationTool/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/Animation.cpp$(ObjectSuffix): Animation.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Animation.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Animation.cpp$(DependSuffix) -MM Animation.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/AnimationTool/AnimationTool/Animation.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Animation.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Animation.cpp$(PreprocessSuffix): Animation.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Animation.cpp$(PreprocessSuffix) Animation.cpp

$(IntermediateDirectory)/App.cpp$(ObjectSuffix): App.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/App.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/App.cpp$(DependSuffix) -MM App.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/AnimationTool/AnimationTool/App.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/App.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/App.cpp$(PreprocessSuffix): App.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/App.cpp$(PreprocessSuffix) App.cpp

$(IntermediateDirectory)/MainScreen.cpp$(ObjectSuffix): MainScreen.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/MainScreen.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/MainScreen.cpp$(DependSuffix) -MM MainScreen.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/AnimationTool/AnimationTool/MainScreen.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/MainScreen.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/MainScreen.cpp$(PreprocessSuffix): MainScreen.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/MainScreen.cpp$(PreprocessSuffix) MainScreen.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r /home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/AnimationTool/AnimationTool/Debug/obj/


