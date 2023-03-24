// MyGLWidget.h
#include "BL2GLWidget.h"

class MyGLWidget : public BL2GLWidget {
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent=0) : BL2GLWidget(parent) {}
    ~MyGLWidget();


  protected:

    virtual void initializeGL();

    virtual void ini_camera();

    virtual void paintGL();

    virtual void projectTransform();

    virtual void viewTransform();

    virtual void carregaShaders();

    GLuint projLoc;
    GLuint viewLoc;

    float FOV, ra, zNear, zFar;
    glm::vec3 OBS, VRP, UP;

  private:
    int printOglError(const char file[], int line, const char func[]);

};
