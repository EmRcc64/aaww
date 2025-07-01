#pragma once

class CPythonMessenger : public CSingleton<CPythonMessenger>
{
	public:
		typedef std::set<std::string> TFriendNameMap;
		typedef std::map<std::string, BYTE> TGuildMemberStateMap;

		enum EMessengerGroupIndex
		{
			MESSENGER_GRUOP_INDEX_FRIEND,
			MESSENGER_GRUOP_INDEX_GUILD,
#ifdef ENABLE_MESSENGER_TEAM
			MESSENGER_GROUP_INDEX_TEAM,
#endif
		};

	public:
		CPythonMessenger();
		virtual ~CPythonMessenger();

		void Destroy();

		// Friend
		void RemoveFriend(const char * c_szKey);
		void OnFriendLogin(const char * c_szKey);
		void OnFriendLogout(const char * c_szKey);
#ifdef ENABLE_MESSENGER_TEAM
		void OnTeamLogin(const char * c_szKey);
		void OnTeamLogout(const char * c_szKey);
#endif
		BOOL IsFriendByKey(const char * c_szKey);
		BOOL IsFriendByName(const char * c_szName);

		// Guild
		void AppendGuildMember(const char * c_szName);
		void RemoveGuildMember(const char * c_szName);
		void RemoveAllGuildMember();
		void LoginGuildMember(const char * c_szName);
		void LogoutGuildMember(const char * c_szName);
		void RefreshGuildMember();

		void SetMessengerHandler(PyObject* poHandler);

	protected:
		TFriendNameMap m_FriendNameMap;
#ifdef ENABLE_MESSENGER_TEAM
		TFriendNameMap m_TeamNameMap;
#endif
		TGuildMemberStateMap m_GuildMemberStateMap;

	private:
		PyObject * m_poMessengerHandler;
};
//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe
