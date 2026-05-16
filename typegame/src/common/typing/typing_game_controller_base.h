#ifndef __TYPEGAME_TYPING_GAME_CONTROLLER_BASE_H__
#define __TYPEGAME_TYPING_GAME_CONTROLLER_BASE_H__

#include <QChar>
#include <QRectF>

#include "common/core/igame_controller.h"

/* ------------------------------------------------------------------
 // 文件名     : typing_game_controller_base.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : typing_game_controller_base 头文件声明
------------------------------------------------------------------ */

class TypingGameModelBase;

class TypingGameControllerBase : public IGameController
{
public:
    explicit TypingGameControllerBase(TypingGameModelBase* model);
    ~TypingGameControllerBase() override = default;

    TypingGameControllerBase(const TypingGameControllerBase&) = delete;
    TypingGameControllerBase& operator=(const TypingGameControllerBase&) = delete;

    void initialize() override;
    void startGame() override;
    void pauseGame() override;
    void resumeGame() override;
    void stopGame() override; 
    void restartGame() override;

    void update(int delta_ms) override;
    bool handleLetterInput(QChar input_letter) override;

    void setSceneSize(int width, int height);
    QRectF sceneRect() const;

protected:
    TypingGameModelBase* model() const;

    virtual void onUpdate(int delta_ms);
    virtual bool onLetterInput(QChar input_letter, bool base_handled);
    virtual void onSceneSizeChanged(const QRectF& scene_rect);

private:
    TypingGameModelBase* m_model = nullptr;
    bool m_initialized = false;

    QRectF m_sceneRect = QRectF(0, 0, 800, 1000);
};

#endif // __TYPEGAME_TYPING_GAME_CONTROLLER_BASE_H__
