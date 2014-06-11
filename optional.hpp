
template <class T>
class optional
{
public:

/*
	template <class Tx...>
	optional(Tx a) : valid(true) { new (value T(x)); }
*/

	optional() : valid(false) {}

	optional(T && x) : valid(true) { new(&value) T(std::move(x)); }

	optional(const T & x) : valid(true) { new (&value) T(x); }

	optional(const optional & other) : valid(other.valid) { if(valid) new (&value) T(other.value); }

	optional(optional && other) : valid(other.valid) 
	{
		if(valid)
			new (&value) T(std::move(other.value));
		other.valid = false;
	}

	optional & operator= (const optional & other)
	{
		if(other.valid && valid)
			value = other.value;
		else
		{
			if(valid)
		       value.~T(); 
		   valid = other.valid;
		   if(valid)
		   		new (value) T(other.value);
		}
		return *this;
	}

	// maybe(const T& _v) {
	//  maybe(const maybe& other) {
	~optional() 
	{
    	if (valid)
       		value.~T();
  	}

	operator bool () const { return  valid; }
	T &  operator * () { if(valid) return value; else throw std::bad_exception();}
	const T &  operator * () const { if(valid) return value; else throw std::bad_exception(); }

private:
	bool valid;
	union {
		T value; // C++11: non trivial constructor
	};
};
