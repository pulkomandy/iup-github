ifeq ($(OS), Windows_NT)
  TECMAKE_CMD = $(MAKE) --no-print-directory -f ../tecmakewin.mak TEC_UNAME=$(TEC_UNAME)
  WINLIBS = iupluaole iupluafiledlg
else
  TECMAKE_CMD = $(MAKE) --no-print-directory -f ../tecmake.mak
endif

ifneq ($(shell uname), Haiku)
  SCINTILLA = iuplua_scintilla iupluascripterdlg
endif

.PHONY: do_all iuplua iupluacd iupluacontrols iupluagl iupluaglcontrols iuplua_plot iuplua_mglplot $(SCINTILLA) iupluaim iupluaimglib iupluatuio iupluaole iupluafiledlg iupluaweb
do_all: iuplua iupluacd iupluacontrols iupluagl iupluaglcontrols iuplua_plot iuplua_mglplot $(SCINTILLA) iupluaim iupluaimglib iupluatuio $(WINLIBS) iupluaweb

iuplua:
	@$(TECMAKE_CMD)  

iupluacd:
	@$(TECMAKE_CMD) MF=iupcd 

iupluacontrols:
	@$(TECMAKE_CMD) MF=iupcontrols 

iupluagl:
	@$(TECMAKE_CMD) MF=iupgl 

iupluaglcontrols:
	@$(TECMAKE_CMD) MF=iupglcontrols 

iuplua_plot:
	@$(TECMAKE_CMD) MF=iup_plot 

iuplua_mglplot:
	@$(TECMAKE_CMD) MF=iup_mglplot 

iuplua_scintilla:
	@$(TECMAKE_CMD) MF=iup_scintilla 

iupluaim:
	@$(TECMAKE_CMD) MF=iupim 

iupluaimglib:
	@$(TECMAKE_CMD) MF=iupimglib 
  
iupluatuio:
	@$(TECMAKE_CMD) MF=iuptuio 
  
iupluaole:
	@$(TECMAKE_CMD) MF=iupole 
  
iupluafiledlg:
	@$(TECMAKE_CMD) MF=iupfiledlg
  
iupluaweb:
	@$(TECMAKE_CMD) MF=iupweb 
  
iupluascripterdlg:
	@$(TECMAKE_CMD) MF=iupluascripterdlg
