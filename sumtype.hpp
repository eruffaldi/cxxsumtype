#pragma once
#include <utility> 
#include <iostream>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <functional>

template <typename T>
struct Alignof
{
	static constexpr size_t value = alignof(T);
};

template <typename T>
struct Sizeof
{
	static constexpr size_t value = sizeof(T);
};

// similar to the newcpp post
template <template <typename> class Pred, typename... Args>
struct tmax;
 
template <template <typename> class Pred, typename First>
struct tmax<Pred, First>
{
	typedef First type;
};
 
template <template <typename> class Pred, typename First, typename... Args>
struct tmax<Pred, First, Args...>
{
  typedef typename tmax<Pred, Args...>::type next;
  typedef typename std::conditional<Pred<First>::value >= Pred<next>::value,First,next>::type type;
};


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

template <size_t looked, size_t current, typename... Args>
struct find_index;
 
template <size_t looked, size_t current, typename First>
struct find_index<looked, current, First>
{
	typedef First type;
};
 
template <size_t looked, size_t current, typename First, typename... Args>
struct find_index<looked,current,First, Args...>
{
  typedef typename find_index<looked,current+1,Args...>::type next;
  typedef typename std::conditional<looked == current,First,next>::type type;
};



template <class looked, int current, typename... Args>
struct find_type;
 
template <class looked, int current, typename First>
struct find_type<looked, current, First>
{
	enum { index = std::is_same<looked,First>::value ? current : -1 };
};
 
template <class looked, int current, typename First, typename... Args>
struct find_type<looked,current,First, Args...>
{
	enum { index = std::is_same<looked,First>::value ?  current : find_type<looked,current+1,Args...>::index };
};

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

template <int current, typename... Args>
struct find_indexrun;
 
template <int current, typename First>
struct find_indexrun<current, First>
{
	static void destroy(int index, unsigned char * value)
	{
		if(index == current)
			((First*)value)->~First();
	}

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

	template <class T>
	static typename std::remove_reference<T>::type::result_type visit(int index, unsigned char * value, T && visitor)
	{
		if(index == current)
			return visitor(*(First*)value);
		else
			throw std::bad_exception();
	}

	template <typename... FArgs>
	static typename std::function< typename std::tuple_element<0,std::tuple<FArgs...> >::type >::result_type tvisit(int index, unsigned char * value, std::tuple<FArgs...> && visitor)
	{
		if(index == current)
			return std::get<current>(visitor)(*(First*)value);
		else
			throw std::bad_exception();
	}

	static size_t size(int index)
	{
		return index == current ? sizeof(First) : 0;
	}

	static void assign(int index, unsigned char * value, const unsigned char * other)
	{
		if(index == current)
			(*(First*)value) = *(const First*)other;
		else
			throw std::bad_exception();
	}

};


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

	template <typename... FArgs>
	static typename std::function< typename std::tuple_element<0,std::tuple<FArgs...> >::type >::result_type tvisit(int index, unsigned char * value, std::tuple<FArgs...> && visitor)
	{
		if(index == current)
			return std::get<current>(visitor)(*(First*)value);
		else
			return next::tvisit(index,value,visitor);
	}

};

template<typename ...Args>
struct optional_sumtype;

template <typename... Args>
class sumtype
{
public:
	typedef sumtype<Args...> self;
	typedef typename find_biggest<Args...>::type bigtype;
	typedef find_indexrun<0,Args...> rtype;

	// type to int: compiletype vs runtime
	// int to type: compiletype vs runtime

	// byindex<index>::type => gives T as type 
	template <size_t index>
	struct byindex : find_index<index,0,Args...>
	{
	};

	// bytype<T>::index => gives value or -1 
	template <class T>
	struct bytype : find_type<T,0,Args...>
	{
	};

	// bytype<T>::index => gives value or -1 
	template <class T>
	struct bytypeex : find_typeex<T,0,Args...>
	{
	};


	~sumtype() { dtor(); }

	template <class T>
	sumtype(const T & what) : _index(bytype<T>::index) 
	{
		static_assert(bytype<T>::index != -1,"only supported types with explicit type match");
		std::cout << "copy T " << typeid(T).name() << std::endl;
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

#if 0
	template <typename... FArgs>
	typename std::function< typename std::tuple_element<0,std::tuple<FArgs...> >::type >::result_type  tvisit(std::tuple<FArgs...> && visitor)
	{
		return rtype::tvisit(_index,value,visitor);
	}
#endif


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

template<typename ...Args>
struct optional_sumtype: public sumtype<Args...>
{
public:
	typedef sumtype<Args...> selffull;
	typedef typename selffull::rtype rtype;

	using sumtype<Args...>::sumtype; 

	optional_sumtype()  {}

	operator bool () const { return index() != -1; }

	void clear() { ((selffull*)this)->dtor(); };

	int index() const { return ((selffull*)this)->index(); }

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

