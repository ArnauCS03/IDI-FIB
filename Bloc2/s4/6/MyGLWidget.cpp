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
    zNear = 0.1f;//0.4f;
    zFar = 10.0f;
    OBS = glm::vec3(0, 0, 3);
    VRP = glm::vec3(0, 0, 0);
    UP = glm::vec3(0, 1, 0);

    projectTransform();
    viewTransform();
}


void MyGLWidget::paintGL() {

    //nou paint, ja no caseta

    // Esborrem el frame-buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Carreguem la transformació de model
    modelTransformHomer ();

    // Activem el VAO
    glBindVertexArray (VAO_Homer);

    // pintem
    glDrawArrays(GL_TRIANGLES, 0, homer.faces().size()*3);

    glBindVertexArray (0);


    // Carreguem la transformació de model
    modelTransformTerra ();

    // Activem el VAO
    glBindVertexArray (VAO_Terra);

    // pintem
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray (0);

}


void MyGLWidget::modelTransformHomer() {
    glm::mat4 transform (1.0f);
    transform = glm::rotate(transform, angle, glm::vec3(0, 1, 0));
    transform = glm::scale(transform, glm::vec3(escala));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::modelTransformTerra() {
    glm::mat4 transform (1.0f);
    transform = glm::scale(transform, glm::vec3(escala));
    transform = glm::translate(transform, glm::vec3(0, -1, 0));
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

    homer.load("../../../models/HomerProves.obj");
    // Creació del VAO
    glGenVertexArrays(1, &VAO_Homer);
    glBindVertexArray(VAO_Homer);

    // Creació del buffer amb les dades dels vèrtexs
    GLuint VBO1;
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);

    //nou-----
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*homer.faces().size()*3*3, homer.VBO_vertices(), GL_STATIC_DRAW);

    // Activem l'atribut
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);


    // colors vertex
    GLuint VBO_color;
    glGenBuffers(1, &VBO_color);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);

    //nou-----
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*homer.faces().size()*3*3, homer.VBO_matdiff(), GL_STATIC_DRAW);

    // Activem l'atribut
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);  //ojo 3
    glEnableVertexAttribArray(colorLoc);


    // Desactivem el VAO
    glBindVertexArray(0);



    glm::vec3 Vertices[4];
    Vertices[0] = glm::vec3(-2.0, 0.0, -2.0); //frontal esquerra
    Vertices[1] = glm::vec3(2.0, 0.0, -2.0);  //frontal dreta
    Vertices[2] = glm::vec3(-2.0, 0.0, 2.0);  //posterior esq
    Vertices[3] = glm::vec3(2.0, 0.0, 2.0);  //posterior dreta

    // VAO Terra
    glGenVertexArrays(1, &VAO_Terra);
    glBindVertexArray(VAO_Terra);

    // Creació del buffer amb les dades dels vèrtexs
    GLuint VBO_Terra;
    glGenBuffers(1, &VBO_Terra);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra);


    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    // Activem l'atribut
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glm::vec3 Colors[1];
    Colors[0] = glm::vec3(128./255., 77./255., 46./255.); //color marro

    GLuint VBO_color_Terra;
    glGenBuffers(1, &VBO_color_Terra);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_Terra);

    //nou-----
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);

    // Activem l'atribut
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);  //ojo 3
    glEnableVertexAttribArray(colorLoc);


}


void MyGLWidget::carregaShaders() {
    BL2GLWidget::carregaShaders();
    projLoc = glGetUniformLocation(program->programId(), "proj");
    viewLoc = glGetUniformLocation(program->programId(), "view");
}
