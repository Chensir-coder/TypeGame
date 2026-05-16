#include "typing_game_stats.h"

#include <algorithm>

TypingGameStats::TypingGameStats()
{
    reset();
}

void TypingGameStats::reset()
{
    m_score = 0;
    m_successCount = 0;
    m_missCount = 0;
    m_inputCount = 0;
    m_rejectedInputCount = 0;
    m_typingKeystrokeTotal = 0;
    m_typingKeystrokeCorrect = 0;
}

int TypingGameStats::score() const
{
    return m_score;
}

void TypingGameStats::setScore(int score)
{
    m_score = std::max(0, score);
}

int TypingGameStats::successCount() const
{
    return m_successCount;
}

int TypingGameStats::missCount() const
{
    return m_missCount;
}

int TypingGameStats::inputCount() const
{
    return m_inputCount;
}

int TypingGameStats::rejectedInputCount() const
{
    return m_rejectedInputCount;
}

int TypingGameStats::accuracy() const
{
    const int total = m_successCount + m_missCount;

    if (total <= 0) {
        return 0;
    }

    return (m_successCount * 100) / total;
}

void TypingGameStats::recordTypingKeystroke(bool correct)
{
    ++m_typingKeystrokeTotal;
    if (correct) {
        ++m_typingKeystrokeCorrect;
    }
}

int TypingGameStats::typingKeystrokeTotal() const
{
    return m_typingKeystrokeTotal;
}

int TypingGameStats::typingKeystrokeCorrect() const
{
    return m_typingKeystrokeCorrect;
}

int TypingGameStats::typingInputAccuracyPercent() const
{
    if (m_typingKeystrokeTotal <= 0) {
        return 0;
    }

    return (m_typingKeystrokeCorrect * 100) / m_typingKeystrokeTotal;
}

void TypingGameStats::addScore(int value)
{
    if (value <= 0) {
        return;
    }

    m_score += value;
}

void TypingGameStats::reduceScore(int value)
{
    if (value <= 0) {
        return;
    }

    m_score = std::max(0, m_score - value);
}

void TypingGameStats::recordSuccess(int scoreValue)
{
    ++m_successCount;
    ++m_inputCount;

    addScore(scoreValue);
}

void TypingGameStats::recordMiss(int penaltyValue)
{
    ++m_missCount;

    reduceScore(penaltyValue);
}

void TypingGameStats::recordRejectedInput()
{
    ++m_inputCount;
    ++m_rejectedInputCount;
}

bool TypingGameStats::isTargetCompleted(int targetSuccessCount) const
{
    return targetSuccessCount > 0 && m_successCount >= targetSuccessCount;
}

bool TypingGameStats::isMissLimitReached(int maxMissCount) const
{
    return maxMissCount > 0 && m_missCount >= maxMissCount;
}
