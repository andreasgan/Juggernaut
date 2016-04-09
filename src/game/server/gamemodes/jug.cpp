/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/shared/config.h>

#include <game/server/entities/character.h>
#include <game/server/player.h>
#include "jug.h"

CGameControllerJUG::CGameControllerJUG(class CGameContext *pGameServer) : IGameController(pGameServer)
{
	m_pGameServer = pGameServer;
	m_pGameType = "JUG";
}

//This is called from player.cpp (snap)
bool CGameControllerJUG::IsJuggernaut(int ClientID){
	if(current_jug){
		if(current_jug->GetCID() == ClientID)
			return true;
		return false;
	}else{
		NewJuggernaut();
		return false;
	}
}

void CGameControllerJUG::NewJuggernaut(class CPlayer *pPlayer){
	//Random juggernaut
	if(!pPlayer){
	  int count = 0;
		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			if(GameServer()->m_apPlayers[i])
				count++;
		}
		int random_id = rand() % count;
		pPlayer = GameServer()->m_apPlayers[random_id];
	}
	//end random

	current_jug = pPlayer;

	//Give 50 health
	current_jug->GetCharacter()->SetHealth(50);

	char buf[512];
	str_format(buf, sizeof(buf), "%s is the new juggernaut!", GameServer()->Server()->ClientName(current_jug->GetCID()));
	GameServer()->CreateSoundGlobal(SOUND_CTF_CAPTURE);
	GameServer()->SendChat(-1, CGameContext::CHAT_ALL, buf);
	GameServer()->SendBroadcast(buf, -1);
}

int CGameControllerJUG::OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon)
{
	IGameController::OnCharacterDeath(pVictim, pKiller, Weapon);

	if(pVictim){
		//If it was a disconnect/teamswitch, and it was juggernaut -> New random juggernaut
		if(Weapon == WEAPON_GAME && IsJuggernaut(pVictim->GetPlayer()->GetCID())){
			NewJuggernaut();
		}
  }

	if(Weapon != WEAPON_GAME && Weapon != WEAPON_SELF && Weapon != WEAPON_WORLD)
	{
		if(pKiller && pVictim)
		{
			if(current_jug->GetCID() == pVictim->GetPlayer()->GetCID())
			{
				NewJuggernaut(pKiller);
			}
		}
	}

	return 0;
}

void CGameControllerJUG::Tick()
{
	IGameController::Tick();
}
