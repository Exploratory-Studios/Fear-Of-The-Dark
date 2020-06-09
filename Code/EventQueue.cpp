#include "EventQueue.h"

namespace EventModule {

    std::vector<Event> EventQueue::m_events;
    Logger* EventQueue::m_logger = Logger::getInstance();

    void EventQueue::triggerEvent(Event& evnt, bool log/* = true*/) {
        /// Triggers an event, adding it to the queue
        m_events.push_back(evnt);
        if(log) m_logger->log("Event Triggered: " + evnt.getType());
    }
    void EventQueue::triggerEvent(std::string type, std::vector<ScriptingModule::Argument> args, bool log/* = true*/) {
        /// Triggers an event, adding it to the queue (triggers constructor)
        m_events.emplace_back(type, args);
        if(log) m_logger->log("Event Triggered: " + type);
    }

    std::vector<Event> EventQueue::getEvents() {
        return m_events;
    }

    void EventQueue::clearQueue() {
        m_events.clear();
    }

}
