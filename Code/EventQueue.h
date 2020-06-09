#pragma once

#include <vector>
#include <string>

#include "Logging.h"

#include "ScriptQueue.h"

namespace EventModule {

    class Event {
        public:
            Event(std::string type, std::vector<ScriptingModule::Argument> args) : m_type(type), m_arguments(args) { }

            std::string& getType() { return m_type; } /// Gets type string. Reference for speed.
            std::vector<ScriptingModule::Argument>& getArguments() { return m_arguments; } /// Gets the arguments. Reference for speed.

        private:
            std::string m_type; /// This is a string that defines the type/name of the event. This is used to apply it to certain scripts, functions, or otherwise.
            std::vector<ScriptingModule::Argument> m_arguments; /// This is unique to each event called and is passed to scripts who yield for the event type.
    };


    class EventQueue {
        friend class Scripter;
        public:
            static void triggerEvent(Event& evnt, bool log = true); /// Triggers an event, adding it to the queue
            static void triggerEvent(std::string type, std::vector<ScriptingModule::Argument> args, bool log = true); /// Triggers an event, adding it to the queue (triggers constructor)

            static std::vector<Event> getEvents(); /// Returns all events.

            static void clearQueue(); /// Clears all events from the queue.

        private:
            static std::vector<Event> m_events; /// Holds all triggered events until cleared.
            static Logger* m_logger;

    };

}
