#pragma once
#include "Graphics.h"
#include <string>
#include "Vec2.h"
#include "MainWindow.h"
#include "Font.h"
#include "Colors.h"
#include "Light.h"

using namespace std;

enum EventType {
	buttonDown
};



class Interface {
public:
	struct InterfaceEvent {
		EventType type;
		string parent;
	};
	class BaseInterfaceElement {
	public:
		void Draw(Font &font, Graphics & gfx);
		Vec2 GetCenterVec();
		string GetText();
		BaseInterfaceElement(string name_in, Vec2 vec_in, 
			int width_in, int height_in, Color c_in, int border_in=0);
	public:
		Interface *intf;
		string name;
		Vec2 vec;
		int width, height;
		int border = 0;
		Color c;
	};
	class Label : public BaseInterfaceElement {
	public:
		using BaseInterfaceElement::BaseInterfaceElement;
	};
	class Button : public BaseInterfaceElement{
	public:
		using BaseInterfaceElement::BaseInterfaceElement;
		void Draw(MainWindow &wnd, Font &font, Graphics & gfx);
		void Update(MainWindow &wnd);
	};

	class LightButton{
	public:
		Light light;
		Interface *intf;
		Vec2 vec;
		Vec2 targetVec;
		int id;
		float moveStep = 0.3;
		LightButton(Vec2 vec_in, int id_in, int intensity = 500);
		void Update();

	};
	class LightButtonGroup {
	public:
		Interface *intf;
		int distance;
		Vec2 vec;
		LightButtonGroup(Vec2 vec_in, int distance_in);
		void Update(MainWindow &wnd);
		vector<LightButton> content;
		void operator +=(LightButton button_in);
		int Draw(int selected_in, class Player &player_in, Font &font, Graphics &gfx);
		void UpdateButtonStatus(class Player &player_in);
		int ActiveItem(Player &player_in);
	};

	operator bool();
	void operator +=(Label label_in);
	void operator +=(Button button_in);
	void operator +=(LightButtonGroup lbg_in);
	void Draw(MainWindow &wnd, Font &font, Graphics &gfx);
	void Update(MainWindow &wnd);
	InterfaceEvent Get();
	vector<LightButtonGroup> lbg;
private:
	vector<Button> buttons;
	vector<Label> labels;
	vector<InterfaceEvent> events;

};