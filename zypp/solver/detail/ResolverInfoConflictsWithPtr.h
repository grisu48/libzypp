/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* ResolverInfoConflictsWithPtr.h
 *
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

#ifndef _ResolverInfoConflictsWithPtr_h
#define _ResolverInfoConflictsWithPtr_h

#include <y2util/RepDef.h>
#include <zypp/solver/detail/ResolverInfoPtr.h>

///////////////////////////////////////////////////////////////////
namespace ZYPP {
//////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//	CLASS NAME : ResolverInfoConflictsWithPtr
//	CLASS NAME : constResolverInfoConflictsWithPtr
///////////////////////////////////////////////////////////////////
DEFINE_DERIVED_POINTER(ResolverInfoConflictsWith, ResolverInfo);

///////////////////////////////////////////////////////////////////
}; // namespace ZYPP
///////////////////////////////////////////////////////////////////

#endif // _ResolverInfoConflictsWithPtr_h
