#ifndef M_JACKOUT_H
#define M_JACKOUT_H

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
#include <qtimer.h>
#include <qdialog.h>
#include <alsa/asoundlib.h>
#include <jack/jack.h>
#include "synthdata.h"
#include "module.h"
#include "port.h"

#define MODULE_JACKOUT_WIDTH                 90
#define MODULE_JACKOUT_HEIGHT                80

class M_jackout : public Module
{
  Q_OBJECT

  private:
    float gain;
    float mixer_gain[2]; 
    int agc;
    Module *module_in[2];
    Port *port_in[2];
    
  public: 
    float **inData[2];
    jack_default_audio_sample_t *jackdata[2];
    jack_port_t *port_out[2];
                            
  public:
    M_jackout(QWidget* parent=0, const char *name=0, SynthData *p_synthdata=0);
    ~M_jackout();

  protected:
    virtual void paintEvent(QPaintEvent *ev);
  
  public slots:
    void generateCycle();
    void showConfigDialog();
};
  
#endif
