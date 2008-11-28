#include "Tools.h"
#include <zypp/ResObjects.h>
#include <zypp/ProgressData.h>
#include <zypp/sat/WhatObsoletes.h>

#include "zypp/pool/GetResolvablesToInsDel.h"

Pathname mroot( "/tmp/Bb" );
TestSetup test( mroot, Arch_ppc64 );

#define LCStack   "IOrder::Stack"
#define LCCache   "IOrder::Cache"
#define LCVerbose "IOrder::Verbose"

bool progressReceiver( const ProgressData & v )
{
  DBG << "...->" << v << endl;
  return true;
}

struct RunnableCache
{
  typedef std::tr1::unordered_map<sat::Solvable,TriBool> CacheType;
  typedef std::vector<sat::Solvable>                     AnalyzeStack;

  RunnableCache()
  : _ltag( "[0000]" )
  {}

  /**
   * Test whether there is a runnable provider for each requirement.
   */
  bool isRunnable( const PoolItem & pi ) const
  { return isRunnable( pi.satSolvable() ); }

  bool isRunnable( sat::Solvable solv_r ) const
  {
    SEC << "Runnable?       " << solv_r << endl;
    if ( _isRunnable( solv_r ) )
    {
      MIL << "Runnable:       " << solv_r << endl;
      return true;
    }
    ERR << "NotRunnable:    " << solv_r << endl;
    return false;
  }

  /**
   * Test whether there is a runnable provider for each pre-requirement.
   */
  bool isInstallable( const PoolItem & pi ) const
  { return isInstallable( pi.satSolvable() ); }

  bool isInstallable( sat::Solvable solv_r ) const
  {
    SEC << "Installable?    " << solv_r << endl;
    tribool & cent( get( solv_r ) ); // if (cached) runnable then also installable.
    if ( cent || checkCaps( solv_r.prerequires() ) )
    {
      MIL << "Installable:    " << solv_r << endl;
      return true;
    }
    ERR << "NotInstallable: " << solv_r << endl;
    return false;
  }

  /** Clear the cache. */
  void clear() const
  {
    _cache.clear();
    _stack.clear();
    _ltag = "[0000]";
    _INT(LCCache) << "Cache cleared!" << endl;
  }

  private:
    /** Internal version without loging for recursive calls. */
    bool _isRunnable( const PoolItem & pi ) const
    { return _isRunnable( pi.satSolvable() ); }

    bool _isRunnable( sat::Solvable solv_r ) const
    {
      tribool & cent( get( solv_r ) );
      if ( indeterminate( cent ) )
        cent = analyze( solv_r );
      return cent;
    }

    /**
     * Determine whether this solvable is runnable.
     */
    bool analyze( sat::Solvable solv_r ) const
    {
      if ( ! push( solv_r ) )
      {
        if ( _stack.back() != solv_r )
          _SEC(LCStack) << _ltag << "** CYCLE: " << solv_r << " " << _stack << endl;
        // else it's a self requirement
        return true; // assume runnable?
      }
      _INT(LCStack) << _ltag << "->" << solv_r << " " << _stack << endl;
      bool ret = checkCaps( solv_r.requires() );
      _INT(LCStack) << _ltag << "<-" << solv_r << " " << _stack << endl;
      if ( ! pop( solv_r ) )
      {
        _SEC(LCStack) << "** Stack corrupted! Expect " << solv_r << " " << _stack << endl;
      }
      return ret;
    }

    /**
     * For each capability find a runnable provider.
     */
    bool checkCaps( Capabilities caps_r ) const
    {
      for_( it, caps_r.begin(), caps_r.end() )
      {
        if ( ! findRunnableProvider( *it ) )
          return false;
      }
      return true;
    }

    /**
     * Find a runnable provider of a capability on system.
     *
     * A runnable package is already installed and all of
     * its requirements are met by runnable packages.
     */
    bool findRunnableProvider( Capability cap_r ) const
    {
      _MIL(LCVerbose) << _ltag << "  " << cap_r << endl;
      sat::WhatProvides prv( cap_r );
      for_( pit, prv.begin(), prv.end() )
      {
        if ( ! *pit )
        {
          _DBG(LCVerbose) << _ltag << "     by system" << endl;
          return true; // noSolvable provides: i.e. system provides
        }

        PoolItem pi( *pit );
        if ( pi.status().onSystem() )
        {
          if ( _isRunnable( pi ) )
          {
            _DBG(LCVerbose) << _ltag << "    " << pi << endl;
            return true;
          }
          else
          {
            _WAR(LCVerbose) << _ltag << "    " << pi << endl;
          }
        }
      }
      ERR << _ltag << "    NO runnable provider for " << cap_r << endl;
      return false;
    }

  private:
    /** Push a new solvable to the AnalyzeStack, or return false is already on stack. */
    bool push( sat::Solvable solv_r ) const
    {
      if ( find( _stack.begin(), _stack.end(), solv_r ) == _stack.end() )
      {
        _stack.push_back( solv_r );
        _ltag = str::form( "[%04lu]", _stack.size() );
        return true;
      }
      // cycle?
      return false;
    }

    /** Pop solvable from AnalyzeStack (expecting it to be \c solv_r). */
    bool pop( sat::Solvable solv_r ) const
    {
      if ( _stack.back() == solv_r )
      {
        _stack.pop_back();
        _ltag = str::form( "[%0l4u]", _stack.size() );
        return true;
      }
      // stack corrupted?
      return false;
    }

    /** Return cache entry, initializing new entries with \ref indeterminate.*/
    tribool & get( sat::Solvable solv_r ) const
    {
      CacheType::iterator it( _cache.find( solv_r ) );
      if ( it == _cache.end() )
        return (_cache[solv_r] = indeterminate);
      return _cache[solv_r];
    }

    mutable CacheType    _cache;
    mutable AnalyzeStack _stack;
    mutable std::string  _ltag;
};

RunnableCache rcache;

//==================================================

bool upgrade()
{
  bool rres = false;
  {
    zypp::base::LogControl::TmpLineWriter shutUp;
    UpgradeStatistics u;
    rres = getZYpp()->resolver()->doUpgrade( u );
  }
  if ( ! rres )
  {
    ERR << "upgrade " << rres << endl;
    getZYpp()->resolver()->problems();
    return false;
  }
  MIL << "upgrade " << rres << endl;
  return true;
}

bool solve()
{
  static unsigned run = 0;
  USR << "Solve " << run++ << endl;
  bool rres = false;
  {
    //zypp::base::LogControl::TmpLineWriter shutUp;
    rres = getZYpp()->resolver()->resolvePool();
  }
  if ( ! rres )
  {
    ERR << "resolve " << rres << endl;
    getZYpp()->resolver()->problems();
    return false;
  }

  return true;
}

bool verify()
{
  bool rres = solve();
  ResPool pool( test.pool() );
  for_( it, make_filter_begin<resfilter::ByTransact>(pool),
        make_filter_end<resfilter::ByTransact>(pool) )
  {
    if ( it->status().transacts() &&
         it->status().isBySolver() )
    {
      WAR << "MISSING " << *it << endl;
    }
  }
  return rres;
}

inline void save()
{
  test.poolProxy().saveState();
}

inline void restore()
{
  test.poolProxy().restoreState();
}

void display( const pool::GetResolvablesToInsDel & collect, std::set<IdString> interested )
{
  USR << "======================================================================" << endl;
  USR << "=== DELETE" << endl;
  USR << "======================================================================" << endl;
  if ( 1 )
  {
    ProgressData tics( collect._toDelete.size() );
    tics.name( "DELETE" );
    tics.sendTo( &progressReceiver );
    tics.toMin();

    for_( it, collect._toDelete.begin(), collect._toDelete.end() )
    {
      tics.incr();

      it->status().setTransact( true, ResStatus::USER );
//       vdumpPoolStats( SEC << "Transacting:"<< endl,
//                       make_filter_begin<resfilter::ByTransact>(pool),
//                       make_filter_end<resfilter::ByTransact>(pool) ) << endl;

      if ( !interested.empty() && interested.find( it->satSolvable().ident() ) == interested.end() )
      {
        MIL << "..." << *it << endl;
        continue;
      }

      rcache.clear();
      if ( ! rcache.isInstallable( *it ) )
      {
        USR << "FAILED DEL " << *it << endl;
      }
    }
  }

  USR << "======================================================================" << endl;
  USR << "=== INSTALL" << endl;
  USR << "======================================================================" << endl;
  if ( 1 )
  {
    ProgressData tics( collect._toInstall.size() );
    tics.name( "INSTALL" );
    tics.sendTo( progressReceiver );
    tics.toMin();


    for_( it, collect._toInstall.begin(), collect._toInstall.end() )
    {
      tics.incr();

      ui::Selectable::Ptr p( ui::Selectable::get( *it ) );
      p->setCandidate( *it );
      p->setToInstall(); // also deletes the installed one
//       vdumpPoolStats( SEC << "Transacting:"<< endl,
//                       make_filter_begin<resfilter::ByTransact>(pool),
//                       make_filter_end<resfilter::ByTransact>(pool) ) << endl;


      if ( !interested.empty() && interested.find( p->ident() ) == interested.end() )
      {
        MIL << "..." << *it << endl;
        continue;
      }

      rcache.clear();

      sat::WhatObsoletes obs( *it );
      for_( it, obs.begin(), obs.end() )
      {
        if ( ! rcache.isInstallable( *it ) )
        {
          USR << "FAILED OBS " << *it << endl;
        }
      }

      if ( ! rcache.isInstallable( *it ) )
      {
        USR << "FAILED INS " << *it << endl;
      }
    }
  }
}

void display( const pool::GetResolvablesToInsDel & collect, IdString ident_r )
{
  std::set<IdString> interested;
  interested.insert( ident_r );
  display( collect, interested );
}


void display( const pool::GetResolvablesToInsDel & collect )
{
  std::set<IdString> interested;
  display( collect, interested );
}


/******************************************************************
**
**      FUNCTION NAME : main
**      FUNCTION TYPE : int
*/
int main( int argc, char * argv[] )
{
  INT << "===[START]==========================================" << endl;

  Pathname mroot( "/tmp/Bb" );
  TestSetup test( mroot, Arch_i686 ); // <<< arch

  ResPool pool( test.pool() );
  sat::Pool satpool( test.satpool() );

  {
    zypp::base::LogControl::TmpLineWriter shutUp;
    test.loadTarget();
    test.loadTestcaseRepos( "/suse/ma/BUGS/153548/YaST2/solverTestcase" ); // <<< repos
  }
  save();


  { // <<< transaction
    zypp::base::LogControl::TmpLineWriter shutUp;
    getPi<Product>( "SUSE_SLED" ).status().setTransact( true, ResStatus::USER );
    getPi<Package>( "kernel-pae" ).status().setTransact( true, ResStatus::USER );
    getPi<Package>( "sled-release" ).status().setTransact( true, ResStatus::USER );
    getPi<Pattern>( "apparmor" ).status().setTransact( true, ResStatus::USER );
    getPi<Pattern>( "desktop-base" ).status().setTransact( true, ResStatus::USER );
    getPi<Pattern>( "desktop-gnome" ).status().setTransact( true, ResStatus::USER );
    getPi<Pattern>( "x11" ).status().setTransact( true, ResStatus::USER );
    upgrade();
  }
  vdumpPoolStats( USR << "Transacting:"<< endl,
                  make_filter_begin<resfilter::ByTransact>(pool),
                  make_filter_end<resfilter::ByTransact>(pool) ) << endl;
  pool::GetResolvablesToInsDel collect( pool, pool::GetResolvablesToInsDel::ORDER_BY_MEDIANR );

  USR << ui::Selectable::get( "libtiff" ) << endl;

  restore();
  {
    //base::LogControl::TmpLineWriter shutUp( new log::FileLineWriter( "iorder.log" ) );
    std::set<IdString> interested;
    interested.insert( IdString("libtiff") );
    display( collect, interested );
  }

 INT << "===[END]============================================" << endl << endl;
  zypp::base::LogControl::TmpLineWriter shutUp;
  return 0;
}
