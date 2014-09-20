#include "sumtype.hpp"

sumtype<int,std::string> test(int index)
{
	if(index == 0)
		return 10;
	else if(index == 1)
		return std::string("ciao");
#ifdef EXACTMATCHTEST
	else
		return "ciao"; // raise error CT
#else
	else
		throw std::bad_exception();
#endif

}

int main()
{
	#if 0
	optional<std::string> a("ciao");
	if(a)
		std::cout << *a << std::endl;
	optional<std::string> b = a;
	optional<std::string> c;
	//std::cout << *c << std::endl;

	bityped<std::string,int> bi(10);

	std::cout << "which:"<<bi.isfirst() << std::endl;
	std::cout << bi.get2() << std::endl;
#endif

	typedef sumtype<int,std::string> t1;
	t1 w1(std::string("ciao"));

	std::cout << "w1 index " << w1.index() << std::endl;
	std::cout << "w1 gettype " << w1.gettype<std::string>() << std::endl;
	std::cout << "w1 getindex " << w1.getindex<1>() << std::endl;
	std::cout << "w1 getindex sizeof " << sizeof(w1.getindex<1>()) << std::endl;

	//sumtype<int,float> w2;

	std::cout << "sizeof " << sizeof(w1) << std::endl;
	//std::cout << "sizeof " << sizeof(w2) << std::endl;
	std::cout << "byindex type is string " << sizeof(t1::byindex<1>::type) << std::endl;
	std::cout << "byindex type is int " << sizeof(t1::byindex<0>::type) << std::endl;

	std::cout << "CT bytypeex type is string " << (t1::bytypeex<std::string>::index) << std::endl;
	std::cout << "CT bytypeex type is int " << (t1::bytypeex<int>::index) << std::endl;
//	std::cout << "CT bytypeex type is float missing error compile " << (t1::bytypeex<float>::index) << std::endl;

	std::cout << "CT bytype type is string " << (t1::bytype<std::string>::index) << std::endl;
	std::cout << "CT bytype type is int " << (t1::bytype<int>::index) << std::endl;
	std::cout << "CT bytype type is float missing " << (t1::bytype<float>::index) << std::endl;
	std::cout << "RT current size " << w1.size() << std::endl;
	std::cout << "RT is std::string? " << w1.is<std::string>() << std::endl;

#ifdef TEST_ISEXCEPTION
	std::cout << "RT is bool? " << w1.is<bool>() << std::endl;
#endif

	t1 w1b(w1);
	std::cout << "w1 getindex " << w1.getindex<1>() << std::endl;
	std::cout << "w1b getindex " << w1b.getindex<1>() << std::endl;

	std::cout << "**test returns int\n";
	w1b = test(0);
	std::cout << "**test returns dump\n";
	std::cout << "w1b test(0) getindex " << w1b.is<int>() << std::endl;

	std::cout << "**test returns std::string\n";
	w1b = test(1);
	std::cout << "w1b test(1) getindex " << w1b.is<int>() << std::endl;

#ifdef EXACTMATCHTEST
	std::cout << "**test returns char*\n";
	w1b = test(2);
	std::cout << "w1b test(2) getindex " << w1b.is<int>() << std::endl;
#endif

	optional_sumtype<int,std::string> opt(10);
	std::cout << (bool)opt << std::endl;

	class xvisit
	{
	public:
		typedef int result_type;

		result_type operator()(int & x)
		{
			std::cout << "iciao " << x << std::endl;
			return 10;
		}
		result_type operator() (std::string & x)
		{
			std::cout << "sciao " << x << std::endl;
			return 11;
		}
	};
	std::cout << opt.visit(xvisit()) << std::endl;

	int a = 10;
	std::cout << opt.select(
			[a] (int x) { return (int)10+a; },
			[] (std::string w) { return (int)20; }
		);



	opt.clear();
	std::cout << (bool)opt << std::endl;
	return 0;
}