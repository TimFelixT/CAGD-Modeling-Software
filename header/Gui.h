#include <Ultralight/Ultralight.h>
#include <AppCore/AppCore.h>
#include "ViewPanel.h"

using namespace ultralight;

class Gui : public LoadListener {

public:
    Gui(ViewPanel*, void (*)(), bool*);
    virtual ~Gui() {};
    void loadData();
    void run();

    void addCurves();
    void refresh();

    //virtual void OnClose() override {};
    //virtual void OnResize(uint32_t width, uint32_t height) override {};

private:
	RefPtr<App> app;
	RefPtr<Window> window;
	RefPtr<Overlay> overlay;
    ViewPanel* viewPanel;

    virtual void Gui::OnDOMReady(ultralight::View*);
    
    /* JS Methods */
    void OnAddNewPoint(const JSObject&, const JSArgs&);
    void OnDeleteLastPoint(const JSObject&, const JSArgs&);
    void OnPointChange(const JSObject&, const JSArgs&);
    void OnToggleBezier(const JSObject&, const JSArgs&);
    void OnToggleHighlightControlpoints(const JSObject&, const JSArgs&);
    void OnDegreeIncrease(const JSObject&, const JSArgs&);

    /* OpenGL Funktionspointer */
    void (*glutPostRedisplay)();
    bool* needInit;
    void updateDisplay();
};