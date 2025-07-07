#include "stdafx.h"
#include "constants.h"
#include "char.h"
#include "desc_client.h"
#include "unique_item.h"
#include "utils.h"
#include "packet.h"
#include "log.h"
#include "quest.h"
#include "questmanager.h"
#include "premium_system.h"
#include "db.h"

CPremiumSystem::CPremiumSystem()
{
}

bool CPremiumSystem::IsPremium(LPCHARACTER ch)
{
	if(NULL == ch)
		return false;
	
	char szQuery[2048];
	int count = 0;
	snprintf(szQuery, sizeof(szQuery), "SELECT COUNT(*) FROM player.premium_list WHERE pid = %i", ch->GetPlayerID());
	auto msg = DBManager::instance().DirectQuery(szQuery);
	MYSQL_RES *res = msg->Get()->pSQLResult;
	
	if(!res)
		return false;
	
	MYSQL_ROW row = mysql_fetch_row(res);
	str_to_number(count, row[0]);
	
	if(count <= 0)
		return false;
	else if(count > 1)
		sys_log(0, "Multiple premiums for player: %s, pid: %i, count: %i", ch->GetName(), ch->GetPlayerID(), count);
	
	return true;
}

bool CPremiumSystem::IsPremium2(LPCHARACTER ch)
{
	if(NULL == ch)
		return false;
	
	if(!ch->IsAffectFlag(AFF_PREMIUM))
		return false;
	return true;
}

void CPremiumSystem::SetPremium(LPCHARACTER ch, int days)
{
	if((NULL == ch) || (IsPremium(ch)))
		return;
	
	if(days == 0)
		days = 3650;
	
	//DB PART
	char szQuery[2048];
	snprintf(szQuery, sizeof(szQuery), "INSERT INTO player.premium_list(pid, expiration) VALUES (%i, DATE_ADD(NOW(), INTERVAL %i DAY))", ch->GetPlayerID(), days);
	DBManager::instance().DirectQuery(szQuery);
	//END OF DB PART
	
	//AFFECT GIVE PART
	ch->SetAffectFlag(AFF_PREMIUM);
	//END OF AFFECT GIVE PART
	
#ifdef ENABLE_UPDATE_PACKET_IN_PREMIUM_SYSTEM
	ch->UpdatePacket();
#endif

#ifdef ENABLE_PREMIUM_LOG_SAVES
	//LOG
	CPremiumManager::instance().Log(ch->GetPlayerID(), "Added premium", ch->GetDesc()->GetHostName());
	//END OF LOG
#endif
	return;
}

void CPremiumSystem::RemovePremium(LPCHARACTER ch)
{
	if((NULL == ch) || (!IsPremium2(ch)))
		return;

	//DB PART
	char szQuery[2048];
	snprintf(szQuery, sizeof(szQuery), "INSERT INTO log.premium_expired(pid, registration, expiration) SELECT pid, registration, expiration FROM player.premium_list WHERE pid = %i", ch->GetPlayerID());
	DBManager::instance().DirectQuery(szQuery);
	
	snprintf(szQuery, sizeof(szQuery), "DELETE FROM player.premium_list WHERE pid = %i", ch->GetPlayerID());
	DBManager::instance().DirectQuery(szQuery);
	//END OF DB PART

	//AFFECT GIVE PART
	ch->ResetAffectFlag(AFF_PREMIUM);
	//END OF AFFECT GIVE PART
	
#ifdef ENABLE_UPDATE_PACKET_IN_PREMIUM_SYSTEM
	ch->UpdatePacket();
#endif

#ifdef ENABLE_PREMIUM_LOG_SAVES
	//LOG
	CPremiumManager::instance().Log(ch->GetPlayerID(), "Removed premium" ,ch->GetDesc()->GetHostName());
	//END OF LOG
#endif	
	return;
}

void CPremiumSystem::RefreshPremium(LPCHARACTER ch)
{
	if(NULL == ch)
		return;

	TPacketGCRefreshPremium RefreshPremiumGC;
	RefreshPremiumGC.bHeader = HEADER_GC_REFRESH_PREMIUM;

	ch->GetDesc()->Packet(&RefreshPremiumGC, sizeof(TPacketGCRefreshPremium));
	return;
}

void CPremiumSystem::ExpirationCheck(LPCHARACTER ch)
{
	if((NULL == ch) || (!IsPremium2(ch)))
		return;

	char szQuery[2048]; int rows;
	snprintf(szQuery, sizeof(szQuery), "SELECT expiration FROM player.premium_list WHERE pid = %i", ch->GetPlayerID());
	auto msg = DBManager::instance().DirectQuery(szQuery);
	MYSQL_RES *res = msg->Get()->pSQLResult;
	MYSQL_ROW row = mysql_fetch_row(res);
	
	if(!res)
		return;
	
	if((rows = mysql_num_rows(res)) <= 0)
		return;
	
	char time_now[80]; time_t now = time(0); struct tm tstruct; tstruct = *localtime(&now);
	strftime(time_now, sizeof(time_now), "%Y-%m-%d %H:%M:%S", &tstruct);
	if(strcmp(time_now, row[0]) > 0)
		RemovePremium(ch);
	
	return;
}


#ifdef ENABLE_BOX_GIVING
void CPremiumSystem::BoxGive(LPCHARACTER ch)
{
	if((!ch) || (!IsPremium2(ch)))
		return;
	
	int box_id = 0;
	const char* why = "Premium Box";
	box_id = PREMIUM_BOX_ID;
	
	if(box_id <= 0)
		return;
	
	char szQuery[2048];
	snprintf(szQuery, sizeof(szQuery), 
					"INSERT INTO player.item_award (pid, login, vnum, count, why, mall) VALUES (%i, '%s', %i, %i, '%s', %i)", 
					ch->GetPlayerID(), ch->GetDesc()->GetAccountTable().login, box_id, PREMIUM_BOX_COUNT, why, 1);
	DBManager::instance().DirectQuery(szQuery);
	
	snprintf(szQuery, sizeof(szQuery), "UPDATE player.premium_list SET next_box_earn=DATE_ADD(NOW(), INTERVAL %i DAY) WHERE pid = %i", PREMIUM_BOX_INTERVAL, ch->GetPlayerID());
	DBManager::instance().DirectQuery(szQuery);
	
#ifdef ENABLE_PREMIUM_LOG_SAVES
	//LOG
	CPremiumManager::instance().Log(ch->GetPlayerID(), "Premium box given" ,ch->GetDesc()->GetHostName());
	//END OF LOG
#endif
	return;
}

void CPremiumSystem::BoxCheck(LPCHARACTER ch)
{
	if((!ch) || (!IsPremium2(ch)))
		return;

	char szQuery[512]; int rows;
	snprintf(szQuery, sizeof(szQuery), "SELECT next_box_earn FROM player.premium_list WHERE pid = %i", ch->GetPlayerID());
	auto msg = DBManager::instance().DirectQuery(szQuery);
	MYSQL_RES *res = msg->Get()->pSQLResult;
	MYSQL_ROW row = mysql_fetch_row(res);
	
	if(!res)
		return;
	
	if((rows = mysql_num_rows(res)) <= 0)
		return;
	
	char time_now[80]; time_t now = time(0); struct tm tstruct; tstruct = *localtime(&now);
	strftime(time_now, sizeof(time_now), "%Y-%m-%d %H:%M:%S", &tstruct);
	if(strcmp(time_now, row[0]) > 0)
		BoxGive(ch);
	
	return;
}
#endif



CPremiumManager::CPremiumManager()
{
}

#ifdef ENABLE_PREMIUM_LOG_SAVES
void CPremiumManager::Log(int pid, const char* event, const char* ip)
{
	char szQuery[2048];
	snprintf(szQuery, sizeof(szQuery), "INSERT DELAYED INTO log.premium_log(pid, event, ip) VALUES (%i, '%s', '%s')", pid, event, ip);
	DBManager::instance().DirectQuery(szQuery);	
	return;
}

#endif

