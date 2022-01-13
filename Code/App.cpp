#include "App.h"

#include "Singletons.h"
#include "CustomXMLTypes.h"

App::App() {
	getWindow()->setTitle("Fear Of The Dark");
	getWindow()->setSize(1024, 624);

	addScreens();

	BARE2D::ResourceManager::setAssetsPathPrefix(ASSETS_FOLDER_PATH);
	BARE2D::ResourceManager::setTexturePathPrefix("Textures/");

	initXML();
}

App::~App() {
}

void App::addScreens() {
	Singletons::createWorld();

	m_startupScreen		= new StartupScreen(getWindow(), getInputManager());					/// 0
	m_mainMenuScreen	= new MainMenuScreen(getWindow());										/// 1
	m_optionsMenuScreen = new OptionsMenuScreen(getWindow());									/// 2
	m_loadScreen		= new LoadScreen(getWindow(), m_WorldIOManager);						/// 3
	m_waitScreen		= new WaitScreen(getWindow(), m_WorldIOManager);						/// 4
	m_gameplayScreen	= new GameplayScreen(getWindow(), m_WorldIOManager, getInputManager()); /// 5

	/// MAKE SURE YOU ADD SCREENS IN THE ORDER OF THEIR INDICES

	getScreenList()->addEntryScreen(m_startupScreen); /// 0
	getScreenList()->addScreen(m_mainMenuScreen);	  /// 1
	getScreenList()->addScreen(m_optionsMenuScreen);  /// 2
	getScreenList()->addScreen(m_loadScreen);		  /// 3
	getScreenList()->addScreen(m_waitScreen);		  /// 4
	getScreenList()->addScreen(m_gameplayScreen);	  /// 5
}

void App::initXML() {
	// Register all the different types of data.
	BARE2D::XMLDataManager::addDataType<XMLModule::TileData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::TileContainerData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::ParticleData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::EntityData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::EntityNPCData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::EntityProjectileData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::EntityItemData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::ItemData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::ItemConsumableData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::ItemArmourData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::ItemWeaponData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::ItemBlockData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::BiomeData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::EraData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::LootDropData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::LootTableData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::StructureData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::QuestData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::QuestObjectiveData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::DialogueQuestionData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::DialogueResponseData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::AnimationData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::SkeletalAnimationData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::AttackData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::MeleeAttackData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::RangedAttackData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::MagicAttackData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::BuffData>();
	BARE2D::XMLDataManager::addDataType<XMLModule::FluidData>();

	// Load all the files.
	BARE2D::XMLDataManager::loadXML(ASSETS_FOLDER_PATH + "/Data/");
}
