QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    _UI/EncyclopediaData.cpp \
    _UI/EncyclopediaDialog.cpp \
    _UI/gameWidght.cpp \
    _UI/menuWidget.cpp \
    _UI/settingsWidget.cpp \
    _bullet/areadamagebullet.cpp \
    _bullet/bullet.cpp \
    _bullet/bulletmanager.cpp \
    _monster/monster.cpp \
    _monster/monsterSpawner.cpp \
    _monster/normalmonster.cpp \
    _monster/flymonsters.cpp \
    _others/global.cpp \
    _others/main.cpp \
    _others/map.cpp \
    _others/playerbase.cpp \
    _others/utils.cpp \
    _player_and_resource/player.cpp \
    _player_and_resource/playerUI.cpp \
    _player_and_resource/resource.cpp \
    _player_and_resource/resourcemanager.cpp \
    _tower/buildmanager.cpp \
    _tower/explorationmarker.cpp \
    _tower/previewtower.cpp \
    _tower/tower.cpp \
    _tower/towerButton.cpp \


HEADERS += \
    _UI/EncyclopediaData.h \
    _UI/EncyclopediaDialog.h \
    _UI/gameWidght.h \
    _UI/menuWidget.h \
    _UI/saveConflictWidget.h \
    _UI/settingsWidget.h \
    _bullet/areadamagebullet.h \
    _bullet/bullet.h \
    _bullet/bulletmanager.h \
    _monster/monster.h \
    _monster/monsterSpawner.h \
    _monster/normalmonster.h \
    _monster/flymonsters.h \
    _others/global.h \
    _others/map.h \
    _others/playerbase.h \
    _others/structVector.h \
    _others/utils.h \
    _player_and_resource/player.h \
    _player_and_resource/playerUI.h \
    _player_and_resource/resource.h \
    _player_and_resource/resourcemanager.h \
    _tower/buildmanager.h \
    _tower/explorationmarker.h \
    _tower/previewtower.h \
    _tower/tower.h \
    _tower/towerButton.h \


FORMS += \
    _UI/gameWidght.ui \
    _UI/menuWidget.ui \
    _UI/gameWidght.ui \
    _UI/menuWidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    png.qrc

DISTFILES += \
    tileData.txt \
    tileState.txt
