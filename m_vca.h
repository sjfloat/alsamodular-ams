#ifndef M_VCA_H
#define M_VCA_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qwidget.h>
#include <qstring.h>
#include <qslider.h>   
#include <qcheckbox.h>  
#include <qlabel.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <alsa/asoundlib.h>
#include "synthdata.h"
#include "module.h"
#include "port.h"

#define MODULE_VCA_WIDTH                 85
#define MODULE_VCA_HEIGHT               135

class M_vca : public Module
{
  Q_OBJECT

  private:
    float gain1, gain2, in1, in2, out;
    Module *in_M_gain1, *in_M_in1, *in_M_gain2, *in_M_in2; 
    Port *port_M_gain1, *port_M_in1, *port_M_gain2, *port_M_in2, *port_out;
    
  public: 
    float **gainData1, **gainData2, **inData1, **inData2;       
                            
  public:
    M_vca(QWidget* parent=0, const char *name=0, SynthData *p_synthdata=0);
    ~M_vca();

  protected:
    virtual void paintEvent(QPaintEvent *ev);
  
  public slots:
    void generateCycle();
    void showConfigDialog();
};
  
#endif
