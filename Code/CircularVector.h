#pragma once

#include <vector>

/*
 * This is the circularArray class. It's basically just a wrapper for a 1d vector that wraps around. 
 * Overflows and underflows are rerouted to what they would be if the vector was attached at each end 
 * like a circleeeee!
 * 
 * Disgusting formatting comes directly from the source files for C++. Don't mind my plagiarism.
 */
template<typename T> class CircularVector : public std::vector<T> {
  public:
	typename std::vector<T>::reference operator[](typename std::vector<T>::size_type __n) _GLIBCXX_NOEXCEPT {
		while(__n > this->size())
			__n += this->size();
		if(__n > this->size())
			__n = __n % this->size();

		std::vector<T>::__profcxx_vector2list_invalid_operator(this->_M_vect2list_info);
		return std::vector<T>::_M_base()[__n];
	}
	typename std::vector<T>::const_reference operator[](typename std::vector<T>::size_type __n) const _GLIBCXX_NOEXCEPT {
		while(__n > this->size())
			__n += this->size();
		if(__n > this->size())
			__n = __n % this->size();

		std::vector<T>::__profcxx_vector2list_invalid_operator(this->_M_vect2list_info);
		return std::vector<T>::_M_base()[__n];
	}
};
