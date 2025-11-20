cmake_minimum_required(VERSION 3.16)

# ��Ŀ����
project(Dream LANGUAGES CXX)

# ���� C++ ��׼
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ������ǰĿ¼������ #include "MyClass.h"
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# �Զ����� Qt �� MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ���� Qt6 �ĺ���ģ��
# ȷ��������Ŀ��Ҫ��ģ�鶼�������г�
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia)

# ������ִ���ļ�����Դ�ļ�
add_executable(Dream WIN32
        main.cpp
        MainWindow.h
        MainWindow.cpp
        GameManager.h
        GameManager.cpp
        Player.h
        Player.cpp
        Tower.h
        Tower.cpp
        Enemy.h
        Enemy.cpp
        Bullet.h
        Bullet.cpp
        WaveManager.h
        WaveManager.cpp
        LevelEditorWidget.h
        LevelEditorWidget.cpp
        LevelLoader.h
        LevelLoader.cpp
        Obstacle.cpp
        Obstacle.h
        GameMap.h
        GameMap.cpp
        resources.qrc
        enemy_data.json
        tower_data.json
        auto_widget.cpp
        auto_widget.h
        widget_menu.h widget_menu.cpp widget_menu.ui
        widget_choose_level.h widget_choose_level.cpp widget_choose_level.ui
        widget_level_loading.h widget_level_loading.cpp widget_level_loading.ui
        widget_ingame.h widget_ingame.cpp widget_ingame.ui
        IconButton.h IconButton.cpp
        widget_post_game.h widget_post_game.cpp widget_post_game.ui
        InspirationBulb.h
        widget_building_list.cpp widget_building_list.h widget_building_list.ui
        InspirationBulb.h
        InspirationBulb.cpp
        KnowledgeTree.h
        KnowledgeTree.cpp
        FishingCatPillow.h
        FishingCatPillow.cpp
        AbilitiesImpl.h
        EnemyAbility.h
        LiveCoffee.h
        LiveCoffee.cpp
        NightRadio.h
        NightRadio.cpp
        PettingCatTime.h
        PettingCatTime.cpp
        WarmMemory.h
        WarmMemory.cpp
        FriendCompanion.h
        FriendCompanion.cpp
        widget_ingame.cpp
        widget_ingame.h
        widget_ingame.ui
        auto_widget.h
        auto_widget.cpp
        AbilitiesImpl.h
        EnemyAbility.h
        LiveCoffee.h
        LiveCoffee.cpp
        NightRadio.h
        NightRadio.cpp
        PettingCatTime.h
        PettingCatTime.cpp
        WarmMemory.h
        WarmMemory.cpp
        widget_pause_menu.h widget_pause_menu.cpp widget_pause_menu.ui
        widget_reference_book.h widget_reference_book.cpp widget_reference_book.ui
        # ������ .ui �� .qrc �ļ���Ҳ����������
        # MainWindow.ui
        # resources.qrc
)

target_link_libraries(Dream PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Multimedia)
target_compile_definitions(Dream PRIVATE PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")