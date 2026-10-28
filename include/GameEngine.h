//==============================================================================
// GameEngine.h
//------------------------------------------------------------------------------
// 游戏引擎：解释 StoryData 中的数据表，驱动场景流转、线索收集、
// 选项条件过滤、结局判定与“死亡—回溯”循环。
//==============================================================================
#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <string>
#include <string_view>
#include <unordered_set>

#include "GameTypes.h"

class GameEngine
{
public:
    GameEngine();
    void run();

private:
    std::unordered_set<std::string> ownedClues_;
    std::string currentSceneId_;
    bool running_ = true;
    int  loopCount_ = 0;

    void playScene(const Scene& scene);
    int  presentMenu(const Scene& scene);
    void grantClue(std::string_view id);
    bool isChoiceAvailable(const Choice& choice) const;
    bool hasClue(std::string_view id) const;
    std::size_t ownedCountOfTier(ClueTier tier) const;
    void showClueJournal() const;
    void triggerEnding(const Ending& ending);
    void rewindToHub();
};

#endif // GAME_ENGINE_H
