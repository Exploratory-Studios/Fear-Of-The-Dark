#include "Logging.h"

Logger* Logger::m_instance = nullptr;

Logger* Logger::getInstance() {
	if(!m_instance)
		m_instance = new Logger;
	return m_instance;
}
