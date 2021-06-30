#include "MetaData.h"

SaveDataTypes::MetaData::MetaData(std::vector<SaveDataTypes::MetaData_Aspect>& data) {
	init(data);
}

void SaveDataTypes::MetaData::init(std::vector<SaveDataTypes::MetaData_Aspect>& data) {
	for(unsigned int i = 0; i < data.size(); i++) {
		std::pair<std::string, std::string> p = std::make_pair(data[i].key, data[i].val);
		m_data.insert(p);
	}
}

bool SaveDataTypes::MetaData::getElement(std::string& key, std::string& var) {
	auto i = m_data.find(
		key); // Try to access data. If it doesn't exist, returns m_data.end(). If successful, it returns an iterator to data.
	if(i == m_data.end()) {
		return false; // Didn't find it
	}
	var = i->second; // Found it, set var.
	return true;
}

void SaveDataTypes::MetaData::setElement(std::string& key, std::string& val) {
	m_data[key] =
		val; // This operator ([]) either accesses an existing element, or creates and accesses a non-existent one.
	// No matter the circumstances, the end data will have the element with value `val`
}

std::string SaveDataTypes::MetaData::getElements() {
	std::string ret;

	for(auto obj: m_data) {
		ret += "\"" + obj.first + "\"=\"" + obj.second + "\", ";
	}

	return ret;
}

void SaveDataTypes::MetaData::getLuaArguments(std::vector<ScriptingModule::Argument>& args) {
	/// Adds all elements from this object to the vector `args`. Does not modify previously-existing objects in `args`.
	/// Note: All new elements in `args` will have the flag `isMetadata` enabled, so that they're initialized in Lua as part of a table inside the global table.
	for(auto obj: m_data) {
		// Loop through every object.
		// Create an Argument object and populate it.
		ScriptingModule::Argument arg(obj.first, obj.second, true);
		args.push_back(arg);
	}
}

void SaveDataTypes::MetaData::readFromLuaTable(lua_State* state, int tableIndex) {
	/** Loads and sets all elements to those in an existing Lua table, on the stack with index `tableIndex`. **/

	// We need to loop through, getting the next value until the table doesn't actually exist.
	std::vector<MetaData_Aspect> args;
	lua_pushnil(state); // First "key"
	tableIndex--;		// Because we pushed a key, the table is pushed farther down.

	if(lua_type(state, tableIndex) == LUA_TTABLE) {
		while(lua_next(
			state,
			tableIndex)) { // uses first key ("nil") to find first real key and value. After that, it uses each consecutive key to find the next key and value pair.
			MetaData_Aspect a(lua_tostring(state, -2),
							  lua_tostring(state, -1)); // lua_next pushes the value (-1) and key (-2) onto the stack
			lua_pop(state, 1); // Get rid of the value, leaving the key to tell lua_next what's next.
			args.push_back(a);
		}
	}

	lua_pop(state, 1); // Get rid of the key

	init(args);
}

void SaveDataTypes::MetaData::read(std::ifstream& file) {
	// Mirroring saving, we need the size of the read.
	unsigned int length;
	file.read(reinterpret_cast<char*>(&length), sizeof(unsigned int));

	for(unsigned int i = 0; i < length; i++) { // Now here's where it gets odd.
		// As we saved each string fairly raw, with only an extra '\0' to show us where each ended, we need to read character-by-character until we find it.
		std::string key;
		while(true) { // blah, blah, infinite loops are bad, blah... We'll just break out.
			char n;
			file.read(&n, sizeof(char)); // Read next character into n.
			if(n == '\0') { // If we've found the null-terminator, break out of the loop (don't add to the 'key' string)
				break;
			} else {
				key += n; // Add characters one-by-one into the key string.
			}
		}

		// Do it all again for the value!
		std::string val;
		while(true) {
			char n;
			file.read(&n, sizeof(char)); // Read next character into n.
			if(n == '\0') { // If we've found the null-terminator, break out of the loop (don't add to the 'key' string)
				break;
			} else {
				val += n; // Add characters one-by-one into the key string.
			}
		}

		// At this point, we've extracted both the key and value, now we just need to add them!
		setElement(key, val); // Boom! Done.
	}

	// m_data should be completely constructed now.
}

void SaveDataTypes::MetaData::save(std::ofstream& file) {
	// BTW: This is a costly operation, traversing an unordered_map (compared to ordered)

	// First, we need to establish the length of data.
	unsigned int length = m_data.size();
	file.write(reinterpret_cast<char*>(&length), sizeof(unsigned int));

	// Then loop through all data, writing keys and values in sequence.
	for(auto i: m_data) { // use a fancy for-each loop. Never slower than a regular for loop :)
		file.write(i.first.c_str(), sizeof(char) * i.first.length());	// Write key
		file.write(i.second.c_str(), sizeof(char) * i.second.length()); // Write value
		// Each of these c-strings include a null-terminator ('\0'), so we can identify the ends of them each like that.
	}

	// That's all!
}
