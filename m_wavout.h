#ifndef M_WAVOUT_H
#define M_WAVOUT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qwidget.h>
#include <qstring.h>
#include <qslider.h>   
#include <qcheckbox.h>  
#include <qlabel.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qdialog.h>
#include <QFile>
#include <alsa/asoundlib.h>
#include "synthdata.h"
#include "module.h"
#include "port.h"

#define MODULE_WAVOUT_WIDTH                 90
#define MODULE_WAVOUT_HEIGHT                80

class M_wavout : public Module
{
  Q_OBJECT

  private:
    QFile wavfile;
    long wavDataSize;
    float gain;
    float mixer_gain[2]; 
    int agc;
    float doRecord;
    QString wavname;
    QTimer *timer;
    Port *port_in[2];
    char outbuf[8];
    char *wavdata;
    float *floatdata;
    
  public: 
    float **inData[2];
                            
  public:
    M_wavout(QWidget* parent=0, const char *name=0);
    ~M_wavout();
    int setGain(float p_gain);
    float getGain();

  public slots:
    void generateCycle();
    void showConfigDialog();
    void recordToggled(bool on);
    void recordClicked();
    void openBrowser();
    void stopClicked();
    void createWav();
    void timerProc();
};
  
#endif
