/// This class exists only to show the compiler some declarations

#include "XMLDataTypes.h"

namespace XMLModule {

	/*template<class T>
	void Attribute<T>::setDefault() {
		setData<T>((T)0);
	}*/

	template<>
	void Attribute<glm::vec2>::setDefault();
	template<>
	void Attribute<std::vector<unsigned int>>::setDefault();
	template<>
	void Attribute<std::vector<float>>::setDefault();
	template<>
	void Attribute<std::vector<glm::vec2>>::setDefault();
	template<>
	void Attribute<std::string>::setDefault();

	/*template<class T>
	T& GenericData::getAttributeT(std::string index) {
	    Attribute a = m_attributes[index];

	    return boost::get<T>(a.data);
	}*/

}
