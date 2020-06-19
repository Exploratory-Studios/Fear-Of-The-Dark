/// This class exists only to show the compiler templated functions' implementations in the XMLDataTypes.h file.

#pragma once

namespace XMLModule {

    template<typename T>
    void GenericData::getAttribute(::std::string name, T& t) { /// TODO: Error checking and cleaning!
        t = boost::get<T>(m_attributes[name].data);
    }

    template<class T>
    T& GenericData::getAttributeT(std::string index) {
        Attribute a = m_attributes[index];

        return boost::get<T>(a.data);
    }

}
