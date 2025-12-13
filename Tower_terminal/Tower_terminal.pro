QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bullet.cpp \
    bulletmanager.cpp \
    gameWidght.cpp \
    global.cpp \
    main.cpp \
    map.cpp \
    menuWidget.cpp \
    monster.cpp \
    monsterSpawner.cpp \
    player.cpp \
    playerUI.cpp \
    resource.cpp \
    resourcemanager.cpp \
    tower.cpp \
    towerButton.cpp \
    utils.cpp \

HEADERS += \
    bullet.h \
    bulletmanager.h \
    gameWidght.h \
    global.h \
    map.h \
    menuWidget.h \
    monster.h \
    monsterSpawner.h \
    player.h \
    playerUI.h \
    resource.h \
    resourcemanager.h \
    structVector.h \
    tower.h \
    towerButton.h \
    utils.h \

FORMS += \
    gameWidght.ui \
    menuWidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    png.qrc
