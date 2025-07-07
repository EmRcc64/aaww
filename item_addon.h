#ifndef __ITEM_ADDON_H
#define __ITEM_ADDON_H

class CItemAddonManager : public singleton<CItemAddonManager>
{
	public:
		CItemAddonManager();
		virtual ~CItemAddonManager();

		void ApplyAddonTo(int iAddonType, LPITEM pItem);
};

#endif
//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe
