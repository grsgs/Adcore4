/*
 * Copyright (C) 2012-2013 Reign of Darkness Network <http://arkania.net>
 * Copyright (C) 2012-2013 Odysseum <http://www.odysseumserver.com>
 * Copyright (C) 2011-2012 Project SkyFire <http://www.projectskyfire.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
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

#include "ScriptMgr.h"
#include "bastion_of_twilight.h"

enum Yells
{
	SAY_EVENT_INTRO                       = -1210000,
	SAY_EVENT_HALFUS                      = -1210001,
	SAY_EVENT_HALFUS_DEAD                 = -1210002,
	
	SAY_EVENT_TWIN_DRAGONS                = -1210003,
	SAY_EVENT_TWIN_DRAGONS_VALIONA_1      = -1212000,
	SAY_EVENT_TWIN_DRAGONS_THERALION_1    = -1212001,
	SAY_EVENT_TWIN_DRAGONS_VALIONA_2      = -1212002,
	SAY_EVENT_TWIN_DRAGONS_THERALION_2    = -1212003,

	SAY_EVENT_ASCENDANT_COUNCIL           = -1210004,
	SAY_EVENT_ASCENDANT_COUNCIL_DEAD      = -1210005,

	SAY_EVENT_CHOGALL_1                   = -1210006,
	SAY_EVENT_CHOGALL_2                   = -1210007,
};

class npc_chogall_entrance_event : public CreatureScript
{
public:
    npc_chogall_entrance_event() : CreatureScript("npc_chogall_entrance_event") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chogall_entrance_eventAI (creature);
    }

    struct npc_chogall_entrance_eventAI : public ScriptedAI
    {
        npc_chogall_entrance_eventAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
        }

        InstanceScript* instance;
		
		bool IntroActivated;

        void Reset() 
		{
			IntroActivated = false;
		}
		
		void MoveInLineOfSight(Unit* who)
        {
            if (who->GetTypeId() != TYPEID_PLAYER)
                return;

			if (who->GetExactDist(me) <= 5.0f && !IntroActivated)
			{
				IntroActivated = true;
                DoScriptText(SAY_EVENT_INTRO, me);
			}
		}
		
        void UpdateAI( uint32 Diff)
        {
        }
    };
};

class npc_chogall_halfus_event : public CreatureScript
{
public:
    npc_chogall_halfus_event() : CreatureScript("npc_chogall_halfus_event") { }
	
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chogall_halfus_eventAI (creature);
    }
	
    struct npc_chogall_halfus_eventAI : public ScriptedAI
    {
        npc_chogall_halfus_eventAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
        }
		
        InstanceScript* instance;
		
		bool HalfusActivated;
		
        void Reset() 
		{
			HalfusActivated = false;
		}
		
		void MoveInLineOfSight(Unit* who)
        {
            if (who->GetTypeId() != TYPEID_PLAYER)
                return;
			
			if (who->GetExactDist(me) <= 50.0f && !HalfusActivated)
			{
				HalfusActivated = true;
                DoScriptText(SAY_EVENT_HALFUS, me);
			}
		}
		
        void UpdateAI( uint32 Diff)
        {
        }
    };
};

class npc_chogall_halfus_dead_event : public CreatureScript
{
public:
    npc_chogall_halfus_dead_event() : CreatureScript("npc_chogall_halfus_dead_event") { }
	
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chogall_halfus_dead_eventAI (creature);
    }
	
    struct npc_chogall_halfus_dead_eventAI : public ScriptedAI
    {
        npc_chogall_halfus_dead_eventAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
        }
		
        InstanceScript* instance;
		
		uint32 HalfusDead;
		
		bool HalfusDeadActivated;
		
        void Reset() 
		{
			HalfusDead = 5000;
			
			HalfusDeadActivated = false;
		}
		
        void UpdateAI( uint32 diff)
        {
			if (!HalfusDeadActivated)
			{
				if (HalfusDead <= diff)
				{
					DoScriptText(SAY_EVENT_HALFUS_DEAD, me);
					HalfusDeadActivated = true;
					HalfusDead = 100000;
				}
				else 
					HalfusDead -= diff;
			}
        }
    };
};

class npc_chogall_twin_dragons_event : public CreatureScript
{
public:
    npc_chogall_twin_dragons_event() : CreatureScript("npc_chogall_twin_dragons_event") { }
	
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chogall_twin_dragons_eventAI (creature);
    }
	
    struct npc_chogall_twin_dragons_eventAI : public ScriptedAI
    {
        npc_chogall_twin_dragons_eventAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
        }
		
        InstanceScript* instance;
		
		uint32 EventStep;
        uint32 NextEventTimer;
		
		bool TwinDragonsActivated1;
		bool TwinDragonsActivated2;
		
        void Reset() 
		{
			EventStep = 0;
			NextEventTimer = 0;
			
			TwinDragonsActivated1 = false;
			TwinDragonsActivated2 = false;
		}
		
		void NextStep(uint32 uiTime = 1000)
        {
            ++EventStep;
            NextEventTimer = uiTime;
        }
		
		void MoveInLineOfSight(Unit* who)
        {
            if (who->GetTypeId() != TYPEID_PLAYER)
                return;
			
			if (who->GetExactDist(me) <= 50.0f && !TwinDragonsActivated1)
			{
				TwinDragonsActivated1 = true;
                EventStep = 1;
			}
		}
		
        void UpdateAI( uint32 diff)
        {
			if (NextEventTimer <= diff && !TwinDragonsActivated2)
            {
                switch (EventStep)
                {
                    case 0:
						break;
					case 1:
						DoScriptText(SAY_EVENT_TWIN_DRAGONS, me);
						NextStep(6000);
						break;
					case 2:
						if (Creature *valiona = me->FindNearestCreature(NPC_VALIONA, 100000, true))
							DoScriptText(SAY_EVENT_TWIN_DRAGONS_VALIONA_1, valiona);
						
						NextStep(6000);
						break;
					case 3:
						if (Creature *theralion = me->FindNearestCreature(NPC_THERALION, 100000, true))
							DoScriptText(SAY_EVENT_TWIN_DRAGONS_THERALION_1, theralion);
						
						NextStep(8000);
						break;
					case 4:
						if (Creature *valiona = me->FindNearestCreature(NPC_VALIONA, 100000, true))
							DoScriptText(SAY_EVENT_TWIN_DRAGONS_VALIONA_2, valiona);
						
						NextStep(6000);
						break;
					case 5:
						if (Creature *theralion = me->FindNearestCreature(NPC_THERALION, 100000, true))
							DoScriptText(SAY_EVENT_TWIN_DRAGONS_THERALION_2, theralion);
						
						NextStep(8000);
						break;
					case 6:
						TwinDragonsActivated2 = true;
						break;
				}
            }
            else
                NextEventTimer -= diff;
        }
    };
};

class npc_chogall_ascendant_council_event : public CreatureScript
{
public:
    npc_chogall_ascendant_council_event() : CreatureScript("npc_chogall_ascendant_council_event") { }
	
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chogall_ascendant_council_eventAI (creature);
    }
	
    struct npc_chogall_ascendant_council_eventAI : public ScriptedAI
    {
        npc_chogall_ascendant_council_eventAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
        }
		
        InstanceScript* instance;

		bool AscendantCouncilActivated;
		
        void Reset() 
		{
			AscendantCouncilActivated = false;
		}

		void MoveInLineOfSight(Unit* who)
        {
            if (who->GetTypeId() != TYPEID_PLAYER)
                return;
			
			if (who->GetExactDist(me) <= 50.0f && !AscendantCouncilActivated)
			{
				AscendantCouncilActivated = true;
                DoScriptText(SAY_EVENT_ASCENDANT_COUNCIL, me);
			}
		}
		
        void UpdateAI( uint32 diff)
        {
        }
    };
};

class npc_chogall_ascendant_council_dead_event : public CreatureScript
{
public:
    npc_chogall_ascendant_council_dead_event() : CreatureScript("npc_chogall_ascendant_council_dead_event") { }
	
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chogall_ascendant_council_dead_eventAI (creature);
    }
	
    struct npc_chogall_ascendant_council_dead_eventAI : public ScriptedAI
    {
        npc_chogall_ascendant_council_dead_eventAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
        }
		
        InstanceScript* instance;
		
		uint32 AscendantCouncilDead;
		
		bool AscendantCouncilDeadActivated;
		
        void Reset() 
		{
			AscendantCouncilDead = 6000;
			
			AscendantCouncilDeadActivated = false;
		}
		
        void UpdateAI( uint32 diff)
        {
			if (!AscendantCouncilDeadActivated)
			{
				if (AscendantCouncilDead <= diff)
				{
					DoScriptText(SAY_EVENT_ASCENDANT_COUNCIL_DEAD, me);
					AscendantCouncilDeadActivated = true;
					AscendantCouncilDead = 100000;
				}
				else 
					AscendantCouncilDead -= diff;
			}
        }
    };
};

class npc_chogall_intro_event : public CreatureScript
{
public:
    npc_chogall_intro_event() : CreatureScript("npc_chogall_intro_event") { }
	
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chogall_intro_eventAI (creature);
    }
	
    struct npc_chogall_intro_eventAI : public ScriptedAI
    {
        npc_chogall_intro_eventAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
        }
		
        InstanceScript* instance;

		bool ChogallActivated1;
		bool ChogallActivated2;

		uint32 EventStep;
		uint32 NextEventTimer;
		
        void Reset() 
		{
			ChogallActivated1 = false;
			ChogallActivated2 = false;

			EventStep = 0;
			NextEventTimer = 0;
		}

		void NextStep(uint32 uiTime = 1000)
        {
            ++EventStep;
            NextEventTimer = uiTime;
        }

		void MoveInLineOfSight(Unit* who)
        {
            if (who->GetTypeId() != TYPEID_PLAYER)
                return;
			
			if (who->GetExactDist(me) <= 50.0f && !ChogallActivated1)
			{
				ChogallActivated1 = true;
				EventStep = 1;
			}
		}
		
        void UpdateAI( uint32 diff)
        {
			if (NextEventTimer <= diff && !ChogallActivated2)
            {
                switch (EventStep)
                {
                    case 0:
						break;
					case 1:
						DoScriptText(SAY_EVENT_CHOGALL_1, me);
						NextStep(15000);
						break;
					case 2:
					    DoScriptText(SAY_EVENT_CHOGALL_2, me);
					    ChogallActivated2 = true;
					    EventStep = 0;
						break;
				}
            }
            else
                NextEventTimer -= diff;
        }
    };
};

void AddSC_bastion_of_twilight()
{
    new npc_chogall_entrance_event();
	new npc_chogall_halfus_event();
	new npc_chogall_halfus_dead_event();
	new npc_chogall_twin_dragons_event();
	new npc_chogall_ascendant_council_event();
	new npc_chogall_ascendant_council_dead_event();
	new npc_chogall_intro_event();
}