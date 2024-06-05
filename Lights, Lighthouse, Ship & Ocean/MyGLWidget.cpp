#include "MyGLWidget.h"
#include <glm/gtx/color_space.hpp>
#include <iostream>
#include <QTimer>
MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  xClick = yClick = 0;
  DoingInteractive = NONE;

  QTimer *timer = new QTimer(this);
      connect(timer, &QTimer::timeout, this, QOverload<>::of(&MyGLWidget::tick));
      timer->start(100);
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();  

  glClearColor(0.0, 0.0, 0.0, 1.0); // defineix color de fons (d'esborrat)
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  carregaShaders();
  iniEscena ();
  iniCamera ();
}

void MyGLWidget::iniCamera ()
{
    ra = 1.0;
    angleY = 0.0;
    angleX = 0.2;
    projectTransform ();
    viewTransform ();
}


void MyGLWidget::modelTransformTerra()
{
    glm::mat4 TG = glm::mat4(1.0f);
    TG = glm::translate(TG, glm::vec3(20.0,1.0,0.0));
    TG = glm::scale(TG, glm::vec3(20, 2.5, 20));
    TG = glm::translate(TG, glm::vec3(0.0,-1.0,0.0));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformDebug(glm::vec3 pos)
{
    glm::mat4 TG = glm::mat4(1.0f);
    TG = glm::translate(TG,pos);
    TG = glm::scale(TG, glm::vec3(0.2, 0.2, 0.2));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformMar()
{
    glm::mat4 TG = glm::mat4(1.0f);
    TG = glm::translate(TG, glm::vec3(-20.0,0.0,0.0));
    TG = glm::scale(TG, glm::vec3(20, 2, 20));
    TG = glm::translate(TG, glm::vec3(0.0,-1.0,0.0));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformMoll()
{
    // Codi per a la TG necessària
    glm::mat4 TG = glm::mat4(1.0f);
    TG = glm::rotate(TG, glm::radians((float)90), glm::vec3(0, 1, 0));
    TG = glm::scale(TG, glm::vec3(escalaModels[MOLL]) );
    TG = glm::translate(TG, glm::vec3(0, 0, -7.81));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::resizeGL (int w, int h)
{
  ample = w;
  alt = h;
  projectTransform ();
}


void MyGLWidget::projectTransform ()
{
  float fov, zn, zf;
  glm::mat4 Proj;  // Matriu de projecció
  
  fov = float(M_PI/3.0);
  zn = radiEsc*0.25;
  zf = 3*radiEsc;

  float rav =  width() /  (float)height() ;
  if(rav<ra){
      fov= 2*atan(tan(0.5*fov)/rav);
  }

  Proj = glm::perspective(fov, rav, zn, zf);

  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform ()
{
  // Matriu de posició i orientació
  View = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -1.3*radiEsc));
  View = glm::rotate(View, angleX, glm::vec3(1, 0, 0));
  View = glm::rotate(View, -angleY, glm::vec3(0, 1, 0));
  View = glm::translate(View, -centreEsc-glm::vec3(0,2,-2));

  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::tick(){
    temps++;
    makeCurrent();
    update();
}


void MyGLWidget::mousePressEvent (QMouseEvent *e)
{
  xClick = e->x();
  yClick = e->y();

  if (e->button() & Qt::LeftButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}

void MyGLWidget::mouseReleaseEvent( QMouseEvent *)
{
  DoingInteractive = NONE;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  makeCurrent();
  if (DoingInteractive == ROTATE)
  {
    // Fem la rotació
    angleY += (e->x() - xClick) * M_PI / 180.0;    
    angleX += (e->y() - yClick) * M_PI / 180.0;
    viewTransform ();
  }

  xClick = e->x();
  yClick = e->y();

  update ();
} 


void MyGLWidget::calculaCapsaModel (Model &p, float &escala, float alcadaDesitjada, glm::vec3 &centreBase)
{
  // Càlcul capsa contenidora i valors transformacions inicials
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = p.vertices()[0];
  miny = maxy = p.vertices()[1];
  minz = maxz = p.vertices()[2];
  for (unsigned int i = 3; i < p.vertices().size(); i+=3)
  {
    if (p.vertices()[i+0] < minx)
      minx = p.vertices()[i+0];
    if (p.vertices()[i+0] > maxx)
      maxx = p.vertices()[i+0];
    if (p.vertices()[i+1] < miny)
      miny = p.vertices()[i+1];
    if (p.vertices()[i+1] > maxy)
      maxy = p.vertices()[i+1];
    if (p.vertices()[i+2] < minz)
      minz = p.vertices()[i+2];
    if (p.vertices()[i+2] > maxz)
      maxz = p.vertices()[i+2];
  }

  escala = alcadaDesitjada/(maxy-miny);
  centreBase[0] = (minx+maxx)/2.0; centreBase[1] = miny; centreBase[2] = (minz+maxz)/2.0;
}

void MyGLWidget::creaBuffersModels ()
{
	
	// Creació de buffers manuals (terra i mar)
    glm::vec3 ct(0.85, 0.4, 0.35);
    creaBuffersQuadrat(ct, &VAO_Terra, -1);

    glm::vec3 cm(0.0, 0.1, 0.8);
    creaBuffersQuadrat(cm, &VAO_Mar, 0.7);
	
	// Càrrega dels models
	for(int i=0;i<NUM_MODELS;i++) {
		models[i].load(modelPath[i]);	  
	}

	// Creació de VAOs i VBOs per pintar els models
	glGenVertexArrays(NUM_MODELS, &VAO_models[0]);

    float alcadaDesitjada[NUM_MODELS] = {10, 10, 1, 14};

	for (int i = 0; i < NUM_MODELS; i++)
	{
		// Calculem la capsa contenidora del model
		calculaCapsaModel (models[i], escalaModels[i], alcadaDesitjada[i], centreBaseModels[i]);

		glBindVertexArray(VAO_models[i]);

		GLuint VBO[6];
		glGenBuffers(6, VBO);

		// geometria
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3, models[i].VBO_vertices(), GL_STATIC_DRAW);
		glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(vertexLoc);

		// Buffer de normals
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3, models[i].VBO_normals(), GL_STATIC_DRAW);
		glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(normalLoc);

		// Buffer de component ambient
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3, models[i].VBO_matamb(), GL_STATIC_DRAW);
		glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(matambLoc);

		// Buffer de component difusa
        glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3, models[i].VBO_matdiff(), GL_STATIC_DRAW);
		glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(matdiffLoc);

		// Buffer de component especular
        glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3, models[i].VBO_matspec(), GL_STATIC_DRAW);
		glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(matspecLoc);

		// Buffer de component shininness
        glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3, models[i].VBO_matshin(), GL_STATIC_DRAW);
		glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(matshinLoc);

		glBindVertexArray(0);	  	  
	}
	glBindVertexArray (0);
}

void MyGLWidget::creaBuffersQuadrat(glm::vec3 c, GLuint* VAO, float spec)
{


    // Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
    // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
            -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
             1.0f, 1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f,-1.0f,
             1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f,-1.0f,
             1.0f,-1.0f,-1.0f,
             1.0f, 1.0f,-1.0f,
             1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f,-1.0f,
             1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f,-1.0f, 1.0f,
             1.0f,-1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f,-1.0f,-1.0f,
             1.0f, 1.0f,-1.0f,
             1.0f,-1.0f,-1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f,-1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f, 1.0f,-1.0f,
            -1.0f, 1.0f,-1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
             1.0f,-1.0f, 1.0f
        };
    static const GLfloat g_normal_buffer_data[] = {
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,//
             0.0f, 0.0f,-1.0f,
             0.0f, 0.0f,-1.0f,
             0.0f, 0.0f,-1.0f,//
             0.0f,-1.0f, 0.0f,
             0.0f,-1.0f, 0.0f,
             0.0f,-1.0f, 0.0f,//
             0.0f, 0.0f,-1.0f,
             0.0f, 0.0f,-1.0f,
             0.0f, 0.0f,-1.0f,//
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,//
             0.0f,-1.0f, 0.0f,
             0.0f,-1.0f, 0.0f,
             0.0f,-1.0f, 0.0f,//
             0.0f, 0.0f, 1.0f,
             0.0f, 0.0f, 1.0f,
             0.0f, 0.0f, 1.0f,//
             1.0f, 0.0f, 0.0f,
             1.0f, 0.0f, 0.0f,
             1.0f, 0.0f, 0.0f,//
             1.0f, 0.0f, 0.0f,
             1.0f, 0.0f, 0.0f,
             1.0f, 0.0f, 0.0f,//
             0.0f, 1.0f, 0.0f,
             0.0f, 1.0f, 0.0f,
             0.0f, 1.0f, 0.0f,//
             0.0f, 1.0f, 0.0f,
             0.0f, 1.0f, 0.0f,
             0.0f, 1.0f, 0.0f,//
             0.0f, 0.0f, 1.0f,
             0.0f, 0.0f, 1.0f,
             0.0f, 0.0f, 1.0f//
        };

    const int vertices = 36;

    glm::vec3 col[vertices];
    glm::vec3 spc[vertices];
    glm::vec3 sp = glm::vec3(spec, spec, spec);
    GLfloat shs[vertices];
    GLfloat sh = 100;
    for(int i=0;i<vertices;i++) {
        col[i] = c;
        spc[i] =sp;
        shs[i] = sh;
    }




  // VAO
  glGenVertexArrays(1, VAO);
  glBindVertexArray(*VAO);

  GLuint VBO[6];
  glGenBuffers(6, VBO);

  // geometria
  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // normals
  glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_normal_buffer_data), g_normal_buffer_data, GL_STATIC_DRAW);
  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // Buffer de component ambient
  glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(col), col, GL_STATIC_DRAW);
  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(col), col, GL_STATIC_DRAW);
  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);

  if(spec>0){
    glBufferData(GL_ARRAY_BUFFER, sizeof(spc), col, GL_STATIC_DRAW);
  } else {
    glBufferData(GL_ARRAY_BUFFER, sizeof(col), col, GL_STATIC_DRAW);
  }
  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(shs), shs, GL_STATIC_DRAW);
  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);
   
  glBindVertexArray (0);
}





