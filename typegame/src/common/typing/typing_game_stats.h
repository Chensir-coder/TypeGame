#ifndef __TYPEGAME_TYPING_GAME_STATS_H__
#define __TYPEGAME_TYPING_GAME_STATS_H__


/* ------------------------------------------------------------------
 // 文件名     : typing_game_stats.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : typing_game_stats 头文件声明
------------------------------------------------------------------ */

class TypingGameStats
{
public:
    TypingGameStats();

    void reset();

    int score() const;
    void setScore(int score);

    int successCount() const;
    int missCount() const;
    int inputCount() const;
    int rejectedInputCount() const;

    int accuracy() const;

    /**
     * 逐键输入统计（与 success/miss 计分解耦，供太空大战等模式在专用输入路径中记录）。
     * 输入准确率 = correct / total * 100（total 为 0 时由界面显示为 —）。
     */
    void recordTypingKeystroke(bool correct);
    int typingKeystrokeTotal() const;
    int typingKeystrokeCorrect() const;
    int typingInputAccuracyPercent() const;

    void addScore(int value);
    void reduceScore(int value);

    void recordSuccess(int scoreValue);
    void recordMiss(int penaltyValue);
    void recordRejectedInput();

    bool isTargetCompleted(int targetSuccessCount) const;
    bool isMissLimitReached(int maxMissCount) const;

private:
    int m_score = 0;
    int m_successCount = 0;
    int m_missCount = 0;
    int m_inputCount = 0;
    int m_rejectedInputCount = 0;

    int m_typingKeystrokeTotal = 0;
    int m_typingKeystrokeCorrect = 0;
};

#endif // __TYPEGAME_TYPING_GAME_STATS_H__
