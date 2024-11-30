#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H


#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QTimer>
#include "Camera.h"

struct AABB {
    QVector3D min;
    QVector3D max;
};

enum CollisionFace {
    NO_COLLISION,
    COLLISION_X,
    COLLISION_Y,
    COLLISION_Z
};

enum class Filter {
    None,
    Invert,
    Gray
};

class CoreFunctionWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit CoreFunctionWidget(QWidget* parent = nullptr);
    ~CoreFunctionWidget();

signals:
    void projection_change();
    void collisionDetected(const QString& message);
protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    int mouse_x, mouse_y;

private:
    void setupShaders();
    void setupTextures();
    void setupVertices();
    void setupCube(GLuint &VAO, GLuint &VBO, float size, QVector3D color);
    void setupFrameBuffer();

    GLuint loadCubemap(std::vector<std::string> faces);
    void loadConfig();
    AABB calculateAABB(const QVector3D& position, float size);
    CollisionFace checkCollision(const AABB& a, const AABB& b);

    QOpenGLShaderProgram shaderProgram;
    QOpenGLShaderProgram skyboxShaderProgram;
    QOpenGLShaderProgram cubeShaderProgram;

    GLuint quadVAO, quadVBO;
    GLuint fbo, rbo, textureColorBuffer;
    QOpenGLShaderProgram invertShaderProgram;
    QOpenGLShaderProgram grayShaderProgram;

    GLuint skyboxVAO, skyboxVBO, skyboxTexture;

    GLuint cube1VAO, cube1VBO;
    float cube1Size;
    QVector3D cube1Position, cube1Rotation, cube1Color;
    AABB cube1AABB;

    GLuint cube2VAO, cube2VBO;
    float cube2Size;
    QVector3D cube2Position, cube2Rotation, cube2Color;
    AABB cube2AABB;

    GLuint cubeVBO, cubeVAO, texture1, texture2;
    QVector3D cubePosition, cubeVelocity;
    AABB cubeAABB;

    GLuint EBO;
    
    float deltaTime;
    QElapsedTimer timer;
    QTimer updateTimer;

    AABB boundaryAABB;

    Filter currentFilter = Filter::None;

    Camera cam;
public:
    bool use_perspective = true;
};


#endif // OPENGLWIDGET_H
