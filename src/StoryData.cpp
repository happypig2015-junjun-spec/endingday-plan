//==============================================================================
// StoryData.cpp
//------------------------------------------------------------------------------
// ★★ 剧情数据库（拓展游戏时主要编辑本文件）★★
//
// 本表包含：
//   kClues   —— 9 条三档线索 + 12 件关联道具；
//   kScenes  —— 80+ 个互相关联的场景节点（含 20 个死亡回溯点）；
//   kEndings —— 3 个结局（基础 / 进阶 / 真结局）。
//
// 场景之间通过“道具 / 线索条件”彼此关联，形成一张探索网：
//   例如：地铁列车(撬棍) → 避难所储藏室 / 居民楼设备间；
//         检查站军车(门禁卡) → 军械库(手枪) → 红区 / 实验室收容区；
//         公园防空洞(防护服) → 诊所隔离区 / 实验室深冷库 / 医院 ICU；
//         B3传单(密码) → A2通讯(坐标) → 地下实验室 → S1/S2/S3 → 真结局。
//
// 所有数据使用 C++20 指定初始化器书写，字段含义见 GameTypes.h。
//==============================================================================
#include "StoryData.h"

#include <algorithm>
#include <functional>

namespace story
{

//==============================================================================
// 一、线索与道具表
//==============================================================================
namespace
{

const std::vector<Clue> kClues =
{
    //---------------- 基础线索（现象层）-------------------------------------
    Clue{
        .id = "B1",
        .tier = ClueTier::Basic,
        .name = "被重新封装的矿泉水",
        .detail = "批号 ECHO-W-00，生产日期早于灰雾日 11 天，瓶口有二次高温封装"
                  "痕迹，瓶底有絮状沉淀。说明污染在灾难前就被蓄意装入瓶中。"
    },
    Clue{
        .id = "B2",
        .tier = ClueTier::Basic,
        .name = "实验动物耳标",
        .detail = "中央公园的变异狗佩戴 ECHO-K 系列编号金属耳标。它们不是偶然"
                  "感染的流浪动物，而是来自某座设施、被编号管理的“资产”。"
    },
    Clue{
        .id = "B3",
        .tier = ClueTier::Basic,
        .name = "代号《回声》的政府传单",
        .detail = "空投传单要求居民禁止饮用一切市政供水，空白处留有无线电频率"
                  "与手写密码，暗示“回声”是一场有组织行动的代号。"
    },

    //---------------- 进阶线索（阴谋层）-------------------------------------
    Clue{
        .id = "A1",
        .tier = ClueTier::Advanced,
        .name = "流调员的早期病例",
        .detail = "所有早期患者同日抵达同一供水点，如同被“同时按下开关”，不符合"
                  "自然传染病的传播规律。末页红笔批注：ECHO？非自然传播？"
    },
    Clue{
        .id = "A2",
        .tier = ClueTier::Advanced,
        .name = "破译的军方加密通讯",
        .detail = "ECHO 样本已转移至 D 区地下设施（坐标 7-3-9）；“零号宿主”仍在"
                  "“锚点”周边活动，一旦接触立即启动回溯协议。"
    },
    Clue{
        .id = "A3",
        .tier = ClueTier::Advanced,
        .name = "死者的水箱作业日记",
        .detail = "灰雾日前 5 天，ECHO 人员在楼顶水箱“检修”；前一天有人饮下自来水"
                  "后发病。日记断言：水是被“提前写好”的。"
    },

    //---------------- 高级线索（真相层）-------------------------------------
    Clue{
        .id = "S1",
        .tier = ClueTier::Supreme,
        .name = "ECHO-0 初代样本标签",
        .detail = "初代株提取自“时间锚点介质”，标签中没有任何“武器 / 投放”字样——"
                  "它从诞生起就不是为了被投放，而是为了被“带回”。"
    },
    Clue{
        .id = "S2",
        .tier = ClueTier::Supreme,
        .name = "研究员的循环录音",
        .detail = "第 97 次循环记录：回声是时间介质，病毒随意识回溯进入更早的"
                  "时间线；零号宿主，就是那个反复在锚点醒来的人。"
    },
    Clue{
        .id = "S3",
        .tier = ClueTier::Supreme,
        .name = "主控室真相文件",
        .detail = "病毒唯一的传播途径是回溯。终止方式：集齐全部真相后停止回溯，"
                  "哪怕这条时间线里的自己终将死去。"
    },

    //---------------- 关联道具（Item：不计入推理进度）-----------------------
    Clue{ .id = "tool_crowbar",   .tier = ClueTier::Item,
          .name = "生锈的撬棍",   .detail = "地铁废弃列车里找到，能撬开锈蚀的门。" },
    Clue{ .id = "tool_medicine",  .tier = ClueTier::Item,
          .name = "急救药品",     .detail = "超市冷库中保存完好的抗生素与止血药，能救人。" },
    Clue{ .id = "tool_radiopart", .tier = ClueTier::Item,
          .name = "电台电子管",   .detail = "军车仪表盘上拆下的备件，能修好各种无线电设备。" },
    Clue{ .id = "tool_keycard",   .tier = ClueTier::Item,
          .name = "军方门禁卡",   .detail = "军官遗体上的磁卡，可打开军械库与实验室主管室。" },
    Clue{ .id = "tool_weapon",    .tier = ClueTier::Item,
          .name = "九二式手枪",   .detail = "军械库中尚有弹药的手枪，危急时刻能保命。" },
    Clue{ .id = "tool_protection",.tier = ClueTier::Item,
          .name = "全套防护服",   .detail = "公园防空洞封存的防护服，可抵御高浓度感染环境。" },
    Clue{ .id = "tool_map",       .tier = ClueTier::Item,
          .name = "手绘城区地图", .detail = "超市天台绘制，标出了信号塔、供水厂与红区方位。" },
    Clue{ .id = "tool_wrench",    .tier = ClueTier::Item,
          .name = "管钳扳手",     .detail = "居民楼设备间工具，可关闭供水总阀。" },
    Clue{ .id = "tool_freeze",    .tier = ClueTier::Item,
          .name = "冷冻试剂瓶",   .detail = "超市冷库中贴着 ECHO 标签、被异常低温保存的试剂。" },
    Clue{ .id = "tool_scales",    .tier = ClueTier::Item,
          .name = "变异鱼鳞",     .detail = "公园池塘死鱼身上的鳞片，皮下组织呈青黑色丝状。" },
    Clue{ .id = "tool_tankres",   .tier = ClueTier::Item,
          .name = "水箱残留样本", .detail = "居民楼天台水箱内壁刮下的残留物，与 ECHO 絮状物一致。" },
    Clue{ .id = "tool_codes",     .tier = ClueTier::Item,
          .name = "幸存者密码本", .detail = "501 幸存者所赠，记着供水厂与医院门禁的数字密码。" },
};

//==============================================================================
// 二、场景表（按区域分块，注释清晰）
//==============================================================================
const std::vector<Scene> kScenes =
{
    //==========================================================================
    // 【区域 0】序章 & 中枢
    //==========================================================================
    Scene{
        .id = "start",
        .title = "",
        .paragraphs =
        {
            "================================================\n"
            "        E C H O   O F   D O O M S D A Y\n"
            "                  末  日  回  声\n"
            "================================================\n",
            "灰雾降临的第七天。",
            "你在地铁深处的应急避难所里醒来。头顶灯管发出垂死的嗡鸣，应急广播"
            "早已只剩下循环的电流声。",
            "你记得一件不该记得的事——就在昨天，你死了。被带着铁锈味的饮水杀死；"
            "被长着人手的野狗撕开；被实验室警报声中涌出的白雾吞没……",
            "而每一次死亡，你都会回到这里，回到这张冰冷的行军床上。世界重置，"
            "伤痕消失，只有记忆，像烧红的铁印，留在脑海里。",
            "你把那些用命换来的信息称为“线索”。你隐约明白：这座城市的末日藏着"
            "一个答案，而答案，需要你一次次走进雾里，把它拼完整。"
        },
        .choices =
        {
            Choice{ .text = "睁开眼睛，从行军床上坐起", .target = "hub" }
        }
    },

    Scene{
        .id = "hub",
        .title = "避难所 · 时间锚点",
        .paragraphs =
        {
            "避难所是你唯一的锚点。墙上的粉笔痕迹记录着你发动回溯的次数。",
            "四个方向通向城市的不同角落，更深处还有地铁隧道与排水渠。收音机里，"
            "一个沙哑的女声在循环：“……不要饮用自来水……不要相信穿白色防护服的人……”",
            "你检查了一下背包，深吸一口气。"
        },
        .choices =
        {
            //--- 避难所内部 ---
            Choice{ .text = "查看墙壁上密密麻麻的刻字",       .target = "shelter_wall" },
            Choice{ .text = "用撬棍撬开储藏室锈死的门",       .target = "shelter_storage",
                    .requireAll = { "tool_crowbar" } },
            Choice{ .text = "用电子管修好角落的收音机",       .target = "shelter_radio",
                    .requireAll = { "tool_radiopart" } },
            //--- 五大表层区域 ---
            Choice{ .text = "【东】前往惠民超市",             .target = "market_outside" },
            Choice{ .text = "【北】前往中央公园",             .target = "park_entrance" },
            Choice{ .text = "【西】前往军警检查站",           .target = "cp_outside" },
            Choice{ .text = "【南】前往社区诊所",             .target = "clinic_lobby" },
            Choice{ .text = "【东南】搜索居民楼",             .target = "apt_lobby" },
            //--- 深层区域 ---
            Choice{ .text = "【下】沿地铁隧道探索",           .target = "metro_entrance" },
            Choice{ .text = "沿排水渠寻找 D 区地下设施",      .target = "sewer_entrance",
                    .requireAll = { "A2" } },
            Choice{ .text = "按地图前往信号塔",               .target = "tower_base",
                    .requireAll = { "tool_map" } },
            Choice{ .text = "按地图前往城市供水厂",           .target = "wp_gate",
                    .requireAll = { "tool_map" } },
            //--- 推理与结局 ---
            Choice{ .text = "【推理】整合基础线索，得出结论",  .target = "",
                    .requireAll = { "B1", "B2", "B3" },        .endingId = "ending_basic" },
            Choice{ .text = "【推理】整合进阶线索，得出结论",  .target = "",
                    .requireAll = { "A1", "A2", "A3" },        .endingId = "ending_advanced" },
            Choice{ .text = "走向感知中的“锚点核心”",         .target = "core_gate",
                    .requireAll = { "B1","B2","B3","A1","A2","A3",
                                    "S1","S2","S3" } },
            //--- 退出 ---
            Choice{ .text = "放下一切，离开游戏",             .target = "__quit" },
        }
    },

    //==========================================================================
    // 【区域 0.5】避难所内部（3 个关联场景）
    //==========================================================================
    Scene{
        .id = "shelter_wall",
        .title = "避难所 · 墙壁刻字",
        .paragraphs =
        {
            "墙壁上留着前几任“醒来者”的刻字，有些已经模糊：",
            "“第 3 次。别喝自来水。”\n"
            "“第 11 次。别相信穿白防护服的人，他们不是来救我们的。”\n"
            "“第 40 次。我好像……在雾里看见了另一个我。”\n"
            "“第 96 次。如果你读得到这些字，去地下，去看那段录音。”",
            "刻字到这里戛然而止，最后一道划痕深得几乎凿穿墙壁。"
        },
        .choices = { Choice{ .text = "回到锚点", .target = "hub" } }
    },
    Scene{
        .id = "shelter_storage",
        .title = "避难所 · 储藏室",
        .paragraphs =
        {
            "你用撬棍撬开锈死的门。储藏室里积着灰，货架上只剩几截蜡烛和一本"
            "被水泡过的《应急手册》。",
            "手册扉页有人潦草写着：“锚点不是避难所，是病毒的‘插座’。每次醒来，"
            "都是它在充电。”"
        },
        .choices = { Choice{ .text = "收起手册，回到锚点", .target = "hub" } }
    },
    Scene{
        .id = "shelter_radio",
        .title = "避难所 · 修好的收音机",
        .paragraphs =
        {
            "你换上电子管，收音机嘶嘶响了几声，一个清晰的女声传出——正是循环"
            "广播里的那个人，但这一次她在说新的话：",
            "“……如果你能听到完整频段，说明你也在循环里。我在学校的救援点，"
            "红区深处。求求你，来之前，先找到真相，否则你只会一遍遍死在我面前。”",
            "信号随即中断。"
        },
        .choices = { Choice{ .text = "记住她的话，回到锚点", .target = "hub" } }
    },

    //==========================================================================
    // 【区域 1】惠民超市（7 个普通场景 + 2 个死亡场景）
    //==========================================================================
    Scene{
        .id = "market_outside",
        .title = "惠民超市 · 门外",
        .paragraphs =
        {
            "卷帘门半开着，像一张垂死的嘴。门口血迹向内延伸，消失在黑暗里，"
            "里面传来潮湿的咀嚼声。",
            "建筑外侧有一架生锈的消防梯，通向超市天台。"
        },
        .choices =
        {
            Choice{ .text = "从半开的正门进去",       .target = "market_front" },
            Choice{ .text = "绕到员工通道",           .target = "market_back" },
            Choice{ .text = "攀爬消防梯上天台",       .target = "market_roof" },
            Choice{ .text = "返回避难所",             .target = "hub" },
        }
    },
    Scene{
        .id = "market_front",
        .title = "惠民超市 · 正门",
        .paragraphs =
        {
            "货架倒了一半。一个佝偻的身影伏在收银台后咀嚼，脖子以活人做不到的"
            "角度歪着。",
            "一只空罐头滚到你脚边。"
        },
        .choices =
        {
            Choice{ .text = "扔出罐头，引开它",      .target = "market_warehouse" },
            Choice{ .text = "压低身子从货架底爬过去",.target = "market_warehouse" },
            Choice{ .text = "硬冲过去",              .target = "death_market" },
            Choice{ .text = "退回门外",              .target = "market_outside" },
        }
    },
    Scene{
        .id = "market_back",
        .title = "惠民超市 · 员工通道",
        .paragraphs =
        {
            "通道狭窄安静。一具员工尸体靠在墙上，手里攥着冷库钥匙，皮肤下有"
            "青黑色线条在游走。"
        },
        .choices =
        {
            Choice{ .text = "小心取下钥匙，进入冷库",.target = "market_coldroom" },
            Choice{ .text = "翻动尸体寻找其他东西",  .target = "death_market" },
            Choice{ .text = "不碰尸体，退回去",      .target = "market_outside" },
        }
    },
    Scene{
        .id = "market_warehouse",
        .title = "惠民超市 · 仓库",
        .paragraphs =
        {
            "仓库里堆满“应急供水”，瓶身标签崭新，瓶口却有一圈极细的二次封装"
            "痕迹。",
            "深处是经理室，门缝里透出一线光。"
        },
        .choices =
        {
            Choice{ .text = "检查整箱矿泉水的封装",  .target = "market_b1",
                    .grantedClue = "", .requireNone = { "B1" } },
            Choice{ .text = "拧开一瓶“应急供水”喝下",.target = "death_water" },
            Choice{ .text = "进入经理室",            .target = "market_office" },
            Choice{ .text = "返回避难所",            .target = "hub" },
        }
    },
    Scene{
        .id = "market_b1",
        .title = "惠民超市 · 线索",
        .paragraphs =
        {
            "你举起一瓶水对着光：瓶底沉着几缕几乎看不见的絮状物，批号 ECHO-W-00，"
            "生产日期比灰雾日早了整整 11 天。",
            "不是水源被污染了——是有人，在灾难发生之前，把污染装进了瓶子。",
            "你没有带走任何一瓶水，把批号和封瓶痕迹刻进脑海。"
        },
        .choices =
        {
            Choice{ .text = "（收好线索）",          .target = "market_warehouse",
                    .grantedClue = "B1" },
        }
    },
    Scene{
        .id = "market_coldroom",
        .title = "惠民超市 · 冷库",
        .paragraphs =
        {
            "你用钥匙打开冷库，寒气扑面而来。货架上除了冻货，还有一箱保存完好的"
            "急救药品；最里面的冰架上，单独冻着一支贴 ECHO 标签的试剂瓶。",
            "冷库门在身后缓缓回弹。"
        },
        .choices =
        {
            Choice{ .text = "取走药品和冷冻试剂",    .target = "market_cold_got",
                    .requireNone = { "tool_freeze" } },
            Choice{ .text = "让冷库门彻底关上，摸黑停留", .target = "death_coldroom" },
            Choice{ .text = "立即离开冷库",          .target = "market_back" },
        }
    },
    Scene{
        .id = "market_cold_got",
        .title = "惠民超市 · 冷库",
        .paragraphs =
        {
            "你把药品和冷冻试剂塞进背包。试剂瓶冷得反常，即便隔着保温层，仍让你"
            "指尖发麻。",
            "你用卡子顶住冷库门，迅速退了出去。"
        },
        .choices =
        {
            Choice{ .text = "（收好物品）",          .target = "market_back",
                    .grantedClue = "tool_medicine" },
        }
    },
    Scene{
        .id = "market_office",
        .title = "惠民超市 · 经理室",
        .paragraphs =
        {
            "电脑居然还亮着。进货记录里，灰雾日前 11 天有一批“市政公益供水”入库，"
            "供货方一栏写着 ECHO，经手人签名处是空白。",
            "记录最下方有一行未发出的留言：“他们说这是政府物资，可我从没见过"
            "送水的人，货是半夜自己出现在门口的。”"
        },
        .choices =
        {
            Choice{ .text = "记下记录，返回仓库",    .target = "market_warehouse" },
        }
    },
    Scene{
        .id = "market_roof",
        .title = "惠民超市 · 天台",
        .paragraphs =
        {
            "天台上风很大，吹散了些许灰雾。你第一次俯瞰整座城市：城北的信号塔、"
            "城东的供水厂、城西被铁丝网封死的红区，尽收眼底。",
            "你凭记忆在一张广告纸背面画出城区地图。"
        },
        .choices =
        {
            Choice{ .text = "（收好手绘地图）",      .target = "market_outside",
                    .grantedClue = "tool_map", .requireNone = { "tool_map" } },
            Choice{ .text = "直接沿消防梯下去",      .target = "market_outside" },
        }
    },
    Scene{
        .id = "death_market",
        .title = "",
        .paragraphs =
        {
            "那东西猛地抬头，速度快得不像活人。冰冷的手指扣住你的喉咙，你被拖进"
            "黑暗，咀嚼声变成了你自己的。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_water",
        .title = "",
        .paragraphs =
        {
            "水很凉，带着一丝微甜。半小时后，高热从脊柱炸开，你感到皮肤下有东西"
            "在游动、生长，喉咙里涌出不属于你的低语。水壶从手中滑落。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_coldroom",
        .title = "",
        .paragraphs =
        {
            "冷库门在身后咔嗒一声彻底锁死。寒气一点点渗进骨头，你拍打着门，"
            "声音被厚重的冰层吞没。黑暗中，连哈气都结成了霜。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 2】中央公园（6 个普通场景 + 2 个死亡场景）
    //==========================================================================
    Scene{
        .id = "park_entrance",
        .title = "中央公园 · 入口",
        .paragraphs =
        {
            "公园被灰雾吞没一半，成群的死鸟铺在草坪上，像一层灰色的雪。",
            "长椅上坐着一位一动不动的老人；灌木丛后传来低低的呜咽，夹杂着金属"
            "碰撞的轻响。更深处可见池塘与一座半掩的防空洞。"
        },
        .choices =
        {
            Choice{ .text = "蹲在灌木后观察",        .target = "park_bush" },
            Choice{ .text = "走向长椅上的老人",      .target = "park_oldman" },
            Choice{ .text = "前往池塘",              .target = "park_pond" },
            Choice{ .text = "前往防空洞",            .target = "park_bunker" },
            Choice{ .text = "返回避难所",            .target = "hub" },
        }
    },
    Scene{
        .id = "park_bush",
        .title = "中央公园 · 灌木丛",
        .paragraphs =
        {
            "你屏住呼吸。三只瘦骨嶙峋的流浪狗走出雾来，皮肤下有东西在不规则地"
            "起伏，像水流穿过薄冰。",
            "每只狗的左耳都钉着金属耳标：ECHO-K-07、K-08、K-09。",
            "它们不是被感染的流浪动物——它们是“编号”。"
        },
        .choices =
        {
            Choice{ .text = "（记下耳标）",          .target = "park_entrance",
                    .grantedClue = "B2", .requireNone = { "B2" } },
            Choice{ .text = "捡起石头驱赶它们",      .target = "death_park" },
        }
    },
    Scene{
        .id = "park_oldman",
        .title = "中央公园 · 长椅",
        .paragraphs =
        {
            "老人早已没有呼吸，怀里紧紧抱着孩子的照片，背面写着：“西边检查站有"
            "空投，别喝任何水，爸爸对不起你。”",
            "你把照片放回他怀里，站起身。"
        },
        .choices = { Choice{ .text = "回到公园入口", .target = "park_entrance" } }
    },
    Scene{
        .id = "park_pond",
        .title = "中央公园 · 池塘",
        .paragraphs =
        {
            "池塘水面漂浮着大片死鱼，鱼腹朝天地挤在岸边。一条鱼的鳃还在抽动，"
            "皮下组织呈青黑色丝状。",
            "池水看起来异常清澈，清澈得不正常。"
        },
        .choices =
        {
            Choice{ .text = "刮下一片鱼鳞收好",      .target = "park_pond_got",
                    .requireNone = { "tool_scales" } },
            Choice{ .text = "弯腰捧起池水查看",      .target = "death_pond" },
            Choice{ .text = "离开池塘",              .target = "park_entrance" },
        }
    },
    Scene{
        .id = "park_pond_got",
        .title = "中央公园 · 池塘",
        .paragraphs =
        {
            "你把鳞片装进空袋。鳞片在掌心微微发凉，青黑的纹路像极了 B1 瓶底的"
            "絮状物——它们来自同一个东西。"
        },
        .choices =
        {
            Choice{ .text = "（收好鳞片）",          .target = "park_pond",
                    .grantedClue = "tool_scales" },
        }
    },
    Scene{
        .id = "park_bunker",
        .title = "中央公园 · 防空洞",
        .paragraphs =
        {
            "防空洞铁门挂着密码锁，锁盘上有动物抓挠的痕迹。",
            "你想起耳标上的编号——K-07、K-08、K-09。"
        },
        .choices =
        {
            Choice{ .text = "输入耳标编号 070809",   .target = "park_bunker_open",
                    .requireAll = { "B2" } },
            Choice{ .text = "盲目尝试其他密码",      .target = "death_park" },
            Choice{ .text = "离开防空洞",            .target = "park_entrance" },
        }
    },
    Scene{
        .id = "park_bunker_open",
        .title = "中央公园 · 防空洞",
        .paragraphs =
        {
            "锁扣弹开。防空洞内整齐封存着一箱物资：罐头、净水片，还有一套叠得"
            "平整、从未拆封的全套防护服。",
            "墙上贴着一张旧地图，标注着 D 区地下设施的通风口，与外界的排水渠"
            "相连。"
        },
        .choices =
        {
            Choice{ .text = "（穿上防护服带走）",    .target = "park_entrance",
                    .grantedClue = "tool_protection", .requireNone = { "tool_protection" } },
            Choice{ .text = "记下通风口位置",        .target = "park_entrance" },
        }
    },
    Scene{
        .id = "death_park",
        .title = "",
        .paragraphs =
        {
            "响声在雾里传出很远。十几双眼睛在雾中同时亮起，它们从四面八方涌来，"
            "皮毛下是游动的青黑色血管——你这才明白，“三只”从来不是全部。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_pond",
        .title = "",
        .paragraphs =
        {
            "池水触手的瞬间，无数青黑色细丝缠上你的手腕，把你向水里拽。你看见"
            "水面下密密麻麻全是鱼——和一张张泡得发白的人脸。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 3】军警检查站（6 个普通场景 + 2 个死亡场景）
    //==========================================================================
    Scene{
        .id = "cp_outside",
        .title = "军警检查站 · 外围",
        .paragraphs =
        {
            "路障东倒西歪，一辆军车侧翻在路边，车门弹孔像一排黑色眼睛。",
            "指挥帐篷还在，里面的无线电亮着微弱红灯；军车后方是军械库帐篷，"
            "更深处有一道通向红区的密封铁门。"
        },
        .choices =
        {
            Choice{ .text = "搜查指挥帐篷",          .target = "cp_tent" },
            Choice{ .text = "调试无线电",            .target = "cp_radio" },
            Choice{ .text = "搜查侧翻的军车",        .target = "cp_vehicle" },
            Choice{ .text = "用门禁卡打开军械库",    .target = "cp_armory",
                    .requireAll = { "tool_keycard" } },
            Choice{ .text = "推开红区密封铁门",      .target = "cp_deepgate" },
            Choice{ .text = "返回避难所",            .target = "hub" },
        }
    },
    Scene{
        .id = "cp_tent",        .title = "检查站 · 指挥帐篷",
        .paragraphs =
        {
            "空投物资箱散落一地，最里面的铁箱还封着。",
            "帐篷角落，一颗拔掉保险的诡雷用细线连着箱盖，线在暗处闪着微光。"
        },
        .choices =
        {
            Choice{ .text = "小心收起油包里的传单",  .target = "cp_b3",
                    .requireNone = { "B3" } },
            Choice{ .text = "用力去拽最里面的箱子",  .target = "death_cp_trap" },
            Choice{ .text = "退出帐篷",              .target = "cp_outside" },
        }
    },
    Scene{
        .id = "cp_b3",
        .title = "检查站 · 传单",
        .paragraphs =
        {
            "传单印着红色通告：“代号【回声】行动：禁止饮用一切市政供水，居民"
            "就地等待转移。”",
            "空白处有人用铅笔写下一串频率与一句话：“他们说这是演习，可我见过"
            "他们在水箱里放东西。”"
        },
        .choices =
        {
            Choice{ .text = "（收好传单）",          .target = "cp_tent",
                    .grantedClue = "B3" },
        }
    },
    Scene{
        .id = "cp_radio",
        .title = "检查站 · 无线电",
        .paragraphs =
        {
            "喇叭里是杂乱电流声，偶尔夹杂变形的音节，像有人在很远的地方用密码"
            "说话。"
        },
        .choices =
        {
            Choice{ .text = "按传单频率与密码调试",  .target = "cp_a2",
                    .requireAll = { "B3" }, .requireNone = { "A2" } },
            Choice{ .text = "盲目旋转频率旋钮",      .target = "death_cp_radio" },
            Choice{ .text = "返回",                  .target = "cp_outside" },
        }
    },
    Scene{
        .id = "cp_a2",
        .title = "检查站 · 破译的通讯",
        .paragraphs =
        {
            "噪音突然分开，一个冷静的男声清晰传出：",
            "“……ECHO 样本已转移至 D 区地下设施，坐标 7-3-9。重复，零号宿主仍在"
            "锚点周边活动，一旦接触立即启动回溯协议……”",
            "录音在此循环。你记下了坐标——入口就在城市排水渠尽头。"
        },
        .choices =
        {
            Choice{ .text = "（记下坐标）",          .target = "cp_radio",
                    .grantedClue = "A2" },
        }
    },
    Scene{
        .id = "cp_vehicle",
        .title = "检查站 · 军车",
        .paragraphs =
        {
            "军车驾驶室被压得变形，仪表盘上插着一支完好的电子管；后排军官的"
            "遗体上，挂着一张军方门禁卡。"
        },
        .choices =
        {
            Choice{ .text = "拆下电子管",            .target = "cp_vehicle",
                    .grantedClue = "tool_radiopart", .requireNone = { "tool_radiopart" } },
            Choice{ .text = "取下门禁卡",            .target = "cp_vehicle",
                    .grantedClue = "tool_keycard",   .requireNone = { "tool_keycard" } },
            Choice{ .text = "离开军车",              .target = "cp_outside" },
        }
    },
    Scene{
        .id = "cp_armory",
        .title = "检查站 · 军械库",
        .paragraphs =
        {
            "军械库里大部分武器已被带走，只剩一支压在箱底的九二式手枪和两个"
            "弹匣，角落里还有几枚震撼弹。"
        },
        .choices =
        {
            Choice{ .text = "取走手枪和震撼弹",      .target = "cp_armory_got",
                    .requireNone = { "tool_weapon" } },
            Choice{ .text = "空手离开",              .target = "cp_outside" },
        }
    },
    Scene{
        .id = "cp_armory_got",
        .title = "检查站 · 军械库",
        .paragraphs = { "你把枪上膛、插在腰后。有了它，红区那道铁门似乎也不再那么可怕。" },
        .choices =
        {
            Choice{ .text = "（收好武器）",          .target = "cp_outside",
                    .grantedClue = "tool_weapon" },
        }
    },
    Scene{
        .id = "cp_deepgate",
        .title = "检查站 · 红区铁门",
        .paragraphs =
        {
            "铁门后是被铁丝网封死的红区，雾气浓得几乎看不见指尖，远处隐约有"
            "教堂的尖顶。"
        },
        .choices =
        {
            Choice{ .text = "全副武装，推门进入红区",.target = "redzone_street",
                    .requireAll = { "tool_weapon", "tool_protection" } },
            Choice{ .text = "没有把握，先退回去",    .target = "cp_outside" },
        }
    },
    Scene{
        .id = "death_cp_trap",
        .title = "",
        .paragraphs =
        {
            "细线在指缝间绷紧，你只来得及看清那是一颗老式手雷。白光吞没了帐篷，"
            "也吞没了这条时间线。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_cp_radio",
        .title = "",
        .paragraphs =
        {
            "电台发出尖锐啸叫。哨卡外的雾里，无数身影同时转向帐篷，你听见了"
            "奔跑声——不是一个人的，是几十个。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 4】社区诊所（4 个普通场景 + 2 个死亡场景）
    //==========================================================================
    Scene{
        .id = "clinic_lobby",
        .title = "社区诊所 · 大厅",
        .paragraphs =
        {
            "玻璃门碎了一半，药味和甜腻的腐味混在一起。",
            "治疗室门虚掩着，里面传来压抑喘息；档案室黑得没有一丝光；走廊尽头"
            "是检验室与贴着警示条的隔离区。"
        },
        .choices =
        {
            Choice{ .text = "推开治疗室的门",        .target = "clinic_treatment" },
            Choice{ .text = "摸黑走进档案室",        .target = "death_clinic_archive" },
            Choice{ .text = "进入检验室",            .target = "clinic_lab" },
            Choice{ .text = "穿防护服进入隔离区",    .target = "clinic_quarantine",
                    .requireAll = { "tool_protection" } },
            Choice{ .text = "返回避难所",            .target = "hub" },
        }
    },
    Scene{
        .id = "clinic_treatment",
        .title = "诊所 · 治疗室",
        .paragraphs =
        {
            "一个中年男人靠在药柜旁，腿被倒塌的铁架砸中，鲜血浸透裤腿。他看见"
            "你，又警惕地望向不断作响的大门。",
            "“别……别先管药，”他咬牙说，“门外那些东西，快进来了。”"
        },
        .choices =
        {
            Choice{ .text = "先撕外套为他包扎止血",  .target = "clinic_a1",
                    .requireNone = { "A1" } },
            Choice{ .text = "先翻他身后的药柜",      .target = "death_clinic_rob" },
            Choice{ .text = "离开治疗室",            .target = "clinic_lobby" },
        }
    },
    Scene{
        .id = "clinic_a1",
        .title = "诊所 · 早期病例",
        .paragraphs =
        {
            "你扎紧他的伤口。男人缓过一口气，从贴身口袋掏出一叠湿透的纸。",
            "“我爱人是流调员，失踪前留下的。所有早期病人同一天去了同一个供水点，"
            "那不是传染病该有的速度——他们像被同时按下了开关。”",
            "病历最后一页红笔圈着：ECHO？非自然传播？"
        },
        .choices =
        {
            Choice{ .text = "（收好病例）",          .target = "clinic_lobby",
                    .grantedClue = "A1" },
        }
    },
    Scene{
        .id = "clinic_lab",
        .title = "诊所 · 检验室",
        .paragraphs =
        {
            "显微镜下还残留着样本涂片。你用 B1 批号调来的水样记录比对，又举起"
            "超市冷库带来的冷冻试剂——涂片里的青黑丝状物，与两者完全吻合。",
            "检验单上写着：“非已知病原体，遇回溯性观测活性异常升高。”"
        },
        .choices =
        {
            Choice{ .text = "记下结果，返回大厅",    .target = "clinic_lobby",
                    .requireAll = { "B1" } },
            Choice{ .text = "没有依据，直接离开",    .target = "clinic_lobby" },
        }
    },
    Scene{
        .id = "clinic_quarantine",
        .title = "诊所 · 隔离区",
        .paragraphs =
        {
            "防护服里，你靠近隔离病房。玻璃后的早期病人还清醒，他看见你，反而"
            "平静地笑了：",
            "“你也在循环里，对不对？我数过，我是第 3 个‘最早’的病人，可每一次"
            "醒来，总有人比我更早。去医院停尸房看看吧，那里躺着第 0 个。”"
        },
        .choices =
        {
            Choice{ .text = "记下这条线索，退出隔离区", .target = "clinic_lobby" },
        }
    },
    Scene{
        .id = "death_clinic_archive",
        .title = "",
        .paragraphs =
        {
            "手电扫过铁皮柜，最里面的柜门后蹲着的东西猛地回头——白大褂还穿在"
            "身上，胸牌名字被血污盖住。你连呼救都来不及。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_clinic_rob",
        .title = "",
        .paragraphs =
        {
            "你越过他去抓抗生素。治疗室的门被撞开，他没有为你包扎，你也没有"
            "为他停留——门外的东西公平地收走了你们两个。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 5】居民楼（5 个普通场景 + 3 个死亡场景）
    //==========================================================================
    Scene{
        .id = "apt_lobby",
        .title = "居民楼 · 楼道",
        .paragraphs =
        {
            "楼道里贴满停水通知，最早的一张是灰雾日前 11 天。",
            "402 室门虚掩、飘出腐臭；地下室门缝透出一线摇晃灯光；楼梯通向天台"
            "水箱；501 室门紧闭、里面传来微弱敲击；设备间的门锈死了。"
        },
        .choices =
        {
            Choice{ .text = "推开 402 室",            .target = "apt_402" },
            Choice{ .text = "下地下室",              .target = "death_apt_basement" },
            Choice{ .text = "上天台",                .target = "apt_roof" },
            Choice{ .text = "敲 501 室的门",          .target = "apt_501" },
            Choice{ .text = "用撬棍开设备间",        .target = "apt_utility",
                    .requireAll = { "tool_crowbar" } },
            Choice{ .text = "返回避难所",            .target = "hub" },
        }
    },
    Scene{
        .id = "apt_402",
        .title = "居民楼 · 402 室",
        .paragraphs =
        {
            "桌上摊着一本日记，主人蜷在床上，早已没了声息。衣柜门半开，塞满"
            "没来得及带走的行李。"
        },
        .choices =
        {
            Choice{ .text = "阅读日记",              .target = "apt_a3",
                    .requireNone = { "A3" } },
            Choice{ .text = "翻找衣柜",              .target = "death_apt_loot" },
            Choice{ .text = "离开 402",              .target = "apt_lobby" },
        }
    },
    Scene{
        .id = "apt_a3",
        .title = "居民楼 · 日记",
        .paragraphs =
        {
            "字迹从工整变得潦草：",
            "“灰雾日前 5 天，穿白防护服的人在楼顶水箱作业，车上印着 ECHO，说是"
            "水质检修。”\n"
            "“前一天我喝了自来水，今天皮肤下有东西在动。水不是被动过手脚——是"
            "早就被写好了。”"
        },
        .choices =
        {
            Choice{ .text = "（收好日记）",          .target = "apt_lobby",
                    .grantedClue = "A3" },
        }
    },
    Scene{
        .id = "apt_roof",
        .title = "居民楼 · 天台水箱",
        .paragraphs =
        {
            "巨大的水箱内壁，在 A3 所述的“作业”位置，残留着一圈未被冲洗干净的"
            "青黑色附着物。"
        },
        .choices =
        {
            Choice{ .text = "刮取残留样本",          .target = "apt_roof_got",
                    .requireAll = { "A3" }, .requireNone = { "tool_tankres" } },
            Choice{ .text = "直接返回",              .target = "apt_lobby" },
        }
    },
    Scene{
        .id = "apt_roof_got",
        .title = "居民楼 · 天台",
        .paragraphs =
        {
            "残留物在袋中微微蠕动，与 B1 瓶底、诊所涂片里的东西一模一样。"
            "你离“是谁、何时、如何”又近了一步。"
        },
        .choices =
        {
            Choice{ .text = "（收好样本）",          .target = "apt_lobby",
                    .grantedClue = "tool_tankres" },
        }
    },
    Scene{
        .id = "apt_501",
        .title = "居民楼 · 501 室",
        .paragraphs =
        {
            "门后是一个腿部受伤的年轻男人，他警惕地举起碎玻璃，看清你没有恶意"
            "后才松了口气。",
            "“我知道很多他们的密码……但我快不行了，伤口感染了。”"
        },
        .choices =
        {
            Choice{ .text = "用急救药品为他处理伤口",.target = "apt_501_save",
                    .requireAll = { "tool_medicine" }, .requireNone = { "tool_codes" } },
            Choice{ .text = "空手强行闯入",          .target = "death_apt_501" },
            Choice{ .text = "退出去",                .target = "apt_lobby" },
        }
    },
    Scene{
        .id = "apt_501_save",
        .title = "居民楼 · 501 室",
        .paragraphs =
        {
            "你给他注射了抗生素。男人缓过来，把一本记满数字密码的小本塞给你：",
            "“供水厂中控密码、医院太平间门禁……都在里面。替我把真相带出去。”"
        },
        .choices =
        {
            Choice{ .text = "（收好密码本）",        .target = "apt_lobby",
                    .grantedClue = "tool_codes" },
        }
    },
    Scene{
        .id = "apt_utility",
        .title = "居民楼 · 设备间",
        .paragraphs =
        {
            "你撬开设备间，墙上挂着一把管钳扳手，供水总阀的结构图画在旁边。"
        },
        .choices =
        {
            Choice{ .text = "取走管钳扳手",          .target = "apt_utility_got",
                    .requireNone = { "tool_wrench" } },
            Choice{ .text = "直接离开",              .target = "apt_lobby" },
        }
    },
    Scene{
        .id = "apt_utility_got",
        .title = "居民楼 · 设备间",
        .paragraphs = { "有了管钳，供水厂的管道阀门就能由你亲手关闭。" },
        .choices =
        {
            Choice{ .text = "（收好扳手）",          .target = "apt_lobby",
                    .grantedClue = "tool_wrench" },
        }
    },
    Scene{
        .id = "death_apt_basement",
        .title = "",
        .paragraphs =
        {
            "应急灯熄灭又亮起，围成一圈的身影齐刷刷回头，每一张脸你都认识——"
            "是这栋楼的邻居们。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_apt_loot",
        .title = "",
        .paragraphs =
        {
            "行李哗啦砸在地上，床上的“尸体”猛地坐起——它一直醒着。腐臭的气息"
            "扑面而来。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_apt_501",
        .title = "",
        .paragraphs =
        {
            "你空手逼近，男人拼死挥动碎玻璃，扭打声引来了楼道里的东西。黑暗里，"
            "谁也没能活下去。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 6】地铁隧道（3 个普通场景 + 1 个死亡场景）
    //==========================================================================
    Scene{
        .id = "metro_entrance",
        .title = "地铁隧道 · 入口",
        .paragraphs =
        {
            "避难所下方的隧道漆黑一片，手电光里，轨道向两个方向延伸，一节废弃"
            "列车停在站台边。",
            "更深处有一间亮着微弱指示灯的信号室。"
        },
        .choices =
        {
            Choice{ .text = "登上废弃列车",          .target = "metro_train" },
            Choice{ .text = "进入信号室",            .target = "metro_signal" },
            Choice{ .text = "沿隧道向黑暗深处走",    .target = "death_metro" },
            Choice{ .text = "返回避难所",            .target = "hub" },
        }
    },
    Scene{
        .id = "metro_train",
        .title = "地铁隧道 · 废弃列车",
        .paragraphs =
        {
            "车厢座椅东倒西歪，维修工的工具箱还挂在扶手上，里面是一根生锈的"
            "撬棍。",
            "驾驶室的线路图显示：这条地铁维护线，一直通向 D 区地下设施的货运口。"
        },
        .choices =
        {
            Choice{ .text = "拿走撬棍",              .target = "metro_train",
                    .grantedClue = "tool_crowbar",    .requireNone = { "tool_crowbar" } },
            Choice{ .text = "记下线路图",            .target = "metro_entrance" },
        }
    },
    Scene{
        .id = "metro_signal",
        .title = "地铁隧道 · 信号室",
        .paragraphs =
        {
            "信号台缺了一支电子管。"
        },
        .choices =
        {
            Choice{ .text = "用电子管修好信号台",    .target = "metro_signal_on",
                    .requireAll = { "tool_radiopart" } },
            Choice{ .text = "返回",                  .target = "metro_entrance" },
        }
    },
    Scene{
        .id = "metro_signal_on",
        .title = "地铁隧道 · 信号室",
        .paragraphs =
        {
            "信号灯重新亮起，维护隧道的锁扣咔嗒弹开。你确认：穿过这条维护隧道，"
            "可以不经排水渠、直接抵达 D 区设施的货运口。",
            "（你已掌握实验室的另一条通路。）"
        },
        .choices =
        {
            Choice{ .text = "记下，返回隧道",        .target = "metro_entrance" },
        }
    },
    Scene{
        .id = "death_metro",
        .title = "",
        .paragraphs =
        {
            "你沿着轨道越走越深，手电骤然熄灭。黑暗中列车汽笛毫无征兆地响起——"
            "这个早已停运的世界里，不该还有列车。你被光圈吞没。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 7】排水渠 & D 区地下实验室（核心：10 个普通场景 + 4 个死亡场景）
    //==========================================================================
    Scene{
        .id = "sewer_entrance",
        .title = "排水渠 · 入口",
        .paragraphs =
        {
            "你顺着 A2 通讯的坐标找到排水渠入口，污水里漂着青黑色的絮状物，"
            "空气里有刺鼻的甜腥味。"
        },
        .choices =
        {
            Choice{ .text = "继续深入",              .target = "sewer_fork" },
            Choice{ .text = "返回避难所",            .target = "hub" },
        }
    },
    Scene{
        .id = "sewer_fork",
        .title = "排水渠 · 岔路",
        .paragraphs =
        {
            "渠水在此分岔：左侧是干涸的维护通道，右侧是被污水淹没的暗渠，"
            "隐约能看见后方合金门的轮廓。"
        },
        .choices =
        {
            Choice{ .text = "走干涸的维护通道",      .target = "lab_gate" },
            Choice{ .text = "强渡污水暗渠",          .target = "death_sewer" },
            Choice{ .text = "退回",                  .target = "sewer_entrance" },
        }
    },
    Scene{
        .id = "lab_gate",
        .title = "D 区设施 · 大门",
        .paragraphs =
        {
            "厚重的合金门紧闭，门禁读卡器上印着你已熟悉的 ECHO 标志；门旁的"
            "通风管道少了两颗螺丝。"
        },
        .choices =
        {
            Choice{ .text = "拧开通风管钻进去",      .target = "lab_corridor" },
            Choice{ .text = "用铁棍撬开合金门",      .target = "death_lab_gate" },
            Choice{ .text = "原路返回",              .target = "sewer_entrance" },
        }
    },
    Scene{
        .id = "lab_corridor",
        .title = "D 区设施 · 走廊",
        .paragraphs =
        {
            "你落进一条雪白走廊，培养舱整齐排列。指示牌指向五个方向：样本室、"
            "办公区、深冷库、生物收容区、主管室，走廊尽头是主控室。"
        },
        .choices =
        {
            Choice{ .text = "进入样本室",            .target = "lab_sample" },
            Choice{ .text = "进入办公区",            .target = "lab_office" },
            Choice{ .text = "进入深冷库",            .target = "lab_freezer" },
            Choice{ .text = "进入生物收容区",        .target = "lab_containment" },
            Choice{ .text = "进入主管室",            .target = "lab_director" },
            Choice{ .text = "进入走廊尽头的主控室",  .target = "lab_control" },
            Choice{ .text = "撤离设施",              .target = "sewer_entrance" },
        }
    },
    Scene{
        .id = "lab_sample",
        .title = "D 区设施 · 样本室",
        .paragraphs =
        {
            "培养舱内悬浮着青黑色絮状物，最近的舱体标签写着：ECHO-0 初代株，"
            "提取源——时间锚点介质，日期早于灰雾日 15 天。"
        },
        .choices =
        {
            Choice{ .text = "拍下全部样本标签",      .target = "lab_s1",
                    .requireNone = { "S1" } },
            Choice{ .text = "砸碎培养舱取样",        .target = "death_lab_containment" },
            Choice{ .text = "返回走廊",              .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_s1",
        .title = "D 区设施 · 样本标签",
        .paragraphs =
        {
            "没有一个字提到“武器”。这东西从诞生起就不是为了被投放——而是为了"
            "被“带回”。你把标签一张张拍进脑海。"
        },
        .choices =
        {
            Choice{ .text = "（收好线索）",          .target = "lab_corridor",
                    .grantedClue = "S1" },
        }
    },
    Scene{
        .id = "lab_office",
        .title = "D 区设施 · 办公区",
        .paragraphs =
        {
            "桌上文件散落一地，一台老式录音机还插着电，磁带停在开头。"
        },
        .choices =
        {
            Choice{ .text = "播放录音",              .target = "lab_s2",
                    .requireNone = { "S2" } },
            Choice{ .text = "返回走廊",              .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_s2",
        .title = "D 区设施 · 录音",
        .paragraphs =
        {
            "疲惫的女声在沙沙声中响起：“第 97 次循环记录。回声是时间介质，每一次"
            "回溯，病毒都跟着意识回到更早的时间线。零号宿主，是那个反复在锚点"
            "醒来的人。”",
            "“集齐全部真相后——永远不要再回来。”"
        },
        .choices =
        {
            Choice{ .text = "（收好录音）",          .target = "lab_corridor",
                    .grantedClue = "S2" },
        }
    },
    Scene{
        .id = "lab_freezer",
        .title = "D 区设施 · 深冷库",
        .paragraphs =
        {
            "库门需要抵御极低温度与高浓度污染。"
        },
        .choices =
        {
            Choice{ .text = "穿防护服进入",          .target = "lab_freezer_in",
                    .requireAll = { "tool_protection" } },
            Choice{ .text = "无防护硬闯",            .target = "death_lab_containment" },
            Choice{ .text = "返回走廊",              .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_freezer_in",
        .title = "D 区设施 · 深冷库",
        .paragraphs =
        {
            "冷气里冻着一排排标着循环编号的日志。最新一页写着：“第 0 次循环的"
            "样本，来自一个尚未发生的未来。病毒不是被‘发明’的，是被锚点从未来"
            "‘接’回来的。”",
            "你把日志与 S1、S2 默默对应。"
        },
        .choices =
        {
            Choice{ .text = "退出深冷库",            .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_containment",
        .title = "D 区设施 · 生物收容区",
        .paragraphs =
        {
            "厚重的隔离门后传来缓慢的撞击声，门缝渗出青黑色雾气。"
        },
        .choices =
        {
            Choice{ .text = "持枪进入",              .target = "lab_containment_in",
                    .requireAll = { "tool_weapon" } },
            Choice{ .text = "徒手进入",              .target = "death_lab_containment" },
            Choice{ .text = "返回走廊",              .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_containment_in",
        .title = "D 区设施 · 收容区",
        .paragraphs =
        {
            "隔离舱中央悬浮着一个人形轮廓，它的脸——和你一模一样。它隔着玻璃"
            "安静地看着你，口型说着同一句话：",
            "“我是上一次循环里，没能走出去的你。”",
            "你握紧枪，最终没有开火，把它的存在记在心里。"
        },
        .choices =
        {
            Choice{ .text = "退出收容区",            .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_director",
        .title = "D 区设施 · 主管室",
        .paragraphs =
        {
            "门禁需要军方磁卡。"
        },
        .choices =
        {
            Choice{ .text = "刷门禁卡进入",          .target = "lab_director_in",
                    .requireAll = { "tool_keycard" } },
            Choice{ .text = "返回走廊",              .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_director_in",
        .title = "D 区设施 · 主管室",
        .paragraphs =
        {
            "主管已伏在桌上，手边是一封未寄出的自白：",
            "“我们以为能从未来借回救命的技术，锚点打开的那一刻，接回来的却是"
            "病毒。它沿着回溯一路向上游传播，越想纠正，末日来得越早。能终结它的"
            "人，必须愿意在最后一条时间线里，不再复活。”"
        },
        .choices =
        {
            Choice{ .text = "收起自白，退出",        .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_control",
        .title = "D 区设施 · 主控室",
        .paragraphs =
        {
            "巨大的服务器无声运转，主屏上的 ECHO 计划总览需要三重验证：样本编号、"
            "录音密钥、以及一段尚未补齐的内容。"
        },
        .choices =
        {
            Choice{ .text = "输入样本编号与录音密钥",.target = "lab_s3",
                    .requireAll = { "S1", "S2" }, .requireNone = { "S3" } },
            Choice{ .text = "强行拔出存储阵列",      .target = "death_lab_pull" },
            Choice{ .text = "返回走廊",              .target = "lab_corridor" },
        }
    },
    Scene{
        .id = "lab_s3",
        .title = "D 区设施 · 真相文件",
        .paragraphs =
        {
            "最后一段文字缓缓展开：“病毒不通过空气、水或接触传播，它通过回溯传播。"
            "每一次死亡后的回溯，都是一次向过去的播种。”",
            "“终止方式只有一个：带着全部真相，停止回溯。哪怕这条时间线里的你，"
            "终将死去。”",
            "你站在屏幕前，终于懂了这座城市为何沦陷——它的末日，始于你一次次"
            "“醒来”。"
        },
        .choices =
        {
            Choice{ .text = "（收好真相文件）",      .target = "lab_corridor",
                    .grantedClue = "S3" },
        }
    },
    Scene{
        .id = "death_sewer",
        .title = "",
        .paragraphs =
        {
            "污水漫过口鼻，青黑色细丝顺着七窍钻入。你在自己的倒影里，看见无数张"
            "循环中沉没的脸。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_lab_gate",
        .title = "",
        .paragraphs =
        {
            "警报炸开，门缝喷出的白雾先一步把你吞没。你最后看见的，是自己的手臂"
            "以肉眼可见的速度变得透明。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_lab_containment",
        .title = "",
        .paragraphs =
        {
            "青黑色雾气裹住你，没有疼痛，只有一种被无数双眼睛同时注视的感觉——"
            "仿佛过去与未来的每一条时间线，都在这一刻看见了你。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_lab_pull",
        .title = "",
        .paragraphs =
        {
            "红色警报铺满房间，喷淋头喷出白雾，铁门一道道落下。你以为自己在"
            "毁掉真相，却只是毁掉了这条时间线里唯一的出口。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 8】红区（3 个普通场景 + 2 个死亡场景）
    //==========================================================================
    Scene{
        .id = "redzone_street",
        .title = "红区 · 封锁街道",
        .paragraphs =
        {
            "雾气浓得几乎看不见指尖，废弃车辆在雾里排成一条死寂的河。",
            "三条岔路分别通向尖顶的教堂、挂着彩旗的学校、以及一栋黑沉沉的医院；"
            "雾深处有缓慢拖动的声响。"
        },
        .choices =
        {
            Choice{ .text = "进入教堂",              .target = "redzone_church" },
            Choice{ .text = "前往学校",              .target = "redzone_school" },
            Choice{ .text = "前往医院",              .target = "hospital_lobby" },
            Choice{ .text = "循着拖动声走进雾深处",  .target = "death_redzone" },
            Choice{ .text = "退回检查站",            .target = "cp_deepgate" },
        }
    },
    Scene{
        .id = "redzone_church",
        .title = "红区 · 教堂",
        .paragraphs =
        {
            "教堂里聚着十几个幸存者，烛光摇曳。一位老神父把你领到告解室：",
            "“医院里躺着第 0 号病人，学校楼顶有还能用的救援电台。孩子，记住——"
            "在这里，真相比子弹更能救命。”"
        },
        .choices =
        {
            Choice{ .text = "记下情报，离开教堂",    .target = "redzone_street" },
        }
    },
    Scene{
        .id = "redzone_school",
        .title = "红区 · 学校",
        .paragraphs =
        {
            "教学楼里空无一人，楼顶广播室的设备半新，只是缺了一支关键电子管。",
            "操场方向传来成群游荡的脚步声。"
        },
        .choices =
        {
            Choice{ .text = "装上电子管，呼叫救援",  .target = "redzone_school_call",
                    .requireAll = { "tool_radiopart" } },
            Choice{ .text = "徒手爬上旗杆大声呼救",  .target = "death_redzone_school" },
            Choice{ .text = "离开学校",              .target = "redzone_street" },
        }
    },
    Scene{
        .id = "redzone_school_call",
        .title = "红区 · 学校",
        .paragraphs =
        {
            "电流声后，一个声音回应：“收到，救援直升机将在天亮抵达，请坚持。”",
            "你松了口气，却在设备玻璃的倒影里看见：自己的身后，空无一人的楼道"
            "站满了“人”。你忽然明白，这一幕你已经经历过很多次。"
        },
        .choices =
        {
            Choice{ .text = "退出广播室",            .target = "redzone_street" },
        }
    },
    Scene{
        .id = "death_redzone",
        .title = "",
        .paragraphs =
        {
            "拖动声越来越近，你走进雾最浓的地方。一张脸从雾里贴到你面前——它"
            "穿着和你一样的衣服，带着和你一样的伤痕，只是没有眼珠。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_redzone_school",
        .title = "",
        .paragraphs =
        {
            "你的呼喊在操场上空回荡，回应它的是齐刷刷抬起的头。无数身影从教学楼"
            "涌出，旗杆下的空地，成了这条时间线的终点。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 9】医院（4 个普通场景 + 2 个死亡场景）
    //==========================================================================
    Scene{
        .id = "hospital_lobby",
        .title = "医院 · 大厅",
        .paragraphs =
        {
            "大厅的挂号屏还亮着，滚动着永远叫不完的号。",
            "电梯停在底层，楼梯通向病房楼层；ICU 在走廊东侧；地下一层是停尸房，"
            "门禁锁着。"
        },
        .choices =
        {
            Choice{ .text = "前往病房楼层",          .target = "hospital_ward" },
            Choice{ .text = "穿防护服进入 ICU",      .target = "hospital_icu",
                    .requireAll = { "tool_protection" } },
            Choice{ .text = "用密码打开停尸房门禁",  .target = "hospital_morgue",
                    .requireAll = { "tool_codes" } },
            Choice{ .text = "徒手撬开停尸房门",      .target = "death_hospital" },
            Choice{ .text = "离开医院",              .target = "redzone_street" },
        }
    },
    Scene{
        .id = "hospital_ward",
        .title = "医院 · 病房",
        .paragraphs =
        {
            "病房床头卡上的入院时间，全部是灰雾日凌晨同一时刻，与 A1 病历完全"
            "吻合——“同时按下开关”。",
            "值班室白板上写着：“全部患者均有同一特征：体内检出‘回溯性标记’。”"
        },
        .choices =
        {
            Choice{ .text = "返回大厅",              .target = "hospital_lobby" },
        }
    },
    Scene{
        .id = "hospital_icu",
        .title = "医院 · ICU",
        .paragraphs =
        {
            "ICU 里，一位插着管的研究员尚有微弱意识，他看见防护服上的灰尘，"
            "艰难地开口：",
            "“停尸房……第 0 号……不是病人，是‘锚点’本身……别再让他醒来……”"
            "心电监护仪随即拉成一条直线。"
        },
        .choices =
        {
            Choice{ .text = "退出 ICU",              .target = "hospital_lobby" },
        }
    },
    Scene{
        .id = "hospital_morgue",
        .title = "医院 · 停尸房",
        .paragraphs =
        {
            "冷柜最深处，编号 000 的抽屉拉开：里面躺着一具盖着白布的躯体，"
            "胸前标签写着——“来源：时间锚点；身份：未知（与报告人面容一致）”。",
            "你颤抖着揭开白布，看见的是自己沉睡的脸。"
        },
        .choices =
        {
            Choice{ .text = "合上抽屉，退出停尸房",  .target = "hospital_lobby" },
        }
    },
    Scene{
        .id = "death_hospital",
        .title = "",
        .paragraphs =
        {
            "你用力撬门，停尸房的冷柜在身后一个个自行滑开。你回头，每一个抽屉里"
            "都躺着同一个人，他们同时睁开了眼。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_hospital_icu",
        .title = "",
        .paragraphs =
        {
            "没有防护服，ICU 高浓度的空气在你踏入的瞬间侵入肺腑。你看见自己的手"
            "迅速布满青黑纹路，监护仪上，你的心跳和那些患者并成了同一条线。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 10】信号塔（3 个普通场景 + 1 个死亡场景）
    //==========================================================================
    Scene{
        .id = "tower_base",
        .title = "信号塔 · 塔底",
        .paragraphs =
        {
            "锈红色的信号塔直插灰雾，塔底机房的门虚掩，旋转楼梯向上盘旋，"
            "望不到头。"
        },
        .choices =
        {
            Choice{ .text = "沿旋转楼梯向上攀爬",    .target = "tower_climb" },
            Choice{ .text = "返回",                  .target = "hub" },
        }
    },
    Scene{
        .id = "tower_climb",
        .title = "信号塔 · 楼梯",
        .paragraphs =
        {
            "风从钢梁间呼啸穿过，楼梯在脚下轻轻摇晃。你已攀到云端，下方城市"
            "缩成一片灰色的棋盘。"
        },
        .choices =
        {
            Choice{ .text = "稳住，继续登顶",        .target = "tower_top" },
            Choice{ .text = "俯身向下张望",          .target = "death_tower" },
            Choice{ .text = "退回塔底",              .target = "tower_base" },
        }
    },
    Scene{
        .id = "tower_top",
        .title = "信号塔 · 广播室",
        .paragraphs =
        {
            "塔顶广播室的发射机仍可使用，只是缺了电子管。在这里，声音可以覆盖"
            "整座城市。"
        },
        .choices =
        {
            Choice{ .text = "装上电子管，向全城广播真相", .target = "tower_broadcast",
                    .requireAll = { "tool_radiopart" } },
            Choice{ .text = "强行短接发射机",      .target = "death_tower" },
            Choice{ .text = "下楼",                .target = "tower_climb" },
        }
    },
    Scene{
        .id = "tower_broadcast",
        .title = "信号塔 · 广播",
        .paragraphs =
        {
            "你对着话筒，把“回声”、投毒、零号宿主与循环的一切，一字一句广播出去。",
            "你想象无数人在收音机前抬头——但你也清楚，若你此刻死去、再次回溯，"
            "这场广播将从未发生。要让真相留下，唯有让自己不再醒来。"
        },
        .choices =
        {
            Choice{ .text = "下楼，回到锚点",        .target = "tower_base" },
        }
    },
    Scene{
        .id = "death_tower",
        .title = "",
        .paragraphs =
        {
            "脚下一滑 / 电弧一闪，你从钢梁间坠落。风声里，城市的灯光在你眼前迅速"
            "放大，最后归于一片漆黑。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 11】城市供水厂（3 个普通场景 + 2 个死亡场景）
    //==========================================================================
    Scene{
        .id = "wp_gate",
        .title = "供水厂 · 大门",
        .paragraphs =
        {
            "供水厂安静得反常，蓄水池像一面面灰色的镜子。中控楼的大门锁着数字密码。"
        },
        .choices =
        {
            Choice{ .text = "用密码打开中控楼",      .target = "wp_tanks",
                    .requireAll = { "tool_codes" } },
            Choice{ .text = "翻越围墙进入",          .target = "death_wp" },
            Choice{ .text = "返回",                  .target = "hub" },
        }
    },
    Scene{
        .id = "wp_tanks",
        .title = "供水厂 · 水箱群",
        .paragraphs =
        {
            "巨大的水箱顶部，焊着一台不属于供水厂的“自动投放装置”，出料口残留着"
            "青黑色附着物。"
        },
        .choices =
        {
            Choice{ .text = "用天台残留样本比对",    .target = "wp_tanks_match",
                    .requireAll = { "tool_tankres" } },
            Choice{ .text = "用管钳关闭投放总阀",    .target = "wp_tanks_valve",
                    .requireAll = { "tool_wrench" } },
            Choice{ .text = "下到敞开的清水池查看",  .target = "death_wp_drown" },
            Choice{ .text = "进入中控室",            .target = "wp_control" },
        }
    },
    Scene{
        .id = "wp_tanks_match",
        .title = "供水厂 · 水箱群",
        .paragraphs =
        {
            "比对结果完全一致：投放装置的残留物，与居民楼水箱、超市瓶底、诊所涂片"
            "里的东西同源。",
            "你终于从“物证”上锁定了污染的起点。"
        },
        .choices =
        {
            Choice{ .text = "返回",                  .target = "wp_tanks" },
        }
    },
    Scene{
        .id = "wp_tanks_valve",
        .title = "供水厂 · 水箱群",
        .paragraphs =
        {
            "你用管钳死死拧住总阀，管道发出沉闷的轰鸣，投放装置的指示灯一盏盏"
            "熄灭。",
            "这一条时间线里，被污染的水终于停止了输送——虽然你知道，回溯之后，"
            "它仍会重来。"
        },
        .choices =
        {
            Choice{ .text = "返回",                  .target = "wp_tanks" },
        }
    },
    Scene{
        .id = "wp_control",
        .title = "供水厂 · 中控室",
        .paragraphs =
        {
            "电脑里的投放记录从灰雾日前 15 天开始，执行方一栏始终是 ECHO，"
            "触发条件写着一行冰冷的字：",
            "“每当锚点发生一次回溯，自动向对应时间线补投一次。”",
            "记录与 S3 互为印证——你越挣扎，它越严密。"
        },
        .choices =
        {
            Choice{ .text = "离开供水厂",            .target = "wp_gate" },
        }
    },
    Scene{
        .id = "death_wp",
        .title = "",
        .paragraphs =
        {
            "你翻上围墙，电网的蓝光在雾里一闪。你甚至没来得及感到疼痛，世界就"
            "连同电流一起，归于寂静。"
        },
        .choices = {}, .isDeath = true
    },
    Scene{
        .id = "death_wp_drown",
        .title = "",
        .paragraphs =
        {
            "清水池的水面下，青黑色细丝像水草一样缠上你的脚踝，把你缓缓拖向池底。"
            "你最后看见的，是水面上一圈圈扩散的、属于你自己的倒影。"
        },
        .choices = {}, .isDeath = true
    },

    //==========================================================================
    // 【区域 12】锚点核心（通往真结局）
    //==========================================================================
    Scene{
        .id = "core_gate",
        .title = "锚点核心 · 门前",
        .paragraphs =
        {
            "你循着九条线索的指引，来到避难所最深处一扇从未见过的门前。门后没有"
            "雾气，只有温暖而安静的光，像一个等待被结束的梦。",
            "门心刻着一行字：“入此门者，当舍一切重来。”"
        },
        .choices =
        {
            Choice{ .text = "推门，走入光中",        .target = "core_anchor" },
            Choice{ .text = "还没有准备好，退回去",  .target = "hub" },
        }
    },
    Scene{
        .id = "core_anchor",
        .title = "锚点核心",
        .paragraphs =
        {
            "光的中央，悬浮着这座城市所有时间线的“锚点”，它随着你的心跳轻轻搏动。",
            "你想起收容区里那个和你一模一样的人，想起停尸房里编号 000 的躯体，"
            "想起录音里那句“集齐之后，永远不要再回来”。",
            "你可以选择关闭它——这一次，你将不再醒来。"
        },
        .choices =
        {
            Choice{ .text = "亲手关闭锚点，终结一切循环", .target = "",
                    .endingId = "ending_true" },
            Choice{ .text = "再给自己一次“重来”",    .target = "hub" },
        }
    },

    // （场景表结束）
};

//==============================================================================
// 三、结局表
//==============================================================================
const std::vector<Ending> kEndings =
{
    //---------------- 基础线索结局 ------------------------------------------
    Ending{
        .id = "ending_basic",
        .title = "结局 · 漫长的等待",
        .paragraphs =
        {
            "你把三条基础线索在地上排开：被提前封装的水、带着编号的实验动物、"
            "印着“回声”的传单。",
            "你得出结论：这是一场通过水源扩散的瘟疫，政府正在救援。只要锁好门、"
            "不喝自来水、安静等待，黎明总会来。",
            "于是你锁死避难所的门，数着储备一天天等下去。",
            "门外的灰雾没有散。广播里的女声循环了无数遍，然后彻底沉默。你成了"
            "最后一个等待黎明的人——而黎明，永远停在了下一次循环里。",
            "【达成结局：漫长的等待】——帷幕仍未揭开，更深的真相在雾中等你。"
        }
    },
    //---------------- 进阶线索结局 ------------------------------------------
    Ending{
        .id = "ending_advanced",
        .title = "结局 · 撕幕者",
        .paragraphs =
        {
            "你把三条进阶线索与基础线索并排：被“同时按下”的开关、写着坐标的"
            "加密通讯、水箱上的作业记录。",
            "你得出结论：这不是天灾，是一场精心策划的人为泄露，代号“回声”。",
            "你冲回电台、登上信号塔，把真相向全城广播。你听见远处有人回应、有人"
            "哭泣、有人开始行动。",
            "然后你死了——总会死的。你再次在锚点醒来，广播从未发生，世界从未"
            "听过。你在无数条时间线里重复着同一场冲锋，而真相永远停在下一次循环。",
            "【达成结局：撕幕者】——你撕开了帷幕，却还走不出帷幕。"
        }
    },
    //---------------- 真结局（三档全收集）----------------------------------
    Ending{
        .id = "ending_true",
        .title = "真结局 · 锚点之外",
        .paragraphs =
        {
            "九条线索在你脑海中拼成完整图景：病毒通过回溯传播，你是零号宿主，"
            "你每一次“醒来”，都是向过去的一次播种。",
            "这一次，你没有冲向任何一个方向。",
            "你把所有线索、坐标、密码和真相封进防水箱，交给在诊所里活下来的男人——"
            "他的爱人，那位流调员，会知道该怎么做。",
            "然后你回到锚点核心，在光中平躺下来，亲手关闭了它。你感到意识在下沉，"
            "像一条终于停摆的钟。",
            "这一次，你没有再醒来。",
            "……时间线第一次不再被改写。灰雾没有被“带回”过去，于是它在这条"
            "时间线里，被活着的人一点点击退。",
            "多年以后，重建的城市里，孩子们在干净的喷泉边追逐。公园的新长椅上"
            "钉着一块铜牌，上面写着：",
            "“献给那个没有回来的人。是你让明天，终于只属于明天。”",
            "【达成真结局：锚点之外】——感谢你走完了《末日回声》。"
        }
    },
};

} // anonymous namespace

//==============================================================================
// 四、只读查询接口实现
//==============================================================================
const std::vector<Clue>& clues() { return kClues; }
const std::vector<Scene>& scenes() { return kScenes; }
const std::vector<Ending>& endings() { return kEndings; }

const Clue* findClue(std::string_view id)
{
    auto it = std::find_if(kClues.begin(), kClues.end(),
        [&](const Clue& c) { return c.id == id; });
    return it == kClues.end() ? nullptr : &*it;
}

const Scene* findScene(std::string_view id)
{
    auto it = std::find_if(kScenes.begin(), kScenes.end(),
        [&](const Scene& s) { return s.id == id; });
    return it == kScenes.end() ? nullptr : &*it;
}

const Ending* findEnding(std::string_view id)
{
    auto it = std::find_if(kEndings.begin(), kEndings.end(),
        [&](const Ending& e) { return e.id == id; });
    return it == kEndings.end() ? nullptr : &*it;
}

std::size_t clueCountOfTier(ClueTier tier)
{
    return static_cast<std::size_t>(
        std::count_if(kClues.begin(), kClues.end(),
            [&](const Clue& c) { return c.tier == tier; }));
}

} // namespace story
