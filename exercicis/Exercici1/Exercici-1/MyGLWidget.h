#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();
    
  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ();

    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ();
 
    // resize - Es cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);  

    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);

  private:
    void creaBuffersQuadrat();
    void creaBuffersCavall();
    
    void carregaShaders();
    
    void modelTransformQuadrat(glm::vec3 posicio);
    void modelTransformCavall(glm::vec3 posicio);
    
    // program
    QOpenGLShaderProgram *program;
    // attribute locations
    GLuint vertexLoc;
    // uniform locations
    GLuint TGLoc;

    // uniform dels colors
    GLuint colorLoc;

    bool menja;

    float scl;

    float angle;

    // VAOs
    GLuint VAOQuadrat, VAOCavall;

    // viewport
    GLint ample, alt;    
    
    // colors
    glm::vec3 blancTauler = glm::vec3(233.0/255.0,216.0/255.0,170.0/255.0);
    glm::vec3 negreTauler = glm::vec3(63.0/255.0,56.0/255.0,50.0/255.0);
    glm::vec3 blanc = glm::vec3(1.0);
    glm::vec3 negre = glm::vec3(0.0);    
};
