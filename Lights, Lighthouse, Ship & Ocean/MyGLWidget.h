#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QMouseEvent>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "model.h"




class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core 
{
  Q_OBJECT

  public:
	MyGLWidget (QWidget *parent=0);
	~MyGLWidget ();
public slots:
    void tick();

  protected:
	// initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
	virtual void initializeGL ( );

	// resizeGL - És cridat quan canvia la mida del widget
	virtual void resizeGL (int width, int height);

	// mouse{Press/Release/Move}Event - Són cridades quan es realitza l'event 
	// corresponent de ratolí
	virtual void mousePressEvent (QMouseEvent *event);
	virtual void mouseReleaseEvent (QMouseEvent *event);
	virtual void mouseMoveEvent (QMouseEvent *event);

  protected:
	//--------------------------------------------
	// Inicialitzacions
    virtual void iniEscena () = 0;
	void iniCamera ();
	//--------------------------------------------
	// Creació de buffers
	void creaBuffersModels();
    void creaBuffersQuadrat(glm::vec3 c, GLuint* VAO, float spec);

	//--------------------------------------------
    virtual void carregaShaders () = 0;
	//--------------------------------------------
	void projectTransform ();
	void viewTransform ();
	//--------------------------------------------
	// Matrius de transformació de cada objecte
	void modelTransformTerra();
	void modelTransformMar();
    void modelTransformMoll();

	void modelTransformDebug(glm::vec3 pos);

    virtual void modelTransformFar1() = 0;
    virtual void modelTransformFar2() = 0;
    virtual void modelTransformVaixell() = 0;
	//--------------------------------------------
	void calculaCapsaModel (Model &p, float &escala, float alcadaDesitjada, glm::vec3 &centreBase);



	//--------------------------------------------------------------
	//          GLOBALS D'ESCENA
	glm::vec3 centreEsc;
	float radiEsc, ra;
	GLint ample, alt;

	//--------------------------------------------------------------
	//          SHADERS
	//--------------------------------------------------------------
	QOpenGLShaderProgram *program;


	//--------------------------------------------------------------
	// model
	// enum models - els models estan en un array de VAOs (VAO_models), aquest enum és per fer més llegible el codi.
	typedef enum { FAR_1 = 0, FAR_2 = 1, MOLL=2, VAIXELL=3, NUM_MODELS = 4} ModelType;
	const char* modelPath[4]= {
				"./models/light_house_p1.obj",
				"./models/light_house_p2.obj",
				"./models/pier.obj",
				"./models/ship_and_torches.obj"};
	Model models[NUM_MODELS];
	// paràmetres calculats a partir de la capsa contenidora del model
	glm::vec3 centreBaseModels[NUM_MODELS];
	float escalaModels[NUM_MODELS];    
	//--------------------------------------------------------------
	// VAO names
	GLuint VAO_models[NUM_MODELS];
	GLuint VAO_Mar, VAO_Terra;    


    // temps
    float temps = 0;


	//--------------------------------------------------------------
	// INTERACCIÓ
	typedef  enum {NONE, ROTATE} InteractiveAction;
	InteractiveAction DoingInteractive;
	int xClick, yClick;
    float angleX, angleY;


    //--------------------------------------------------------------
    // uniform locations
    GLuint transLoc, projLoc, viewLoc;
    GLuint vertexLoc, normalLoc, matambLoc, matdiffLoc, matspecLoc, matshinLoc;
    GLuint tempsLoc;

    glm::mat4 View;
};

