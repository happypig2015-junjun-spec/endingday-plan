//==============================================================================
// GameEngine.cpp
//------------------------------------------------------------------------------
// GameEngine.h 的实现。解释剧情数据表，驱动：
//   清屏 -> 标题 -> 正文逐字播放 -> （死亡回溯 | 选项菜单），
// 并维护线索集合、线索本、进度与结局判定。
//==============================================================================
#include "GameEngine.h"

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "StoryData.h"
#include "Terminal.h"

namespace
{
// 不同阶段线索 / 物品在屏幕上的配色。
std::string_view tierColor(ClueTier tier)
{
    switch (tier)
    {
        case ClueTier::Basic:    return term::kCyan;
        case ClueTier::Advanced: return term::kYellow;
        case ClueTier::Supreme:  return term::kMagenta;
        case ClueTier::Item:     return term::kGreen;
    }
    return term::kReset;
}

const char* tierLabel(ClueTier tier)
{
    switch (tier)
    {
        case ClueTier::Basic:    return "基础线索";
        case ClueTier::Advanced: return "进阶线索";
        case ClueTier::Supreme:  return "高级线索";
        case ClueTier::Item:     return "随身物品";
    }
    return "";
}
} // namespace

GameEngine::GameEngine()
    : currentSceneId_(std::string(story::kSceneStart))
{
}

//==============================================================================
// 主循环
//==============================================================================
void GameEngine::run()
{
    while (running_)
    {
        const Scene* scene = story::findScene(currentSceneId_);
        if (scene == nullptr)
        {
            // 兜底：任何非法跳转都回到中枢，避免游戏卡死。
            currentSceneId_ = std::string(story::kSceneHub);
            continue;
        }
        playScene(*scene);
    }
}

//==============================================================================
// 播放一个场景
//==============================================================================
void GameEngine::playScene(const Scene& scene)
{
    term::clear();

    // 标题（非空时加粗显示）。
    if (!scene.title.empty())
    {
        term::slowPrint(std::string(term::kBold) + "—— " + scene.title + " ——" +
                        std::string(term::kReset) + "\n");
    }

    // 正文逐段逐字播放。
    for (const std::string& paragraph : scene.paragraphs)
    {
        term::slowPrint(paragraph);
        term::slowPrint("\n");
    }

    // 进入中枢时，额外显示三档线索收集进度。
    if (scene.id == story::kSceneHub)
    {
        auto line = std::string("\n") + std::string(term::kDim) +
                    "线索进度  基础 " +
                    std::to_string(ownedCountOfTier(ClueTier::Basic)) + "/" +
                    std::to_string(story::clueCountOfTier(ClueTier::Basic)) +
                    "   进阶 " +
                    std::to_string(ownedCountOfTier(ClueTier::Advanced)) + "/" +
                    std::to_string(story::clueCountOfTier(ClueTier::Advanced)) +
                    "   高级 " +
                    std::to_string(ownedCountOfTier(ClueTier::Supreme)) + "/" +
                    std::to_string(story::clueCountOfTier(ClueTier::Supreme)) +
                    std::string(term::kReset) + "\n";
        term::slowPrint(line);
    }

    // 死亡场景：发动时间回溯。
    if (scene.isDeath)
    {
        term::slowPrint(std::string(term::kRed) +
                        "\n  ▌ 时间在此断裂……" + std::string(term::kReset));
        term::pause(std::string(term::kRed) +
                    "  ▌ 发动回溯，回到锚点 [按回车]" + std::string(term::kReset));
        rewindToHub();
        return;
    }

    // 普通场景：呈现选项菜单。
    const int pickedIndex = presentMenu(scene);
    if (!running_) return; // 玩家在菜单中选择了退出

    const Choice& picked = scene.choices[static_cast<std::size_t>(pickedIndex)];

    // 授予线索 / 物品。
    grantClue(picked.grantedClue);

    // 触发结局。
    if (!picked.endingId.empty())
    {
        const Ending* ending = story::findEnding(picked.endingId);
        if (ending != nullptr) triggerEnding(*ending);
        return;
    }

    // 特殊目标：退出游戏。
    if (picked.target == "__quit")
    {
        running_ = false;
        return;
    }

    // 跳转：目标为空则回中枢。
    currentSceneId_ = picked.target.empty() ? std::string(story::kSceneHub)
                                           : picked.target;
}

//==============================================================================
// 选项菜单：过滤、展示、读取（含线索本 / 退出的特殊输入）
//==============================================================================
int GameEngine::presentMenu(const Scene& scene)
{
    // 收集当前状态下“可见”的选项下标。
    std::vector<int> visible;
    for (std::size_t i = 0; i < scene.choices.size(); ++i)
    {
        if (isChoiceAvailable(scene.choices[i]))
            visible.push_back(static_cast<int>(i));
    }

    while (true)
    {
        term::slowPrint("\n");
        for (std::size_t n = 0; n < visible.size(); ++n)
        {
            const Choice& choice = scene.choices[static_cast<std::size_t>(visible[n])];
            term::slowPrint(std::to_string(n + 1) + ". " + choice.text + "\n");
        }
        term::slowPrint(std::string(term::kGray) +
                        "（输入编号选择；L=查看线索本；Q=退出游戏）" +
                        std::string(term::kReset) + "\n");

        const int input = term::readChoice(static_cast<int>(visible.size()));
        if (input == -1)
        {
            showClueJournal();
            continue; // 看完线索本后重新展示菜单
        }
        if (input == -2)
        {
            running_ = false;
            return 0;
        }
        return visible[static_cast<std::size_t>(input - 1)];
    }
}

//==============================================================================
// 状态判定
//==============================================================================
bool GameEngine::hasClue(std::string_view id) const
{
    return ownedClues_.find(std::string(id)) != ownedClues_.end();
}

bool GameEngine::isChoiceAvailable(const Choice& choice) const
{
    // requireAll：必须全部拥有。
    for (const std::string& id : choice.requireAll)
    {
        if (!hasClue(id)) return false;
    }
    // requireNone：必须全部未拥有。
    for (const std::string& id : choice.requireNone)
    {
        if (hasClue(id)) return false;
    }
    return true;
}

std::size_t GameEngine::ownedCountOfTier(ClueTier tier) const
{
    std::size_t count = 0;
    for (const Clue& clue : story::clues())
    {
        if (clue.tier == tier && hasClue(clue.id)) ++count;
    }
    return count;
}

//==============================================================================
// 获得线索 / 物品
//==============================================================================
void GameEngine::grantClue(std::string_view id)
{
    if (id.empty()) return;
    const auto [it, inserted] = ownedClues_.insert(std::string(id));
    if (!inserted) return; // 重复获得，不重复提示

    const Clue* clue = story::findClue(id);
    if (clue == nullptr) return;

    const std::string color(tierColor(clue->tier));
    const std::string kind = (clue->tier == ClueTier::Item) ? "获得物品" : "获得线索";
    term::slowPrint("\n" + color + "  ▶ " + kind + "【" + clue->name + "】" +
                    std::string(term::kReset) + "\n");
}

//==============================================================================
// 线索本
//==============================================================================
void GameEngine::showClueJournal() const
{
    term::clear();
    term::slowPrint(std::string(term::kBold) + "════ 线 索 本 ════" +
                    std::string(term::kReset) + "\n");

    const ClueTier tiers[] = { ClueTier::Basic, ClueTier::Advanced,
                               ClueTier::Supreme, ClueTier::Item };
    for (ClueTier tier : tiers)
    {
        const std::string color(tierColor(tier));
        term::slowPrint("\n" + color + std::string(term::kBold) + "【" +
                        tierLabel(tier) + "】" +
                        std::string(term::kReset) + "\n");

        for (const Clue& clue : story::clues())
        {
            if (clue.tier != tier) continue;
            if (hasClue(clue.id))
            {
                term::slowPrint(color + "  ■ " + clue.name + std::string(term::kReset) +
                                "\n      " + clue.detail + "\n");
            }
            else
            {
                term::slowPrint(std::string(term::kGray) + "  □ ？？？（尚未发现）" +
                                std::string(term::kReset) + "\n");
            }
        }
    }
    term::pause();
}

//==============================================================================
// 结局
//==============================================================================
void GameEngine::triggerEnding(const Ending& ending)
{
    term::clear();
    term::slowPrint(std::string(term::kBold) + std::string(term::kYellow) +
                    "════ " + ending.title + " ════" +
                    std::string(term::kReset) + "\n\n");
    for (const std::string& paragraph : ending.paragraphs)
    {
        term::slowPrint(paragraph);
        term::slowPrint("\n");
    }

    // 真结局：全剧终，结束游戏。
    if (ending.id == "ending_true")
    {
        term::slowPrint("\n" + std::string(term::kBold) +
                        "—— 全 剧 终 ——" + std::string(term::kReset) + "\n");
        running_ = false;
        return;
    }

    // 非真结局：允许继续探索，去凑齐更深的线索。
    term::slowPrint("\n" + std::string(term::kGray) +
                    "（更深的真相仍在雾中，你可以继续探索。）" +
                    std::string(term::kReset) + "\n");
    term::slowPrint("1. 回到锚点，继续探索    2. 就此结束游戏\n");
    while (true)
    {
        const int input = term::readChoice(2);
        if (input == 1)
        {
            currentSceneId_ = std::string(story::kSceneHub);
            return;
        }
        if (input == 2)
        {
            running_ = false;
            return;
        }
    }
}

//==============================================================================
// 回溯
//==============================================================================
void GameEngine::rewindToHub()
{
    ++loopCount_;
    currentSceneId_ = std::string(story::kSceneHub);
}
