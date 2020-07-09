##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=DataMakerFOTD
ConfigurationName      :=Debug
WorkspacePath          :=/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code
ProjectPath            :=/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/DataMakerFOTD/DataMakerFOTD
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Davis-Dev
Date                   :=09/07/20
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
OutputFile             :=bin/Debug/DataMakerFOTD
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="DataMakerFOTD.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)../../../Code $(IncludeSwitch)include $(IncludeSwitch)../../../../GLEngine $(IncludeSwitch)/usr/include/x86_64-linux-gnu/cegui-0.8.7 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)GLEngine $(LibrarySwitch)boost_thread $(LibrarySwitch)boost_filesystem $(LibrarySwitch)boost_system $(LibrarySwitch)lua5.3 $(LibrarySwitch)CEGUIBase-0 $(LibrarySwitch)CEGUIOpenGLRenderer-0 $(LibrarySwitch)SDL2 $(LibrarySwitch)SDL2_mixer $(LibrarySwitch)SDL2_ttf $(LibrarySwitch)GL $(LibrarySwitch)GLU $(LibrarySwitch)GLEW $(LibrarySwitch)pthread 
ArLibs                 :=  "libGLEngine.a" "libboost_thread.a" "libboost_filesystem.a" "libboost_system.a" "lua5.3" "CEGUIBase-0" "CEGUIOpenGLRenderer-0" "SDL2" "SDL2_mixer" "SDL2_ttf" "GL" "GLU" "GLEW" "pthread" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)../../../../GLEngine/bin/Release/ $(LibraryPathSwitch)/usr/lib/x86_64-linux-gnu/ $(LibraryPathSwitch)./ $(LibraryPathSwitch)../../../ 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -std=c++17 -g3 -g -DLINUX -fexceptions  $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/up_up_up_Code_XMLDataTypes.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_Code_ScriptQueue.cpp$(ObjectSuffix) $(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/Objects.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_Code_XMLData.cpp$(ObjectSuffix) $(IntermediateDirectory)/MainScreen.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_Code_MetaData.cpp$(ObjectSuffix) $(IntermediateDirectory)/App.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_up_up_Code_Logging.cpp$(ObjectSuffix) 



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
$(IntermediateDirectory)/up_up_up_Code_XMLDataTypes.cpp$(ObjectSuffix): ../../../Code/XMLDataTypes.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_Code_XMLDataTypes.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_Code_XMLDataTypes.cpp$(DependSuffix) -MM ../../../Code/XMLDataTypes.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/XMLDataTypes.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_Code_XMLDataTypes.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_Code_XMLDataTypes.cpp$(PreprocessSuffix): ../../../Code/XMLDataTypes.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_Code_XMLDataTypes.cpp$(PreprocessSuffix) ../../../Code/XMLDataTypes.cpp

$(IntermediateDirectory)/up_up_up_Code_ScriptQueue.cpp$(ObjectSuffix): ../../../Code/ScriptQueue.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_Code_ScriptQueue.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_Code_ScriptQueue.cpp$(DependSuffix) -MM ../../../Code/ScriptQueue.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/ScriptQueue.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_Code_ScriptQueue.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_Code_ScriptQueue.cpp$(PreprocessSuffix): ../../../Code/ScriptQueue.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_Code_ScriptQueue.cpp$(PreprocessSuffix) ../../../Code/ScriptQueue.cpp

$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/DataMakerFOTD/DataMakerFOTD/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/Objects.cpp$(ObjectSuffix): Objects.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Objects.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Objects.cpp$(DependSuffix) -MM Objects.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/DataMakerFOTD/DataMakerFOTD/Objects.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Objects.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Objects.cpp$(PreprocessSuffix): Objects.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Objects.cpp$(PreprocessSuffix) Objects.cpp

$(IntermediateDirectory)/up_up_up_Code_XMLData.cpp$(ObjectSuffix): ../../../Code/XMLData.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_Code_XMLData.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_Code_XMLData.cpp$(DependSuffix) -MM ../../../Code/XMLData.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/XMLData.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_Code_XMLData.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_Code_XMLData.cpp$(PreprocessSuffix): ../../../Code/XMLData.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_Code_XMLData.cpp$(PreprocessSuffix) ../../../Code/XMLData.cpp

$(IntermediateDirectory)/MainScreen.cpp$(ObjectSuffix): MainScreen.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/MainScreen.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/MainScreen.cpp$(DependSuffix) -MM MainScreen.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/DataMakerFOTD/DataMakerFOTD/MainScreen.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/MainScreen.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/MainScreen.cpp$(PreprocessSuffix): MainScreen.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/MainScreen.cpp$(PreprocessSuffix) MainScreen.cpp

$(IntermediateDirectory)/up_up_up_Code_MetaData.cpp$(ObjectSuffix): ../../../Code/MetaData.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_Code_MetaData.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_Code_MetaData.cpp$(DependSuffix) -MM ../../../Code/MetaData.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/MetaData.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_Code_MetaData.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_Code_MetaData.cpp$(PreprocessSuffix): ../../../Code/MetaData.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_Code_MetaData.cpp$(PreprocessSuffix) ../../../Code/MetaData.cpp

$(IntermediateDirectory)/App.cpp$(ObjectSuffix): App.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/App.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/App.cpp$(DependSuffix) -MM App.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Tools/DataMakerFOTD/DataMakerFOTD/App.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/App.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/App.cpp$(PreprocessSuffix): App.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/App.cpp$(PreprocessSuffix) App.cpp

$(IntermediateDirectory)/up_up_up_Code_Logging.cpp$(ObjectSuffix): ../../../Code/Logging.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_up_Code_Logging.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_up_Code_Logging.cpp$(DependSuffix) -MM ../../../Code/Logging.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/davis-dev/Documents/Programming/C++/CodingGithub/Fear-Of-The-Dark/Code/Logging.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_up_Code_Logging.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_up_Code_Logging.cpp$(PreprocessSuffix): ../../../Code/Logging.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_up_Code_Logging.cpp$(PreprocessSuffix) ../../../Code/Logging.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


