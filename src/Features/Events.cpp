#include "Cleaner.h"
#include "gmlib/mc/world/Level.h"
#include "gmlib/mc/world/actor/Player.h"

namespace Cleaner {

void setShouldIgnore(gmlib::GMActor* ac) { ac->addTag("cleaner:ignore"); }

void ListenEvents() {
    auto& eventBus = ll::event::EventBus::getInstance();
    auto& config   = Cleaner::Entry::getInstance().getConfig();
    
    // ItemSpawnEvent
    eventBus.emplaceListener<ila::mc::SpawnItemActorAfterEvent>([&config](ila::mc::SpawnItemActorAfterEvent& event) {
        auto& item = event.itemActor();
        if (event.spawner()) {
            auto pl = (gmlib::GMPlayer*)event.spawner();
            if (pl->isPlayer() && !pl->isAlive()) { // 玩家死亡掉落物保护
                auto ac = (gmlib::GMActor*)&item;
                setShouldIgnore(ac);
                return;
            }
        }
        if (config.ItemDespawn.Enabled) {
            // 检查白名单
            auto list        = config.ItemDespawn.WhiteList;
            auto type        = item.item().getTypeName();
            for (auto& key : list) {
                if (isMatch(type, key)) {
                    return;
                }
            }
            // 设置自定义消失时间
            item.lifeTime() = config.ItemDespawn.DespawnTime;
        }
    });

    eventBus.emplaceListener<ila::mc::ActorPickupItemAfterEvent>([&config](ila::mc::ActorPickupItemAfterEvent& event) {
        if (config.CleanMobs.EnableAutoExclude) {
            auto mob = (gmlib::GMActor*)&event.self();
            setShouldIgnore(mob);
        }
    });
}

} // namespace Cleaner