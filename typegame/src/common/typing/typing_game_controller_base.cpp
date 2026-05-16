#include "typing_game_controller_base.h"

#include "common/typing/typing_game_model_base.h"

TypingGameControllerBase::TypingGameControllerBase(TypingGameModelBase* model)
    : m_model(model)
{
}

void TypingGameControllerBase::initialize()
{
    if (m_model == nullptr)
    {
        return;
    }

    m_model->initialize();

    m_model->config().setSceneWidth(static_cast<int>(m_sceneRect.width()));
    m_model->config().setSceneHeight(static_cast<int>(m_sceneRect.height()));

    m_initialized = true;
}

void TypingGameControllerBase::startGame()
{
    if (m_model == nullptr)
    {
        return;
    }

    if (!m_initialized)
    {
        initialize();
    }

    m_model->startGame();
}

void TypingGameControllerBase::pauseGame()
{
    if (m_model == nullptr)
    {
        return;
    }

    m_model->pauseGame();
}

void TypingGameControllerBase::resumeGame()
{
    if (m_model == nullptr)
    {
        return;
    }

    m_model->resumeGame();
}

void TypingGameControllerBase::stopGame()
{
    if (m_model == nullptr)
    {
        return;
    }

    m_model->stopGame();
}

void TypingGameControllerBase::restartGame()
{
    if (m_model == nullptr)
    {
        return;
    }

    m_model->resetModel();

    m_model->config().setSceneWidth(static_cast<int>(m_sceneRect.width()));
    m_model->config().setSceneHeight(static_cast<int>(m_sceneRect.height()));

    m_model->startGame();
}

void TypingGameControllerBase::update(int delta_ms)
{
    if (m_model == nullptr)
    {
        return;
    }

    if (delta_ms <= 0)
    {
        return;
    }

    m_model->update(delta_ms);

    onUpdate(delta_ms);
}

bool TypingGameControllerBase::handleLetterInput(QChar input_letter)
{
    if (m_model == nullptr)
    {
        return false;
    }

    const bool base_handled = m_model->handleInput(input_letter);

    return onLetterInput(input_letter, base_handled);
}

void TypingGameControllerBase::setSceneSize(int width, int height)
{
    if (width <= 0 || height <= 0)
    {
        return;
    }

    m_sceneRect = QRectF(0, 0, width, height);

    if (m_model != nullptr)
    {
        m_model->config().setSceneWidth(width);
        m_model->config().setSceneHeight(height);
    }

    onSceneSizeChanged(m_sceneRect);
}

QRectF TypingGameControllerBase::sceneRect() const
{
    return m_sceneRect;
}

TypingGameModelBase* TypingGameControllerBase::model() const
{
    return m_model;
}

void TypingGameControllerBase::onUpdate(int delta_ms)
{
    Q_UNUSED(delta_ms);
}

bool TypingGameControllerBase::onLetterInput(QChar input_letter, bool base_handled)
{
    Q_UNUSED(input_letter);
    return base_handled;
}

void TypingGameControllerBase::onSceneSizeChanged(const QRectF& scene_rect)
{
    Q_UNUSED(scene_rect);
}
