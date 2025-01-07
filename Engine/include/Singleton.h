#pragma once
#include <iostream>
#include "include/Export.h"

/*!
 * \brief Util class for making non copyable
 */
class Noncopyable {
public:
	Noncopyable() = default;
	~Noncopyable() = default;

private:
	Noncopyable(const Noncopyable &) = delete;
	Noncopyable &operator=(const Noncopyable &) = delete;
};

template<typename T> class Singleton : Noncopyable
{
public:
	static  T& instance();


	Singleton() = default;
	Singleton(const Singleton &s) = delete;
	virtual ~Singleton();

};



template<typename T> T& Singleton<T>::instance()
{
	static T t;
	return t;
}

template<typename T> Singleton<T>::~Singleton()
{
}