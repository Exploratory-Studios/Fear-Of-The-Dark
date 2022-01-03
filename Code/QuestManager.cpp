#include "QuestManager.h"

#include <LuaScript.hpp>
#include <Logger.hpp>

namespace QuestModule
{

	QuestManager::QuestManager()
	{
		// Nothing really to put here. It's just a manager.
	}

	void QuestManager::startQuest(unsigned int id)
	{
		/// Retrieves (rather, the Quest object retrieves its own data) and starts a quest with id `id`

		Quest quest(id);
		quest.start();

		m_currentQuests.push_back(quest);
	}

	void QuestManager::update()
	{
		/// Updates all current scripts.
		for(unsigned int i = 0; i < m_currentQuests.size(); i++)
		{
			m_currentQuests[i].update();
		}

		/// Event system was here
	}

	void QuestManager::setFlag(unsigned int id, bool value)
	{
		/// Sets a flag to a value of `value`
		m_flags[id] = value;

		/// Event system was used here to set a flag by command. Kind of silly if you ask me.
	}

	bool QuestManager::getFlag(unsigned int id) const
	{
		/// Returns the bool value of a flag with id `id`
		bool val = false;

		try
		{
			val = m_flags.at(id);
		}
		catch(const std::out_of_range& err)
		{
			BARE2D::Logger::getInstance()->log(
			    "WARNING: An attempt to access a flag with ID: " + std::to_string(id) + " failed! (Flag doesn't exist)",
			    true);
		}

		return val;
	}

	std::vector<Quest> QuestManager::getCurrentQuests()
	{
		/// Returns all current quests
		return m_currentQuests;
	}

	std::vector<Quest> QuestManager::getFinishedQuests()
	{
		/// Returns all finished quests
		return m_finishedQuests;
	}
} // namespace QuestModule
