#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <qwidget.h>
#include <qstring.h>
#include <qslider.h>   
#include <qcheckbox.h> 
#include <qsplitter.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qstringlist.h>
#include <qlineedit.h>
#include <QTreeWidget>
#include <alsa/asoundlib.h>
#include "synthdata.h"
#include "midiwidget.h"
#include "midicontroller.h"
#include "midicontrollerlist.h"
#include "midiguicomponent.h"
#include "module.h"
#include "midislider.h"
#include "intmidislider.h"
#include "midicombobox.h"
#include "midicheckbox.h"
#include "midipushbutton.h"
#include "guiwidget.h"



int MidiControllerModel::rowCount(const QModelIndex &parent) const
{
  if (!parent.isValid())
    return midiControllerList.count();

  const MidiController *c = (const MidiController *)parent.internalPointer();
  if (!c)
    return midiControllerList.at(parent.row()).context->mcAbles.count();

  return 0;
}

QVariant MidiControllerModel::data(const QModelIndex &index, int role) const
{
  if (index.isValid() && role == Qt::DisplayRole) {
    const MidiController *c = (const MidiController *)index.internalPointer();
    MidiControllableBase *mcAble = NULL;
    if (c)
      mcAble = c->context->mcAbles.at(index.row());

    if (mcAble) {
      if (index.column())
	return mcAble->module.configDialog->windowTitle();
      else
	return mcAble->name;
    } else
      if (!c && !index.column()) {
	QString qs;
	c = &midiControllerList.at(index.row());
	return qs.sprintf("type: %d channel: %d param: %d",
			  c->type(), c->ch(), c->param());
      }
  }
  return QVariant();
}

QVariant MidiControllerModel::headerData(int section, Qt::Orientation orientation,
					 int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return section ? "Module" : "MIDI Controller / Parameter";
  
  return QVariant();
}

QModelIndex MidiControllerModel::index(int row, int column,
				       const QModelIndex &parent) const
{
  if (parent.isValid())
    return createIndex(row, column,
		       (void*)&midiControllerList.at(parent.row()));
  else
    return createIndex(row, column, (void*)NULL);
}

QModelIndex MidiControllerModel::parent(const QModelIndex &child) const
{
  if (child.isValid() && child.internalPointer()) {
    const MidiController *c = (const MidiController *)child.internalPointer();
    int row = c - midiControllerList.data();
    return index(row, 0);
  }
  return QModelIndex();
}

int MidiControllerModel::columnCount(const QModelIndex &/*parent*/) const
{
  return 2;
}

void MidiControllerModel::insert(QVector<MidiController>::iterator c,
				 MidiController &mc)
{
  int row = &*c - midiControllerList.data();
  beginInsertRows(QModelIndex(), row, row);
  QVector<MidiController>::iterator n = midiControllerList.insert(c, mc);
  n->context = new MidiControllerContext();
  endInsertRows();
}

int ModuleModel::rowCount(const QModelIndex &parent) const
{
  if (!parent.isValid())
    return list.count();
  
  if (!parent.internalPointer())
    return list.at(parent.row())->midiControllables.count();

  return 0;
}

QVariant ModuleModel::data(const QModelIndex &index, int role) const
{
  if (index.isValid() && role == Qt::DisplayRole) {
    Module *m = (Module *)index.internalPointer();
    if (!m) {
      if (index.column() == 0 && index.row() < list.count())
	return list.at(index.row())->configDialog->windowTitle();
    } else
      if (index.row() < m->midiControllables.count()) {
	MidiControllableBase *mcAble = m->midiControllables.at(index.row());
	if (index.column())
	  return mcAble->midiSign ? "1" : "-1";
	else
	  return mcAble->name;
      }
  }
  return QVariant();
}

QVariant ModuleModel::headerData(int section, Qt::Orientation orientation,
				 int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return section ? "MIDI Sign" : "Module / Parameter";
  
  return QVariant();
}

QModelIndex ModuleModel::index(int row, int column, const QModelIndex &parent) const
{
  if (parent.isValid())
    return createIndex(row, column, list.at(parent.row()));
  else
    return createIndex(row, column, (void *)NULL);
}

QModelIndex ModuleModel::parent(const QModelIndex &child) const
{
  if (child.isValid() && child.internalPointer())
    return index(list.indexOf((Module *)child.internalPointer()), 0);

  return QModelIndex();
}

int ModuleModel::columnCount(const QModelIndex &/*parent*/) const
{
  return 2;
}


MidiWidget::MidiWidget(QWidget* parent, const char *name) 
  : QWidget(parent)
  , mgc(NULL)
  , vbox(this)
  , midiControllerModel(midiControllerList)
  , selectedControlMcAble(-1)
  , midiControllable(NULL)
{
  setObjectName(name);
  int l1;
  QString qs;

  vbox.setMargin(10);
  vbox.setSpacing(5);
  
  noteControllerEnabled = false;
  followConfig = false;
  followMidi = false;

  QSplitter *listViewBox = new QSplitter();
  vbox.addWidget(listViewBox, 2);
  midiControllerListView = new QTreeView(listViewBox);
  midiControllerListView->setModel(&midiControllerModel);
  midiControllerListView->setAllColumnsShowFocus(true);
  moduleListView = new QTreeView(listViewBox);
  moduleListView->setModel(&moduleModel);
  moduleListView->setAllColumnsShowFocus(true);
  QObject::connect(moduleListView->selectionModel(),
		   SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
		   this,
		   SLOT(guiControlChanged(const QItemSelection &, const QItemSelection &)));
  QObject::connect(midiControllerListView->selectionModel(),
		   SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
		   this,
		   SLOT(midiControlChanged(const QItemSelection &, const QItemSelection &)));
  QVBoxLayout *controlFrame = new QVBoxLayout();
  controlFrame->setSpacing(5);
  vbox.addLayout(controlFrame);
  guiControlParent = new QFrame(); // QVBoxLayout
  controlFrame->addWidget(guiControlParent);
  guiControlParent->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  currentGUIcontrol = new QVBoxLayout(guiControlParent);
  currentGUIcontrol->setMargin(5);
  floatHelperLayout = new QHBoxLayout();
  currentGUIcontrol->addLayout(floatHelperLayout);
  logCheck = new QCheckBox("Log");
  floatHelperLayout->addWidget(logCheck);
  QObject::connect(logCheck, SIGNAL(toggled(bool)), this, SLOT(setLogMode(bool)));
  floatHelperLayout->addStretch();
  newMinButton = new QPushButton("Set Min");
  floatHelperLayout->addWidget(newMinButton);
  QObject::connect(newMinButton, SIGNAL(clicked()), this, SLOT(setNewMin()));
  floatHelperLayout->addStretch();
  newMaxButton = new QPushButton("Set Max");
  floatHelperLayout->addWidget(newMaxButton);
  QObject::connect(newMaxButton, SIGNAL(clicked()), this, SLOT(setNewMax()));
  floatHelperLayout->addStretch();
  resetMinMaxButton = new QPushButton("Reset Min/Max");
  floatHelperLayout->addWidget(resetMinMaxButton);
  QObject::connect(resetMinMaxButton, SIGNAL(clicked()), this, SLOT(setInitialMinMax()));
  showFloatHelpers(false);

  //  currentGUIcontrol = NULL;
  QHBoxLayout *checkbuttonBox = new QHBoxLayout();
  controlFrame->addLayout(checkbuttonBox);
  checkbuttonBox->setSpacing(10);
  checkbuttonBox->setMargin(5);
  QStringList channelNames;
  channelNames << "Omni";
  for (l1 = 1; l1 < 17; l1++) {
    qs.sprintf("%4d", l1);
    channelNames << qs;
  }
  QHBoxLayout *midiChannelBox = new QHBoxLayout();
  controlFrame->addLayout(midiChannelBox);
  QLabel *channelText = new QLabel();
  midiChannelBox->addWidget(channelText);
  channelText->setText("MIDI Channel:");
  QComboBox *comboBox = new QComboBox();
  midiChannelBox->addWidget(comboBox);
  midiChannelBox->addStretch();
  midiChannelBox->addStretch();
  midiChannelBox->addStretch();
  comboBox->addItems(channelNames);
  comboBox->setFixedSize(comboBox->sizeHint());
  QObject::connect(comboBox, SIGNAL(highlighted(int)), this, SLOT(updateMidiChannel(int)));
  midiChannelBox->addStretch();
  addGuiButton = new QPushButton("Add to Parameter View");
  addGuiButton->setEnabled(false);
  midiChannelBox->addWidget(addGuiButton);
  QObject::connect(addGuiButton, SIGNAL(clicked()), this, SLOT(addToParameterViewClicked()));
  midiChannelBox->addStretch();
  QHBoxLayout *buttonBox = new QHBoxLayout();
  controlFrame->addLayout(buttonBox);
  buttonBox->setSpacing(5);
  buttonBox->setMargin(5);
  buttonBox->addStretch();
  noteCheck = new QCheckBox("Enable note events");
  checkbuttonBox->addWidget(noteCheck);
  noteCheck->setChecked(noteControllerEnabled);
  configCheck = new QCheckBox("Follow Configuration Dialog");
  checkbuttonBox->addWidget(configCheck);
  configCheck->setChecked(followConfig);
  midiCheck = new QCheckBox("Follow MIDI");
  checkbuttonBox->addWidget(midiCheck);
  midiCheck->setChecked(followMidi);         
  QObject::connect(noteCheck, SIGNAL(clicked()), this, SLOT(noteControllerCheckToggle()));
  QObject::connect(configCheck, SIGNAL(clicked()), this, SLOT(configCheckToggle()));
  QObject::connect(midiCheck, SIGNAL(clicked()), this, SLOT(midiCheckToggle()));
  buttonBox->addStretch();
  bindButton = new QPushButton("Bind");
  bindButton->setEnabled(false);
  buttonBox->addWidget(bindButton);
  buttonBox->addStretch();
  clearButton = new QPushButton("Clear Binding");
  buttonBox->addWidget(clearButton);
  clearButton->setEnabled(false);
  buttonBox->addStretch();
  clearAllButton = new QPushButton("Clear All");
  buttonBox->addWidget(clearAllButton);
  buttonBox->addStretch();
  midiSignButton = new QPushButton("Toggle MIDI Sign");
  buttonBox->addWidget(midiSignButton);
  midiSignButton->setEnabled(false);
  buttonBox->addStretch();
  QObject::connect(bindButton, SIGNAL(clicked()), this, SLOT(bindClicked()));
  QObject::connect(clearButton, SIGNAL(clicked()), this, SLOT(clearClicked()));
  QObject::connect(clearAllButton, SIGNAL(clicked()), this, SLOT(clearAllClicked()));
  QObject::connect(midiSignButton, SIGNAL(clicked()), this, SLOT(toggleMidiSign()));
}

MidiWidget::~MidiWidget() {

}

void MidiWidget::clearAllClicked()
{
  for (int l1 = 0; l1 < midiControllerList.count(); l1++) {
    const MidiController &c = midiControllerList.at(l1);
    while (c.context->mcAbles.count()) {
      MidiControllableBase *mcAble = c.context->mcAbles.at(0);
      mcAble->disconnectController(c);
    }
  }
  midiControllerModel.beginRemoveRows(QModelIndex(), 0, midiControllerList.count() - 1);
  midiControllerList.clear();
  midiControllerModel.endRemoveRows();  
}

void MidiWidget::addMidiController(MidiControllerKey mck)
{
  MidiController mc(mck.getKey());
  typeof(midiControllerList.end()) c(midiControllerList.end());
  if (!midiControllerList.empty()) {
    c = qLowerBound(midiControllerList.begin(), midiControllerList.end(), mck);
    if (c != midiControllerList.end()) {
      if (*c == mc) {
	return;
      }
    }
  }
  midiControllerModel.insert(c, mc);
  midiControllerListView->resizeColumnToContents(0);
}

void MidiWidget::addMidiControllable(MidiControllerKey mck,
				     MidiControllableBase *mcAble)
{
  typeof(midiControllerList.constEnd()) c
    = qBinaryFind(midiControllerList.constBegin(),
		  midiControllerList.constEnd(), mck);
  if (c == midiControllerList.end()) {
    StdErr  << __PRETTY_FUNCTION__ << ":" << __LINE__ << endl;
    return;
  }

  int row = &*c - midiControllerList.data();
  int childRow = c->context->mcAbles.count();

  midiControllerModel.beginInsertRows(midiControllerModel.index(row, 0),
				      childRow, childRow);
  c->context->mcAbles.append(mcAble);
  midiControllerModel.endInsertRows();  
  moduleListView->resizeColumnToContents(0);
}

void MidiWidget::removeMidiControllable(MidiControllerKey mck, MidiControllableBase *mcAble)
{
  typeof(midiControllerList.constEnd()) c
    = qBinaryFind(midiControllerList.constBegin(),
		  midiControllerList.constEnd(), mck);
  if (c == midiControllerList.end()) {
    StdErr  << __PRETTY_FUNCTION__ << ":" << __LINE__ << endl;
    return;
  }

  int row = &*c - midiControllerList.data();
  int childRow = c->context->mcAbles.indexOf(mcAble);
  if (childRow != -1) {
    midiControllerModel.beginRemoveRows(midiControllerModel.index(row, 0),
					childRow, childRow);
    c->context->mcAbles.removeAll(mcAble);
    midiControllerModel.endRemoveRows();  
  }
  setActiveMidiControllers();
}

void MidiWidget::clearClicked()
{
  if (selectedController.isValid() && selectedControlMcAble != -1) {
    typeof(midiControllerList.constEnd()) c
      = qBinaryFind(midiControllerList.constBegin(),
		    midiControllerList.constEnd(), selectedController);
    if (c == midiControllerList.end()) {
      StdErr  << __PRETTY_FUNCTION__ << ":" << __LINE__ << endl;
      return;
    }

    MidiControllableBase *mcAble = c->context->mcAbles.at(selectedControlMcAble);
    mcAble->disconnectController(selectedController);
  }
}

void MidiWidget::addToParameterViewClicked() {

  QString qs, qs2, qs3;
  bool ok, foundFrameName, foundTabName;
  int l1, frameIndex, tabIndex;

  if (!midiControllable)
    return;

  if (synthdata->guiWidget->presetCount > 0) {
    qs.sprintf("This will erase all presets for this configuration. Continue ?");
    QMessageBox questionContinue("AlsaModularSynth", qs, QMessageBox::NoIcon,
                                 QMessageBox::Yes | QMessageBox::Default, QMessageBox::No  | QMessageBox::Escape, QMessageBox::NoButton);
    if (questionContinue.exec() == QMessageBox::No) {
      return;
    }
  }
  qs = QInputDialog::getText(this, "AlsaModularSynth", "Add this parameter to frame:", QLineEdit::Normal, currentFrameName, &ok);
  currentFrameName = qs;
  if (qs.isEmpty()) {
    return;
  }
  foundFrameName = false;
  frameIndex = 0;
  if ((l1 =synthdata->guiWidget->frameNameList.indexOf(qs.trimmed())) >= 0) {
    foundFrameName = true;
    frameIndex = l1;
  }
  if (!foundFrameName) {
    qs2 = "Frame " + qs + " does not exist. Create ?"; 
    QMessageBox question("AlsaModularSynth", qs2, QMessageBox::NoIcon, QMessageBox::Yes | QMessageBox::Default,
                         QMessageBox::No  | QMessageBox::Escape, QMessageBox::NoButton);
    if (question.exec() == QMessageBox::Yes) {
      qs3 = QInputDialog::getText(this, "AlsaModularSynth", "Add frame to tab:", QLineEdit::Normal, currentTabName, &ok);
      currentTabName = qs3;
      foundTabName = false;
      tabIndex = 0;
      if ((l1 =synthdata->guiWidget->tabNameList.indexOf(qs3.trimmed())) >= 0) {
        foundTabName = true;
        tabIndex = l1;
        synthdata->guiWidget->setTab(tabIndex);
      } else {
        qs2 = "Tab " +qs3 + " does not exist. Create ?";
        QMessageBox question("AlsaModularSynth", qs2, QMessageBox::NoIcon, QMessageBox::Yes | QMessageBox::Default,
                             QMessageBox::No  | QMessageBox::Escape, QMessageBox::NoButton);
        if (question.exec() == QMessageBox::Yes) {
	  //printf("Creating tab %s.\n", qs3.latin1());
          synthdata->guiWidget->addTab(qs3.trimmed());
        } else {
          return;
        }
      }
      //      printf("Creating frame %s.\n", qs.latin1());
      synthdata->guiWidget->addFrame(qs.trimmed());
    } else {
      return;
    }
  } else
    synthdata->guiWidget->setFrame(frameIndex);

  //  qs2.sprintf("%s  ID %d", midiGUIcomponent->objectName(), module->moduleID);
  qs2 = midiControllable->name + "  ID " +
    QString().setNum(midiControllable->module.moduleID);
  qs = QInputDialog::getText(this, "AlsaModularSynth", "Parameter name:", QLineEdit::Normal, qs2, &ok);
  synthdata->guiWidget->addParameter(midiControllable, qs);
}

void MidiWidget::bindClicked()
{
  if (midiControllable && selectedController.isValid() && selectedControlMcAble == -1) {
    int row = midiControllerList.indexOf(selectedController.getKey());
    midiControllerListView->setExpanded(midiControllerModel.index(row, 0), true);
    midiControllable->connectToController(selectedController);
    setActiveMidiControllers();
  }
}

void MidiWidget::noteControllerCheckToggle() {

  noteControllerEnabled = noteCheck->isChecked();  
}

void MidiWidget::configCheckToggle() {

  followConfig = configCheck->isChecked();  
}

void MidiWidget::midiCheckToggle() {

  followMidi = midiCheck->isChecked();  
}

void MidiWidget::addModule(Module *m)
{
  if (!m->midiControllables.count())
    return;

  int row = moduleModel.list.count();

  moduleModel.beginInsertRows(QModelIndex(), row, row);
  moduleModel.list.append(m);
  moduleModel.endInsertRows();
}

void MidiWidget::removeModule(Module *m)
{
  synthdata->moduleList.removeAll(m);
  synthdata->decModuleCount();

  int row = moduleModel.list.indexOf(m);
  if (row == -1)
    return;

  if (m->midiControllables.contains(midiControllable)) {
    delete mgc;
    mgc = NULL;
    midiControllable = NULL;
  }
  moduleModel.beginRemoveRows(QModelIndex(), row, row);
  moduleModel.list.removeAll(m);
  moduleModel.endRemoveRows();
}  

void MidiWidget::toggleMidiSign()
{
  if (midiControllable == NULL)
    return;
  midiControllable->midiSign = !midiControllable->midiSign;
  Module *m = &midiControllable->module;
  QModelIndex mMi =
    moduleModel.index(moduleModel.list.indexOf(m), 0);
  QModelIndex mgcMi =
    moduleModel.index(m->midiControllables.indexOf(midiControllable), 1, mMi);
  emit moduleModel.dataChanged(mgcMi, mgcMi);
}

void MidiWidget::guiControlChanged(const QItemSelection &selected,
				   const QItemSelection &/*deselected*/)
{
  Module *module;
  bool success = false;

  if (midiControllable) {
    delete mgc;
    mgc = NULL;
    midiControllable = NULL;
  }

  if (selected.indexes().count() > 0) {
    const QModelIndex mi = selected.indexes().at(0);
    module = (Module *)mi.internalPointer();
    if (module && mi.row() < module->midiControllables.count()) {
      midiControllable = module->midiControllables.at(mi.row());
      success = true;
    }
  }
  midiSignButton->setEnabled(success);
  addGuiButton->setEnabled(success);
  if (!success)
    return;

  mgc = dynamic_cast<MidiGUIcomponent *>(midiControllable->mcws.at(0))->createTwin();
  currentGUIcontrol->insertWidget(0, mgc);//, 10, Qt::AlignHCenter);
  showFloatHelpers(dynamic_cast<MidiControllableFloat *>(midiControllable));
}

void MidiWidget::midiControlChanged(const QItemSelection &selected,
				    const QItemSelection &/*deselected*/)
{
  selectedController = MidiControllerKey();
  selectedControlMcAble = -1;
  bool bindEnable = false,
    clearEnable = false;

  if (selected.indexes().count() > 0) {
    const QModelIndex mi = selected.indexes().at(0);
    const MidiController *mc = (const MidiController *)mi.internalPointer();
    if (mc) {
      selectedController = ((const MidiController *)mi.internalPointer())->getKey();
      selectedControlMcAble = mi.row();
      clearEnable = true;
    } else {
      selectedController = midiControllerList.at(mi.row()).getKey();
      bindEnable = true;
    }
  }
  bindButton->setEnabled(bindEnable);
  clearButton->setEnabled(clearEnable);
}

void MidiWidget::setLogMode(bool on)
{
  dynamic_cast<MidiControllableFloat *>(midiControllable)->setLog(on);
}

void MidiWidget::setNewMin()
{
  dynamic_cast<MidiControllableFloat *>(midiControllable)->setNewMin();
}

void MidiWidget::setNewMax()
{        
  dynamic_cast<MidiControllableFloat *>(midiControllable)->setNewMax();
}  

void MidiWidget::setInitialMinMax()
{
  dynamic_cast<MidiControllableFloat *>(midiControllable)->resetMinMax();
}  

void MidiWidget::setSelectedController(MidiControllerKey mck)
{
  if (!(mck == selectedController)) {
    typeof(midiControllerList.constEnd()) c
      = qBinaryFind(midiControllerList.constBegin(),
		    midiControllerList.constEnd(), mck);
    if (c == midiControllerList.end()) {
      StdErr  << __PRETTY_FUNCTION__ << ":" << __LINE__ << endl;
      return;
    }
    midiControllerListView->selectionModel()->
      select(midiControllerModel.
	     index(&*c - midiControllerList.data(), 0),
	     QItemSelectionModel::ClearAndSelect);
  }
}

void MidiWidget::selectMcAble(MidiControllableBase &mcAble)
{
  int row = moduleModel.list.indexOf(&mcAble.module);
  QModelIndex index = moduleModel.index(row, 0);
  row = mcAble.module.midiControllables.indexOf(&mcAble);
  index = moduleModel.index(row, 0, index);
  moduleListView->scrollTo(index);
  moduleListView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
}

void MidiWidget::showFloatHelpers(bool show)
{
  if (show) {
    logCheck->blockSignals(true);
    logCheck->setChecked(static_cast<MidiControllableFloat *>(midiControllable)->isLog);
    logCheck->blockSignals(false);
  }
  logCheck->setVisible(show);
  newMinButton->setVisible(show);
  newMaxButton->setVisible(show);
  resetMinMaxButton->setVisible(show);
}

void MidiWidget::updateMidiChannel(int index) {

  synthdata->midiChannel = index - 1;
}

void MidiWidget::setActiveMidiControllers()
{
  typeof(synthdata->activeMidiControllers) New = new typeof(*synthdata->activeMidiControllers);
  for (typeof(midiControllerList.constBegin()) mc = midiControllerList.constBegin();
       mc != midiControllerList.constEnd(); ++mc) {
    MidiControllerContext *amcc = NULL;
    for (typeof(mc->context->mcAbles.constBegin()) mca = mc->context->mcAbles.constBegin();
	 mca != mc->context->mcAbles.constEnd(); ++mca)
      if ((*mca)->module.connected()) {
	if (!amcc) {
	  New->append(mc->getKey());
	  amcc = New->back().context = new MidiControllerContext;
	}
	amcc->mcAbles.append(*mca);
      }
  }

  typeof(synthdata->activeMidiControllers) old = synthdata->activeMidiControllers;

  pthread_mutex_lock(&synthdata->rtMutex);
  synthdata->activeMidiControllers = New;
  pthread_mutex_unlock(&synthdata->rtMutex);

  delete old;
}
