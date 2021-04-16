#include "Inventory.h"
#include <math.h>
#include "Colors.h"
#include "Font.h"
#include "MainWindow.h"
#include "Player.h"
#include "Effect.h"

using namespace std;

Inventory::Inventory() {

}

void Inventory::Draw(Vec2 vec, int scale, int spacing, Font &font, Graphics &gfx) {
	Vec2 drawVec = vec + Vec2(spacing, spacing);
	int i = 0;
	bool isDebug = true;
	font.Draw("Inventory:", drawVec, Colors::White, Colors::Magenta, scale, gfx);
	drawVec.y += (font.getGlyphHeight()*scale + spacing);
	for (auto &item : items) {
		string name = item.second.GetName();
		string details = "";
		int detailsRows = 0;
		for (auto &update : item.second.updates) {
			detailsRows++;
			if (detailsRows > 1) {
				details += "\n";
			}
			string data = to_string(update.second);
			data = data.substr(0, data.find('.') + 2);
			details += "make " + Spell::GetParameterDesc(update.first) + " " + data;
		}
		for (auto &mod : item.second.modulations) {
			detailsRows++;
			if (detailsRows > 1) {
				details += "\n";
			}
			string data = to_string(mod.second);
			data = data.substr(0, data.find('.') + 2);
			details += "modify " + Spell::GetParameterDesc(mod.first) + " by " + data;
		}
		if (item.second.GetDesc() != "") {
			details = item.second.GetDesc();
		}
		font.Draw(name, drawVec, Colors::White, Colors::Magenta, scale/2, gfx);
		drawVec.y += (font.getGlyphHeight()*scale/2 + spacing);
		font.Draw("-" + details, drawVec, Colors::White, Colors::Magenta, scale/2, gfx);
		drawVec.y += (font.getGlyphHeight()*(scale/2) * detailsRows + spacing);
		i++;
	}
}

float Inventory::GetData(float output_in,  SpellParameters spellP_in)
{
	float output = output_in;
	float modulation = 1;
	for (auto &item : items) {
		if (item.second.GetUpdate(spellP_in) != 0) {
			output = item.second.GetUpdate(spellP_in);
		}
		modulation *= item.second.GetModulation(spellP_in);
	}
	return output * modulation;
}

void Inventory::AddItem(MinorItem item_in)
{
	items.emplace(std::make_pair(items.size(), item_in));
}
