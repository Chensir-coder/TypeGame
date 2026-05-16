#ifndef __TYPEGAME_ASSET_PATHS_H__
#define __TYPEGAME_ASSET_PATHS_H__

#include <QString>

/* ------------------------------------------------------------------
 // 文件名     : asset_paths.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : UI资源路径定义
------------------------------------------------------------------ */

namespace UiAssetPaths
{
namespace Common
{
inline QString mainDialogBackground() { return QStringLiteral(":/Common/Images/MAIN_DLG_BG.png"); }
inline QString startButton() { return QStringLiteral(":/Common/Images/PUBLIC_START.png"); }
inline QString pauseButton() { return QStringLiteral(":/Common/Images/PUBLIC_PAUSE.png"); }
inline QString setupButton() { return QStringLiteral(":/Common/Images/PUBLIC_SETUP.png"); }
inline QString endButton() { return QStringLiteral(":/Common/Images/PUBLIC_END.png"); }
inline QString exitButton() { return QStringLiteral(":/Common/Images/PUBLIC_EXIT.png"); }
inline QString okButton() { return QStringLiteral(":/Common/Images/OK.png"); }
inline QString cancelButton() { return QStringLiteral(":/Common/Images/CANCEL.png"); }

inline QString btnClickSound() { return QStringLiteral(":/Common/Sound/BTN_CLICK.wav"); }
inline QString glideSound() { return QStringLiteral(":/Common/Sound/GLIDE.wav"); }
inline QString typeSound() { return QStringLiteral(":/Common/Sound/TYPE.wav"); }
inline QString defaultImage() { return QStringLiteral(":/Common/Images/DEFAULT.png"); }
inline QString mainDialogExit() { return QStringLiteral(":/Common/Images/MAIN_DLG_EXIT.png"); }
inline QString mainDialogReplay() { return QStringLiteral(":/Common/Images/MAIN_DLG_REPLAY.png"); }
inline QString noButton() { return QStringLiteral(":/Common/Images/NO.png"); }
inline QString sliderBackground() { return QStringLiteral(":/Common/Images/SLIDER_BG.png"); }
inline QString sliderHandle() { return QStringLiteral(":/Common/Images/SLIDER_SLIDER.png"); }
inline QString yesButton() { return QStringLiteral(":/Common/Images/YES.png"); }
inline QString aniButtonClickSound() { return QStringLiteral(":/Common/Sounds/ANIBTN_CLICK.wav"); }
inline QString aniButtonEnterSound() { return QStringLiteral(":/Common/Sounds/ANIBTN_ENTER.wav"); }

}

namespace Apple
{
inline QString background() { return QStringLiteral(":/Apple/Images/APPLE_BACKGROUND.png"); }
inline QString basket() { return QStringLiteral(":/Apple/Images/APPLE_BASKET.png"); }
inline QString normalApple() { return QStringLiteral(":/Apple/Images/APPLE_NORMAL.png"); }
inline QString badApple() { return QStringLiteral(":/Apple/Images/APPLE_BAD.png"); }
inline QString smallApple() { return QStringLiteral(":/Apple/Images/APPLE_SMALL.png"); }
inline QString setupIcon() { return QStringLiteral(":/Apple/Images/APPLE_SETUP.png"); }
inline QString dialogBackground() { return QStringLiteral(":/Apple/Images/APPLE_DLG_BG.png"); }
inline QString nextButton() { return QStringLiteral(":/Apple/Images/APPLE_DLG_NEXT.png"); }
inline QString endButton() { return QStringLiteral(":/Apple/Images/APPLE_DLG_END.png"); }
inline QString retryButton() { return QStringLiteral(":/Apple/Images/APPLE_DLG_REPLAY.png"); }

inline QString backgroundSound() { return QStringLiteral(":/Apple/Sounds/APPLE_BG.wav"); }
inline QString inSound() { return QStringLiteral(":/Apple/Sounds/APPLE_IN.wav"); }
}

namespace SpaceWar
{
inline QString mainMenuBackground() { return QStringLiteral(":/Spacewar/Images/SPACE_MAINMENU_BG.png"); }
inline QString background() { return QStringLiteral(":/Spacewar/Images/SPACE_BACKGROUND.png"); }
inline QString startButton() { return QStringLiteral(":/Spacewar/Images/SPACE_START.png"); }
inline QString optionButton() { return QStringLiteral(":/Spacewar/Images/SPACE_OPTION.png"); }
inline QString exitButton() { return QStringLiteral(":/Spacewar/Images/SPACE_EXIT.png"); }
inline QString appleSetupIcon() { return QStringLiteral(":/Spacewar/Images/APPLE_SETUP.png"); }
inline QString checkboxButton() { return QStringLiteral(":/Spacewar/Images/CHECKBOX_BUTTON.png"); }
inline QString bomb() { return QStringLiteral(":/Spacewar/Images/SPACE_BOMB.png"); }
inline QString captionBack() { return QStringLiteral(":/Spacewar/Images/SPACE_CAPTION_BACK.png"); }
inline QString enemy0() { return QStringLiteral(":/Spacewar/Images/SPACE_ENEMY_0.png"); }
inline QString enemy4() { return QStringLiteral(":/Spacewar/Images/SPACE_ENEMY_4.png"); }
inline QString explosion0() { return QStringLiteral(":/Spacewar/Images/SPACE_EXPLOSION_0.png"); }
inline QString hiscore() { return QStringLiteral(":/Spacewar/Images/SPACE_HISCORE.png"); }
inline QString hiscoreBackground() { return QStringLiteral(":/Spacewar/Images/SPACE_HISCORE_BG.png"); }
inline QString labelLife() { return QStringLiteral(":/Spacewar/Images/SPACE_LABEL_LIFE.png"); }
inline QString labelScore() { return QStringLiteral(":/Spacewar/Images/SPACE_LABEL_SCORE.png"); }
inline QString labelTime() { return QStringLiteral(":/Spacewar/Images/SPACE_LABEL_TIME.png"); }
inline QString lifeIcon() { return QStringLiteral(":/Spacewar/Images/SPACE_LIFE.png"); }
inline QString lifeOver() { return QStringLiteral(":/Spacewar/Images/SPACE_LIFE_OVER.png"); }
inline QString returnButton() { return QStringLiteral(":/Spacewar/Images/SPACE_RETURN.png"); }
inline QString reward() { return QStringLiteral(":/Spacewar/Images/SPACE_REWARD.png"); }
inline QString ship() { return QStringLiteral(":/Spacewar/Images/SPACE_SHIP.png"); }
inline QString stars() { return QStringLiteral(":/Spacewar/Images/SPACE_STARS.png"); }
inline QString backgroundSound() { return QStringLiteral(":/Spacewar/Sounds/SPACE_BG.wav"); }
inline QString blastSound() { return QStringLiteral(":/Spacewar/Sounds/SPACE_BLAST.wav"); }
inline QString planeOutSound() { return QStringLiteral(":/Spacewar/Sounds/SPACE_PLANEOUT.wav"); }
inline QString shootSound() { return QStringLiteral(":/Spacewar/Sounds/SPACE_SHOOT.wav"); }
inline QString wordOutSound() { return QStringLiteral(":/Spacewar/Sounds/SPACE_WORDOUT.wav"); }
inline QString upgradeSound() { return QStringLiteral(":/Spacewar/Sounds/UPGRADE.wav"); }
}
}

#endif // __TYPEGAME_ASSET_PATHS_H__
