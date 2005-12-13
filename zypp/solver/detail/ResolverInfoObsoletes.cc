/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* ResolverInfoObsoletes.cc
 *
 * Copyright (C) 2000-2002 Ximian, Inc.
 * Copyright (C) 2005 SUSE Linux Products GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <map>

#include <zypp/solver/detail/ResolverInfo.h>
#include <zypp/solver/detail/ResolverInfoObsoletes.h>

///////////////////////////////////////////////////////////////////
namespace ZYPP {
//////////////////////////////////////////////////////////////////

using namespace std;

IMPL_DERIVED_POINTER(ResolverInfoObsoletes, ResolverInfo);

//---------------------------------------------------------------------------


string
ResolverInfoObsoletes::asString ( void ) const
{
    return toString (*this);
}


string
ResolverInfoObsoletes::toString ( const ResolverInfoObsoletes & obsoletes)
{
    string res;

    res += ResolverInfo::toString (obsoletes);
    res += string ("replaced by ") + obsoletes.resolvablesToString(false);

    return res;
}


ostream &
ResolverInfoObsoletes::dumpOn( ostream & str ) const
{
    str << asString();
    return str;
}


ostream&
operator<<( ostream& os, const ResolverInfoObsoletes & obsoletes)
{
    return os << obsoletes.asString();
}

//---------------------------------------------------------------------------

ResolverInfoObsoletes::ResolverInfoObsoletes (constResolvablePtr resolvable, constResolvablePtr obsoletes)
    : ResolverInfoContainer (RESOLVER_INFO_TYPE_OBSOLETES, resolvable, RESOLVER_INFO_PRIORITY_USER, obsoletes)
{
}


ResolverInfoObsoletes::~ResolverInfoObsoletes ()
{
}

//---------------------------------------------------------------------------

ResolverInfoPtr
ResolverInfoObsoletes::copy (void) const
{
    ResolverInfoObsoletesPtr cpy = new ResolverInfoObsoletes(resolvable(), NULL);

    ((ResolverInfoContainerPtr)cpy)->copy (this);

    return cpy;
}


///////////////////////////////////////////////////////////////////
}; // namespace ZYPP
///////////////////////////////////////////////////////////////////

