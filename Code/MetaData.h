#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>

#include "LuaHeaders.h"
#include "ScriptQueue.h"

namespace SaveDataTypes {

	struct MetaData_Aspect {
		MetaData_Aspect(std::string nameP, int valP) : key(nameP), val(std::to_string(valP)) {
		}
		MetaData_Aspect(std::string nameP, float valP) : key(nameP), val(std::to_string(valP)) {
		}
		MetaData_Aspect(std::string nameP, std::string valP) : key(nameP), val(valP) {
		}

		std::string key;
		std::string val;
	};

	class MetaData {
	  public:
		MetaData() {
		}											  // Simply initializes empty data fields
		MetaData(std::vector<MetaData_Aspect>& data); // Constructs a metadata object with the given data.

		// Attempts to retrieve an element's value with key key, and place its value into the variable var. Returns true if successful, false otherwise.
		bool getElement(std::string& key, std::string& var);
		// Sets an element's value using the given key and value.
		void setElement(std::string& key, std::string& val);

		std::string getElements(); // Returns a string of all elements. used for debugging

		void getLuaArguments(std::vector<::ScriptingModule::Argument>& args);

		void readFromLuaTable(lua_State* state, int tableIndex);

		MetaData& operator+=(MetaData& other) {
			/// Adds all elements from the other to this.
			for(auto obj: other.m_data) {
				std::string key, val;

				key = obj.first;
				val = obj.second;

				setElement(key, val);
			}

			return *this;
		}

		// Truncates data from the given file into this object
		void read(std::ifstream& file);
		// Concatenates data from this object to the given file
		void save(std::ofstream& file);

	  private:
		void init(std::vector<MetaData_Aspect>& data);

		std::unordered_map<std::string, std::string>
			m_data; // unordered maps are better for non-data traversal situations. This is one of those.
	};

} // namespace SaveDataTypes
