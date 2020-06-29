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

	//Kurven
	global["OnAddNewPoint"] = BindJSCallback(&Gui::OnAddNewPoint);
	global["OnDeleteLastPoint"] = BindJSCallback(&Gui::OnDeleteLastPoint);
	global["OnPointChange"] = BindJSCallback(&Gui::OnPointChange);
	global["OnToggleBezier"] = BindJSCallback(&Gui::OnToggleBezier);
	global["OnToggleHighlightControlpoints"] = BindJSCallback(&Gui::OnToggleHighlightControlpoints);
	global["OnDegreeIncrease"] = BindJSCallback(&Gui::OnDegreeIncrease);
	global["OnResetCurves"] = BindJSCallback(&Gui::OnResetCurves);
	global["OnChangeStructureT"] = BindJSCallback(&Gui::OnChangeStructureT);
	global["OnToggleStructure"] = BindJSCallback(&Gui::OnToggleStructure);
	global["OnSplitCurve"] = BindJSCallback(&Gui::OnSplitCurve);
	global["OnCreateCurve"] = BindJSCallback(&Gui::OnCreateCurve);
	global["OnDeleteCurve"] = BindJSCallback(&Gui::OnDeleteCurve);
	global["OnCenterCurve"] = BindJSCallback(&Gui::OnCenterCurve);

	//Flächen
	global["OnToggleShader"] = BindJSCallback(&Gui::OnToggleShader);
	global["OnIncreaseSurfaceT"] = BindJSCallback(&Gui::OnIncreaseSurfaceT);
	global["OnDecreaseSurfaceT"] = BindJSCallback(&Gui::OnDecreaseSurfaceT);

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
				args.push_back(pv.weight);

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

void Gui::addCurveSubDivide(std::vector<PointVector> points) {
	JSContextRef myContext = overlay->view()->js_context();
	SetJSContext(myContext);
	JSObject global = JSGlobalObject();
	JSFunction addCurve = global["addCurve"];

	if (addCurve.IsValid()) {
		JSArgs args;

		for (int i = 0; i < viewPanel->allCurves.size(); i++) {

			CurveBezier& c = *(viewPanel->allCurves[i]);

			for (PointVector pv : points) {
				args.push_back(pv.xCoor);
				args.push_back(pv.yCoor);
				args.push_back(pv.zCoor);
				args.push_back(pv.homoCoor);

			}
			args.push_back(i);
			if (dynamic_cast<Bernstein*>(&c)) {
				args.push_back(0);
			}
			else {
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
	double x = args[0].ToNumber();
	double y = args[1].ToNumber();
	double z = args[2].ToNumber();
	double homo = args[3].ToNumber();
	int curveIndex = args[4].ToInteger();

	CurveBezier& c = *(viewPanel->allCurves.at(curveIndex));
	c.addPointEnd(PointVector(x, y, z, homo));
	c.updateCurveBuffer();
	c.setInitialized(false);
	updateDisplay();

}
void Gui::OnDeleteLastPoint(const JSObject& thisObject, const JSArgs& args) {
	int curveIndex = args[0].ToInteger();
	int canDelete = args[1].ToInteger();

	if (canDelete == -1) {
		cout << "Kein Loeschen - 3 Punkte sollte eine Bezierkurve schon haben!" << endl;
	} else {

		CurveBezier& c = *(viewPanel->allCurves.at(curveIndex));
		c.deleteLastPoint();
		c.updateCurveBuffer();
		c.setInitialized(false);
		updateDisplay();
	}

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
		c.obj->vertices.at(pointIndex).weight = value;
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
void Gui::OnToggleStructure(const JSObject& thisObject, const JSArgs& args) {
	viewPanel->toggleStructure();
	updateDisplay();
}
void Gui::OnChangeStructureT(const JSObject& thisObject, const JSArgs& args) {
	double tValue = args[0].ToNumber();
	int curveType = args[1].ToInteger();

	if (curveType == 0) {
		viewPanel->bernsteinT = tValue;
	}
	else {
		viewPanel->deCasteljauT = tValue;
	}

	viewPanel->drawStructure(curveType);
	updateDisplay();

}

void Gui::OnSplitCurve(const JSObject& thisObject, const JSArgs& args) {

	vector<float> t_vec;
	std::vector<PointVector> newCurve;

	if (args[0].IsArray()) {
		JSArray arr = args[0].ToArray();
		for (int i = 0; i < arr.length(); i++) {
			t_vec.push_back(arr[i].ToNumber());
		}
		int curveIndex = args[1].ToInteger();

		viewPanel->subdivision(t_vec[0], newCurve, curveIndex);

		PolyObject *po = new PolyObject(viewPanel->program);
		po->setVertices(newCurve);

		PolyObject* controlPoints = new PolyObject(viewPanel->program);
		controlPoints->setVertices(newCurve);
		for (int i = 0; i < newCurve.size() - 1; i++) {
			controlPoints->pushIndex(i);
			controlPoints->pushIndex(i + 1);
			controlPoints->pushColor();
		}
		controlPoints->pushColor();

		CurveBezier& c = *(viewPanel->allCurves.at(curveIndex));
		if (dynamic_cast<Bernstein*>(&c)) {
			Bernstein* b = new Bernstein(controlPoints,viewPanel->program);
			b->obj->setVertices(newCurve);
			b->setControlStructure(po);
			b->setInitialized(false);
			viewPanel->allCurves.push_back(b);
		}
		else {
			DeCasteljau* d = new DeCasteljau(controlPoints, viewPanel->program);
			d->obj->setVertices(newCurve);					
			d->setControlStructure(po);
			d->setInitialized(false);
			viewPanel->allCurves.push_back(d);
		}
		c.updateCurveBuffer();
		c.initialized = false;
		updateDisplay();
	} else {
		cout << "Keine Korrekte Eingabe zur Unterteilung!" << endl;
	}
}
void Gui::OnCenterCurve(const JSObject& thisObject, const JSArgs& args) {
	int curveIndex = args[0].ToInteger();
	CurveBezier& c = *(viewPanel->allCurves.at(curveIndex));
	c.centerCurve();
	c.initialized = false;
	updateDisplay();
}

void Gui::OnCreateCurve(const JSObject& thisObject, const JSArgs& args) {

}
void Gui::OnDeleteCurve(const JSObject& thisObject, const JSArgs& args) {
	int curveIndex = args[0].ToInteger();
	cout << curveIndex << endl;

	viewPanel->allCurves.erase(viewPanel->allCurves.begin() + curveIndex);
	updateDisplay();
}

void Gui::OnToggleShader(const JSObject& thisObject, const JSArgs& args) {
	int shaderIndex = args[0].ToInteger();
	cout << "Shader: " << shaderIndex << endl;


	//updateDisplay();
}
void Gui::OnIncreaseSurfaceT(const JSObject& thisObject, const JSArgs& args) {
	for (int i = 0; i < viewPanel->allSurfaces.size(); i++) {
		viewPanel->allSurfaces.at(i)->increaseTesselatingRate();
	}
}
void Gui::OnDecreaseSurfaceT(const JSObject& thisObject, const JSArgs& args) {
	for (int i = 0; i < viewPanel->allSurfaces.size(); i++) {
		viewPanel->allSurfaces.at(i)->decreaseTesselatingRate();
	}
}




void Gui::updateDisplay() {
	*needInit = true;
	glutPostRedisplay();
}








