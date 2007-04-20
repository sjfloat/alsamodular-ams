######################################################################
# 
######################################################################

TEMPLATE = app

INCLUDEPATH += .
LIBS += -lclalsadrv -ljack -lasound

LADSPA_PATH = $$(LADSPA_PATH)
isEmpty( LADSPA_PATH ) {
LADSPA_PATH = /usr/lib/ladspa:/usr/local/lib/ladspa
}

QMAKE_CXXFLAGS += -DLADSPA_PATH=\"$$LADSPA_PATH\"

# Input
HEADERS += canvas.h \
           canvasfunction.h \
           configdialog.h \
           envelope.h \
           filter.h \
           floatintmidislider.h \
           function.h \
           guiwidget.h \
           intmidislider.h \
           ladspadialog.h \
           m_ad.h \
           m_advenv.h \
           m_advmcv.h \
           m_amp.h \
           m_conv.h \
           m_cvs.h \
           m_delay.h \
           m_dynamicwaves.h \
           m_env.h \
           m_function.h \
           m_inv.h \
           m_ladspa.h \
           m_lfo.h \
           m_mcv.h \
           m_midiout.h \
           m_mix.h \
           m_mphlfo.h \
           m_noise.h \
           m_noise2.h \
           m_pcmin.h \
           m_pcmout.h \
           m_quantizer.h \
           m_ringmod.h \
           m_scmcv.h \
           m_scope.h \
           m_scquantizer.h \
           m_seq.h \
           m_sh.h \
           m_slew.h \
           m_spectrum.h \
           m_stereomix.h \
           m_vca.h \
           m_vcdoubledecay.h \
           m_vcenv.h \
           m_vcenv2.h \
           m_vcf.h \
           m_vco.h \
           m_vco2.h \
           m_vcorgan.h \
           m_vcpanning.h \
           m_vcswitch.h \
           m_vquant.h \
           m_wavout.h \
           main.h \
           midicheckbox.h \
           midicombobox.h \
           midicontroller.h \
           midicontrollerlist.h \
           midiguicomponent.h \
           midipushbutton.h \
           midislider.h \
           midiwidget.h \
           modularsynth.h \
           module.h \
           multi_envelope.h \
           port.h \
           prefwidget.h \
           scopescreen.h \
           synthdata.h \
           textedit.h
SOURCES += canvas.cpp \
           canvasfunction.cpp \
           configdialog.cpp \
           envelope.cpp \
           filter.cpp \
           floatintmidislider.cpp \
           function.cpp \
           guiwidget.cpp \
           intmidislider.cpp \
           ladspadialog.cpp \
           m_ad.cpp \
           m_advenv.cpp \
           m_advmcv.cpp \
           m_amp.cpp \
           m_conv.cpp \
           m_cvs.cpp \
           m_delay.cpp \
           m_dynamicwaves.cpp \
           m_env.cpp \
           m_function.cpp \
           m_inv.cpp \
           m_ladspa.cpp \
           m_lfo.cpp \
           m_mcv.cpp \
           m_midiout.cpp \
           m_mix.cpp \
           m_mphlfo.cpp \
           m_noise.cpp \
           m_noise2.cpp \
           m_pcmin.cpp \
           m_pcmout.cpp \
           m_quantizer.cpp \
           m_ringmod.cpp \
           m_scmcv.cpp \
           m_scope.cpp \
           m_scquantizer.cpp \
           m_seq.cpp \
           m_sh.cpp \
           m_slew.cpp \
           m_spectrum.cpp \
           m_stereomix.cpp \
           m_vca.cpp \
           m_vcdoubledecay.cpp \
           m_vcenv.cpp \
           m_vcenv2.cpp \
           m_vcf.cpp \
           m_vco.cpp \
           m_vco2.cpp \
           m_vcorgan.cpp \
           m_vcpanning.cpp \
           m_vcswitch.cpp \
           m_vquant.cpp \
           m_wavout.cpp \
           main.cpp \
           midicheckbox.cpp \
           midicombobox.cpp \
           midicontroller.cpp \
           midicontrollerlist.cpp \
           midiguicomponent.cpp \
           midipushbutton.cpp \
           midislider.cpp \
           midiwidget.cpp \
           modularsynth.cpp \
           module.cpp \
           multi_envelope.cpp \
           port.cpp \
           prefwidget.cpp \
           scopescreen.cpp \
           synthdata.cpp \
           textedit.cpp
