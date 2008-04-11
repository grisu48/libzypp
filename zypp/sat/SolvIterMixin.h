/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file	zypp/sat/SolvIterMixin.h
 *
*/
#ifndef ZYPP_SAT_SOLVITERMIXIN_H
#define ZYPP_SAT_SOLVITERMIXIN_H

#include <iosfwd>

#include "zypp/base/PtrTypes.h"
#include "zypp/base/Iterator.h"
#include "zypp/base/Tr1hash.h"

///////////////////////////////////////////////////////////////////
namespace zypp
{ /////////////////////////////////////////////////////////////////

  class asPoolItem; // transform functor

  namespace ui
  {
    class asSelectable; // transform functor
  }

  ///////////////////////////////////////////////////////////////////
  namespace sat
  { /////////////////////////////////////////////////////////////////

    class Solvable;

    namespace solvitermixin_detail
    {
      /** Unify by \c ident \c (kind:name).
       * Return true on the 1st appearance of a new \c ident. This is
       * used in \ref SolvIterMixin when mapping a  Solvable iterator
       * to a Selectable iterator.
      */
      struct UnifyByIdent
      {
        bool operator()( const Solvable & solv_r ) const;

        typedef std::tr1::unordered_set<unsigned> Uset;
        UnifyByIdent()
          : _uset( new Uset )
        {}
        shared_ptr<Uset> _uset;
      };
    } // namespace solvitermixin_detail


    ///////////////////////////////////////////////////////////////////
    //
    //	CLASS NAME : SolvIterMixin<Derived,DerivedSolvable_iterator>
    //
    /** Base class providing common iterator types based on a \ref Solvable iterator.
     *
     * A class deriving from \ref SolvIterMixin must provide two methods
     * \c begin and \c end returning iterator over \ref sat::Solvable.
     *
     * \ref SolvIterMixin will then provide iterators over the corresponding
     * \ref PoolItem and \ref ui::Selectable_Ptr.
     *
     * \code
     *     namespace detail
     *     {
     *       class WhatProvidesIterator;
     *     }
     *
     *     class WhatProvides : public SolvIterMixin<WhatProvides,detail::WhatProvidesIterator>
     *     {
     *       public:
     *         typedef detail::WhatProvidesIterator const_iterator;
     *
     *         // Iterator pointing to the first Solvable.
     *         const_iterator begin() const;
     *
     *         // Iterator pointing behind the last Solvable.
     *         const_iterator end() const;
     *
     *     };
     *
     *     namespace detail
     *     {
     *       class WhatProvidesIterator : public boost::iterator_adaptor<
     *           WhatProvidesIterator          // Derived
     *          , const detail::IdType *       // Base
     *          , const Solvable               // Value
     *          , boost::forward_traversal_tag // CategoryOrTraversal
     *          , const Solvable               // Reference
     *       >
     *       {
     *          ...
     *       };
     *     }
     * \endcode
     * \ingroup g_CRTP
     */
    template <class Derived,class DerivedSolvable_iterator>
    class SolvIterMixin
    {
      public:
        /** \name Iterate as Solvable */
        //@{
        typedef  DerivedSolvable_iterator Solvable_iterator;
        Solvable_iterator solvableBegin() const
        { return self().begin(); }
        Solvable_iterator solvableEnd() const
        { return self().end(); }
        //@}

        /** \name Iterate as PoolItem */
        //@{
        typedef transform_iterator<asPoolItem,Solvable_iterator> PoolItem_iterator;
        PoolItem_iterator poolItemBegin() const
        { return make_transform_iterator( solvableBegin(), asPoolItem() ); }
        PoolItem_iterator poolItemEnd() const
        { return make_transform_iterator( solvableEnd(), asPoolItem() ); }
        //@}

      private:
        typedef filter_iterator<solvitermixin_detail::UnifyByIdent,Solvable_iterator> UnifiedSolvable_iterator;
      public:
        /** \name Iterate ui::Selectable::Ptr */
        //@{
        typedef transform_iterator<ui::asSelectable,UnifiedSolvable_iterator> Selectable_iterator;
        Selectable_iterator selectableBegin() const
        { return make_transform_iterator( unifiedSolvableBegin(), ui::asSelectable() ); }
        Selectable_iterator selectableEnd() const
        { return make_transform_iterator( unifiedSolvableEnd(), ui::asSelectable() ); }
        //@}

      private:
        /** \name Iterate unified Solbvables to be transformed into Selectable. */
        //@{
        UnifiedSolvable_iterator unifiedSolvableBegin() const
        { return make_filter_iterator( solvitermixin_detail::UnifyByIdent(), solvableBegin(), solvableEnd() ); }
        UnifiedSolvable_iterator unifiedSolvableEnd() const
        { return make_filter_iterator( solvitermixin_detail::UnifyByIdent(), solvableEnd(), solvableEnd() );; }
        //@}
      private:
        const Derived & self() const
        { return *static_cast<const Derived*>( this ); }
      protected:
        SolvIterMixin() {}
        ~SolvIterMixin() {}
        SolvIterMixin(const SolvIterMixin &) {}
        void operator=(const SolvIterMixin &) {}
     };
    ///////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////
  } // namespace sat
  ///////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
#endif // ZYPP_SAT_SOLVITERMIXIN_H
