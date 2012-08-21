/**
 * \file storage_object.h
 * \brief CStorageObject
 * \date 2012-08-18 09:02GMT
 * \author Jan Boon (Kaetemi)
 * CStorageObject
 */

/*
 * Copyright (C) 2012  by authors
 *
 * This file is part of RYZOM CORE PIPELINE.
 * RYZOM CORE PIPELINE is free software: you can redistribute it
 * and/or modify it under the terms of the GNU Affero General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * RYZOM CORE PIPELINE is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License along with RYZOM CORE PIPELINE.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef PIPELINE_STORAGE_OBJECT_H
#define PIPELINE_STORAGE_OBJECT_H
#include <nel/misc/types_nl.h>

// STL includes
#include <sstream>

// NeL includes
#include <nel/misc/stream.h>

// Project includes

namespace PIPELINE {
namespace MAX {
	class CStorageChunks;

struct EStorage : public NLMISC::Exception
{
	EStorage() : NLMISC::Exception("PIPELINE::MAX::EStorage") { }
	EStorage(const char *msg) : NLMISC::Exception(msg) { }
	virtual ~EStorage() throw() { }
};

struct EStorageParse : public EStorage
{
	EStorageParse() : EStorage("PIPELINE::MAX::EStorageParse") { }
	EStorageParse(const char *msg) : EStorage(msg) { }
	virtual ~EStorageParse() throw() { }
};

enum TParseLevel
{
	PARSE_INTERNAL = 0x00000001, // Directly parse basic class formats
	// PARSE_BUILTIN = 0x00000002; // Parse all builtin classes - reserved
	// PARSE_NELDATA = 0x00000004, // Parse all structures related to nel specific data (nel material, node properties, etcetera)
	// PARSE_NEL3D = 0x00000008, // Parse classes to initialize their nel3d equivalent classes
};

// IStorageObject : exposes serial(CStorageStream &stream) and dump(const std::string &pad)
class IStorageObject : public NLMISC::IStreamable
{
public:
	IStorageObject();
	virtual ~IStorageObject();

	// virtual std::string getClassName() = 0; // inherited from NLMISC::IClassable through NLMISC::IStreamable
	// virtual void serial(NLMISC::IStream &stream); // inherited from NLMISC::IStreamable
	std::string toString();
	virtual void toString(std::ostream &ostream, const std::string &pad = "") = 0;

public: // should be protected but that doesn't compile, nice c++!
	// Sets size when reading
	virtual void setSize(sint32 size);
	// Gets the size when writing, return false if unknown
	virtual bool getSize(sint32 &size) const;
	// Only true when inherting from CStorageContainer
	virtual bool isContainer() const;
};

// CStorageContainer : serializes a container chunk
class CStorageContainer : public IStorageObject
{
public:
	// public data
	typedef std::pair<uint16, IStorageObject *> TStorageObjectWithId;
	typedef std::list<TStorageObjectWithId> TStorageObjectContainer;
	TStorageObjectContainer Chunks;

protected:
	// protected data
	bool ChunksOwnsPointers;

public:
	CStorageContainer();
	virtual ~CStorageContainer();

	// inherited
	virtual std::string getClassName();
	virtual void serial(NLMISC::IStream &stream); // only used to wrap a container inside another stream
	virtual void toString(std::ostream &ostream, const std::string &pad = "");

	// virtual
	// Parse this class with given version and parse level filter
	virtual void parse(uint16 version, TParseLevel level);
	// Clean up built data or duplicate unparsed source data, call after serializing build and after parse
	virtual void clean();
	// Build the storage structure needed to store the parsed data back
	virtual void build(uint16 version);
	// Give ownership of the chunks back to the Chunks, must call build first, call instead of clean, reduces the parse level back to 0
	virtual void disown();

public: // should be protected but that doesn't compile, nice c++!
	// inherited
	virtual bool isContainer() const;

protected:
	// override in subclasses, default to parent if not handled
	virtual void serial(CStorageChunks &chunks);
	// Create a storage object by id, override to provide custom serialization
	virtual IStorageObject *createChunkById(uint16 id, bool container);
};

// CStorageRaw : serializes raw data, use for unknown data
class CStorageRaw : public IStorageObject
{
public:
	// public data
	typedef std::vector<uint8> TType;
	TType Value;

public:
	CStorageRaw();
	virtual ~CStorageRaw();

	// inherited
	virtual std::string getClassName();
	virtual void serial(NLMISC::IStream &stream);
	virtual void toString(std::ostream &ostream, const std::string &pad = "");

public: // should be protected but that doesn't compile, nice c++!
	// Sets size when reading
	virtual void setSize(sint32 size);
	// Gets the size when writing, return false if unknown
	virtual bool getSize(sint32 &size) const;
};
/*
// CStorageUCString : serializes an ucstring chunk
class CStorageUCString : public ucstring, public IStorageObject
{
public:
	virtual void serial(CStorageStream *stream);
	virtual void dump(const std::string &pad);
};

// CStorageString : serializes a string chunk
class CStorageString : public std::string, public IStorageObject
{
public:
	virtual void serial(CStorageStream *stream);
	virtual void dump(const std::string &pad);
};

template<typename T>
class CStorageValue : public IStorageObject
{
public:
	T Value;
	virtual void serial(CStorageStream *stream);
	virtual void dump(const std::string &pad);
};
*/

} /* namespace MAX */
} /* namespace PIPELINE */

#endif /* #ifndef PIPELINE_STORAGE_OBJECT_H */

/* end of file */
