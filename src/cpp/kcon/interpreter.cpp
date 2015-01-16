#include "interpreter.h"

using namespace kcon;
using namespace elpa;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_constant( elem_t env, elem_t k ) -> elem_t
{
    return k;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::constant( elem_t elem ) -> elem_t
{
    return _constant( elem_t(), elem );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_parse_count( pcell_t tails ) -> size_t
{
    size_t count = 0;
    int scale = 0;
    while( true )
    {
        count += (tails->head().byte( "Path tails count must be of the form [b b ...]" ) << scale);
        scale += 8;
        System::check( scale != sizeof(void*) * 8, "Path tail count overflow" );

        if( tails->tail().is_byte() )
            break;

        tails = tails->tail().pcell( "Tails tail expected to be pcell" );
    }

    count += (tails->tail().byte() << scale);

    return count;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_parse_path_elem( pcell_t path, size_t& tcount, byte_t& hcount ) -> pcell_t
{
    tcount = _parse_count( path->head().pcell( "Path tails count must be cells" ) );

    if( path->tail().is_byte() )
    {
        hcount = path->tail().byte();
        return 0;
    }

    System::assert( path->tail().is_pcell(), "Path was neither cell nor byte" );

    path = path->tail().pcell();

    hcount = path->head().byte( "Path head count must be a byte" );

    return path->tail().pcell( "Path tail count must not be a byte" );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_select( elem_t env, pcell_t path ) -> elem_t
{
    while( path != 0 )
    {
        size_t tcount;
        byte_t hcount;

        path = _parse_path_elem( path, tcount, hcount );

        while( tcount-- > 0 )
            env = env.pcell( "Tried to access tail of a byte" )->tail();

        while( hcount-- > 0 )
            env = env.pcell( "Tried to access head of a byte" )->head();
    }

    return env;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::select( elem_t elem ) -> elem_t
{
    pcell_t p = elem.pcell( "/ operates only on cells" );

    return _select( p->head(), p->tail().pcell( "/ requires paths of the form [([] b)+]" ) );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_ifcell( pcell_t choices, elem_t cond ) -> elem_t
{
    if( cond.is_pcell() )
        return choices->tail();

    System::assert( cond.is_byte(), "If condition was neither cell nor byte" );
    return choices->head();
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::ifcell( elem_t elem ) -> elem_t
{
    pcell_t p = elem.pcell( "? operates only on cells" );

    return _ifcell( p->head().pcell( "? Requires two choices not a byte"), p->tail() );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_evaluate( elem_t env, pcell_t expr ) -> elem_t
{
    if( expr->head().is_byte() )
        return _reduce( env, expr );

    System::assert( expr->head().is_pcell(), "* expr head is neither cell nor byte" );

    auto_root<elem_t> v( this->allocator(), env );
    auto_root<pcell_t> exprs( this->allocator(), expr );

    elem_t t = _reduce( v, exprs->head().pcell() );

    for(exprs = exprs->tail().pcell( "* cons form requires at least 2 cell based expressions" );
        exprs->head().is_pcell();
        exprs = exprs->tail().pcell( "* cons form requires only cell based expressions" )
    )
        t = this->allocator().new_Cell( _reduce( v, exprs->head().pcell() ), t );

    return this->allocator().new_Cell( _reduce( v, exprs ), t );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::evaluate( elem_t elem ) -> elem_t
{
    pcell_t p = elem.pcell( "* operates only on cells" );

    return _evaluate( p->head(), p->tail().pcell( "* requires cell expression" ) );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_reduce( elem_t env, pcell_t expr ) -> elem_t
{
    byte_t code = expr->head().byte( "@ requires expression code to be a byte" );
    if( code == 0 )
        return _constant( env, expr->tail() );
    else if( code == 1 )
        return _select( env, expr->tail().pcell( "@ 1 requires cell based path" ) );
    else
        System::check( false, "@ requires expression code to be 0 or 1" );

    return elem_t();
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::reduce( elem_t elem ) -> elem_t
{
    pcell_t p = elem.pcell( "@ operates only on cells" );

    return _evaluate( p->head(), p->tail().pcell( "@ requires cell expression" ) );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::execute( elem_t state ) -> elem_t
{
    System::assert( state.is_def(), "Undefined state passed to execute" );

    System::check( state.is_pcell(), "Illegal byte state for !" );

    return state;
}

#include "../elpa/runtime.h"
template class KConInterpreter<Debug, SimpleScheme, TestAllocator>;
template class KConInterpreter<Debug, SimpleScheme, SimpleAllocator>;
template class KConInterpreter<Safe, OptScheme, OptAllocator>;
template class KConInterpreter<Fast, OptScheme, OptAllocator>;
