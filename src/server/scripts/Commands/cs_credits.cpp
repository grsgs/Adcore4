/*
 * Copyright (C) 2013 Mist of Depths <http://www.mist-of-depths.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
Name: credits_commandscript
By Oracraft for Mist of Depths
EndScriptData */

#include "ScriptPCH.h"
#include "Chat.h"

// **** Script Info ****
class credits_commandscript: public CommandScript
{
public:
    credits_commandscript () :
            CommandScript("credits_commandscript")
    {
    }

    static bool HandleCreditsCommand (ChatHandler* handler, const char* /*args*/)
    {
        handler->PSendSysMessage("");
        handler->PSendSysMessage("__[Mist of Depths Credits]______________________________________________");
		handler->PSendSysMessage("Mist-of-Depths <http://www.mist-of-depths.com/>");
		handler->PSendSysMessage("");
        handler->PSendSysMessage("Cette commande permet de connaitre toutes les personnes ayant contribuer au projet.");
        handler->PSendSysMessage("Certain sont des projets existant et sont la base de notre projet. ");
        handler->PSendSysMessage("Tous les noms cité ont eu une inflence direct ou indirect a notre projet");
        handler->PSendSysMessage("");
        handler->PSendSysMessage("Voici les personnes ayant contribuer directement a notre projet :");
        handler->PSendSysMessage(" ");
        handler->PSendSysMessage("Merci a toutes les personnes ayant contribuer a ce projet");
		handler->PSendSysMessage("---------------------------------------------------------");
        handler->PSendSysMessage("Fondateur: ");
		handler->PSendSysMessage("");
        handler->PSendSysMessage("Demontpneu, ");
        handler->PSendSysMessage("");
		handler->PSendSysMessage("");
		handler->PSendSysMessage("Developpeur:");
		handler->PSendSysMessage("");
		handler->PSendSysMessage("Demontpneu, Drarkrai, Popol410, ");
		handler->PSendSysMessage("");
		handler->PSendSysMessage("");
		handler->PSendSysMessage("Testeurs Moderateurs:");
		handler->PSendSysMessage("");
		handler->PSendSysMessage("Drarkrai, Popol410, Glaedr, Etxola, Thrylia, Dragonhearts, Vashek, ");
		handler->PSendSysMessage("");
		handler->PSendSysMessage("");
		handler->PSendSysMessage("Nous honorons la memoire de:");
		handler->PSendSysMessage("Oracraft, ");
		handler->PSendSysMessage("");
        handler->PSendSysMessage("We would also like to thank the following Sites/Communities:");
        handler->PSendSysMessage("TrinityCore: http://www.trinitycore.org");
        handler->PSendSysMessage("Mangos: http://www.getmangos.com");
        handler->PSendSysMessage("");
        handler->PSendSysMessage("__[Mist of Depths Credits]______________________________________________");
        return true;
    }

    ChatCommand* GetCommands () const
    {
        static ChatCommand CreditsCommandTable[] =
        {
        { "credits", SEC_PLAYER, true, &HandleCreditsCommand, "", NULL },
        { NULL, 0, false, NULL, "", NULL } };
        return CreditsCommandTable;
    }
};

void AddSC_credits_commandscript ()
{
    new credits_commandscript();
}


