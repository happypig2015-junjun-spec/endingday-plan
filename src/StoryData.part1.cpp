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
        .id = "cp_tent",