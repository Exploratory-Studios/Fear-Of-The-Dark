#pragma once

#include <vector>

/*
 * This is the circularArray class. It's basically just a wrapper for a 1d vector that wraps around. 
 * Overflows and underflows are rerouted to what they would be if the vector was attached at each end 
 * like a circleeeee!
 */
template<typename T> class CircularVector : public std::vector<T> {
  public:
	CircularVector<T>() {
		m_vec = new std::vector<T>();
	}

	typename std::vector<T>::reference operator[](typename std::vector<T>::size_type n) _GLIBCXX_NOEXCEPT {
		while(n > this->size())
			n += this->size();
		if(n > this->size())
			n = n % this->size();

		return (*m_vec)[n];
	}
	typename std::vector<T>::const_reference operator[](typename std::vector<T>::size_type n) const _GLIBCXX_NOEXCEPT {
		while(n > m_vec->size())
			n += m_vec->size();
		if(n > m_vec->size())
			n = n % m_vec->size();

		return (*m_vec)[n];
	}

  private:
	std::vector<T>* m_vec = nullptr;
};
