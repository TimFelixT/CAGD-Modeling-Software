#include "header/Gui.h"
#include "header/ViewPanel.h" //Vermeidet circular dependencies

Gui::Gui(ViewPanel* view, void (*glutRedisplay)(), bool *initNeeded) {
	viewPanel = view;
	glutPostRedisplay = glutRedisplay;
	needInit = initNeeded;
}

Gui::~Gui() {}

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
	global["OnAddNewPoint"] = BindJSCallback(&Gui::OnAddNewPoint);
	global["OnDeleteLastPoint"] = BindJSCallback(&Gui::OnDeleteLastPoint);
	global["OnPointChange"] = BindJSCallback(&Gui::OnPointChange);
	global["OnToggleBezier"] = BindJSCallback(&Gui::OnToggleBezier);
	global["OnToggleHighlightControlpoints"] = BindJSCallback(&Gui::OnToggleHighlightControlpoints);
	global["OnDegreeIncrease"] = BindJSCallback(&Gui::OnDegreeIncrease);
	global["OnResetCurves"] = BindJSCallback(&Gui::OnResetCurves);

	loadData();
}


void Gui::addCurves() {
	JSContextRef myContext = overlay->view()->js_context();
	SetJSContext(myContext);
	JSObject global = JSGlobalObject();
	JSFunction addCurve = global["addCurve"];

	if (addCurve.IsValid()) {
		JSArgs args;

		for (int i = 0; i < viewPanel->allCurves.size(); i++) {

			CurveBezier& c = *(viewPanel->allCurves[i]);

			std::vector<PointVector> points = c.obj->getVertices();
			for (PointVector pv : points) {
				args.push_back(pv.xCoor);
				args.push_back(pv.yCoor);
				args.push_back(pv.zCoor);
				args.push_back(pv.homoCoor);

			}
			args.push_back(i);
			if (dynamic_cast<Bernstein*>(&c)) {
				args.push_back(0);
			} else {
				args.push_back(1);
			}
			JSValue result = addCurve(args);
			args.clear();
		}		
	}
}
void Gui::OnResetCurves(const JSObject& thisObject, const JSArgs& args) {
	addCurves();
}

void Gui::OnAddNewPoint(const JSObject& thisObject, const JSArgs& args) {

}
void Gui::OnDeleteLastPoint(const JSObject& thisObject, const JSArgs& args) {

}
void Gui::OnPointChange(const JSObject& thisObject, const JSArgs& args) {
	int pointIndex = args[0].ToInteger();
	int coorIndex = args[1].ToInteger();
	double value = args[2].ToNumber();
	int curveIndex = args[3].ToInteger();

	CurveBezier& c = *(viewPanel->allCurves.at(curveIndex));


	switch (coorIndex) {
	case 0:
		c.obj->vertices.at(pointIndex).xCoor = value;
		break;
	case 1:
		c.obj->vertices.at(pointIndex).yCoor = value;
		break;
	case 2:
		c.obj->vertices.at(pointIndex).zCoor = value;
		break;
	case 3:
		c.obj->vertices.at(pointIndex).homoCoor = value;
		break;
	}
	c.updateCurveBuffer();
	c.setInitialized(false);
	updateDisplay();
	
}
void Gui::OnToggleBezier(const JSObject& thisObject, const JSArgs& args) {
	int type = args[0].ToInteger();
	if (type == 0) {
		viewPanel->toggleBezierCurve();
	}
	else {
		viewPanel->toggleBezierCurve();
	}
	glutPostRedisplay();
}
void Gui::OnToggleHighlightControlpoints(const JSObject& thisObject, const JSArgs& args) {
	viewPanel->showPoints();
	glutPostRedisplay();
}
void Gui::OnDegreeIncrease(const JSObject& thisObject, const JSArgs& args) {
	if (viewPanel->bezier_toggle) {
		viewPanel->degreeIncrease();
		updateDisplay();
	}
}

void Gui::updateDisplay() {
	*needInit = true;
	glutPostRedisplay();
}








