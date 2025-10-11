cmake_minimum_required(VERSION 3.16)

# 项目名称
project(Dream LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含当前目录，用于 #include "MyClass.h"
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 自动处理 Qt 的 MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 查找 Qt6 的核心模块
# 确保你的项目需要的模块都在这里列出
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# 定义可执行文件及其源文件
add_executable(Dream
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
        # 如果有 .ui 和 .qrc 文件，也在这里添加
        # MainWindow.ui
        # resources.qrc
)

# 链接 Qt6 的库到你的项目中
target_link_libraries(Dream PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)