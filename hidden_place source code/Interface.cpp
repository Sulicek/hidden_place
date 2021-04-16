#include "Interface.h"
#include "Player.h"

void Interface::BaseInterfaceElement::Draw(Font &font, Graphics & gfx)
{
	gfx.DrawRect(vec.x, vec.y, width, height, c);
	font.DrawNoBg(GetText(), vec + Vec2(border, border), gfx);
}

void Interface::Button::Draw(MainWindow & wnd, Font &font, Graphics & gfx)
{
	if (Vec2::IsColliding(wnd.mouse.GetVec(), vec, 1, 1, width, height)) {
		gfx.DrawRect(vec.x, vec.y, width, height, Colors::CastBarGold);
		gfx.DrawRect(vec.x + border, vec.y + border, width - border*2, height - border*2, c);
	}
	else {
		gfx.DrawRect(vec.x, vec.y, width, height, c);
	}
	font.DrawNoBg(GetText(), vec + Vec2(border, border), gfx);
}

void Interface::Button::Update(MainWindow & wnd)
{
	if (Vec2::IsColliding(wnd.mouse.GetVec(), vec, 1, 1, width, height) && 
		wnd.mouse.LeftIsPressed()) {
		intf->events.push_back({EventType::buttonDown, GetText()});
	}
}

void Interface::Draw(MainWindow &wnd, Font & font, Graphics & gfx)
{
	for (auto &button : buttons) {
		button.Draw(wnd, font, gfx);
	}
	for (auto &label : labels) {
		label.Draw(font, gfx);
	}
}

void Interface::Update(MainWindow & wnd)
{
	for (auto &button : buttons) {
		button.Update(wnd);
	}
}

Interface::InterfaceEvent Interface::Get()
{
	auto out = events[0];
	events.erase(events.begin());
	return out;
	
}

Interface::BaseInterfaceElement::BaseInterfaceElement(string name_in, Vec2 vec_in,
		int width_in, int height_in, Color c_in, int border_in):
	intf(intf)
{
	name = name_in;
	width = width_in;
	height = height_in;
	vec = Vec2(vec_in.x * float(Graphics::ScreenWidth) / 100.0, vec_in.y * float(Graphics::ScreenHeight) / 100) - Vec2(width / 2, height / 2);
	c = c_in;
	border = border_in;
}

Vec2 Interface::BaseInterfaceElement::GetCenterVec()
{
	return Vec2(vec.x + width/2, vec.y + height/2);
}

string Interface::BaseInterfaceElement::GetText()
{
	return "  " + name;
}

Interface::operator bool()
{
	return (events.size() > 0);
}

void Interface::operator+=(Label label_in)
{
	labels.push_back(label_in);
	labels[labels.size() - 1].intf = this;
}

void Interface::operator+=(Button button_in)
{
	buttons.push_back(button_in);
	buttons[buttons.size() - 1].intf = this;
}

void Interface::operator+=(LightButtonGroup lbg_in)
{
	lbg.push_back(lbg_in);
}

Interface::LightButtonGroup::LightButtonGroup(Vec2 vec_in, int distance_in)
{
	vec = vec_in;
	distance = distance_in;
}

void Interface::LightButtonGroup::operator+=(LightButton button_in)
{
	content.push_back(button_in);
	content[content.size() - 1].intf = intf;
}

int Interface::LightButtonGroup::Draw(int selected_in, Player & player_in, Font & font, Graphics & gfx)
{
	std::string debug = "";
	if (player_in.GetUpgradableStats().size() > 0) {
		std::vector<Vec2> points = Vec2::GeneratePoints(vec, player_in.GetUpgradableStats().size(), distance);
		int active = selected_in;
		int selectedStat = 0;
		for (int i = 0; i < player_in.GetUpgradableStats().size(); i++) {
			int usedPosition = i - active;
			int test1 = usedPosition;
			int test2 = player_in.GetUpgradableStats().size();
			bool test3 = test1 >= test2;
			debug += "\t" + to_string(test1) + " >= " + to_string(test2) + " -> " + to_string(test1 >= test2) + "(" + to_string(test3)  + ")"+ "\n";
			if (test3) {
				//debug += "result: " + to_string(usedPosition >= player_in.soulItems.size());
				debug += "fixing overflow\n";
				debug += "before: " + to_string(usedPosition) + "\n";
				usedPosition = (usedPosition % player_in.GetUpgradableStats().size());
				debug += "after: " + to_string(usedPosition) + "\n";
			}
			//debug += "post: " + to_string(usedPosition) + " - ";
			while (usedPosition < 0) {
				debug += "fixing underflow\n";
				debug += "before: " + to_string(usedPosition) + "\n";
				usedPosition += player_in.GetUpgradableStats().size();
				debug += "after: " + to_string(usedPosition) + "\n";
			}
			//debug += "i: " + to_string(i) + " position: " + to_string(usedPosition) + " active: " + to_string(active) + " \n";
			//debug += "\ttarget vec: " + to_string(content[i].targetVec.x) + " : " + to_string(content[i].targetVec.y) + "\n";
			if (usedPosition == 0) {
				selectedStat = i;
			}
			content[i].targetVec = points[usedPosition];
			content[i].Update();
			content[i].light.DrawControlled(content[i].vec, float(player_in.GetStats(player_in.GetUpgradableStats()[i])+1)
					/float(player_in.statMax),
				MinorItem::GetParamterColor(player_in.GetUpgradableStats()[i]), gfx);
			debug += "\n";
		}
		debug += "\n";
		int i = 0;
		for (auto point : content) {
			debug += to_string(i) + " - "+to_string(point.vec.x) + " : " + to_string(point.vec.y) + " \n";
			i++;
		}
		//font.Draw(debug, Vec2(100, 300), Colors::Magenta, Colors::White, gfx);
		return player_in.GetUpgradableStats()[selectedStat];
	}
}

void Interface::LightButtonGroup::UpdateButtonStatus(Player & player_in)
{
	content.erase(content.begin(), content.end());
	std::vector<Vec2> points = Vec2::GeneratePoints(vec, player_in.GetUpgradableStats().size(), distance);
	int active = 0;
	for(int i = 0; i < player_in.GetUpgradableStats().size(); i++){
		int usedPosition = i - active;
		if (usedPosition >= player_in.GetUpgradableStats().size()) {
			usedPosition = (usedPosition % player_in.GetUpgradableStats().size()) + 1;
		}
		while (usedPosition < 0) {
			usedPosition += player_in.GetUpgradableStats().size();
		}
		*this += LightButton(Vec2(Graphics::ScreenWidth/2, Graphics::ScreenHeight), i);
	}
}

int Interface::LightButtonGroup::ActiveItem(Player & player_in)
{
	return player_in.usedSoulItem;
}

Interface::LightButton::LightButton(Vec2 vec_in, int id_in,int intensity):
	light(intensity, -1, false, Colors::Magenta)
{
	id = id_in;
	vec = vec_in;
}

void Interface::LightButton::Update()
{
	vec += (targetVec - vec) * moveStep;
}
