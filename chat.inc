// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_chat
    #undef _inc_chat
#endif
// custom include-guard to ensure we don't duplicate
#if defined _chat_included
	#endinput
#endif
#define _chat_included

#include <a_samp>
#include <YSI\y_colours>
#include <YSI\y_iterate>
#include <logger>
#include <tick-difference>
#include <mentions>
#include <language>
#include <timeutil>

#include "mute.inc"
#include "message.inc"

#include <YSI\y_hooks>


#define COLOUR_CHAT_LOCAL 0xADABD1FF
#define COLOUR_CHAT_RADIO 0xCFD1ABFF
#define C_SPECIAL "{0025AA}"

enum CHAT_MODE {
	CHAT_MODE_LOCAL,	// Speak to players within chatbubble distance
	CHAT_MODE_ACTION,	// Perform /me actions locally
	CHAT_MODE_GLOBAL,	// Speak to all players
	CHAT_MODE_RADIO,	// Speak to players on the same radio frequency
	// todo: remove
//	CHAT_MODE_ADMIN		// Speak to admins
}

static
	CHAT_MODE:chat_Mode[MAX_PLAYERS],
	Float:chat_Freq[MAX_PLAYERS],
	bool:chat_Quiet[MAX_PLAYERS],
	chat_MessageStreak[MAX_PLAYERS],
	chat_LastMessageTick[MAX_PLAYERS];

forward Float:GetPlayerRadioFrequency(playerid);
forward OnPlayerSendChat(playerid, text[], Float:frequency);

hook OnPlayerConnect(playerid) {
	chat_LastMessageTick[playerid] = 0;
	return 1;
}

hook OnPlayerText(playerid, text[]) {
	if(IsPlayerMuted(playerid)) {
		if(GetPlayerMuteRemainder(playerid) == -1) {
			ChatMsgLang(playerid, X11_RED, "MUTEDPERMAN");
		} else {
			ChatMsgLang(playerid, X11_RED, "MUTEDTIMERM", MsToString(GetPlayerMuteRemainder(playerid) * 1000, "%1h:%1m:%1s"));
		}
		return Y_HOOKS_BREAK_RETURN_0;
	} else {
		if(GetTickCountDifference(GetTickCount(), chat_LastMessageTick[playerid]) < 1000) {
			chat_MessageStreak[playerid]++;
			if(chat_MessageStreak[playerid] == 3) {
				TogglePlayerMute(playerid, true, 30);
				ChatMsgLang(playerid, X11_RED, "MUTEDFLOODM");
				return Y_HOOKS_BREAK_RETURN_0;
			}
		} else {
			if(chat_MessageStreak[playerid] > 0) {
				chat_MessageStreak[playerid]--;
			}
		}
	}

	chat_LastMessageTick[playerid] = GetTickCount();

	PlayerSendChat(playerid, text, chat_Mode[playerid], chat_Freq[playerid]);

	return 0;
}

PlayerSendChat(playerid, message[], CHAT_MODE:mode, Float:frequency) {
	if(isnull(message)) {
		return 1;
	}

	if(CallLocalFunction("OnPlayerSendChat", "dsf", playerid, message, frequency)) {
		return 2;
	}

	new
		processedMessage[256],
		line1[256],
		line2[128];

	ExpandMentions(message, processedMessage, 256, X11_SNOW);

	if(mode == CHAT_MODE_LOCAL) {
		log("CHAT_LOCAL",
			_i("playerid", playerid),
			_s("message", processedMessage));

		new
			Float:x,
			Float:y,
			Float:z;

		GetPlayerPos(playerid, x, y, z);

		format(line1, 256, "[Local] (%d) %P"SNOW": %s",
			playerid,
			playerid,
			processedMessage);

		_chat_truncate(line1, line2);

		foreach(new i : Player) {
			if(IsPlayerInRangeOfPoint(i, 40.0, x, y, z)) {
				SendClientMessage(i, COLOUR_CHAT_LOCAL, line1);

				if(!isnull(line2))
					SendClientMessage(i, COLOUR_CHAT_LOCAL, line2);
			}
		}

		SetPlayerChatBubble(playerid, processedMessage, X11_SNOW, 40.0, 10000);
	} else if(mode == CHAT_MODE_GLOBAL) {
		log("CHAT_GLOBAL",
			_i("playerid", playerid),
			_s("message", processedMessage));

		format(line1, 256, "[Global] (%d) %P"SNOW": %s",
			playerid,
			playerid,
			processedMessage);

		_chat_truncate(line1, line2);

		foreach(new i : Player) {
			if(chat_Quiet[i])
				continue;

			SendClientMessage(i, X11_SNOW, line1);

			if(!isnull(line2))
				SendClientMessage(i, X11_SNOW, line2);
		}

		SetPlayerChatBubble(playerid, processedMessage, X11_SNOW, 40.0, 10000);
	} else if(mode == CHAT_MODE_ACTION) {
		log("CHAT_ACTION",
			_i("playerid", playerid),
			_s("message", processedMessage));

		new
			Float:x,
			Float:y,
			Float:z;

		GetPlayerPos(playerid, x, y, z);

		format(line1, 256, "[Local] %P %s",
			playerid,
			processedMessage);

		_chat_truncate(line1, line2);

		foreach(new i : Player) {
			if(IsPlayerInRangeOfPoint(i, 40.0, x, y, z)) {
				SendClientMessage(i, COLOUR_CHAT_LOCAL, line1);

				if(!isnull(line2))
					SendClientMessage(i, COLOUR_CHAT_LOCAL, line2);
			}
		}

		SetPlayerChatBubble(playerid, processedMessage, COLOUR_CHAT_LOCAL, 40.0, 10000);
	}
	/*else if(mode == CHAT_MODE_ADMIN) {
		// log("CHAT_ADMIN",
		// 	_i("playerid", playerid),
		// 	_s("message", message));

		// format(line1, 256, "%C[Admin] (%d) %P"SNOW": %s",
		// 	GetAdminRankColour(GetPlayerAdminLevel(playerid)),
		// 	playerid,
		// 	playerid,
		// 	processedMessage);

		// _chat_truncate(line1, line2);

		// foreach(new i : Player) {
		// 	// todo: import admin module
		// 	// if(GetPlayerAdminLevel(i) > 0) {
		// 	// 	SendClientMessage(i, COLOUR_CHAT_LOCAL, line1);

		// 	// 	if(!isnull(line2))
		// 	// 		SendClientMessage(i, COLOUR_CHAT_LOCAL, line2);
		// 	// }
		// }
	}
	*/
	else if(mode == CHAT_MODE_RADIO) {
		log("CHAT_RADIO",
			_i("playerid", playerid),
			_s("message", processedMessage));

		format(line1, 256, "[%.2f] (%d) %P"SNOW": %s",
			frequency,
			playerid,
			playerid,
			processedMessage);

		_chat_truncate(line1, line2);

		foreach(new i : Player) {
			if(-0.05 < frequency - chat_Freq[i] < 0.05) {
				SendClientMessage(i, COLOUR_CHAT_RADIO, line1);

				if(!isnull(line2))
					SendClientMessage(i, COLOUR_CHAT_RADIO, line2);
			}
		}

		SetPlayerChatBubble(playerid, processedMessage, X11_SNOW, 40.0, 10000);
	}

	return 0;
}

// truncate takes a long string as full, finds a good point to split it based on punctuation or
// whitespace then chops `full` at that point and copies the rest into `line2`.
// returns 1 if it performed the operation and 0 if it didn't need to due to `full` being too short.
_chat_truncate(full[], line2[]) {
	if(strlen(full) > 127) {
		new splitpos;

		// step backwards from the middle of the string, looking for whitespace, capped at 80
		for(new i = 128; i > 0; i--) {
			if(
				full[i] == ' ' ||
				full[i] == ',' ||
				full[i] == '.' ||
				i == 80) {
				splitpos = i;
				break;
			}
		}

		strcat(line2, full[splitpos], 128);
		full[splitpos] = 0;

		return 1;
	}

	return 0;
}

stock GetPlayerChatMode(playerid) {
	if(!IsPlayerConnected(playerid))
		return 0;

	return chat_Mode[playerid];
}

stock SetPlayerChatMode(playerid, chatmode) {
	if(!IsPlayerConnected(playerid))
		return 0;

	chat_Mode[playerid] = chatmode;

	return 1;
}

stock IsPlayerGlobalQuiet(playerid) {
	if(!IsPlayerConnected(playerid))
		return 0;

	return chat_Quiet[playerid];
}

stock Float:GetPlayerRadioFrequency(playerid) {
	if(!IsPlayerConnected(playerid))
		return 0.0;

	return chat_Freq[playerid];
}
stock SetPlayerRadioFrequency(playerid, Float:frequency) {
	if(!IsPlayerConnected(playerid))
		return 0;

	chat_Freq[playerid] = frequency;

	return 1;
}
