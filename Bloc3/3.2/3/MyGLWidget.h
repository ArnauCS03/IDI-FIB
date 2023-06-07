// MyGLWidget.h
#include "BL3GLWidget.h"

class MyGLWidget : public BL3GLWidget {
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent=0) : BL3GLWidget(parent) {}
    ~MyGLWidget();

  protected:

    virtual void initializeGL();

    virtual void iniFocus();

    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void keyPressEvent(QKeyEvent* event);

    virtual void iniMaterialTerra();

    virtual void viewTransform();



    glm::vec4 posFocusEscena, posFocusCamera;
    glm::vec3 colorFocus, llumAmbient;

    GLuint colorFocusLoc, posFocusLoc, llumAmbLoc;

    bool focus_escena;

  private:
    int printOglError(const char file[], int line, const char func[]);
};
