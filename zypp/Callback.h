/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file zypp/Callback.h
 *
*/
#ifndef ZYPP_CALLBACK_H
#define ZYPP_CALLBACK_H

#include "zypp/base/NonCopyable.h"

///////////////////////////////////////////////////////////////////
namespace zypp
{ /////////////////////////////////////////////////////////////////

  /** \todo Eliminate this! */
  namespace HACK {
    class Callback
    {
    };
  } // namespace HACK

  ///////////////////////////////////////////////////////////////////
  /** Callbacks light.
   *
   * \par The task report structure (SENDER SIDE).
   *
   * A default constructible struct derived from callback::ReportBase.
   * It \b must \b not conatin any data, just virtual methods.
   *
   * These are the functions the sender invokes, and which will be forwarded
   * to some receiver.
   *
   * For methods returning non-void, define a reasonable return value,
   * because this is what you get back in case no receiver is listening.
   *
   * That way the sending side does not need to know whether some receiver
   * is listening. And it enables the receiver to return a reasonable value,
   * in case he's got no idea, what to else to return.
   *
   * \code
   *   struct Foo : public callback::ReportBase
   *   {
   *     virtual void ping( int i )
   *     {}
   *     virtual int pong()
   *     { return -1; }
   *
   *   };
   * \endcode
   *
   * \par Sending a Task report (SENDER SIDE).
   *
   * Simply create a callback::SendReport<_Report>, where _Report
   * is your task report structure. Invoke the callback functions
   * as needed. That's it.
   *
   * \note Even creation and destruction of a callback::SendReport
   * are indicated to a receiver. So even in case of an Exception,
   * the receiver is able to recognize, that the task ended.
   * So don't create it without need.
   *
   * \code
   * {
   *   callback::SendReport<Foo> report;
   *   report->ping( 13 );
   *   int response = report->pong();
   * }
   * \endcode
   *
  */
  namespace callback
  { /////////////////////////////////////////////////////////////////

    /**  */
    struct ReportBase
    {
      virtual ~ReportBase()
      {}
    };

    /**  */
    template<class _Report>
      class DistributeReport;

    /**  */
    template<class _Report>
      struct ReceiveReport : public _Report
      {
        typedef DistributeReport<_Report> Distributor;

        virtual void reportbegin()
        {}
        virtual void reportend()
        {}
      };

    /**  */
    template<class _Report>
      struct DistributeReport
      {
       public:
         typedef ReceiveReport<_Report> Receiver;

         static DistributeReport & instance()
         {
           static DistributeReport _self;
           return _self;
         }

         void setReceiver( Receiver & _rec )
         { _receiver = &_rec; }

         void noReceiver()
         { _receiver = &_noReceiver; }

      public:
         Receiver * operator->()
         { return _receiver; }

      private:
        DistributeReport()
        : _receiver( &_noReceiver )
        {}
        Receiver _noReceiver;
        Receiver * _receiver;
      };

    /**  */
    template<class _Report>
      struct SendReport : private zypp::base::NonCopyable
      {
        typedef DistributeReport<_Report> Distributor;

        SendReport()
        { Distributor::instance()->reportbegin(); }

        ~SendReport()
        { Distributor::instance()->reportend(); }

        Distributor & operator->()
        { return Distributor::instance(); }
      };

    /////////////////////////////////////////////////////////////////
  } // namespace callback
  ///////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
#endif // ZYPP_CALLBACK_H
