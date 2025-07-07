#ifdef ENABLE_PREMIUM_SYSTEM
class CPremiumSystem : public singleton<CPremiumSystem>
{
	public :
		CPremiumSystem();
		
		bool		IsPremium(LPCHARACTER ch);
		bool		IsPremium2(LPCHARACTER ch);
		
#ifdef ENABLE_BOX_GIVING
		void		BoxGive(LPCHARACTER ch);
		void		BoxCheck(LPCHARACTER ch);
#endif	
		void		RefreshPremium(LPCHARACTER ch);
		void		SetPremium(LPCHARACTER ch, int days);
		void		RemovePremium(LPCHARACTER ch);
		void		ExpirationCheck(LPCHARACTER ch);
};

class CPremiumManager : public singleton<CPremiumManager>
{
	public :
		CPremiumManager();
		
		void		Log(int pid, const char* event, const char* ip);
};

#endif