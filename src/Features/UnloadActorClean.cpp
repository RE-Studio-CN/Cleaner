#include "Cleaner.h"
#include "Global.h"
#include "gmlib/mc/world/actor/UnloadedActor.h"
#include "mc/nbt/CompoundTag.h"

namespace UnloadActorClean {

bool isSafeToClean(const std::string& type) {
    if (type == "minecraft:item") return false;
    if (type == "minecraft:painting" || type == "minecraft:frame") return false;
    if (type == "minecraft:player") return false;
    if (type.find("boat") != std::string::npos) return false;
    if (type.find("minecart") != std::string::npos) return false;
    if (type == "minecraft:armor_stand") return false; 
    return true;
}

void cleanUnloadActor() {
    auto& config = Cleaner::Entry::getInstance().getConfig();

    auto logger = ll::io::LoggerRegistry::getInstance().getOrCreate("Cleaner");

    int count = 0;

    gmlib::UnloadedActor::foreachUnloadedActor(
        [&](gmlib::UnloadedActor& actor) -> bool {
            std::string type = actor.getTypeName();

            if (!isSafeToClean(type)) {
                return true; 
            }

            if (config.UnloadActorClean.ExcludeTamed || config.UnloadActorClean.ExcludeNamed) {
                auto nbt = actor.getNbt();
                if (nbt) {
                    if (config.UnloadActorClean.ExcludeTamed) {
                        if (nbt->contains("OwnerID")) return true;
                    }
                    
                    if (config.UnloadActorClean.ExcludeNamed) {
                        if (nbt->contains("CustomName")) return true;
                    }
                }
            }

            for (auto& rule : config.UnloadActorClean.CleanList) {
                std::string ruleStr = rule; 
                if (Cleaner::isMatch(type, ruleStr)) {
                    actor.remove();
                    count++;
                    break; 
                }
            }
            return true;
        }
    );

    if (count > 0) {
        logger->info(tr("cleaner.output.finish", {S(count)}) + " (Unloaded Actors)");
    }
}
} // namespace UnloadActorClean