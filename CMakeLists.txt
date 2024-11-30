cmake_minimum_required(VERSION 3.14)

# 项目信息
project(QtOpenGLDemo LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt包
find_package(Qt6 COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets REQUIRED)

# 添加源文件
set(SOURCES
    Camera.cpp
    OpenGLWidget.cpp
    main.cpp
    QtOpenGLDemo.cpp
)

# 添加头文件
set(HEADERS
    Camera.h
    OpenGLWidget.h
    QtOpenGLDemo.h
)

# 添加UI文件
set(FORMS
    QtOpenGLDemo.ui
)

# 添加资源文件
set(RESOURCES
    resources.qrc
)

# 生成UI头文件
qt6_wrap_ui(UI_HEADERS ${FORMS})

# 生成资源文件
qt6_add_resources(RESOURCE_FILES ${RESOURCES})

# 生成moc文件
qt6_wrap_cpp(MOC_SOURCES ${HEADERS})

# 添加可执行文件
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UI_HEADERS}
    ${RESOURCE_FILES}
    ${MOC_SOURCES}
)

# 链接Qt库
target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets)

# 安装目标
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

