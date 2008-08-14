#ifndef PREFWIDGET_H
#define PREFWIDGET_H

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
#include <qdialog.h>
#include <qcombobox.h>
#include <qstringlist.h>
#include <qlineedit.h>
#include <qfile.h>
#include <qtabwidget.h>
#include <QVBoxLayout>
#include <alsa/asoundlib.h>
#include "synthdata.h"

#define PREF_DEFAULT_WIDTH  300
#define PREF_DEFAULT_HEIGHT 300
/** preferences menue of AMS 
 *
 */ 
class PrefWidget : public QWidget
{
  Q_OBJECT

  QVBoxLayout vBox;
    QTabWidget *tabWidget;
    QString loadPath, savePath;
    QWidget *colorBackgroundLabel, *colorModuleBackgroundLabel, *colorModuleBorderLabel, *colorModuleFontLabel,
	    *colorCableLabel, *colorJackLabel;
    QColor colorBackground, colorModuleBackground, colorModuleBorder, colorModuleFont, colorPortFont1, colorPortFont2;
    QColor colorCable, colorJack;
    QComboBox *midiModeComboBox;
    QLineEdit *loadEdit, *saveEdit;
    int midiControllerMode;
   
  public:
    PrefWidget();
    bool loadPref(QString config_fn);
    bool loadPref(int rcFd);

  signals:
    void prefChanged();

  public slots:
    void savePref(int rcFd);
    void submitPref();
    void applyPref();
    void refreshColors();
    void recallColors();
    void storeColors();
    void colorBackgroundClicked();
    void colorModuleBackgroundClicked();
    void colorModuleBorderClicked();
    void colorModuleFontClicked();
    void colorCableClicked();
    void defaultcolorClicked();
    void colorJackClicked();
    void browseLoad();
    void browseSave();
    void updateMidiMode(int);
    void loadPathUpdate();
    void savePathUpdate();
};
  
#endif