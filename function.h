#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qwidget.h>
#include <qstring.h>
#include <qlabel.h>
#include <qcolor.h>
#include <qpointarray.h>
#include <qsizepolicy.h>
#include <qsize.h>
#include "synthdata.h"

#define FUNCTION_MINIMUM_WIDTH        100
#define FUNCTION_MINIMUM_HEIGHT        50
#define MAX_FUNCTIONS                   8
#define MAX_POINTS                     32
#define FUNCTION_WIDTH              10000
#define FUNCTION_HEIGHT             10000

class Function : public QWidget
{
  Q_OBJECT

  private:
    SynthData *synthdata;
    int functionCount;
    int pointCount[MAX_FUNCTIONS];
    QPointArray *points[MAX_FUNCTIONS]; 
    QPointArray *screenPoints[MAX_FUNCTIONS];
    QColor colorTable[MAX_FUNCTIONS];
    float zoomMin[2], zoomMax[2];             // Ranges for zoomed display of functions
    
  protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent (QResizeEvent* );            
    
  public:
    Function(int p_functionCount, QPointArray *p_points[], SynthData *p_synthdata, QWidget* parent=0, const char *name=0);
    ~Function();
    virtual QSize sizeHint() const;
    virtual QSizePolicy sizePolicy() const;

  public slots: 
    void updateFunction(int index);
};
  
#endif
