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
//
//
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <alsa/asoundlib.h>
#include "synthdata.h"
#include "module.h"
#include "port.h"

#define MODULE_VCA_WIDTH                 85
#define MODULE_VCA_HEIGHT               140

class M_vca : public Module
{
  Q_OBJECT

  private:
    float gain1, gain2, in1, in2, out;
    Port *port_M_gain1, *port_M_in1, *port_M_gain2, *port_M_in2, *port_out;
    
  public: 
    float **gainData1, **gainData2, **inData1, **inData2;       
    bool expMode;
                            
  public:
    M_vca(bool p_expMode, QWidget* parent=0);

  public slots:
    void generateCycle();
    void showConfigDialog();
};
  
#endif
