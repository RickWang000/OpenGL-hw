#include "OpenGLWidget.h"
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

void CoreFunctionWidget::loadConfig() {
    QFile file(":/config.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open config file!";
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject json = doc.object();

    // 读取 cube1 配置
    QJsonObject cube1 = json["cube1"].toObject();
    cube1Size = cube1["size"].toDouble();
    cube1Position = QVector3D(cube1["position"].toArray()[0].toDouble(),
                              cube1["position"].toArray()[1].toDouble(),
                              cube1["position"].toArray()[2].toDouble());
    cube1Rotation = QVector3D(cube1["rotation"].toArray()[0].toDouble(),
                              cube1["rotation"].toArray()[1].toDouble(),
                              cube1["rotation"].toArray()[2].toDouble());
    cube1Color = QVector3D(cube1["color"].toArray()[0].toDouble(),
                           cube1["color"].toArray()[1].toDouble(),
                           cube1["color"].toArray()[2].toDouble());

    // 读取 cube2 配置
    QJsonObject cube2 = json["cube2"].toObject();
    cube2Size = cube2["size"].toDouble();
    cube2Position = QVector3D(cube2["position"].toArray()[0].toDouble(),
                              cube2["position"].toArray()[1].toDouble(),
                              cube2["position"].toArray()[2].toDouble());
    cube2Rotation = QVector3D(cube2["rotation"].toArray()[0].toDouble(),
                              cube2["rotation"].toArray()[1].toDouble(),
                              cube2["rotation"].toArray()[2].toDouble());
    cube2Color = QVector3D(cube2["color"].toArray()[0].toDouble(),
                           cube2["color"].toArray()[1].toDouble(),
                           cube2["color"].toArray()[2].toDouble());

    // 读取 cube 速度
    QJsonObject cube = json["cube"].toObject();
    cubeVelocity = QVector3D(cube["velocity"].toArray()[0].toDouble(),
                                cube["velocity"].toArray()[1].toDouble(),
                                cube["velocity"].toArray()[2].toDouble());
}

CoreFunctionWidget::CoreFunctionWidget(QWidget* parent) : QOpenGLWidget(parent)
{
    this->setFocusPolicy(Qt::StrongFocus);
    // 初始化定时器
    timer.start();

    connect(&updateTimer, &QTimer::timeout, this, [this]() { QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection); });
    updateTimer.start(16); // 每16毫秒触发一次，相当于60帧每秒
}

CoreFunctionWidget::~CoreFunctionWidget()
{
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &EBO);
}


void CoreFunctionWidget::initializeGL() {
    this->initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    this->cam.set_initial_distance_ratio(8.0);
    
    loadConfig(); // 加载配置文件

    setupShaders();
    setupTextures();
    setupVertices();
    
    timer.start(); // 初始化计时器

    // 设置边界 AABB
    boundaryAABB.min = QVector3D(-5.0f, -5.0f, -5.0f);
    boundaryAABB.max = QVector3D(5.0f, 5.0f, 5.0f);
}

void CoreFunctionWidget::setupShaders() {
    // 初始化着色器
    bool success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/textures.vert");
    if (!success) {
        qDebug() << "shaderProgram addShaderFromSourceFile failed!" << shaderProgram.log();
        return;
    }

    success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/textures.frag");
    if (!success) {
        qDebug() << "shaderProgram addShaderFromSourceFile failed!" << shaderProgram.log();
        return;
    }

    success = shaderProgram.link();
    if (!success) {
        qDebug() << "shaderProgram link failed!" << shaderProgram.log();
    }

    // 初始化天空盒着色器
    success = skyboxShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/skybox.vert");
    if (!success) {
        qDebug() << "skyboxShaderProgram addShaderFromSourceFile failed!" << skyboxShaderProgram.log();
        return;
    }

    success = skyboxShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/skybox.frag");
    if (!success) {
        qDebug() << "skyboxShaderProgram addShaderFromSourceFile failed!" << skyboxShaderProgram.log();
        return;
    }

    success = skyboxShaderProgram.link();
    if (!success) {
        qDebug() << "skyboxShaderProgram link failed!" << skyboxShaderProgram.log();
    }

    // 初始化方块着色器
    success = cubeShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/cube.vert");
    if (!success) {
        qDebug() << "cubeShaderProgram addShaderFromSourceFile failed!" << cubeShaderProgram.log();
        return;
    }

    success = cubeShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/cube.frag");
    if (!success) {
        qDebug() << "cubeShaderProgram addShaderFromSourceFile failed!" << cubeShaderProgram.log();
        return;
    }

    success = cubeShaderProgram.link();
    if (!success) {
        qDebug() << "cubeShaderProgram link failed!" << cubeShaderProgram.log();
    }
}

void CoreFunctionWidget::setupTextures() {
    // 加载天空盒纹理
    std::vector<std::string> faces
    {
        ":/res/skybox/right.jpg",
        ":/res/skybox/left.jpg",
        ":/res/skybox/top.jpg",
        ":/res/skybox/bottom.jpg",
        ":/res/skybox/front.jpg",
        ":/res/skybox/back.jpg"
    };
    skyboxTexture = loadCubemap(faces);

    // 加载盒子纹理
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    QImage img1 = QImage(":/container.jpg").convertToFormat(QImage::Format_RGB888);
    if (!img1.isNull()) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img1.width(), img1.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, img1.bits());
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    QImage img2 = QImage(":/awesomeface.png").convertToFormat(QImage::Format_RGBA8888).mirrored(true, true);
    if (!img2.isNull()) {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img2.width(), img2.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img2.bits());
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    shaderProgram.bind();   // don't forget to activate/use the shader before setting uniforms!
    glUniform1i(shaderProgram.uniformLocation("texture1"), 0);
    glUniform1i(shaderProgram.uniformLocation("texture2"), 1);
    shaderProgram.release();
}

GLuint CoreFunctionWidget::loadCubemap(std::vector<std::string> faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        QImage img = QImage(faces[i].c_str()).convertToFormat(QImage::Format_RGB888);
        if (!img.isNull())
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, img.width(), img.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, img.bits());
        }
        else
        {
            qDebug() << "Cubemap texture failed to load at path: " << faces[i].c_str();
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void CoreFunctionWidget::setupVertices() {
    // 设置天空盒 VAO 和 VBO
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);


    // 设置立方体 VAO 和 VBO
    float cube_vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    };
    
    unsigned int cube_indices[] = {
        0, 1, 2,
        2, 3, 0,
        4, 5, 6,
        6, 7, 4,
        8, 9, 10,
        10, 11, 8,
        12, 13, 14,
        14, 15, 12,
        16, 17, 18,
        18, 19, 16,
        20, 21, 22,
        22, 23, 20,
    };

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);   //取消VAO绑定
    glBindBuffer(GL_ARRAY_BUFFER, 0);//取消VBO的绑定
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // 设置立方体1
    setupCube(cube1VAO, cube1VBO, cube1Size, cube1Color);

    // 设置立方体2
    setupCube(cube2VAO, cube2VBO, cube2Size, cube2Color);
}

void CoreFunctionWidget::setupCube(GLuint &VAO, GLuint &VBO, float size, QVector3D color) {
    float vertSize = size/2;
    float vertices[] = {
        // positions          // colors
        -vertSize, -vertSize, -vertSize,  color.x(), color.y(), color.z(),
         vertSize, -vertSize, -vertSize,  color.x(), color.y(), color.z(),
         vertSize,  vertSize, -vertSize,  color.x(), color.y(), color.z(),
        -vertSize,  vertSize, -vertSize,  color.x(), color.y(), color.z(),
        -vertSize, -vertSize,  vertSize,  color.x(), color.y(), color.z(),
         vertSize, -vertSize,  vertSize,  color.x(), color.y(), color.z(),
         vertSize,  vertSize,  vertSize,  color.x(), color.y(), color.z(),
        -vertSize,  vertSize,  vertSize,  color.x(), color.y(), color.z()
    };

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        0, 1, 5, 5, 4, 0,
        2, 3, 7, 7, 6, 2,
        0, 3, 7, 7, 4, 0,
        1, 2, 6, 6, 5, 1
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}



void CoreFunctionWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void CoreFunctionWidget::paintGL() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 计算时间差
    qint64 currentTime = timer.elapsed();
    deltaTime = currentTime / 1000.0f;
    timer.restart();

    // 更新立方体位置
    cubePosition += cubeVelocity * deltaTime;

    // 计算立方体的 AABB
    AABB cubeAABB = calculateAABB(cubePosition, 1.0f); // 动态立方体的大小为 1.0
    AABB cube1AABB = calculateAABB(cube1Position, cube1Size);
    AABB cube2AABB = calculateAABB(cube2Position, cube2Size);

    // 检查动态立方体与静态立方体1的碰撞
    CollisionFace collisionFace = checkCollision(cubeAABB, cube1AABB);
    if (collisionFace != NO_COLLISION) {
        qDebug() << "Collision with cube1 detected!";
        qDebug() << "Dynamic Cube Position: " << cubePosition << ", Size: 1.0";
        qDebug() << "Cube1 Position: " << cube1Position << ", Size: " << cube1Size;
        qDebug() << "Collision Box A: min(" << cubeAABB.min.x() << "," << cubeAABB.min.y() << "," << cubeAABB.min.z() << "), max(" << cubeAABB.max.x() << "," << cubeAABB.max.y() << "," << cubeAABB.max.z() << ")";
        qDebug() << "Collision Box B: min(" << cube1AABB.min.x() << "," << cube1AABB.min.y() << "," << cube1AABB.min.z() << "), max(" << cube1AABB.max.x() << "," << cube1AABB.max.y() << "," << cube1AABB.max.z() << ")";
        if (collisionFace == COLLISION_X) {
            cubeVelocity.setX(-cubeVelocity.x());
        } else if (collisionFace == COLLISION_Y) {
            cubeVelocity.setY(-cubeVelocity.y());
        } else if (collisionFace == COLLISION_Z) {
            cubeVelocity.setZ(-cubeVelocity.z());
        }
        // 调整位置以避免下一帧再次检测到碰撞
        cubePosition += cubeVelocity * deltaTime;
    }

    // 检查动态立方体与静态立方体2的碰撞
    collisionFace = checkCollision(cubeAABB, cube2AABB);
    if (collisionFace != NO_COLLISION) {
        qDebug() << "Collision with cube2 detected!";
        qDebug() << "Dynamic Cube Position: " << cubePosition << ", Size: 1.0";
        qDebug() << "Cube2 Position: " << cube2Position << ", Size: " << cube2Size;
        qDebug() << "Collision Box A: min(" << cubeAABB.min.x() << "," << cubeAABB.min.y() << "," << cubeAABB.min.z() << "), max(" << cubeAABB.max.x() << "," << cubeAABB.max.y() << "," << cubeAABB.max.z() << ")";
        qDebug() << "Collision Box B: min(" << cube2AABB.min.x() << "," << cube2AABB.min.y() << "," << cube2AABB.min.z() << "), max(" << cube2AABB.max.x() << "," << cube2AABB.max.y() << "," << cube2AABB.max.z() << ")";
        if (collisionFace == COLLISION_X) {
            cubeVelocity.setX(-cubeVelocity.x());
        } else if (collisionFace == COLLISION_Y) {
            cubeVelocity.setY(-cubeVelocity.y());
        } else if (collisionFace == COLLISION_Z) {
            cubeVelocity.setZ(-cubeVelocity.z());
        }
        // 调整位置以避免下一帧再次检测到碰撞
        cubePosition += cubeVelocity * deltaTime;
    }

     // 检查与边界的碰撞
    if (cubeAABB.min.x() < boundaryAABB.min.x() || cubeAABB.max.x() > boundaryAABB.max.x()) {
        qDebug() << "Collision with boundary detected on X axis!";
        cubeVelocity.setX(-cubeVelocity.x());
        // 调整位置以避免下一帧再次检测到碰撞
        cubePosition.setX(cubePosition.x() + cubeVelocity.x() * deltaTime);
    }
    if (cubeAABB.min.y() < boundaryAABB.min.y() || cubeAABB.max.y() > boundaryAABB.max.y()) {
        qDebug() << "Collision with boundary detected on Y axis!";
        cubeVelocity.setY(-cubeVelocity.y());
        // 调整位置以避免下一帧再次检测到碰撞
        cubePosition.setY(cubePosition.y() + cubeVelocity.y() * deltaTime);
    }
    if (cubeAABB.min.z() < boundaryAABB.min.z() || cubeAABB.max.z() > boundaryAABB.max.z()) {
        qDebug() << "Collision with boundary detected on Z axis!";
        cubeVelocity.setZ(-cubeVelocity.z());
        // 调整位置以避免下一帧再次检测到碰撞
        cubePosition.setZ(cubePosition.z() + cubeVelocity.z() * deltaTime);
    }

    shaderProgram.bind();
    {
        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // render container
        glBindVertexArray(cubeVAO);
        // set uniform mats
        QMatrix4x4 model_mat; // identity
        model_mat.translate(cubePosition); // 使用更新后的位置
        glUniformMatrix4fv(shaderProgram.uniformLocation("model"), 1, GL_FALSE, model_mat.data());
        QMatrix4x4 camera_mat = this->cam.get_camera_matrix();
        glUniformMatrix4fv(shaderProgram.uniformLocation("view"), 1, GL_FALSE, camera_mat.data());

        QMatrix4x4 projection_matrix;
        if (this->use_perspective)
            projection_matrix.perspective(90, 1.0, 0.01, 50.0);
        else
            projection_matrix.ortho(-2, 2, -2, 2, 0.01, 50.0);

        glUniformMatrix4fv(shaderProgram.uniformLocation("projection"), 1, GL_FALSE, projection_matrix.data());

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    shaderProgram.release();

    // 渲染天空盒
    glDepthFunc(GL_LEQUAL);  // 更改深度函数，以便天空盒能在最远处绘制
    skyboxShaderProgram.bind();
    {
        QMatrix4x4 view = this->cam.get_camera_matrix();
        view.setColumn(3, QVector4D(0, 0, 0, 1));
        QMatrix4x4 projection;
        if (this->use_perspective)
            projection.perspective(90, 1.0, 0.01, 50.0);
        else
            projection.ortho(-2, 2, -2, 2, 0.01, 50.0);

        glUniformMatrix4fv(skyboxShaderProgram.uniformLocation("view"), 1, GL_FALSE, view.data());
        glUniformMatrix4fv(skyboxShaderProgram.uniformLocation("projection"), 1, GL_FALSE, projection.data());
        // 绘制天空盒
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
    skyboxShaderProgram.release();
    glDepthFunc(GL_LESS); // 重置深度函数

    // 渲染立方体1
    cubeShaderProgram.bind();
    {
        QMatrix4x4 model;
        model.translate(cube1Position);
        model.rotate(cube1Rotation.x(), QVector3D(1.0f, 0.0f, 0.0f));
        model.rotate(cube1Rotation.y(), QVector3D(0.0f, 1.0f, 0.0f));
        model.rotate(cube1Rotation.z(), QVector3D(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(cubeShaderProgram.uniformLocation("model"), 1, GL_FALSE, model.data());
        QMatrix4x4 view = this->cam.get_camera_matrix();
        glUniformMatrix4fv(cubeShaderProgram.uniformLocation("view"), 1, GL_FALSE, view.data());
        QMatrix4x4 projection;
        if (this->use_perspective)
            projection.perspective(90, 1.0, 0.01, 50.0);
        else
            projection.ortho(-2, 2, -2, 2, 0.01, 50.0);
        glUniformMatrix4fv(cubeShaderProgram.uniformLocation("projection"), 1, GL_FALSE, projection.data());
        glBindVertexArray(cube1VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    cubeShaderProgram.release();

    // 渲染立方体2
    cubeShaderProgram.bind();
    {
        QMatrix4x4 model;
        model.translate(cube2Position);
        model.rotate(cube2Rotation.x(), QVector3D(1.0f, 0.0f, 0.0f));
        model.rotate(cube2Rotation.y(), QVector3D(0.0f, 1.0f, 0.0f));
        model.rotate(cube2Rotation.z(), QVector3D(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(cubeShaderProgram.uniformLocation("model"), 1, GL_FALSE, model.data());
        QMatrix4x4 view = this->cam.get_camera_matrix();
        glUniformMatrix4fv(cubeShaderProgram.uniformLocation("view"), 1, GL_FALSE, view.data());
        QMatrix4x4 projection;
        if (this->use_perspective)
            projection.perspective(90, 1.0, 0.01, 50.0);
        else
            projection.ortho(-2, 2, -2, 2, 0.01, 50.0);
        glUniformMatrix4fv(cubeShaderProgram.uniformLocation("projection"), 1, GL_FALSE, projection.data());
        glBindVertexArray(cube2VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}


AABB CoreFunctionWidget::calculateAABB(const QVector3D& position, float size) {
    AABB aabb;
    aabb.min = position - QVector3D(size, size, size) * 0.5f;
    aabb.max = position + QVector3D(size, size, size) * 0.5f;
    return aabb;
}

CollisionFace CoreFunctionWidget::checkCollision(const AABB& a, const AABB& b) {
    bool collisionX = (a.min.x() <= b.max.x() && a.max.x() >= b.min.x());
    bool collisionY = (a.min.y() <= b.max.y() && a.max.y() >= b.min.y());
    bool collisionZ = (a.min.z() <= b.max.z() && a.max.z() >= b.min.z());

    if (collisionX && collisionY && collisionZ) {
        // Determine which face the collision occurred on
        float overlapX = std::min(a.max.x() - b.min.x(), b.max.x() - a.min.x());
        float overlapY = std::min(a.max.y() - b.min.y(), b.max.y() - a.min.y());
        float overlapZ = std::min(a.max.z() - b.min.z(), b.max.z() - a.min.z());

        if (overlapX < overlapY && overlapX < overlapZ) {
            return COLLISION_X;
        } else if (overlapY < overlapX && overlapY < overlapZ) {
            return COLLISION_Y;
        } else {
            return COLLISION_Z;
        }
    }

    return NO_COLLISION;
}


void CoreFunctionWidget::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_A) {
        this->cam.translate_left(0.2);
    }
    else if (e->key() == Qt::Key_D) {
        this->cam.translate_left(-0.2);
    }
    else if (e->key() == Qt::Key_W) {
        this->cam.translate_up(0.2);
    }
    else if (e->key() == Qt::Key_S) {
        this->cam.translate_up(-0.2);
    }
    else if (e->key() == Qt::Key_F) {
        this->cam.translate_forward(0.2);
    }
    else if (e->key() == Qt::Key_B) {
        this->cam.translate_forward(-0.2);;
    }
    else if (e->key() == Qt::Key_Z) {
        this->cam.zoom_near(0.1);
    }
    else if (e->key() == Qt::Key_X) {
        this->cam.zoom_near(-0.1);
    }
    else if (e->key() == Qt::Key_T) {
        this->use_perspective = !this->use_perspective;
    }

    emit projection_change();

    update();
}

void CoreFunctionWidget::mousePressEvent(QMouseEvent* e) {
    mouse_x = e->position().x();
    mouse_y = e->position().y();
}

void CoreFunctionWidget::mouseMoveEvent(QMouseEvent* e) {
    int x = e->position().x();
    int y = e->position().y();

    if (abs(x - mouse_x) >= 3) {
        if (x > mouse_x) {
//            this->cam.rotate_left(3.0);
            this->cam.rotate_left(-3.0);
        }
        else {
//            this->cam.rotate_left(-3.0);
            this->cam.rotate_left(3.0);
        }
        mouse_x = x;
    }

    if (abs(y - mouse_y) >= 3) {
        if (y > mouse_y) {
            this->cam.rotate_up(-3.0);
        }
        else {
            this->cam.rotate_up(3.0);
        }

        mouse_y = y;
    }

    update();
}

