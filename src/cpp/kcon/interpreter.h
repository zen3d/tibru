#ifndef KCON_INTERPRETER_H
#define KCON_INTERPRETER_H

#include "../elpa/interpreter.h"

using namespace elpa;

namespace kcon {

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class KConShellManager;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class KConInterpreter : public InterpreterBase<System, SchemeT, AllocatorT>
{
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::byte_t byte_t;
    typedef typename Scheme::elem_t elem_t;

    auto _select( elem_t target, pcell_t path ) -> elem_t;
    auto _ifcell( pcell_t choices, elem_t cond ) -> elem_t;
public:
    KConInterpreter( size_t ncells )
        : InterpreterBase<System, SchemeT, AllocatorT>( ncells ) {}

    typedef KConShellManager<System, SchemeT, AllocatorT> ShellManager;

    auto constant( elem_t elem ) -> elem_t;
    auto select( elem_t elem ) -> elem_t;
    auto ifcell( elem_t elem ) -> elem_t;
    auto reduce( elem_t elem ) -> elem_t;
    auto execute( elem_t state ) -> elem_t;
};

}   //namespace

#endif // HEADER_KCON_INTERPRETER
