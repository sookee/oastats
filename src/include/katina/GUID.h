#pragma once
#ifndef _OASTATS_GUID_H_
#define _OASTATS_GUID_H_

/*
 * GUID.h
 *
 *  Created on: Apr 7, 2013
 *      Author: oasookee@gmail.com
 */

#include "types.h"

#include <inttypes.h>

namespace oastats {

using namespace oastats::types;

class GUID
{
	str data;

public:
	const static siz SIZE = 8;

	GUID(): data(SIZE, '0')
	{
		for(siz i = 0; i < SIZE; ++i)
			this->data[i] = '0';
	}

	GUID(const char data[SIZE]): data(SIZE, '0')
	{
		for(siz i = 0; i < SIZE; ++i)
			this->data[i] = data[i];
	}

	GUID(const str& data): data(SIZE, '0')
	{
		for(siz i = 0; i < SIZE && i < data.size(); ++i)
			this->data[i] = data[i];
	}

	GUID(const GUID& guid): data(SIZE, '0')
	{
		for(siz i = 0; i < SIZE; ++i)
			this->data[i] = guid.data[i];
	}

	const GUID& operator=(const GUID& guid)
	{
		for(siz i = 0; i < SIZE; ++i)
			this->data[i] = guid.data[i];
		return *this;
	}

	bool operator==(const GUID& guid) const
	{
		for(siz i = 0; i < SIZE; ++i)
			if(this->data[i] != guid.data[i])
				return false;
		return true;
	}

	bool operator!=(const GUID& guid) const
	{
		return !(*this == guid);
	}

	bool operator<(const GUID& guid) const
	{
		return data < guid.data;
	}

	char& operator[](siz i) { return data[i]; }
	const char& operator[](siz i) const { return data[i]; }
	siz size() const { return SIZE; }

	operator str() const { return data; }

	operator uint32_t() const
	{
		uint32_t i = 0;
		siss iss(data);
		iss >> std::hex >> i;
		return i;
	}

	bool is_bot() const { return data < "00001000"; }
};

inline
sos& operator<<(sos& os, const GUID& guid)
{
	for(siz i = 0; i < guid.size(); ++i)
		os << guid[i];
	return os;
}

inline
sis& operator>>(sis& is, GUID& guid)
{
	for(siz i = 0; i < guid.size(); ++i)
		is.get(guid[i]);
	return is;
}

typedef std::map<GUID, str> guid_str_map;
typedef std::pair<const GUID, str> guid_str_pair;
typedef std::map<GUID, str>::iterator guid_str_iter;
typedef std::map<GUID, str>::const_iterator guid_str_citer;

typedef std::map<siz, GUID> siz_guid_map;
typedef siz_guid_map::value_type siz_guid_map_pair;
typedef siz_guid_map::iterator siz_guid_map_iter;
typedef siz_guid_map::const_iterator siz_guid_map_citer;

typedef std::map<GUID, siz> guid_siz_map;
typedef guid_siz_map::value_type guid_siz_map_pair;
typedef guid_siz_map::iterator guid_siz_map_iter;
typedef guid_siz_map::const_iterator guid_siz_map_citer;

typedef std::multimap<siz, str> siz_str_mmap;
typedef siz_str_mmap::reverse_iterator siz_str_mmap_ritr;
typedef siz_str_mmap::iterator siz_str_mmap_iter;
typedef siz_str_mmap::const_iterator siz_str_mmap_citer;

typedef std::map<GUID, int> guid_int_map;
typedef std::pair<const GUID, int> guid_int_map_pair;
typedef guid_int_map::iterator guid_int_map_iter;
typedef guid_int_map::const_iterator guid_int_map_citer;

extern const GUID null_guid;

/*
 * Create a GUID for bots based on their slot number
 */
inline GUID bot_guid(siz num)
{
	soss oss;
	oss << num;
	str id = oss.str();
	if(id.size() < GUID::SIZE)
		id = str(GUID::SIZE - id.size(), '0') + id;

	return GUID(id.c_str());
}

} // oastats

#endif /* _OASTATS_GUID_H_ */
