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
	addSurfaces();
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
	global["OnChangeDeCasteljauTRange"] = BindJSCallback(&Gui::OnChangeDeCasteljauTRange);
	global["OnDerivativeToggle"] = BindJSCallback(&Gui::OnDerivativeToggle);
	global["OnDerivativeTChange"] = BindJSCallback(&Gui::OnDerivativeTChange);

	//Flächen
	global["OnToggleShader"] = BindJSCallback(&Gui::OnToggleShader);
	global["OnToggleSurface"] = BindJSCallback(&Gui::OnToggleSurface);
	global["OnIncreaseSurfaceT"] = BindJSCallback(&Gui::OnIncreaseSurfaceT);
	global["OnDecreaseSurfaceT"] = BindJSCallback(&Gui::OnDecreaseSurfaceT);
	global["OnSurfacePointEdited"] = BindJSCallback(&Gui::OnSurfacePointEdited);
	global["OnSplitSurface"] = BindJSCallback(&Gui::OnSplitSurface);
	global["OnSurfaceDegreeIncrease"] = BindJSCallback(&Gui::OnSurfaceDegreeIncrease);
	global["OnResetSurfaces"] = BindJSCallback(&Gui::OnResetSurfaces);

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

void Gui::addSurfaces() {
	JSContextRef myContext = overlay->view()->js_context();
	SetJSContext(myContext);
	JSObject global = JSGlobalObject();
	JSFunction addSurface = global["addSurface"];

	if (addSurface.IsValid()) {
		JSArgs args;

		for (int i = 0; i < viewPanel->allSurfaces.size(); i++) {
			PolyObject& controlStructure = *(viewPanel->allSurfaces[i]->controlStructure);
			for (PointVector pv : controlStructure.getVertices()) {
				args.push_back(pv.xCoor);
				args.push_back(pv.yCoor);
				args.push_back(pv.zCoor);
				args.push_back(pv.weight);
			}
			args.push_back(i);
			JSValue result = addSurface(args);
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
	
	if (dynamic_cast<Bernstein*>(&c)) {
		viewPanel->drawStructure(0);
	}
	else {
		viewPanel->drawStructure(1);
	}
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
		if (dynamic_cast<Bernstein*>(&c)) {
			viewPanel->drawStructure(0);
		}
		else {
			viewPanel->drawStructure(1);
		}
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
	if (dynamic_cast<Bernstein*>(&c)) {
		viewPanel->drawStructure(0);
	}
	else {
		viewPanel->drawStructure(1);
	}
	updateDisplay();
}
void Gui::OnToggleBezier(const JSObject& thisObject, const JSArgs& args) {
	viewPanel->toggleBezierCurve();
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

		
		CurveBezier& tmpCurve = *(viewPanel->allCurves.at(curveIndex));

		float u = 1;
		float tmp = t_vec[0];
		float fak = 1;
		for (int i = 0; i < t_vec.size(); i++){
			if(i != 0)fak *= (i + 1);
			tmp = (t_vec[i] / u) / fak;
			viewPanel->subdivision(tmp, newCurve, curveIndex);
			std::reverse(newCurve.begin(), newCurve.end());

			PolyObject* po = new PolyObject(viewPanel->program);
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
				Bernstein* b = new Bernstein(controlPoints, viewPanel->program);
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
			u = u - t_vec[i];
			curveIndex++;
			newCurve.clear();
		}
		if (dynamic_cast<Bernstein*>(&tmpCurve)) {
			viewPanel->drawStructure(0);
		} else {
			viewPanel->drawStructure(1);
		}
		
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

void Gui::OnChangeDeCasteljauTRange(const JSObject& thisObject, const JSArgs& args) {
	for (CurveBezier* c : viewPanel->allCurves) {
		if (dynamic_cast<DeCasteljau*>(c)) {
			dynamic_cast<DeCasteljau*>(c)->toggleTRange();
			c->setInitialized(false);
		}
	}
	updateDisplay();
}

void Gui::OnDerivativeToggle(const JSObject& thisObject, const JSArgs& args) {
	viewPanel->derivative();
}
void Gui::OnDerivativeTChange(const JSObject& thisObject, const JSArgs& args) {
	float t = args[0].ToNumber();
	
	for (CurveBezier* c : viewPanel->allCurves) {
		c->derivative_t = t;
		c->bezier_derivative();
	}
	updateDisplay();
}

void Gui::OnToggleSurface(const JSObject& thisObject, const JSArgs& args) {
	viewPanel->toggleSurface();
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
	updateDisplay();
}
void Gui::OnDecreaseSurfaceT(const JSObject& thisObject, const JSArgs& args) {
	for (int i = 0; i < viewPanel->allSurfaces.size(); i++) {
		viewPanel->allSurfaces.at(i)->decreaseTesselatingRate();
	}
	updateDisplay();
}
void Gui::OnSurfacePointEdited(const JSObject& thisObject, const JSArgs& args) {
	int pointIndex = args[0].ToInteger();
	int coorIndex = args[1].ToInteger();
	double value = args[2].ToNumber();
	int curveIndex = args[3].ToInteger();

	Bezier_Surface& s = *(viewPanel->allSurfaces.at(curveIndex));

	switch (coorIndex) {
	case 0:
		s.controlStructure->vertices.at(pointIndex).xCoor = value;
		break;
	case 1:
		s.controlStructure->vertices.at(pointIndex).yCoor = value;
		break;
	case 2:
		s.controlStructure->vertices.at(pointIndex).zCoor = value;
		break;
	case 3:
		s.controlStructure->vertices.at(pointIndex).weight = value;
		break;
	}
	s.buildControlStructure();
	s.updateBezierSurface();
	updateDisplay();
}

void Gui::OnSplitSurface(const JSObject& thisObject, const JSArgs& args) {
	int surfaceIndex = args[0].ToInteger();
	float splitU = args[1].ToNumber();
	float splitV = args[2].ToNumber();

	if (surfaceIndex == -1) {
		cout << "Keine gueltige Eingabe zur Unterteilung!" << endl;
	} else {
		// Hier aufteilen
		Bezier_Surface& s = *(viewPanel->allSurfaces.at(surfaceIndex));
		s.subdivideU(splitU, splitV, &viewPanel->allSurfaces);
		updateDisplay();
	}	
}
void Gui::OnSurfaceDegreeIncrease(const JSObject& thisObject, const JSArgs& args) {
	//0 = Increase u Richtung, 1 = Increase v Richtung
	int uOrV = args[0].ToInteger();
	int surfaceIndex = args[1].ToInteger();

	Bezier_Surface& s = *(viewPanel->allSurfaces.at(surfaceIndex));

	if (uOrV == 0) {
		s.degree_increase_u();
	} else {
		s.degree_increase_v();
	}
	updateDisplay();
}

void Gui::OnResetSurfaces(const JSObject& thisObject, const JSArgs& args) {
	addSurfaces();
}



void Gui::updateDisplay() {
	*needInit = true;
	glutPostRedisplay();
}








