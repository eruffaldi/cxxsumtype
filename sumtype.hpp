/************************************************************************************

Filename    :   sumtype.hpp
Content     :   Algebrical Data Types for C++11
Created     :   July 2014
Authors     :   Emanuele Ruffaldi

Copyright   :   Emanuele Ruffaldi

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/
#pragma once
#include <utility> 
#include <iostream>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <functional>

//-------------------------------------------------
// Alignment and Size
//-------------------------------------------------


/// this class is used as a functional for providing alignment as constexpr value as used in tmax class
template <typename T>
struct Alignof
{
	static constexpr size_t value = alignof(T);
};

/// this class is used as a functional for providing size as constexpr value as used in tmax class
template <typename T>
struct Sizeof
{
	static constexpr size_t value = sizeof(T);
};

/// given a list of type in Args and a Predicate accepting the type this produces the type with maximum value. Implemented with the variadic recursive pattern
/// The output of this class is a type "type"
/// this is taken from the newcpp post
template <template <typename> class Pred, typename... Args>
struct tmax;
 
/// single type returns type
template <template <typename> class Pred, typename First>
struct tmax<Pred, First>
{
	typedef First type;
};
 
/// recursive step using std::conditional
template <template <typename> class Pred, typename First, typename... Args>
struct tmax<Pred, First, Args...>
{
  typedef typename tmax<Pred, Args...>::type next;
  typedef typename std::conditional<Pred<First>::value >= Pred<next>::value,First,next>::type type;
};


//-------------------------------------------------
// Alignment and Size - alternative version
//-------------------------------------------------


template <typename... Args>
struct find_biggest;


 
template <typename First>
struct find_biggest<First>
{
	typedef First type;
};
 
template <typename First, typename... Args>
struct find_biggest<First, Args...>
{
  typedef typename find_biggest<Args...>::type next;
  typedef typename std::conditional<sizeof(First) >= sizeof(next),First,next>::type type;
};


template <typename... Args>
struct find_biggestalign;
 
template <typename First>
struct find_biggestalign<First>
{
	typedef First type;
};
 
template <typename First, typename... Args>
struct find_biggestalign<First, Args...>
{
  typedef typename find_biggest<Args...>::type next;
  typedef typename std::conditional<alignof(First) >= alignof(next),First,next>::type type;
};


//-------------------------------------------------
// Lambda Expression Lists
//-------------------------------------------------


/// this class extracts information from a function type
template <class R>
struct extractfx;

/// specialization for a const function with single parameter
template <class R, class B>
struct extractfx<R(B) const>
{
	typedef B first;  /// first argument type
	typedef R result; /// return type
};

/// specialization for multiple ones
template <class R, class First, class... Args>
struct extractfx<R(First,Args...) const>
{
	typedef R result;
	typedef First first;
};

/// non const specialization
template <class R, class B>
struct extractfx<R(B)>
{
	typedef B first;
	typedef R result;
};

/// non const specialization
template <class R, class First, class... Args>
struct extractfx<R(First,Args...)>
{
	typedef R result;
	typedef First first;
};

/// class for extracting information from function members pointers and lambdas
template<class FPtr>
struct function_traits;

template<class T, class C>
struct function_traits<T (C::*)>
{
    typedef T signature;
};

/// this class extracts information from a list of lambdas, at the moment returns the first return type
template<typename... Args>
struct lambdalist;

/// single lambda
template<class F>
struct lambdalist<F>
{
	  typedef typename function_traits<decltype(&F::operator())>::signature signature;
	  typedef typename extractfx<signature>::result result;
};

/// recursive
template<class F,typename... Args>
struct lambdalist<F,Args...>
{
  typedef typename function_traits<decltype(&F::operator())>::signature signature;
  typedef typename extractfx<signature>::result result;
};

//-------------------------------------------------
// Indexed Management of Type List
//-------------------------------------------------

/// this class allows to return the looked-th type from the Args list of types
/// current is the moving index in the recursion pattern
template <size_t looked, size_t current, typename... Args>
struct find_index;
 
/// single type
template <size_t looked, size_t current, typename First>
struct find_index<looked, current, First>
{
	typedef typename std::conditional<looked == current,First,void>::type type; /// 
};
 
/// multiple types
template <size_t looked, size_t current, typename First, typename... Args>
struct find_index<looked,current,First, Args...>
{
  typedef typename find_index<looked,current+1,Args...>::type next; /// next in the recursive pattern
  typedef typename std::conditional<looked == current,First,next>::type type;
};

/// this class allows to return the index of the given looked type in the list Args
/// the result is stored in the "index" enumeration element, -1 in case of not found
template <typename looked, int current, typename... Args>
struct find_type;
 
/// last type
template <typename looked, int current, typename First>
struct find_type<looked, current, First>
{
	enum { index = std::is_same<looked,First>::value ? current : -1 };
};

/// recursion 
template <class looked, int current, typename First, typename... Args>
struct find_type<looked,current,First, Args...>
{
	enum { index = std::is_same<looked,First>::value ?  current : find_type<looked,current+1,Args...>::index };
};

/// same as find_type but 
template <class looked, int current, typename... Args>
struct find_typeex;
 
template <class looked, int current>
struct find_typeex<looked, current, looked>
{
	enum { index = current };
};
 
template <class looked, int current, typename First, typename... Args>
struct find_typeex<looked,current,First, Args...>
{
	enum { index = std::is_same<looked,First>::value ?  current : find_type<looked,current+1,Args...>::index };
};

/// recursive helper class
template <int current, typename... Args>
struct find_indexrun;
 
/// recursive helper class, last recursion
template <int current, typename First>
struct find_indexrun<current, First>
{
	/// destruction of type First
	static void destroy(int index, unsigned char * value)
	{
		if(index == current)
			((First*)value)->~First();
	}

	/// copy construction
	static void ctorcopy(int index, unsigned char * value, const unsigned char * other)
	{
		if(index == current)
		{
			std::cout << "ctorcopy " << index << std::endl;
			new (value) First(*(const First*)other);
		}
		else
			throw std::bad_exception();
	}

	/// move construction
	static void ctormove(int index, unsigned char * value, const unsigned char * other)
	{
		if(index == current)
		{
			std::cout << "ctormove " << index << std::endl;
			new (value) First(std::move(*(const First*)other));
		}
		else
		{
			std::cout << "ctormove exception index:" << index << " current:" << current << "\n";
			throw std::bad_exception();
		}
	}

	/// visit over class
	template <class T>
	static typename std::remove_reference<T>::type::result_type visit(int index, unsigned char * value, T && visitor)
	{
		if(index == current)
			return visitor(*(First*)value);
		else
			throw std::bad_exception();
	}

	/// size in bytes
	static size_t size(int index)
	{
		return index == current ? sizeof(First) : 0;
	}

	/// assignment
	static void assign(int index, unsigned char * value, const unsigned char * other)
	{
		if(index == current)
			(*(First*)value) = *(const First*)other;
		else
			throw std::bad_exception();
	}

};

/// recursive helper class
/// recursion step, delegates to next if not matching by index
template <int current, typename First, typename... Args>
struct find_indexrun<current,First, Args...>
{
	typedef find_indexrun<current+1,Args...> next;

	static void destroy(int index, unsigned char * value)
	{
		if(index == current)
			((First*)value)->~First();
		else
		{
			next::destroy(index,value);
		}
	}

	static void ctorcopy(int index, unsigned char * value, const unsigned char * other)
	{
		if(index == current)
		{
			std::cout << "ctorcopy " << index << std::endl;
			new (value) First(*(const First*)other);
		}
		else
			next::ctorcopy(index,value,other);			
	}

	static void ctormove(int index, unsigned char * value, const unsigned char * other)
	{
		if(index == current)
		{
			std::cout << "ctormove " << index << std::endl;
			new (value) First(std::move(*(const First*)other));
		}
		else
			next::ctormove(index,value,other);			
	}

	static void assign(int index, unsigned char * value, const unsigned char * other)
	{
		if(index == current)
			(*(First*)value) = *(const First*)other;
		else
			next::assign(index,value,other);			
	}

	static size_t size(int index)
	{
		return index == current ? sizeof(First) : next::size(index);
	}

	template <class T>
	static typename std::remove_reference<T>::type::result_type visit(int index, unsigned char * value, T && visitor)
	{
		if(index == current)
			return visitor(*(First*)value);
		else
			return next::visit(index,value,visitor);
	}

};

/// forward
template<typename ...Args>
struct optional_sumtype;

/// sumtype aka algebrical data type
template <typename... Args>
class sumtype
{
public:
	typedef sumtype<Args...> self;
	typedef typename find_biggest<Args...>::type bigtype;
	typedef find_indexrun<0,Args...> rtype;

	// type to int: compiletype vs runtime
	// int to type: compiletype vs runtime

	/// helper
	/// byindex<index>::type => gives T as type 
	template <size_t index>
	struct byindex : find_index<index,0,Args...>
	{
	};

	/// helper
	/// bytype<T>::index => gives value or -1 
	template <class T>
	struct bytype : find_type<T,0,Args...>
	{
	};

	/// helper
	/// bytype<T>::index => gives value or -1 
	template <class T>
	struct bytypeex : find_typeex<T,0,Args...>
	{
	};


	~sumtype() { dtor(); }

	template <class T>
	sumtype(const T & what) : _index(bytype<T>::index) 
	{
		static_assert(bytype<T>::index != -1,"only supported types with explicit type match");
		//std::cout << "copy T " << typeid(T).name() << std::endl;
		rtype::ctorcopy(_index,value,(unsigned char*)&what); 
	}

	template <class T>
	sumtype(T && what) : _index(bytype<T>::index) // if copyctor this will give -1
	{
		// unfortunately is_same<T,self> DOES not work
		std::cout << "ctormove self\n" << typeid(T).name() << std::endl; //<< (strcmp(typeid(self).name(),typeid(T).name()) == 0) << " is same as me? " << std::is_same<typename std::remove_cv<T>::type,self>::value << std::endl;
		typedef typename std::conditional<
			std::is_same<T,self&>::value,selfinrtype,
				typename std::conditional<std::is_same<T,self>::value,selfinrtype,rtype>::type >				::type xrtype;
		xrtype::ctormove(_index,value,(unsigned char*)&what);
		//selfrtype<T,Args...>::ctormove(_index,value,(unsigned char*)&what); 
		//selfrtype<T,Args...>::ctormove(_index,value,(unsigned char*)&what); 
	}


	sumtype & operator= (const sumtype & other)
	{
		if(other._index == _index)
		{
			rtype::assign(_index,value,other.value);
		}
		else
		{
			dtor();
			_index = other._index;
			rtype::ctorcopy(_index,value,other.value);
		}
		return *this;
	}

	sumtype & operator= (sumtype && other)
	{
		if(other._index == _index)
		{
			rtype::assign(_index,value,other.value);
		}
		else
		{
			dtor();
			_index = other._index;
			rtype::ctormove(_index,value,other.value);
		}
		return *this;
	}

	/// returns the reference to the content given type
	/// if the type is not active it generates an exception
	template <class T>
	T & gettype()
	{
		static_assert(bytype<T>::index != -1,"sumtype::gettype requires type in sumtype list");

		if(_index == bytype<T>::index)
		{
			return *(T*)value;
		}
		else
			throw std::bad_exception();
	}

	/// given index returns a reference to the type
	/// if the index is not active it generates an exception
	template <int k>
	typename byindex<k>::type & getindex()
	{
		if(_index == k)
		{
			return *(typename byindex<k>::type*)value;
		}
		else
			throw std::bad_exception();
	}

	template <class T>
	bool is() const
	{
		static_assert(bytype<T>::index != -1,"sumtype::is requires type in sumtype list");
		return _index == (bytype<T>::index);
	}

	template <class T>
	typename std::remove_reference<T>::type::result_type visit(T && visitor)
	{
		typedef typename std::remove_reference<T>::type::result_type RT;
		return rtype::visit(_index,value,visitor);
	}

	template <typename F>
	auto select(F x) -> typename lambdalist<F>::result
	{
		typedef typename function_traits<decltype(&F::operator())>::signature signature;
		typedef typename extractfx<signature>::first first;

		if(bytype<first>::index == _index)
			return x(*(first*)value);
		else
			throw std::exception();
	}
	
	template <typename F, typename... FArgs>
	auto select(F x, FArgs... fx) -> typename lambdalist<F>::result
	{
		typedef typename function_traits<decltype(&F::operator())>::signature signature;
		typedef typename extractfx<signature>::first first;

		if(bytype<first>::index == _index)
			return x(*(first*)value);
		else
			return select(fx...);
	}


	int is(int i) const { return _index == i; }

	int size() const
	{
		return rtype::size(_index);
	}

	int index() const
	{
		return _index;
	}

	int maxindex() const
	{
		return sizeof...(Args);
	}

protected:
	friend class optional_sumtype<Args...>;

	/// helper for copy move
	class selfinrtype
	{
	public:
		static void ctormove(int & index, unsigned char * value, unsigned char * xwhat)
		{
			self * what = (self*)xwhat;
			index = what->index();
			std::cout << "self ctormove " << index << std::endl;
			rtype::ctormove(index,value,what->value);
		}
	};	


	void dtor()
	{
		rtype::destroy(_index,value);
		_index = -1;
	}

	int _index;
	union
	{
		unsigned char value[sizeof(bigtype)];
		typename tmax<Alignof,Args...>::type _align;
		//typename find_biggestalign<Args...>::type _align;
	};
};

/// variant of the sumtype with the nil status
template<typename ...Args>
struct optional_sumtype: public sumtype<Args...>
{
public:
	typedef sumtype<Args...> selffull;
	typedef typename selffull::rtype rtype;

	using sumtype<Args...>::sumtype; 

	optional_sumtype()  {}

	/// true if something in the sumtype
	operator bool () const { return index() != -1; }

	/// makes it nil
	void clear() { ((selffull*)this)->dtor(); };

	/// returns the index in the Args
	int index() const { return ((selffull*)this)->index(); }

	/// returns the size of the content
	int size() const
	{
		return index() == -1 ? 0 : rtype::size(index());
	}

	optional_sumtype & operator= (const selffull & other)
	{
		if(other._index == index())
		{
			rtype::assign(index(),((selffull*)this)->value,other.value);
		}
		else
		{
			// TBD
			((selffull*)this)->dtor();
			((selffull*)this)->_index = other._index;
			rtype::ctorcopy(((selffull*)this)->_index,((selffull*)this)->value,other.value);
		}
		return *this;
	}

	optional_sumtype & operator= (selffull && other)
	{
		if(other._index == index())
		{
			rtype::assign(index(),((selffull*)this)->value,other.value);
		}
		else
		{
			// TBD
			((selffull*)this)->dtor();
			((selffull*)this)->_index = other._index;
			rtype::ctormove(((selffull*)this)->_index,((selffull*)this)->value,other.value);
		}
		return *this;
	}
};

