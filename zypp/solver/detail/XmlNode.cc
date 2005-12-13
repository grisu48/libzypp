/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* XmlNode.cc  wrapper for xmlNodePtr from libxml2
 *
 * Copyright (C) 2000-2002 Ximian, Inc.
 * Copyright (C) 2005 SUSE Linux Products GmbH
 *
 * Definition of 'edition'
 *  contains epoch-version-release-arch
 *  and comparision functions
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

#include <zypp/solver/detail/XmlNode.h>

///////////////////////////////////////////////////////////////////
namespace ZYPP {
//////////////////////////////////////////////////////////////////

using namespace std;

IMPL_BASE_POINTER(XmlNode);

//---------------------------------------------------------------------------

XmlNode::XmlNode (const xmlNodePtr node)
    : _node(node)
{
}

XmlNode::XmlNode (const char *name)
    : _node(xmlNewNode (NULL, (const xmlChar *)name))
{
}

XmlNode::~XmlNode ()
{
}

//---------------------------------------------------------------------------

string
XmlNode::asString ( void ) const
{
    return toString (*this);
}


string
XmlNode::toString ( const XmlNode & node )
{
    return "<xmlnode/>";
}


ostream &
XmlNode::dumpOn( ostream & str ) const
{
    str << asString();
    return str;
}


ostream&
operator<<( ostream& os, const XmlNode& node)
{
    return os << node.asString();
}

//---------------------------------------------------------------------------

const char *
XmlNode::getValue (const char *name, const char *deflt) const
{
    char *ret;
    xmlChar *xml_s;
    xmlNode *child;

    xml_s = xmlGetProp(_node, (const xmlChar *)name);
//    if (getenv ("RC_SPEW_XML")) fprintf (stderr, "XmlNode::getValue(%s) xmlGetProp '%s'\n", name, (char *)xml_s);

    if (xml_s) {
	ret = strdup ((const char *)xml_s);
	xmlFree (xml_s);
	return ret;
    }

    child = _node->xmlChildrenNode;

    while (child) {
//	if (getenv ("RC_SPEW_XML")) fprintf (stderr, "XmlNode::getValue(%s) child '%s'\n", name, (const char *)(child->name));
	if (strcasecmp((const char *)(child->name), name) == 0) {
	    xml_s = xmlNodeGetContent(child);
//	    if (getenv ("RC_SPEW_XML")) fprintf (stderr, "XmlNode::getValue(%s) xmlNodeGetContent '%s'\n", name, (char *)xml_s);
	    if (xml_s) {
		ret = strdup ((const char *)xml_s);
		xmlFree (xml_s);
		return ret;
	    }
	}
	child = child->next;
    }

//    if (getenv ("RC_SPEW_XML")) fprintf (stderr, "XmlNode::getValue(%s) NULL\n", name);
    return deflt;
}


const char *
XmlNode::getProp (const char *name, const char *deflt) const
{
    xmlChar *ret;
    char *gs;

    ret = xmlGetProp (_node, (const xmlChar *)name);
//    if (getenv ("RC_SPEW_XML")) fprintf (stderr, "XmlNode::getProp(%s) xmlGetProp '%s'\n", name, (char *)ret);
    if (ret) {
	gs = strdup ((const char *)ret);
	xmlFree (ret);
	return gs;
    }
    return deflt;
}


bool
XmlNode::getIntValue (const char *name, int *value) const
{
    const char *strval;
    char *ret;
    int z;

    strval = this->getValue (name, NULL);
    if (!strval) {
	return false;
    }

    z = strtol (strval, &ret, 10);
    if (*ret != '\0') {
	free ((void *)strval);
	return false;
    }

    free ((void *)strval);
    *value = z;
    return true;
}


int
XmlNode::getIntValueDefault (const char *name, int def) const
{
    int z;
    if (this->getIntValue (name, &z))
	return z;
    else
	return def;
}

	       
unsigned int
XmlNode::getUnsignedIntValueDefault (const char *name, unsigned int def) const
{
    unsigned int z;
    if (this->getUnsignedIntValue (name, &z))
	return z;
    else
	return def;
}


bool
XmlNode::getUnsignedIntValue (const char *name, unsigned int *value) const
{
    const char *strval;
    char *ret;
    int z;

    strval = this->getValue (name, NULL);
    if (!strval) {
	return false;
    }

    z = strtoul (strval, &ret, 10);
    if (*ret != '\0') {
	free ((void *)strval);
	return false;
    }

    free ((void *)strval);
    *value = z;
    return true;
}


unsigned int
XmlNode::getUnsignedIntPropDefault (const char *name, unsigned int def) const
{
    xmlChar *buf;
    unsigned int ret;

    buf = xmlGetProp (_node, (const xmlChar *)name);

    if (buf) {
	ret = strtol ((const char *)buf, NULL, 10);
	xmlFree (buf);
	return (ret);
    } else {
	return (def);
    }
}


const char *
XmlNode::getContent (void) const
{
    xmlChar *buf;
    char *ret;

    buf = xmlNodeGetContent (_node);

    ret = strdup ((const char *)buf);

    xmlFree (buf);

    return (ret);
}


unsigned int
XmlNode::getUnsignedIntContentDefault (unsigned int def) const
{
    xmlChar *buf;
    unsigned int ret;

    buf = xmlNodeGetContent (_node);

    if (buf) {
	ret = strtol ((const char *)buf, NULL, 10);
	xmlFree (buf);
	return (ret);
    } else {
	return (def);
    }
}


const XmlNodePtr
XmlNode::getNode (const char *name) const
{
    xmlNodePtr iter;

    for (iter = _node->xmlChildrenNode; iter; iter = iter->next) {
	if (strcasecmp ((const char *)(iter->name), name) == 0) {
	    return new XmlNode (iter);
	}
    }

    return NULL;
}


//---------------------------------------------------------------------------


void
XmlNode::addTextChild (const char *name, const char *content)
{
    xmlNewTextChild (_node, NULL, (const xmlChar *)name, (const xmlChar *)content);
}

///////////////////////////////////////////////////////////////////
}; // namespace ZYPP
///////////////////////////////////////////////////////////////////

