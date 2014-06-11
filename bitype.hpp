
/**
 * No default ctor differently from optional
 */
template <class T1, class T2>
class bityped
{
private:
	enum State { None, First, Second};
public:
	struct Tag1 {};
	struct Tag2 {};
/*
	template <class Ta, class Tx...>
	bityped(Tx a) { std::bad_exception(); }

	template <class Tx...>
	bityped<T1>(Tx a) : state(First) { new (&value1) T1(x,a); }

	template <class Tx...>
	bityped<T2>(Tx a) : state(Second) { new (&value2) T2(x,a); }
*/

	bityped(const T1 & x) : state(First) { new (&value1) T1(x); } 

	bityped(const T2 & x) : state(Second) { new (&value2) T2(x); } 


	bityped(T1 && x) : state(First) { new (&value1) T1(std::move(x)); } 

	bityped(T2 && x) : state(Second) { new (&value2) T2(std::move(x)); } 

	bityped(Tag1, const T1 &x ) : state(First) { new (&value1) T1(x); } 

	bityped(Tag2, const T2 & x) : state(Second) { new (&value2) T2(x); } 

	bityped(Tag1,  T1 && x ) : state(First) { new (&value1) T1(std::move(x)); } 

	bityped(Tag2,  T2 && x) : state(Second) { new (&value2) T2(std::move(x)); } 

	bityped(const bityped & other) : state(other.state) 
	{
			if(state == First)
				new (&value1) T1(other.value1); 
			else if(state == First)
				 new (&value2) T2(other.value2);		
	}

	bityped(bityped && other) : state(other.state) 
	{
			if(state == First)
				new (&value1) T1(std::move(other.value1));
			else if(state == Second)
				new (&value2) T2(std::move(other.value2));
			other.state = None;
	}

	bityped & operator=(const bityped & other)
	{
		if(state == other.state)
		{
			if(state == First)
				value1 = other.value1;
			else if(state == Second)
				value2 = other.value2;
		}
		else
		{
			if (state == First)
				value1.~T();
			else if(state == Second)
				value2.~T();
			state = other.state;
			if(state == First)
				new (&value1) T1(other.value1); 
			else if(state == Second)
				 new (&value2) T2(other.value2);
		}
		return *this;
	}

	T1 & get(Tag1 t1) { if(state == First) return value1; throw std::bad_exception(); }
	T2 & get(Tag2 t2) { if(state == Second) return value2; throw std::bad_exception(); }

	~bityped() 
	{
		if (state == First)
			value1.~T1();
		else if(state == Second)
			value2.~T2();
  	}

	T1 & get1() {  if(state == First) return value1; throw std::bad_exception();  }

	T2 & get2() {  if(state == Second) return value2; throw std::bad_exception();  }

	bool isfirst() const { return state == First; }

	bool issecond() const { return state == Second; }

	operator int() const { return state == First ? 1 : state == Second ?2 : 0; }

	

/*
	template <class T>
	void get() const { throw std::bad_exception(); }

	template <>
	T1 & get<T1>() { if(state == First) return value1; throw std::bad_exception(); }

	template <>
	T2 & get<T2>() { if(state == Second) return value2; throw std::bad_exception(); }

	T1 & get1() {  if(state == First) return value1; throw std::bad_exception();  }

	T2 & get2() {  if(state == Second) return value2; throw std::bad_exception();  }
*/

private:
	State state;
	union
	{
		T1 value1;
		T2 value2;
	};
};
