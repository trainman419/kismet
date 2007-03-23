/*
    This file is part of Kismet

    Kismet is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Kismet is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Kismet; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __KISCLINETFRAME_H__
#define __KISCLINETFRAME_H__

#include "config.h"

#include "util.h"
#include "messagebus.h"
#include "clinetframework.h"
#include "tcpclient.h"

class KisNetClient;

#define CLIPROTO_CB_PARMS	GlobalRegistry *globalreg, string proto_string, \
	vector<smart_word_token> *proto_parsed, KisNetClient *srccli, void *auxptr
typedef void (*CliProto_Callback)(CLIPROTO_CB_PARMS);

#define CLICONF_CB_PARMS	GlobalRegistry *globalreg, KisNetClient *kcli, \
	int recon, void *auxptr
typedef void (*CliConf_Callback)(CLICONF_CB_PARMS);

class KisNetClient : public ClientFramework {
public:
	KisNetClient();
	KisNetClient(GlobalRegistry *in_globalreg);
	virtual ~KisNetClient();

	// Connect to a server string, proto://host:port
	virtual int Connect(string in_host, int in_reconnect);
	virtual string FetchHost() { return host; }
	virtual int FetchPort() { return port; }

	virtual void AddConfCallback(CliConf_Callback in_cb, int in_recon, void *in_aux);
	virtual void RemoveConfCallback(CliConf_Callback in_cb);
	virtual int FetchConfigured() { return configured; }

	virtual unsigned int MergeSet(unsigned int in_max_fd, fd_set *out_rset,
								  fd_set *out_wset) {
		return netclient->MergeSet(in_max_fd, out_rset, out_wset);
	}

	virtual int Poll(fd_set &in_rset, fd_set &in_wset) {
		return netclient->Poll(in_rset, in_wset);
	}

	virtual int ParseData();
	virtual int KillConnection();

	virtual int Shutdown();

	// Register a handler for a protocol.  There can be multiple handlers.
	virtual int RegisterProtoHandler(string in_proto, string in_fieldlist,
									 CliProto_Callback in_cb, void *in_aux);
	virtual void RemoveProtoHandler(string in_proto, CliProto_Callback in_cb,
									void *in_aux);

	// Grab the list of fields we know about for a proto so clients can
	// request what they want
	virtual int FetchProtoCapabilities(string in_proto, 
									   map<string, int> *ret_fields);

	virtual int InjectCommand(string in_cmdtext);

	virtual int Reconnect();

	virtual string FetchServerName() {
		return server_name;
	}

	virtual int FetchServerUid() {
		return server_uid;
	}

	virtual time_t FetchServerStarttime() {
		return server_starttime;
	}

	// Internal callback handler record
	typedef struct kcli_handler_rec {
		void *auxptr;
		CliProto_Callback callback;
		string fields;
	};

	// Internal conf cb record
	typedef struct kcli_conf_rec {
		void *auxptr;
		CliConf_Callback callback;
		int on_recon;
	};

protected:
	TcpClient *tcpcli;

	char host[MAXHOSTNAMELEN];
	short int port;

	// Callbacks to call when we get configured
	vector<kcli_conf_rec *> conf_cb_vec;

	// Double map of protocols and fields in them, filled in at connection time
	map<string, map<string, int> > proto_field_dmap;

	// Map of protocols to handlers
	map<string, vector<kcli_handler_rec *> > handler_cb_map;

	int reconnect;
	int reconid;
	
	// Have we gotten configure data for everything?
	int configured;

	int cmdid;
	time_t last_disconnect;
	time_t time_connected;
	int num_reconnects;

	time_t last_time;

	// Server start time & uid and such
	time_t server_starttime;
	string server_name;
	int server_uid;
};

#endif

