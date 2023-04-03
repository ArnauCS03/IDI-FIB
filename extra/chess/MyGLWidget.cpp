// MyGLWidget.cpp
#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

MyGLWidget::~MyGLWidget() {
}

//-----

void MyGLWidget::initializeGL() {

    BL2GLWidget::initializeGL();

    ini_camera();

    glEnable(GL_DEPTH_TEST); // Z-buffer

    angle = 0;
}


void MyGLWidget::ini_camera() {

    FOV = float(M_PI)/2.0f;
    ra = 1.0f;
    zNear = 0.1f;
    zFar = 10.0f;
    OBS = glm::vec3(0, 0, 1);
    VRP = glm::vec3(0, 0.3, 0);
    UP = glm::vec3(0, 1, 0);

    projectTransform();
    viewTransform();
}


void MyGLWidget::paintGL() {

    // Esborrem el frame-buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Carreguem la transformació de model
    modelTransformPawn ();

    // Activem el VAO
    glBindVertexArray (VAO_Pawn);

    // pintem
    glDrawArrays(GL_TRIANGLES, 0, pawn.faces().size()*3);

    glBindVertexArray (0);



    modelTransformBishop();

    // Activem el VAO
    glBindVertexArray (VAO_Bishop);

    // pintem
    glDrawArrays(GL_TRIANGLES, 0, bishop.faces().size()*3);

    glBindVertexArray (0);
}


void MyGLWidget::modelTransformPawn() {
    glm::mat4 transform (1.0f);
    transform = glm::rotate(transform, angle, glm::vec3(0.25, 0.7, 0));
    transform = glm::scale(transform, glm::vec3(escala));
    transform = glm::translate(transform, glm::vec3(0, 0, 0));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::modelTransformBishop() {
    glm::mat4 transform (1.0f);
    transform = glm::rotate(transform, angle, glm::vec3(0.0, 0.5, 0.0));
    transform = glm::scale(transform, glm::vec3(escala));
    transform = glm::translate(transform, glm::vec3(-1.0, 0, 0));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::projectTransform() {
    glm::mat4 Proj = glm::perspective(FOV, ra, zNear, zFar);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform() {
    glm::mat4 View = glm::lookAt(OBS, VRP, UP);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}


void MyGLWidget::keyPressEvent(QKeyEvent *event) {

    makeCurrent();

    switch (event -> key()) {

        case Qt::Key_S :  // escalar a més gran
            escala += 0.05;
            break;

        case Qt::Key_D :  // escalar a més petit
            escala -= 0.05;
            break;

        case Qt::Key_R :
            angle += M_PI/4;
            break;

        default: event->ignore(); break;
    }
    update();
}


void MyGLWidget::creaBuffers() {

    pawn.load("models/pawn.obj");
    // Creació del VAO
    glGenVertexArrays(1, &VAO_Pawn);
    glBindVertexArray(VAO_Pawn);

    // Creació del buffer amb les dades dels vèrtexs
    GLuint VBO1;
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);

    //nou-----
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*pawn.faces().size()*3*3, pawn.VBO_vertices(), GL_STATIC_DRAW);

    // Activem l'atribut
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);


    // colors vertex
    GLuint VBO_color;
    glGenBuffers(1, &VBO_color);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);

    //nou-----
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*pawn.faces().size()*3*3, pawn.VBO_matdiff(), GL_STATIC_DRAW);

    // Activem l'atribut
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);  //ojo 3
    glEnableVertexAttribArray(colorLoc);


    //====== Bishop===========
    // Desactivem el VAO
    glBindVertexArray(0);


    bishop.load("models/bishop.obj");
    // Creació del VAO
    glGenVertexArrays(1, &VAO_Bishop);
    glBindVertexArray(VAO_Bishop);

    // Creació del buffer amb les dades dels vèrtexs
    GLuint VBO_Bishop;
    glGenBuffers(1, &VBO_Bishop);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Bishop);

    //nou-----
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*bishop.faces().size()*3*3, bishop.VBO_vertices(), GL_STATIC_DRAW);

    // Activem l'atribut
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);


    // colors vertex
    GLuint VBO_color_Bishop;
    glGenBuffers(1, &VBO_color_Bishop);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_Bishop);

    //nou-----
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*bishop.faces().size()*3*3, bishop.VBO_matdiff(), GL_STATIC_DRAW);

    // Activem l'atribut
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);  //ojo 3
    glEnableVertexAttribArray(colorLoc);


    // Desactivem el VAO
    glBindVertexArray(0);
}


void MyGLWidget::carregaShaders() {
    BL2GLWidget::carregaShaders();
    projLoc = glGetUniformLocation(program->programId(), "proj");
    viewLoc = glGetUniformLocation(program->programId(), "view");
}
