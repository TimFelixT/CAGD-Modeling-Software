#include "header\Gui.h"

Gui::Gui(ViewPanel* view, void (*glutRedisplay)(), bool *initNeeded) {
	viewPanel = view;
	glutPostRedisplay = glutRedisplay;
	needInit = initNeeded;
}

void Gui::loadData() {
	addCurves();
}



void Gui::run() {
	app = App::Create();
	window = Window::Create(app->main_monitor(), 900, 600, false, kWindowFlags_Titled);
	window->SetTitle("CADG Gui");
	app->set_window(*window);
	overlay = Overlay::Create(*window, window->width(), window->height(), 0, 0);
	overlay->view()->set_load_listener(this);
	overlay->view()->LoadURL("file:///gui.html");


	app->Run();
}

void Gui::OnDOMReady(ultralight::View* caller) {
	SetJSContext(caller->js_context());
	JSObject global = JSGlobalObject();
	global["OnPointChange"] = BindJSCallback(&Gui::OnPointChange);

	loadData();
}


void Gui::addCurves() {
	JSContextRef myContext = overlay->view()->js_context();
	SetJSContext(myContext);
	JSObject global = JSGlobalObject();
	JSFunction addCurve = global["addCurve"];

	if (addCurve.IsValid()) {
		JSArgs args;
		std::vector<PointVector> points = viewPanel->bernstein_bezier->obj->getVertices();
		for (PointVector pv : points) {
			args.push_back(pv.xCoor);
			args.push_back(pv.yCoor);
			args.push_back(pv.zCoor);
			args.push_back(pv.homoCoor);
		}
		JSValue result = addCurve(args);
	}
}

void Gui::OnPointChange(const JSObject& thisObject, const JSArgs& args) {
	int point = args[0].ToInteger();
	int index = args[1].ToInteger();
	double value = args[2].ToNumber();

	cout << point << endl;
	cout << index << endl;
	cout << value << endl;

	switch (index) {
	case 0:
		viewPanel->bernstein_bezier->obj->vertices.at(point).xCoor = value;
		break;
	case 1:
		viewPanel->bernstein_bezier->obj->vertices.at(point).yCoor = value;
		break;
	case 2:
		viewPanel->bernstein_bezier->obj->vertices.at(point).zCoor = value;
		break;
	case 3:
		viewPanel->bernstein_bezier->obj->vertices.at(point).homoCoor = value;
		break;
	}
	viewPanel->bernstein_bezier->updateCurveBuffer();
	viewPanel->bernstein_bezier->setInitialized(false);
	updateDisplay();
}

void Gui::updateDisplay() {
	*needInit = true;
	glutPostRedisplay();
}



void Gui::refresh() {

}




