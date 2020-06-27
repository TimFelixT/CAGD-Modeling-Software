#pragma once

#include <Ultralight/Ultralight.h>
#include <AppCore/AppCore.h>

using namespace ultralight;
using namespace std;

class ViewPanel; //Vermeidet circular dependencies

class Gui : public LoadListener {

public:
    Gui(ViewPanel*, void (*)(), bool*);
    ~Gui();
    void loadData();
    void run();


    void addCurves();

private:
	RefPtr<App> app;
	RefPtr<Window> window;
	RefPtr<Overlay> overlay;
    ViewPanel* viewPanel;

    virtual void Gui::OnDOMReady(ultralight::View*);
    
    /* JS Methods */
    void OnResetCurves(const JSObject&, const JSArgs&);
    void OnAddNewPoint(const JSObject&, const JSArgs&);
    void OnDeleteLastPoint(const JSObject&, const JSArgs&);
    void OnPointChange(const JSObject&, const JSArgs&);
    void OnToggleBezier(const JSObject&, const JSArgs&);
    void OnToggleHighlightControlpoints(const JSObject&, const JSArgs&);
    void OnDegreeIncrease(const JSObject&, const JSArgs&);
    void OnToggleStructure(const JSObject&, const JSArgs&);
    void OnChangeStructureT(const JSObject&, const JSArgs&);


    /* OpenGL Funktionspointer */
    void (*glutPostRedisplay)();
    bool* needInit;
    void updateDisplay();
};