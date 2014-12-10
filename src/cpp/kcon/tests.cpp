#include "tests.h"
#include "Allocator.h"
#include "stream.h"
#include "Parser.h"
#include <sstream>

void test_ostream()
{
	Allocator a;
	pcell_t p = new (a) Cell<value_t,pcell_t>{
					0,
					new (a) Cell<pcell_t,value_t>{
						new (a) Cell<value_t,value_t>{3,3},
						2 } };

	std::ostringstream os;
	KConOStream( os ) << "flat = " << p;
	KConOStream( os ) << "deep = " << deep << p;
	test( os.str() == "flat = [0 [3 3] 2]deep = [0 [[3 3] 2]]", "Incorrect printing found '" + os.str() + "'" );
}

void test_io( std::string in, KConOStream::Manip m, std::string out="" )
{
	if( out.empty() )
		out = in;
		
	Allocator a;
	std::istringstream iss( in );
	std::ostringstream oss;
	KConOStream( oss ) << m << Parser( a ).parse( iss );
	
	test( oss.str() == out, "IO failed for: " + in + "\nExpected: " + out + "\nFound:    " + oss.str() );
}

void test_stream()
{
	test_io( "[0 [1 2]]", flat, "[0 1 2]" );
	test_io( "[0 1 2]", deep, "[0 [1 2]]" );
	test_io( "[0 [1 [2 3] 4] 5 6]", flat );
	test_io( "[0 [1 [2 3] 4] 5 6]", deep, "[0 [[1 [[2 3] 4]] [5 6]]]" );
}

void run_tests()
{
	test_ostream();
	test_stream();
}