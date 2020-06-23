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
    void OnPointChange(const JSObject& thisObject, const JSArgs& args);

    /* OpenGL Funktionspointer */
    void (*glutPostRedisplay)();
    bool* needInit;
    void updateDisplay();
};