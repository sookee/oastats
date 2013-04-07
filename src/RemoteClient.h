#pragma once
#ifndef _OASTATS_REMOTECLIENT_H_
#define _OASTATS_REMOTECLIENT_H_
/*
 * RemoteClient.h
 *
 *  Created on: Apr 7, 2013
 *      Author: oasookee@gmail.com
 */

#include "types.h"
#include "socketstream.h"
#include "log.h"
#include "irc.h"

namespace oastats { namespace net {

using namespace oastats::irc;
using namespace oastats::log;
using namespace oastats::types;

class RemoteClient
{
protected:
	bool active;
	str host;
	siz port;

	typedef std::map<str, std::set<char> > chan_map;
	typedef chan_map::iterator chan_map_iter;
	typedef chan_map::const_iterator chan_map_citer;

	chan_map chans; // #channel -> {'c','f','k'}

public:
	RemoteClient(): active(false), host("localhost"), port(7334) {}
	virtual ~RemoteClient() {}

	void on() { active = true; }
	void off() { active = false; }

	void config(const str& host, siz port)
	{
		this->host = host;
		this->port = port;
	}

	void set_chans(const str& chans);
	bool say(char f, const str& text);

	void add_flag(const str& chan, char f) { chans[chan].insert(f); }
	void del_flag(const str& chan, char f) { chans[chan].erase(f); }
	void set_flags(const str& chan, const str& flags)
	{
		for(siz i = 0; i < flags.size(); ++i)
			add_flag(chan, flags[i]);
	}

	void clear_flags()
	{
		for(chan_map_iter chan = chans.begin(); chan != chans.end(); ++chan)
			chan->second.clear();
	}
	void clear_flags(const str& chan) { chans[chan].clear(); }

	bool chat(char f, const str& text) { return say(f, oa_to_IRC(text)); }

	/**
	 * Implementing classes need to override this function.
	 * @param msg The message being sent.
	 * @param res The response from the remote client.
	 * @return false on communications failure.
	 */
	virtual bool send(const str& msg, str& res) = 0;
};

class SkivvyClient
: public RemoteClient
{
	net::socketstream ss;

public:

	// RemoteClient Interface

	virtual bool send(const str& cmd, str& res)
	{
		if(!active)
			return true;

		if(!ss.open(host, port))
		{
			log("error: " << std::strerror(errno));
			return false;
		}
		(ss << cmd).put('\0') << std::flush;
		return std::getline(ss, res, '\0');
	}
};

}} // oastats::net

#endif /* _OASTATS_REMOTECLIENT_H_ */
