#ifndef M_MIX_H
#define M_MIX_H

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

#define MODULE_MIX_WIDTH                 85
#define MODULE_MIX_HEIGHT                40
#define MAX_MIX_IN                       16 
       
class M_mix : public Module
{
  Q_OBJECT

  private:
    QList<Port> in_port_list;
    Port *port_out;
    Module *in_M[MAX_MIX_IN];
    float gain;
    float mixer_gain[MAX_MIX_IN];
        
  public: 
    float **inData[MAX_MIX_IN];
    int in_channels;
                            
  public:
    M_mix(int p_in_channels, QWidget* parent=0, const char *name=0, SynthData *p_synthdata=0);
    ~M_mix();

  protected:
    virtual void paintEvent(QPaintEvent *ev);
  
  public slots:
    void generateCycle();
    void showConfigDialog();
};
  
#endif
