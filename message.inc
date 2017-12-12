#include <YSI\y_va>


stock ChatMsg(playerid, colour, fmat[], {Float,_}:...) {
	new buffer[244];
	format(buffer, sizeof(buffer), fmat, ___(3));
	ChatMsgFlat(playerid, colour, buffer);

	return 1;
}

stock ChatMsgAll(colour, fmat[], {Float,_}:...) {
	new buffer[244];
	format(buffer, sizeof(buffer), fmat, ___(2));
	ChatMsgAllFlat(colour, buffer);

	return 1;
}

stock ChatMsgFlat(playerid, colour, string[]) {
	if(strlen(string) > 127)
	{
		new
			string2[128],
			splitpos;

		for(new c = 128; c > 0; c--)
		{
			if(string[c] == ' ' || string[c] ==  ',' || string[c] ==  '.')
			{
				splitpos = c;
				break;
			}
		}

		strcat(string2, string[splitpos]);
		string[splitpos] = EOS;
		
		SendClientMessage(playerid, colour, string);
		SendClientMessage(playerid, colour, string2);
	}
	else
	{
		SendClientMessage(playerid, colour, string);
	}
	
	return 1;
}

stock ChatMsgAllFlat(colour, string[])
{
	if(strlen(string) > 127)
	{
		new
			string2[128],
			splitpos;

		for(new c = 128; c>0; c--)
		{
			if(string[c] == ' ' || string[c] ==  ',' || string[c] ==  '.')
			{
				splitpos = c;
				break;
			}
		}

		strcat(string2, string[splitpos]);
		string[splitpos] = EOS;

		SendClientMessageToAll(colour, string);
		SendClientMessageToAll(colour, string2);
	}
	else
	{
		SendClientMessageToAll(colour, string);
	}

	return 1;
}

#if defined _language_included

stock ChatMsgLang(playerid, colour, key[], {Float,_}:...) {
	new buffer[244];
	format(buffer, sizeof(buffer), GetLanguageString(GetPlayerLanguage(playerid), key), ___(3));
	ChatMsgFlat(playerid, colour, buffer);

	return 1;
}

stock ChatMsgLangFlat(playerid, colour, key[]) {
	return ChatMsgFlat(playerid, colour, GetLanguageString(GetPlayerLanguage(playerid), key));
}

#endif
