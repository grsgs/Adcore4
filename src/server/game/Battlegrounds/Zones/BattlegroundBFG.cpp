/*
 * Copyright (C) 2011-2013 DepthsCore <http://www.mist-of-depths.com/>
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

#include "BattlegroundBFG.h"
#include "World.h"
#include "WorldPacket.h"
#include "ObjectMgr.h"
#include "BattlegroundMgr.h"
#include "Creature.h"
#include "Language.h"
#include "Object.h"
#include "Player.h"
#include "Util.h"
#include "WorldSession.h"
#include "Battleground.h"
#include "GameObject.h"


uint32 GILNEAS_BGm_HonorScoreTicks[BG_HONOR_MODE_NUM] =
{
    330, // normal honor
    200  // holiday
};

uint32 GILNEAS_BG_Reputation[BG_HONOR_MODE_NUM][GILNEAS_BG_REWARD_NUM] =
{
    200, // normal honor
    150  // holiday
};

BattlegroundBFG::BattlegroundBFG()
{
    m_BuffChange = true;
    BgObjects.resize(GILNEAS_BG_OBJECT_MAX);
    BgCreatures.resize(GILNEAS_BG_ALL_NODES_COUNT + 5); // +5 for aura triggers

    StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_BG_START_TWO_MINUTES;
    StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_BG_START_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_BG_START_HALF_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_BG_HAS_BEGUN;
}

BattlegroundBFG::~BattlegroundBFG() {}

void BattlegroundBFG::Update(uint32 diff) // PostUpdateImpl...
{
    Battleground::Update(diff);

    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        int team_points[BG_TEAMS_COUNT] = { 0, 0 };

        for (int node = 0; node < GILNEAS_BG_DYNAMIC_NODES_COUNT; ++node)
        {
            // 3sec delay to spawn new a banner instead previous despawned one.
            if (m_BannerTimers[node].timer)
            {
                if (m_BannerTimers[node].timer > diff)
                    m_BannerTimers[node].timer -= diff;
                else
                {
                    m_BannerTimers[node].timer = 0;
                    _CreateBanner(node, m_BannerTimers[node].type, m_BannerTimers[node].teamIndex, false);
                }
            }
            // 1 minute cap timer on each node from a contested state.
            if (m_NodeTimers[node])
            {
                if (m_NodeTimers[node] > diff)
                    m_NodeTimers[node] -= diff;
                else
                {
                    m_NodeTimers[node] = 0;

                    // Change from contested to occupied !
                    uint8 teamIndex = m_Nodes[node]-1;
                    m_prevNodes[node] = m_Nodes[node];
                    m_Nodes[node] += 2;

                    // burn current contested banner
                    _DelBanner(node, GILNEAS_BG_NODE_TYPE_CONTESTED, teamIndex);

                    // create new occupied banner
                    _CreateBanner(node, GILNEAS_BG_NODE_TYPE_OCCUPIED, teamIndex, true);
                    _SendNodeUpdate(node);
                    _NodeOccupied(node, (teamIndex == 0) ? ALLIANCE:HORDE);

                    // Message to chatlog
                    if (teamIndex == 0)
                    {
                        // FIXME: need to fix Locales for team and nodes names.
                        SendMessage2ToAll(LANG_BG_BG_NODE_TAKEN, CHAT_MSG_BG_SYSTEM_ALLIANCE, NULL, LANG_BG_BG_ALLY, _GetNodeNameId(node));
                        PlaySoundToAll(GILNEAS_BG_SOUND_NODE_CAPTURED_ALLIANCE);
                    }
                    else
                    {
                        // FIXME: team and node names not localized
                        SendMessage2ToAll(LANG_BG_BG_NODE_TAKEN, CHAT_MSG_BG_SYSTEM_HORDE, NULL, LANG_BG_BG_HORDE, _GetNodeNameId(node));
                        PlaySoundToAll(GILNEAS_BG_SOUND_NODE_CAPTURED_HORDE);
                    }
                }
            }

            for (int team = 0; team < BG_TEAMS_COUNT; ++team)
                if (m_Nodes[node] == team + GILNEAS_BG_NODE_TYPE_OCCUPIED)
                    ++team_points[team];
        }

        // Accumulate points
        for (int team = 0; team < BG_TEAMS_COUNT; ++team)
        {
            int points = team_points[team];
            if (!points)
                continue;

            m_lastTick[team] += diff;
            if (m_lastTick[team] > GILNEAS_BG_TickIntervals[points])
            {
                m_lastTick[team] -= GILNEAS_BG_TickIntervals[points];
                m_TeamScores[team] += GILNEAS_BG_TickPoints[points];
                m_HonorScoreTicks[team] += GILNEAS_BG_TickPoints[points];
                m_ReputationScoreTicks[team] += GILNEAS_BG_TickPoints[points];

                if (m_ReputationScoreTicks[team] >= m_ReputationTicks)
                {
                    (team == TEAM_ALLIANCE) ? RewardReputationToTeam(509, 10, ALLIANCE) : RewardReputationToTeam(510, 10, HORDE);
                    m_ReputationScoreTicks[team] -= m_ReputationTicks;
                }

                if (m_HonorScoreTicks[team] >= m_HonorTicks)
                {
                    RewardHonorToTeam(GetBonusHonorFromKill(1), (team == TEAM_ALLIANCE) ? ALLIANCE : HORDE);
                    m_HonorScoreTicks[team] -= m_HonorTicks;
                }

                if (!m_IsInformedNearVictory && m_TeamScores[team] > GILNEAS_BG_WARNING_NEAR_VICTORY_SCORE)
                {
                    if (team == TEAM_ALLIANCE)
                        SendMessageToAll(LANG_BG_BG_A_NEAR_VICTORY, CHAT_MSG_BG_SYSTEM_NEUTRAL);
                    else
                        SendMessageToAll(LANG_BG_BG_H_NEAR_VICTORY, CHAT_MSG_BG_SYSTEM_NEUTRAL);

                    PlaySoundToAll(GILNEAS_BG_SOUND_NEAR_VICTORY);
                    m_IsInformedNearVictory = true;
                }

                if (m_TeamScores[team] > GILNEAS_BG_MAX_TEAM_SCORE)
                    m_TeamScores[team] = GILNEAS_BG_MAX_TEAM_SCORE;

                if (team == TEAM_ALLIANCE)
                    UpdateWorldState(GILNEAS_BG_OP_RESOURCES_ALLY, m_TeamScores[team]);

                if (team == TEAM_HORDE)
                    UpdateWorldState(GILNEAS_BG_OP_RESOURCES_HORDE, m_TeamScores[team]);
                // update achievement flags
                // we increased m_TeamScores[team] so we just need to check if it is 500 more than other teams resources
                uint8 otherTeam = (team + 1) % BG_TEAMS_COUNT;
                if (m_TeamScores[team] > m_TeamScores[otherTeam] + 500)
                    m_TeamScores500Disadvantage[otherTeam] = true;
            }
        }

        // Test win condition
        if (m_TeamScores[TEAM_ALLIANCE] >= GILNEAS_BG_MAX_TEAM_SCORE)
            EndBattleground(ALLIANCE);

        if (m_TeamScores[TEAM_HORDE] >= GILNEAS_BG_MAX_TEAM_SCORE)
            EndBattleground(HORDE);
    }
}

void BattlegroundBFG::StartingEventCloseDoors()
{
    // Remove banners, auras and buffs
    for (int object = GILNEAS_BG_OBJECT_BANNER_NEUTRAL; object < GILNEAS_BG_DYNAMIC_NODES_COUNT * 8; ++object)
        SpawnBGObject(object, RESPAWN_ONE_DAY);
    for (int i = 0; i < GILNEAS_BG_DYNAMIC_NODES_COUNT * 3; ++i)
        SpawnBGObject(GILNEAS_BG_OBJECT_SPEEDBUFF_LIGHTHOUSE + i, RESPAWN_ONE_DAY);

    // Starting doors
    DoorClose(GILNEAS_BG_OBJECT_GATE_A);
    DoorClose(GILNEAS_BG_OBJECT_GATE_H);
    SpawnBGObject(GILNEAS_BG_OBJECT_GATE_A, RESPAWN_IMMEDIATELY);
    SpawnBGObject(GILNEAS_BG_OBJECT_GATE_H, RESPAWN_IMMEDIATELY);

    // Starting base spirit guides
    _NodeOccupied(GILNEAS_BG_SPIRIT_ALIANCE, ALLIANCE);
    _NodeOccupied(GILNEAS_BG_SPIRIT_HORDE, HORDE);
}

void BattlegroundBFG::StartingEventOpenDoors()
{
    // Spawn neutral banners
    for (int banner = GILNEAS_BG_OBJECT_BANNER_NEUTRAL, i = 0; i < 5; banner += 8, ++i)
        SpawnBGObject(banner, RESPAWN_IMMEDIATELY);
    for (int i = 0; i < GILNEAS_BG_DYNAMIC_NODES_COUNT; ++i)
    {
        // Randomly select buff to spawn
        uint8 buff = urand(0, 2);
        SpawnBGObject(GILNEAS_BG_OBJECT_SPEEDBUFF_LIGHTHOUSE + buff + i * 3, RESPAWN_IMMEDIATELY);
    }
    DoorOpen(GILNEAS_BG_OBJECT_GATE_A);
    DoorOpen(GILNEAS_BG_OBJECT_GATE_H);
}

void BattlegroundBFG::AddPlayer(Player* player)
{
    Battleground::AddPlayer(player);
    // Create score and add it to map, default values are set in constructor
    BattlegroundBFGScore* score = new BattlegroundBFGScore;

    PlayerScores[player->GetGUID()] = score;
}

void BattlegroundBFG::RemovePlayer(Player* /*player*/, uint64 /*guid*/, uint32 /*team*/) {}

void BattlegroundBFG::HandleAreaTrigger(Player* Source, uint32 Trigger)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
    //Area triggers are not handled yet!
    return;

    switch(Trigger)
    {
        case 3866:                                          // Lighthouse
        case 3869:                                          // Waterworks
        case 3867:                                          // Mine
        case 4020:                                          // Unk1
        case 4021:                                          // Unk2
            //break;
        default:
            //TC_LOG_ERROR(LOG_FILTER_SQL,"WARNING: Unhandled AreaTrigger in Battleground: %u", Trigger);
            //Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
            break;
    }
}

/* type: 0-neutral, 1-contested, 3-occupied
    teamIndex: 0-ally, 1-horde */
void BattlegroundBFG::_CreateBanner(uint8 node, uint8 type, uint8 teamIndex, bool delay)
{
    // Just put it into the queue
    if (delay)
    {
        m_BannerTimers[node].timer       = 2000;
        m_BannerTimers[node].type        = type;
        m_BannerTimers[node].teamIndex   = teamIndex;
        return;
    }

    uint8 object = node*8 + type + teamIndex;

    SpawnBGObject(object, RESPAWN_IMMEDIATELY);

    // Handle banner and auras
    if (!type)
        return;

    object = node * 8 + ((type == GILNEAS_BG_NODE_TYPE_OCCUPIED) ? (5 + teamIndex) : 7);
    SpawnBGObject(object, RESPAWN_IMMEDIATELY);
}

void BattlegroundBFG::_DelBanner(uint8 node, uint8 type, uint8 teamIndex)
{
    uint8 object = node*8 + type + teamIndex;
    SpawnBGObject(object, RESPAWN_ONE_DAY);

    // Handle banner and auras
    if (!type)
        return;

    object = node * 8 + ((type == GILNEAS_BG_NODE_TYPE_OCCUPIED) ? (5 + teamIndex) : 7);
    SpawnBGObject(object, RESPAWN_ONE_DAY);
}

int32 BattlegroundBFG::_GetNodeNameId(uint8 node)
{
    switch (node)
    {
        case GILNEAS_BG_NODE_WATERWORKS: return LANG_BG_BG_NODE_WATERWORKS;
        case GILNEAS_BG_NODE_LIGHTHOUSE: return LANG_BG_BG_NODE_LIGHTHOUSE;
        case GILNEAS_BG_NODE_MINE:       return LANG_BG_BG_NODE_MINE;
        default:
            ASSERT(0);
    }
    return 0;
}

void BattlegroundBFG::FillInitialWorldStates(WorldPacket& data)
{
    const uint8 plusArray[] = { 0, 2, 3, 0, 1 };

    // Node icons
    for (uint8 node = 0; node < GILNEAS_BG_DYNAMIC_NODES_COUNT; ++node)
        data << uint32(GILNEAS_BG_OP_NODEICONS[node]) << uint32((m_Nodes[node] == 0)?1:0);

    // Node occupied states
    for (uint8 node = 0; node < GILNEAS_BG_DYNAMIC_NODES_COUNT; ++node)
        for (uint8 i = 1; i < GILNEAS_BG_DYNAMIC_NODES_COUNT; ++i)
            data << uint32(GILNEAS_BG_OP_NODESTATES[node] + plusArray[i]) << uint32((m_Nodes[node] == i)?1:0);

    // How many bases each team owns
    uint8 ally = 0, horde = 0;
    for (uint8 node = 0; node < GILNEAS_BG_DYNAMIC_NODES_COUNT; ++node)
        if (m_Nodes[node] == GILNEAS_BG_NODE_STATUS_ALLY_OCCUPIED)
            ++ally;
        else if (m_Nodes[node] == GILNEAS_BG_NODE_STATUS_HORDE_OCCUPIED)
            ++horde;

    data << uint32(GILNEAS_BG_OP_OCCUPIED_BASES_ALLY)  << uint32(ally);
    data << uint32(GILNEAS_BG_OP_OCCUPIED_BASES_HORDE) << uint32(horde);

    // Team scores
    data << uint32(GILNEAS_BG_OP_RESOURCES_MAX)      << uint32(GILNEAS_BG_MAX_TEAM_SCORE);
    data << uint32(GILNEAS_BG_OP_RESOURCES_WARNING)  << uint32(GILNEAS_BG_WARNING_NEAR_VICTORY_SCORE);
    data << uint32(GILNEAS_BG_OP_RESOURCES_ALLY)     << uint32(m_TeamScores[TEAM_ALLIANCE]);
    data << uint32(GILNEAS_BG_OP_RESOURCES_HORDE)    << uint32(m_TeamScores[TEAM_HORDE]);

    // other unknown
    data << uint32(0x745) << uint32(0x2);           // 37 1861 unk
}

void BattlegroundBFG::_SendNodeUpdate(uint8 node)
{
    // Send node owner state update to refresh map icons on client.
    const uint8 plusArray[] = { 0, 2, 3, 0, 1 };

    if (m_prevNodes[node])
        UpdateWorldState(GILNEAS_BG_OP_NODESTATES[node] + plusArray[m_prevNodes[node]], 0);
    else
        UpdateWorldState(GILNEAS_BG_OP_NODEICONS[node], 0);

    UpdateWorldState(GILNEAS_BG_OP_NODESTATES[node] + plusArray[m_Nodes[node]], 1);

    // How many bases each team owns
    uint8 ally = 0, horde = 0;
    for (uint8 i = 0; i < GILNEAS_BG_DYNAMIC_NODES_COUNT; ++i)
        if (m_Nodes[i] == GILNEAS_BG_NODE_STATUS_ALLY_OCCUPIED)
            ++ally;
        else if (m_Nodes[i] == GILNEAS_BG_NODE_STATUS_HORDE_OCCUPIED)
            ++horde;

    UpdateWorldState(GILNEAS_BG_OP_OCCUPIED_BASES_ALLY, ally);
    UpdateWorldState(GILNEAS_BG_OP_OCCUPIED_BASES_HORDE, horde);
}

void BattlegroundBFG::_NodeOccupied(uint8 node, Team team)
{
    if (!AddSpiritGuide(node, GILNEAS_BG_SpiritGuidePos[node][0], GILNEAS_BG_SpiritGuidePos[node][1], GILNEAS_BG_SpiritGuidePos[node][2], GILNEAS_BG_SpiritGuidePos[node][3], team))
        TC_LOG_ERROR(LOG_FILTER_SQL,"Failed to spawn spirit guide! point: %u, team: %u, ", node, team);

    uint8 capturedNodes = 0;
    for (uint8 i = 0; i < GILNEAS_BG_DYNAMIC_NODES_COUNT; ++i)
    {
        if (m_Nodes[node] == GetTeamIndexByTeamId(team) + GILNEAS_BG_NODE_TYPE_OCCUPIED && !m_NodeTimers[i])
            ++capturedNodes;
    }
    // Todo finish me
    //if (capturedNodes >= 5)
        //CastSpellOnTeam(SPELL_BG_QUEST_REWARD_5_BASES, team);
    //if (capturedNodes >= 4)
        //CastSpellOnTeam(SPELL_BG_QUEST_REWARD_4_BASES, team);

    if (node >= GILNEAS_BG_DYNAMIC_NODES_COUNT) // only dynamic nodes, no start points
        return;

    Creature* trigger = GetBGCreature(node+7); // 0-6 spirit guides

    if (!trigger)
       trigger = AddCreature(WORLD_TRIGGER, node+7, team, GILNEAS_BG_NodePositions[node][0], GILNEAS_BG_NodePositions[node][1], GILNEAS_BG_NodePositions[node][2], GILNEAS_BG_NodePositions[node][3]);

    // Add bonus honor aura trigger creature when node is occupied
    // Cast bonus aura (+50% honor in 25yards)
    // aura should only apply to players who have occupied the node, set correct faction for trigger
    if (trigger)
    {
        trigger->setFaction(team == ALLIANCE ? 84 : 83);
        trigger->CastSpell(trigger, SPELL_HONORABLE_DEFENDER_25Y, false);
    }
}

void BattlegroundBFG::_NodeDeOccupied(uint8 node)
{
    if (node >= GILNEAS_BG_DYNAMIC_NODES_COUNT)
        return;

    // Remove bonus honor aura trigger bunny when node is lost
    if (node < GILNEAS_BG_DYNAMIC_NODES_COUNT)  // Only dynamic nodes, no start points
        DelCreature(node+7);                    // NULL checks are in DelCreature! 0-6 spirit guides

    // Players waiting to resurrect at this node are sent to closest owned graveyard
    std::vector<uint64> ghost_list = m_ReviveQueue[BgCreatures[node]];
    if (!ghost_list.empty())
    {
        WorldSafeLocsEntry const* ClosestGrave = NULL;
        for (std::vector<uint64>::const_iterator itr = ghost_list.begin(); itr != ghost_list.end(); ++itr)
        {
            Player* player = ObjectAccessor::FindPlayer(*itr);
            if (!player)
                continue;

            if (!ClosestGrave)
                ClosestGrave = GetClosestGraveYard(player);

            if (ClosestGrave)
                player->TeleportTo(GetMapId(), ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, player->GetOrientation());
        }
    }

    if (BgCreatures[node])
        DelCreature(node);

    // Buff object is not removed
}

/* Invoked if a player used a banner as a GameObject */
void BattlegroundBFG::EventPlayerClickedOnFlag(Player* source, GameObject* /*target_obj*/)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    uint8 node = GILNEAS_BG_NODE_LIGHTHOUSE;
    GameObject* object = GetBgMap()->GetGameObject(BgObjects[node*8+7]);
    while ((node < GILNEAS_BG_DYNAMIC_NODES_COUNT) && ((!object) || (!source->IsWithinDistInMap(object, 10))))
    {
        ++node;
        object = GetBgMap()->GetGameObject(BgObjects[node*8+GILNEAS_BG_OBJECT_AURA_CONTESTED]);
    }

    if (node == GILNEAS_BG_DYNAMIC_NODES_COUNT)
    {
        TC_LOG_ERROR(LOG_FILTER_SQL,"Player %s (GUID: %u) in Battle for Gilneas initiated EventPlayerClickedOnFlag() but is not near of any flag", source->GetName(), source->GetGUIDLow());
        // this means our player isn't close to any of banners - maybe cheater ??
        return;
    }

    TeamId teamIndex = GetTeamIndexByTeamId(source->GetTeam());

    // Check if player really could use this banner, and has not cheated
    if (!(m_Nodes[node] == 0 || teamIndex == m_Nodes[node]%2))
        return;

    source->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
    uint32 sound = 0;
    // If node is neutral, change to contested
    if (m_Nodes[node] == GILNEAS_BG_NODE_TYPE_NEUTRAL)
    {
        UpdatePlayerScore(source, SCORE_BASES_ASSAULTED, 1);
        m_prevNodes[node] = m_Nodes[node];
        m_Nodes[node] = teamIndex + 1;

        // burn current neutral banner
        _DelBanner(node, GILNEAS_BG_NODE_TYPE_NEUTRAL, 0);

        // create new contested banner
        _CreateBanner(node, GILNEAS_BG_NODE_TYPE_CONTESTED, teamIndex, true);
        _SendNodeUpdate(node);
        m_NodeTimers[node] = GILNEAS_BG_FLAG_CAPTURING_TIME;

        // FIXME: need to fix Locales for team and node names.
        if (teamIndex == 0)
            SendMessage2ToAll(LANG_BG_BG_NODE_CLAIMED, CHAT_MSG_BG_SYSTEM_ALLIANCE, source, _GetNodeNameId(node), LANG_BG_BG_ALLY);
        else
            SendMessage2ToAll(LANG_BG_BG_NODE_CLAIMED, CHAT_MSG_BG_SYSTEM_HORDE, source, _GetNodeNameId(node), LANG_BG_BG_HORDE);

        sound = GILNEAS_BG_SOUND_NODE_CLAIMED;
    }
    // If node is contested
    else if ((m_Nodes[node] == GILNEAS_BG_NODE_STATUS_ALLY_CONTESTED) || (m_Nodes[node] == GILNEAS_BG_NODE_STATUS_HORDE_CONTESTED))
    {
        // If last state is NOT occupied, change node to enemy-contested
        if (m_prevNodes[node] < GILNEAS_BG_NODE_TYPE_OCCUPIED)
        {
            UpdatePlayerScore(source, SCORE_BASES_ASSAULTED, 1);
            m_prevNodes[node] = m_Nodes[node];
            m_Nodes[node] = teamIndex + GILNEAS_BG_NODE_TYPE_CONTESTED;

            // burn current contested banner
            _DelBanner(node, GILNEAS_BG_NODE_TYPE_CONTESTED, !teamIndex);

            // create new contested banner
            _CreateBanner(node, GILNEAS_BG_NODE_TYPE_CONTESTED, teamIndex, true);
            _SendNodeUpdate(node);
            m_NodeTimers[node] = GILNEAS_BG_FLAG_CAPTURING_TIME;

            // FIXME: need to fix Locales for team and node names.
            if (teamIndex == TEAM_ALLIANCE)
                SendMessage2ToAll(LANG_BG_BG_NODE_ASSAULTED, CHAT_MSG_BG_SYSTEM_ALLIANCE, source, _GetNodeNameId(node));
            else
                SendMessage2ToAll(LANG_BG_BG_NODE_ASSAULTED, CHAT_MSG_BG_SYSTEM_HORDE, source, _GetNodeNameId(node));
        }
        // If contested, change back to occupied
        else
        {
            UpdatePlayerScore(source, SCORE_BASES_DEFENDED, 1);
            m_prevNodes[node] = m_Nodes[node];
            m_Nodes[node] = teamIndex + GILNEAS_BG_NODE_TYPE_OCCUPIED;

            // burn current contested banner
            _DelBanner(node, GILNEAS_BG_NODE_TYPE_CONTESTED, !teamIndex);

            // create new occupied banner
            _CreateBanner(node, GILNEAS_BG_NODE_TYPE_OCCUPIED, teamIndex, true);
            _SendNodeUpdate(node);
            m_NodeTimers[node] = 0;
            _NodeOccupied(node, (teamIndex == TEAM_ALLIANCE) ? ALLIANCE:HORDE);

            // FIXME: need to fix Locales for team and node names.
            if (teamIndex == TEAM_ALLIANCE)
                SendMessage2ToAll(LANG_BG_BG_NODE_DEFENDED, CHAT_MSG_BG_SYSTEM_ALLIANCE, source, _GetNodeNameId(node));
            else
                SendMessage2ToAll(LANG_BG_BG_NODE_DEFENDED, CHAT_MSG_BG_SYSTEM_HORDE, source, _GetNodeNameId(node));
        }
        sound = (teamIndex == TEAM_ALLIANCE) ? GILNEAS_BG_SOUND_NODE_ASSAULTED_ALLIANCE : GILNEAS_BG_SOUND_NODE_ASSAULTED_HORDE;
    }
    // If node is occupied, change to enemy-contested
    else
    {
        UpdatePlayerScore(source, SCORE_BASES_ASSAULTED, 1);
        m_prevNodes[node] = m_Nodes[node];
        m_Nodes[node] = teamIndex + GILNEAS_BG_NODE_TYPE_CONTESTED;

        // burn current occupied banner
        _DelBanner(node, GILNEAS_BG_NODE_TYPE_OCCUPIED, !teamIndex);

        // create new contested banner
        _CreateBanner(node, GILNEAS_BG_NODE_TYPE_CONTESTED, teamIndex, true);
        _SendNodeUpdate(node);
        _NodeDeOccupied(node);
        m_NodeTimers[node] = GILNEAS_BG_FLAG_CAPTURING_TIME;

        // FIXME: need to fix Locales for team and node names.
        if (teamIndex == TEAM_ALLIANCE)
            SendMessage2ToAll(LANG_BG_BG_NODE_ASSAULTED, CHAT_MSG_BG_SYSTEM_ALLIANCE, source, _GetNodeNameId(node));
        else
            SendMessage2ToAll(LANG_BG_BG_NODE_ASSAULTED, CHAT_MSG_BG_SYSTEM_HORDE, source, _GetNodeNameId(node));

        sound = (teamIndex == TEAM_ALLIANCE) ? GILNEAS_BG_SOUND_NODE_ASSAULTED_ALLIANCE : GILNEAS_BG_SOUND_NODE_ASSAULTED_HORDE;
    }

    // If node is occupied again, send "X has taken the Y" msg.
    if (m_Nodes[node] >= GILNEAS_BG_NODE_TYPE_OCCUPIED)
    {
        // FIXME: need to fix Locales for team and node names.
        if (teamIndex == TEAM_ALLIANCE)
            SendMessage2ToAll(LANG_BG_BG_NODE_TAKEN, CHAT_MSG_BG_SYSTEM_ALLIANCE, NULL, LANG_BG_BG_ALLY, _GetNodeNameId(node));
        else
            SendMessage2ToAll(LANG_BG_BG_NODE_TAKEN, CHAT_MSG_BG_SYSTEM_HORDE, NULL, LANG_BG_BG_HORDE, _GetNodeNameId(node));
    }
    PlaySoundToAll(sound);
}

bool BattlegroundBFG::SetupBattleground()
{
    for (int i = 0; i < GILNEAS_BG_DYNAMIC_NODES_COUNT; ++i)
    {
        if (!AddObject(GILNEAS_BG_OBJECT_BANNER_NEUTRAL + 8*i, GILNEAS_BG_OBJECTID_NODE_BANNER_0 + i, GILNEAS_BG_NodePositions[i][0], GILNEAS_BG_NodePositions[i][1], GILNEAS_BG_NodePositions[i][2], GILNEAS_BG_NodePositions[i][3], 0, 0, sin(GILNEAS_BG_NodePositions[i][3]/2), cos(GILNEAS_BG_NodePositions[i][3]/2), RESPAWN_ONE_DAY)
            || !AddObject(GILNEAS_BG_OBJECT_BANNER_CONT_A + 8*i, GILNEAS_BG_OBJECTID_BANNER_CONT_A, GILNEAS_BG_NodePositions[i][0], GILNEAS_BG_NodePositions[i][1], GILNEAS_BG_NodePositions[i][2], GILNEAS_BG_NodePositions[i][3], 0, 0, sin(GILNEAS_BG_NodePositions[i][3]/2), cos(GILNEAS_BG_NodePositions[i][3]/2), RESPAWN_ONE_DAY)
            || !AddObject(GILNEAS_BG_OBJECT_BANNER_CONT_H + 8*i, GILNEAS_BG_OBJECTID_BANNER_CONT_H, GILNEAS_BG_NodePositions[i][0], GILNEAS_BG_NodePositions[i][1], GILNEAS_BG_NodePositions[i][2], GILNEAS_BG_NodePositions[i][3], 0, 0, sin(GILNEAS_BG_NodePositions[i][3]/2), cos(GILNEAS_BG_NodePositions[i][3]/2), RESPAWN_ONE_DAY)
            || !AddObject(GILNEAS_BG_OBJECT_BANNER_ALLY + 8*i, GILNEAS_BG_OBJECTID_BANNER_A, GILNEAS_BG_NodePositions[i][0], GILNEAS_BG_NodePositions[i][1], GILNEAS_BG_NodePositions[i][2], GILNEAS_BG_NodePositions[i][3], 0, 0, sin(GILNEAS_BG_NodePositions[i][3]/2), cos(GILNEAS_BG_NodePositions[i][3]/2), RESPAWN_ONE_DAY)
            || !AddObject(GILNEAS_BG_OBJECT_BANNER_HORDE + 8*i, GILNEAS_BG_OBJECTID_BANNER_H, GILNEAS_BG_NodePositions[i][0], GILNEAS_BG_NodePositions[i][1], GILNEAS_BG_NodePositions[i][2], GILNEAS_BG_NodePositions[i][3], 0, 0, sin(GILNEAS_BG_NodePositions[i][3]/2), cos(GILNEAS_BG_NodePositions[i][3]/2), RESPAWN_ONE_DAY)
            || !AddObject(GILNEAS_BG_OBJECT_AURA_ALLY + 8*i, GILNEAS_BG_OBJECTID_AURA_A, GILNEAS_BG_NodePositions[i][0], GILNEAS_BG_NodePositions[i][1], GILNEAS_BG_NodePositions[i][2], GILNEAS_BG_NodePositions[i][3], 0, 0, sin(GILNEAS_BG_NodePositions[i][3]/2), cos(GILNEAS_BG_NodePositions[i][3]/2), RESPAWN_ONE_DAY)
            || !AddObject(GILNEAS_BG_OBJECT_AURA_HORDE + 8*i, GILNEAS_BG_OBJECTID_AURA_H, GILNEAS_BG_NodePositions[i][0], GILNEAS_BG_NodePositions[i][1], GILNEAS_BG_NodePositions[i][2], GILNEAS_BG_NodePositions[i][3], 0, 0, sin(GILNEAS_BG_NodePositions[i][3]/2), cos(GILNEAS_BG_NodePositions[i][3]/2), RESPAWN_ONE_DAY)
            || !AddObject(GILNEAS_BG_OBJECT_AURA_CONTESTED + 8*i, GILNEAS_BG_OBJECTID_AURA_C, GILNEAS_BG_NodePositions[i][0], GILNEAS_BG_NodePositions[i][1], GILNEAS_BG_NodePositions[i][2], GILNEAS_BG_NodePositions[i][3], 0, 0, sin(GILNEAS_BG_NodePositions[i][3]/2), cos(GILNEAS_BG_NodePositions[i][3]/2), RESPAWN_ONE_DAY))
        {
            TC_LOG_ERROR(LOG_FILTER_SQL,"BattleForGilneas: Failed to spawn some object Battleground not created!");
            return false;
        }
    }

    if (!AddObject(GILNEAS_BG_OBJECT_GATE_A, GILNEAS_BG_OBJECTID_GATE_A, GILNEAS_BG_DoorPositions[0][0], GILNEAS_BG_DoorPositions[0][1], GILNEAS_BG_DoorPositions[0][2], GILNEAS_BG_DoorPositions[0][3], GILNEAS_BG_DoorPositions[0][4], GILNEAS_BG_DoorPositions[0][5], GILNEAS_BG_DoorPositions[0][6], GILNEAS_BG_DoorPositions[0][7], RESPAWN_IMMEDIATELY)
        || !AddObject(GILNEAS_BG_OBJECT_GATE_H, GILNEAS_BG_OBJECTID_GATE_H, GILNEAS_BG_DoorPositions[1][0], GILNEAS_BG_DoorPositions[1][1], GILNEAS_BG_DoorPositions[1][2], GILNEAS_BG_DoorPositions[1][3], GILNEAS_BG_DoorPositions[1][4], GILNEAS_BG_DoorPositions[1][5], GILNEAS_BG_DoorPositions[1][6], GILNEAS_BG_DoorPositions[1][7], RESPAWN_IMMEDIATELY))
    {
        TC_LOG_ERROR(LOG_FILTER_SQL,"BattleForGilneas: Failed to spawn door object Battleground not created!");
        return false;
    }

    // Buffs
    for (int i = 0; i < GILNEAS_BG_DYNAMIC_NODES_COUNT; ++i)
    {
        if (!AddObject(GILNEAS_BG_OBJECT_SPEEDBUFF_LIGHTHOUSE + 3 * i, Buff_Entries[0], GILNEAS_BG_BuffPositions[i][0], GILNEAS_BG_BuffPositions[i][1], GILNEAS_BG_BuffPositions[i][2], GILNEAS_BG_BuffPositions[i][3], 0, 0, sin(GILNEAS_BG_BuffPositions[i][3]/2), cos(GILNEAS_BG_BuffPositions[i][3]/2), RESPAWN_ONE_DAY)
            || !AddObject(GILNEAS_BG_OBJECT_SPEEDBUFF_LIGHTHOUSE + 3 * i + 1, Buff_Entries[1], GILNEAS_BG_BuffPositions[i][0], GILNEAS_BG_BuffPositions[i][1], GILNEAS_BG_BuffPositions[i][2], GILNEAS_BG_BuffPositions[i][3], 0, 0, sin(GILNEAS_BG_BuffPositions[i][3]/2), cos(GILNEAS_BG_BuffPositions[i][3]/2), RESPAWN_ONE_DAY)
            || !AddObject(GILNEAS_BG_OBJECT_SPEEDBUFF_LIGHTHOUSE + 3 * i + 2, Buff_Entries[2], GILNEAS_BG_BuffPositions[i][0], GILNEAS_BG_BuffPositions[i][1], GILNEAS_BG_BuffPositions[i][2], GILNEAS_BG_BuffPositions[i][3], 0, 0, sin(GILNEAS_BG_BuffPositions[i][3]/2), cos(GILNEAS_BG_BuffPositions[i][3]/2), RESPAWN_ONE_DAY))
            TC_LOG_ERROR(LOG_FILTER_SQL,"BattleForGilneas: Failed to spawn buff object!");
    }

    return true;
}

void BattlegroundBFG::Reset()
{
    //call parent's class reset
    Battleground::Reset();

    m_TeamScores[TEAM_ALLIANCE]               = 0;
    m_TeamScores[TEAM_HORDE]                  = 0;
    m_lastTick[TEAM_ALLIANCE]                 = 0;
    m_lastTick[TEAM_HORDE]                    = 0;
    m_HonorScoreTicks[TEAM_ALLIANCE]          = 0;
    m_HonorScoreTicks[TEAM_HORDE]             = 0;
    m_ReputationScoreTicks[TEAM_ALLIANCE]     = 0;
    m_ReputationScoreTicks[TEAM_HORDE]        = 0;
    m_IsInformedNearVictory                      = false;
    bool isBGWeekend = sBattlegroundMgr->IsBGWeekend(GetTypeID());
    m_HonorTicks = (isBGWeekend) ? GILNEAS_BG_BGWeekendHonorTicks : GILNEAS_BG_NotBGWeekendHonorTicks;
    m_ReputationTicks = (isBGWeekend) ? GILNEAS_BG_BGWeekendRepTicks : GILNEAS_BG_NotBGWeekendRepTicks;
    m_TeamScores500Disadvantage[TEAM_ALLIANCE] = false;
    m_TeamScores500Disadvantage[TEAM_HORDE]    = false;

    for (uint8 i = 0; i < GILNEAS_BG_DYNAMIC_NODES_COUNT; ++i)
    {
        m_Nodes[i]       = 0;
        m_prevNodes[i]   = 0;
        m_NodeTimers[i]  = 0;
        m_BannerTimers[i].timer = 0;
    }

    for (uint8 i = 0; i < GILNEAS_BG_ALL_NODES_COUNT + 5; ++i)  // +5 for aura triggers
        if (BgCreatures[i])
            DelCreature(i);
}

void BattlegroundBFG::EndBattleground(uint32 winner)
{
    // Win reward
    if (winner == ALLIANCE)
        RewardHonorToTeam(GetBonusHonorFromKill(1), ALLIANCE);

    if (winner == HORDE)
        RewardHonorToTeam(GetBonusHonorFromKill(1), HORDE);

    // Complete map_end rewards (even if no team wins)
    RewardHonorToTeam(GetBonusHonorFromKill(1), HORDE);
    RewardHonorToTeam(GetBonusHonorFromKill(1), ALLIANCE);

    Battleground::EndBattleground(winner);
}

WorldSafeLocsEntry const* BattlegroundBFG::GetClosestGraveYard(Player* player)
{
    TeamId teamIndex = GetTeamIndexByTeamId(player->GetTeam());

    // Is there any occupied node for this team?
    std::vector<uint8> nodes;
    for (uint8 i = 0; i < GILNEAS_BG_DYNAMIC_NODES_COUNT; ++i)
        if (m_Nodes[i] == teamIndex + 3)
            nodes.push_back(i);

    WorldSafeLocsEntry const* good_entry = NULL;

    // If so, select the closest node to place ghost on
    if (!nodes.empty())
    {
        float player_x = player->GetPositionX();
        float player_y = player->GetPositionY();

        float mindist = 999999.0f; // Temp Hack
        for (uint8 i = 0; i < nodes.size(); ++i)
        {
            WorldSafeLocsEntry const* entry = sWorldSafeLocsStore.LookupEntry(GILNEAS_BG_GraveyardIds[nodes[i]]);

            if (!entry)
                continue;

            float dist = (entry->x - player_x)*(entry->x - player_x)+(entry->y - player_y)*(entry->y - player_y);

            if (mindist > dist)
            {
                mindist = dist;
                good_entry = entry;
            }
        }
        nodes.clear();
    }

    // If not, place ghost on starting location
    if (!good_entry)
        good_entry = sWorldSafeLocsStore.LookupEntry(GILNEAS_BG_GraveyardIds[teamIndex+5]);

    return good_entry;
}

void BattlegroundBFG::UpdatePlayerScore(Player* Source, uint32 type, uint32 value, bool doAddHonor)
{
    BattlegroundScoreMap::iterator itr = PlayerScores.find(Source->GetGUID());

    if (itr == PlayerScores.end())      // player was not found...
        return;

    switch (type)
    {
    case SCORE_BASES_ASSAULTED:
        ((BattlegroundBFGScore*)itr->second)->BasesAssaulted += value;
        Source->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, BG_OBJECTIVE_ASSAULT_BASE);
        break;
    case SCORE_BASES_DEFENDED:
        ((BattlegroundBFGScore*)itr->second)->BasesDefended += value;
        Source->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, BG_OBJECTIVE_DEFEND_BASE);
        break;
    default:
        Battleground::UpdatePlayerScore(Source, type, value, doAddHonor);
        break;
    }
}

bool BattlegroundBFG::IsAllNodesConrolledByTeam(uint32 team) const
{
    uint32 count = 0;
    for (int i = 0; i < GILNEAS_BG_DYNAMIC_NODES_COUNT; ++i)
        if ((team == ALLIANCE && m_Nodes[i] == GILNEAS_BG_NODE_STATUS_ALLY_OCCUPIED) || (team == HORDE && m_Nodes[i] == GILNEAS_BG_NODE_STATUS_HORDE_OCCUPIED))
            ++count;

    return count == GILNEAS_BG_DYNAMIC_NODES_COUNT;
}
