/*
 * Credits: silviu20092
 */

#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "Creature.h"
#include "Player.h"
#include "Item.h"
#include "item_upgrade.h"

class npc_item_upgrade : public CreatureScript
{
private:
    bool CloseGossip(Player* player)
    {
        CloseGossipMenuFor(player);
        return true;
    }
public:
    npc_item_upgrade() : CreatureScript("npc_item_upgrade")
    {
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (sItemUpgrade->GetReloading())
        {
            ItemUpgrade::SendMessage(player, "Item Upgrade data is being reloaded by an administrator, please retry.");
            CloseGossipMenuFor(player);
            return false;
        }

        sItemUpgrade->GetPagedData(player).reloaded = false;

        if (!sItemUpgrade->GetEnabled())
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|cffb50505NOT AVAILABLE|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        else
        {
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Choose an item to upgrade", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "See upgraded items", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        }
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Nevermind...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ItemUpgrade::PagedData& pagedData = sItemUpgrade->GetPagedData(player);
        if (sItemUpgrade->GetPagedData(player).reloaded)
        {
            ItemUpgrade::SendMessage(player, "Item Upgrade data was reloaded in the meantime by an administrator, please talk again with the NPC.");
            CloseGossipMenuFor(player);
            return false;
        }

        if (sender == GOSSIP_SENDER_MAIN)
        {
            if (action == GOSSIP_ACTION_INFO_DEF)
            {
                ClearGossipMenuFor(player);
                return OnGossipHello(player, creature);
            }
            else if (action == GOSSIP_ACTION_INFO_DEF + 1)
                return CloseGossip(player);
            else if (action == GOSSIP_ACTION_INFO_DEF + 2)
            {
                sItemUpgrade->BuildUpgradableItemCatalogue(player);
                return sItemUpgrade->AddPagedData(player, creature, 0);
            }
            else if (action == GOSSIP_ACTION_INFO_DEF + 3)
            {
                sItemUpgrade->BuildAlreadyUpgradedItemsCatalogue(player);
                return sItemUpgrade->AddPagedData(player, creature, 0);
            }
        }
        else if (sender == GOSSIP_SENDER_MAIN + 1)
        {
            uint32 id = action - GOSSIP_ACTION_INFO_DEF;
            return sItemUpgrade->TakePagedDataAction(player, creature, id);
        }
        else if (sender == GOSSIP_SENDER_MAIN + 2)
        {
            uint32 page = action - GOSSIP_ACTION_INFO_DEF;
            return sItemUpgrade->AddPagedData(player, creature, page);
        }
        else if (sender == GOSSIP_SENDER_MAIN + 9)
        {
            sItemUpgrade->BuildUpgradableItemCatalogue(player);
            return sItemUpgrade->AddPagedData(player, creature, 0);
        }
        else if (sender == GOSSIP_SENDER_MAIN + 10)
        {
            Item* item = player->GetItemByGuid(pagedData.item.guid);
            if (!sItemUpgrade->IsValidItemForUpgrade(item, player))
            {
                ItemUpgrade::SendMessage(player, "Item is no longer available for upgrade.");
                CloseGossipMenuFor(player);
                return false;
            }

            sItemUpgrade->BuildStatsUpgradeCatalogue(player, item);
            return sItemUpgrade->AddPagedData(player, creature, 0);
        }
        else if (sender == GOSSIP_SENDER_MAIN + 11)
        {
            sItemUpgrade->BuildAlreadyUpgradedItemsCatalogue(player);
            return sItemUpgrade->AddPagedData(player, creature, 0);
        }

        return false;
    }
};

void AddSC_npc_item_upgrade()
{
    new npc_item_upgrade();
}
