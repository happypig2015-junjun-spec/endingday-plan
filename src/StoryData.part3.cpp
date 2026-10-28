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
