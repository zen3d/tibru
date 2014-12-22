#ifndef ELPA_STREAM_H
#define ELPA_STREAM_H

#include "types.h"
#include "memory.h"
#include "container/elpa_stack.h"
#include <ostream>
#include <istream>
#include <sstream>

namespace elpa {

struct Syntax;
struct EOS;

template<class System, MetaScheme class SchemeT>
class elpa_ostream
{
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::byte_t byte_t;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;

	std::ostream& _os;
	bool _flatten;

	auto _format( pcell_t pcell );
	auto _format( byte_t value );

    struct Tail { elem_t elem; size_t len; };
public:
	elpa_ostream( std::ostream& os, bool flatten=true )
		: _os( os ), _flatten( flatten ) {}

    auto setflatten( bool b ) -> elpa_ostream& { _flatten = b; return *this; }

	auto operator<<( pcell_t pcell ) -> elpa_ostream&;
	auto operator<<( byte_t value ) -> elpa_ostream&;
	auto operator<<( elem_t elem ) -> elpa_ostream&;

    template<class T>
    auto operator<<( const T& t ) -> elpa_ostream&
    {
        _os << t;
        return *this;
    }

    typedef elpa_ostream& (*ElpaManip)( elpa_ostream& );

    auto operator<<( ElpaManip m ) -> elpa_ostream&
    {
        return m(*this);
    }

    typedef std::ostream& (*Manip)( std::ostream& );

    auto operator<<( Manip m ) -> elpa_ostream&
    {
        m(_os);
        return *this;
    }
};

template<class System, MetaScheme class SchemeT>
inline auto flat( elpa_ostream<System, SchemeT>& kos ) -> elpa_ostream<System, SchemeT>&
{
    return kos.setflatten( true );
}

template<class System, MetaScheme class SchemeT>
inline auto deep( elpa_ostream<System, SchemeT>& kos ) -> elpa_ostream<System, SchemeT>&
{
    return kos.setflatten( false );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class elpa_istream
{
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::value_t value_t;
    typedef typename Scheme::byte_t byte_t;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;
    typedef AllocatorT<System,SchemeT> Allocator;

    template<class T>
    using auto_root = typename Allocator::template auto_root<T>;

    template<class T>
    using elpa_stack = elpa::container::elpa_stack<System, SchemeT, AllocatorT, T>;

    std::istream& _is;
    Allocator& _alloc;

    auto _parse_byte() -> byte_t;
	auto _parse_elems() -> elem_t;
	auto _reverse_and_reduce( elem_t p ) -> elem_t;
	auto _parse() -> elem_t;
public:
    elpa_istream( std::istream& is, Allocator& alloc )
        : _is( is ), _alloc( alloc ) {}

	auto operator>>( elem_t& elem ) -> elpa_istream&;
};

}	//namespace

#endif