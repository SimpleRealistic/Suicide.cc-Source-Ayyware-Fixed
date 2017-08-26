#include "Controls.h"
#include "RenderManager.h"
#include "Menu.h"



void CControl::SetPosition(int x, int y)
{
	m_x = x;
	m_y = y;
}

void CControl::SetSize(int w, int h)
{
	m_iWidth = w;
	m_iHeight = h;
}

void CControl::GetSize(int &w, int &h)
{
	w = m_iWidth;
	h = m_iHeight;
}

bool CControl::Flag(int f)
{
	if (m_Flags & f)
		return true;
	else
		return false;
}

POINT CControl::GetAbsolutePos()
{
	POINT p;
	RECT client = parent->GetClientArea();
	if (parent)
	{
		p.x = m_x + client.left;
		p.y = m_y + client.top + 29;
	}

	return p;
}

void CControl::SetFileId(std::string fid)
{
	FileIdentifier = fid;
}
#pragma endregion Implementations of the Base control functions

#pragma region CheckBox
CCheckBox::CCheckBox()
{
	Checked = false;

	m_Flags = UIFlags::UI_Clickable | UIFlags::UI_Drawable | UIFlags::UI_SaveFile;
	m_iWidth = 10;
	m_iHeight = 10;

	FileControlType = UIControlTypes::UIC_CheckBox;
}

void CCheckBox::SetState(bool s)
{
	Checked = s;
}

bool CCheckBox::GetState()
{
	return Checked;
}

void CCheckBox::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	Render::GradientV(a.x, a.y, 10, 10, Color(55, 55, 55, 255), Color(40, 40, 40, 245));
	Render::Outline(a.x, a.y, 10, 10, Color(2, 2, 2, 255));

	int red;
	int green;
	int blue;

	if (Checked)
	{
		if (Menu::Window.VisualsTab.ControlColor.GetIndex() == 0)
		{
			red = 144;
			green = 187;
			blue = 32;

			Render::GradientV(a.x, a.y, 10, 10, Color(red, green, blue, 255), Color(red - 15, green - 15, blue - 15, 245));
		}
		else if (Menu::Window.VisualsTab.ControlColor.GetIndex() == 1)
		{
			red = 70;
			green = 70;
			blue = 255;

			Render::GradientV(a.x, a.y, 10, 10, Color(red, green, blue, 255), Color(red - 15, green - 15, blue - 15, 245));
		}
		else if (Menu::Window.VisualsTab.ControlColor.GetIndex() == 2)
		{
			red = 255;
			green = 70;
			blue = 70;

			Render::GradientV(a.x, a.y, 10, 10, Color(red, green, blue, 255), Color(red - 15, green - 15, blue - 15, 245));
		}
		else if (Menu::Window.VisualsTab.ControlColor.GetIndex() == 3)
		{
			red = 231;
			green = 183;
			blue = 92;

			Render::GradientV(a.x, a.y, 10, 10, Color(red, green, blue, 255), Color(red - 15, green - 15, blue - 15, 245));
		}
		Render::Outline(a.x, a.y, 10, 10, Color(2, 2, 2, 255));
	}
}

void CCheckBox::OnUpdate() { m_iWidth = 10; m_iHeight = 10; }

void CCheckBox::OnClick()
{
	Checked = !Checked;
}
#pragma endregion Implementations of the Check Box functions

#pragma region Label
CLabel::CLabel()
{
	m_Flags = UIFlags::UI_Drawable;
	Text = "Default";
	FileIdentifier = "Default";
}

void CLabel::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	Render::Text(a.x, a.y - 1, Color(244, 244, 244, 255), Render::Fonts::Menu, Text.c_str());
}

void CLabel::SetText(std::string text)
{
	Text = text;
}

void CLabel::OnUpdate() {}
void CLabel::OnClick() {}
#pragma endregion Implementations of the Label functions

#pragma region GroupBox
CGroupBox::CGroupBox()
{
	Items = 1;
	iYAdd = 0;
	ItemSpacing = 20;
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_RenderFirst;
	Text = "Default";
	FileIdentifier = "Default";
}
CGroupBox1::CGroupBox1()
{
	Items = 1;
	iYAdd = 0;
	ItemSpacing = 20;
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_RenderFirst;
	Text = "Default";
	FileIdentifier = "Default";
}
void CGroupBox1::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	RECT txtSize = Render::GetTextSize(Render::Fonts::SUICIDE, Text.c_str());
	Render::Clear(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, Color(90, 90, 90, 1));
	Render::Text(a.x + (m_iWidth / 2) - (txtSize.right / 2), a.y - (txtSize.bottom / 2) - 1, Color(244, 244, 244, 255), Render::Fonts::SUICIDE, Text.c_str());

	//Color(48, 48, 48, 255)

	Render::Line(a.x, a.y, a.x + (m_iWidth / 2) - (txtSize.right / 2) - 2, a.y, Color(48, 48, 48, 0));
	Render::Line(a.x + (m_iWidth / 2) + (txtSize.right / 2) + 2, a.y, a.x + m_iWidth, a.y, Color(48, 48, 48, 0));
	Render::Line(a.x, a.y, a.x, a.y + m_iHeight, Color(48, 48, 48, 0));
	Render::Line(a.x, a.y + m_iHeight, a.x + m_iWidth, a.y + m_iHeight, Color(48, 48, 48, 0));
	Render::Line(a.x + m_iWidth, a.y, a.x + m_iWidth, a.y + m_iHeight, Color(48, 48, 48, 0));
}

void CGroupBox1::SetText(std::string text)
{
	Text = text;
}
void CGroupBox::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	RECT txtSize = Render::GetTextSize(Render::Fonts::Menu, Text.c_str());
	Render::Clear(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, Color(90, 90, 90, 1));
	Render::Text(a.x + (m_iWidth / 2) - (txtSize.right / 2), a.y - (txtSize.bottom / 2) - 1, Color(244, 244, 244, 255), Render::Fonts::Menu, Text.c_str());

	//Color(48, 48, 48, 255)

	Render::Line(a.x, a.y, a.x + (m_iWidth / 2) - (txtSize.right / 2) - 2, a.y, Color(48, 48, 48, 255));
	Render::Line(a.x + (m_iWidth / 2) + (txtSize.right / 2) + 2, a.y, a.x + m_iWidth, a.y, Color(48, 48, 48, 255));
	Render::Line(a.x, a.y, a.x, a.y + m_iHeight, Color(48, 48, 48, 255));
	Render::Line(a.x, a.y + m_iHeight, a.x + m_iWidth, a.y + m_iHeight, Color(48, 48, 48, 255));
	Render::Line(a.x + m_iWidth, a.y, a.x + m_iWidth, a.y + m_iHeight, Color(48, 48, 48, 255));
}

void CGroupBox::SetText(std::string text)
{
	Text = text;
}

void CGroupBox::SetSpacing(int Spacing)
{
	ItemSpacing = Spacing;
}

void CGroupBox::PlaceCheckBox(std::string Label, CTab *Tab, CControl* control)
{
	int x = m_x + 29;
	int y = m_y + Items * 24;

	int cw, ch;
	control->SetPosition(x, y);
	control->GetSize(cw, ch);
	control->SetSize((m_iWidth / 2) - 32, ch);
	Tab->RegisterControl(control);

	x += 20;

	CLabel* label = new CLabel;
	label->SetPosition(x, y);
	label->SetText(Label);
	Tab->RegisterControl(label);
	Items++;
}

void CGroupBox::PlaceOtherControl(std::string Label, CTab *Tab, CControl* control)
{
	int x = m_x + 29;
	int y = m_y + Items * 24;

	int cw, ch;
	control->SetPosition(x, y);
	control->GetSize(cw, ch);
	control->SetSize((m_iWidth / 2) - 32, ch);
	Tab->RegisterControl(control);

	x += m_iWidth / 2 - 25;

	CLabel* label = new CLabel;
	label->SetPosition(x, y);
	label->SetText(Label);
	Tab->RegisterControl(label);
	Items++;
}

void CGroupBox::PlaceLabledControl(std::string Label, CTab *Tab, CControl* control)
{
	if (control->FileControlType == UIControlTypes::UIC_CheckBox)
	{
		int x = m_x + 34;
		int y = m_y + Items * ItemSpacing + iYAdd;

		CLabel* label = new CLabel;
		label->SetPosition(x, y);
		label->SetText(Label);
		Tab->RegisterControl(label);

		x = m_x + 16;

		int cw, ch;
		control->SetPosition(x, y);
		control->GetSize(cw, ch);
		control->SetSize((m_iWidth / 2) - 32, ch);
		Tab->RegisterControl(control);
	}
	else if (control->FileControlType == UIControlTypes::UIC_ComboBox)
	{
		int x = m_x + 34;
		int y = m_y + Items * ItemSpacing + iYAdd;

		x += m_iWidth / 2 - 13;

		RECT txtSize = Render::GetTextSize(Render::Fonts::Menu, Label.c_str());
		y = y + 10 - (txtSize.bottom / 2);

		CLabel* label = new CLabel;
		label->SetPosition(x, y);
		label->SetText(Label);
		Tab->RegisterControl(label);

		x = m_x + 34;
		y = m_y + Items * ItemSpacing + iYAdd;

		int cw, ch;
		control->SetPosition(x, y);
		control->GetSize(cw, ch);
		control->SetSize((m_iWidth / 2) - 25, ch);
		Tab->RegisterControl(control);

		iYAdd += 6;
	}
	else if (control->FileControlType == UIControlTypes::UIC_Slider)
	{
		int x = m_x + 34;
		int y = m_y + Items * ItemSpacing + iYAdd - 1;

		x += m_iWidth / 2 - 13;

		CLabel* label = new CLabel;
		label->SetPosition(x, y);
		label->SetText(Label);
		Tab->RegisterControl(label);

		x = m_x + 34;
		y += 1;

		int cw, ch;
		control->SetPosition(x, y);
		control->GetSize(cw, ch);
		control->SetSize((m_iWidth / 2) - 25, ch);
		Tab->RegisterControl(control);
	}
	else if (control->FileControlType == UIControlTypes::UIC_Button)
	{
		int x = m_x + 34;
		int y = m_y + Items * ItemSpacing + iYAdd - 2;

		x += m_iWidth / 2;

		CLabel* label = new CLabel;
		label->SetPosition(x, y);
		label->SetText(Label);
		Tab->RegisterControl(label);

		x = m_x + 34;
		y += 2;

		int cw, ch;
		control->SetPosition(x, y);
		control->GetSize(cw, ch);
		control->SetSize(m_iWidth, 24);
		Tab->RegisterControl(control);
	}
	else
	{
		int x = m_x + 34;
		int y = m_y + Items * ItemSpacing + iYAdd;

		x += m_iWidth / 2 - 13;

		CLabel* label = new CLabel;
		label->SetPosition(x, y);
		label->SetText(Label);
		Tab->RegisterControl(label);

		x = m_x + 34;

		int cw, ch;
		control->SetPosition(x, y);
		control->GetSize(cw, ch);
		control->SetSize((m_iWidth / 2) - 25, ch);
		Tab->RegisterControl(control);
	}
	Items++;
}

void CGroupBox::OnUpdate() {}
void CGroupBox::OnClick() {}
void CGroupBox1::OnUpdate() {}
void CGroupBox1::OnClick() {}
#pragma endregion Implementations of the Group Box functions

#pragma region Sliders
CSlider::CSlider()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	format = FORMAT_INT;
	FileControlType = UIControlTypes::UIC_Slider;
}

void CSlider::Draw(bool hover)
{
	int red;
	int green;
	int blue;

	POINT a = GetAbsolutePos();

	Render::GradientV(a.x, a.y + 1, m_iWidth, 7, Color(55, 55, 55, 255), Color(40, 40, 40, 245));

	float Ratio = Value / (Max - Min);
	float Location = Ratio*m_iWidth;

	if (Menu::Window.VisualsTab.ControlColor.GetIndex() == 0)
	{
		red = 144;
		green = 187;
		blue = 32;

		Render::GradientV(a.x, a.y + 1, Location, 7, Color(red, green, blue, 255), Color(red - 20, green - 20, blue - 20, 245));
	}
	else if (Menu::Window.VisualsTab.ControlColor.GetIndex() == 1)
	{
		red = 70;
		green = 70;
		blue = 255;

		Render::GradientV(a.x, a.y + 1, Location, 7, Color(red, green, blue, 255), Color(red - 20, green - 20, blue - 20, 245));
	}
	else if (Menu::Window.VisualsTab.ControlColor.GetIndex() == 2)
	{
		red = 255;
		green = 70;
		blue = 70;

		Render::GradientV(a.x, a.y + 1, Location, 7, Color(red, green, blue, 255), Color(red - 20, green - 20, blue - 20, 245));
	}
	else if (Menu::Window.VisualsTab.ControlColor.GetIndex() == 3)
	{
		red = 231;
		green = 183;
		blue = 92;

		Render::GradientV(a.x, a.y + 1, Location, 7, Color(red, green, blue, 255), Color(red - 20, green - 20, blue - 20, 245));
	}

	Render::Outline(a.x - 1, a.y, m_iWidth + 2, 9, Color(2, 2, 2, 255));

	char buffer[24];
	if (format == FORMAT_DECDIG2)
		sprintf_s(buffer, "%.2f", Value);
	else if (format == FORMAT_DECDIG1)
		sprintf_s(buffer, "%.1f", Value);
	else if (format == FORMAT_INT)
		sprintf_s(buffer, "%1.0f", Value);

	RECT txtSize = Render::GetTextSize(Render::Fonts::Slider, buffer);
	if (Location == 0)
		Render::Text(a.x + (Location)-txtSize.right / 2 - 2 + 3, a.y + 1, Color(244, 244, 244, 255), Render::Fonts::Slider, buffer);
	else
		Render::Text(a.x + (Location)-txtSize.right / 2 - 2, a.y + 1, Color(244, 244, 244, 255), Render::Fonts::Slider, buffer);
}

void CSlider::OnUpdate() {
	POINT a = GetAbsolutePos();
	m_iHeight = 11;

	if (DoDrag)
	{
		if (GUI.GetKeyState(VK_LBUTTON))
		{
			POINT m = GUI.GetMouse();
			float NewX;
			float Ratio;
			NewX = m.x - a.x - 1;
			if (NewX < 0) NewX = 0;
			if (NewX > m_iWidth) NewX = m_iWidth;
			Ratio = NewX / float(m_iWidth);
			Value = Min + (Max - Min)*Ratio;
		}
		else
		{
			DoDrag = false;
		}
	}
}

void CSlider::OnClick() {
	POINT a = GetAbsolutePos();
	RECT SliderRegion = { a.x, a.y, m_iWidth, 11 };
	if (GUI.IsMouseInRegion(SliderRegion))
	{
		DoDrag = true;
	}
}

float CSlider::GetValue()
{
	return Value;
}

void CSlider::SetValue(float v)
{
	Value = v;
}

void CSlider::SetBoundaries(float min, float max)
{
	Min = min; Max = max;
}

void CSlider::SetFormat(SliderFormat type)
{
	format = type;
}


CSliderFloat::CSliderFloat()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	format = FORMAT_INT;
	FileControlType = UIControlTypes::UIC_Slider;
}

void CSliderFloat::Draw(bool hover)
{
	int red;
	int green;
	int blue;

	POINT a = GetAbsolutePos();

	Render::GradientV(a.x, a.y + 1, m_iWidth, 7, Color(55, 55, 55, 255), Color(40, 40, 40, 245));

	float Ratio = Value / (Max - Min);
	float Location = Ratio*m_iWidth;

	if (Menu::Window.VisualsTab.ControlColor.GetIndex() == 0)
	{
		red = 144;
		green = 187;
		blue = 32;

		Render::GradientV(a.x, a.y + 1, Location, 7, Color(red, green, blue, 255), Color(red - 20, green - 20, blue - 20, 245));
	}
	else if (Menu::Window.VisualsTab.ControlColor.GetIndex() == 1)
	{
		red = 70;
		green = 70;
		blue = 255;

		Render::GradientV(a.x, a.y + 1, Location, 7, Color(red, green, blue, 255), Color(red - 20, green - 20, blue - 20, 245));
	}
	else if (Menu::Window.VisualsTab.ControlColor.GetIndex() == 2)
	{
		red = 255;
		green = 70;
		blue = 70;

		Render::GradientV(a.x, a.y + 1, Location, 7, Color(red, green, blue, 255), Color(red - 20, green - 20, blue - 20, 245));
	}
	else if (Menu::Window.VisualsTab.ControlColor.GetIndex() == 3)
	{
		red = 231;
		green = 183;
		blue = 92;

		Render::GradientV(a.x, a.y + 1, Location, 7, Color(red, green, blue, 255), Color(red - 20, green - 20, blue - 20, 245));
	}

	Render::Outline(a.x - 1, a.y, m_iWidth + 2, 9, Color(2, 2, 2, 255));

	char buffer[24];
	if (format == FORMAT_DECDIG2)
		sprintf_s(buffer, "%.2f", Value);
	else if (format == FORMAT_DECDIG1)
		sprintf_s(buffer, "%.1f", Value);
	else if (format == FORMAT_INT)
		sprintf_s(buffer, "%.2f", Value);

	RECT txtSize = Render::GetTextSize(Render::Fonts::Slider, buffer);
	if (Location == 0)
		Render::Text(a.x + (Location)-txtSize.right / 2 - 2 + 3, a.y + 1, Color(244, 244, 244, 255), Render::Fonts::Slider, buffer);
	else
		Render::Text(a.x + (Location)-txtSize.right / 2 - 2, a.y + 1, Color(244, 244, 244, 255), Render::Fonts::Slider, buffer);
}

void CSliderFloat::OnUpdate() {
	POINT a = GetAbsolutePos();
	m_iHeight = 11;

	if (DoDrag)
	{
		if (GUI.GetKeyState(VK_LBUTTON))
		{
			POINT m = GUI.GetMouse();
			float NewX;
			float Ratio;
			NewX = m.x - a.x - 1;
			if (NewX < 0) NewX = 0;
			if (NewX > m_iWidth) NewX = m_iWidth;
			Ratio = NewX / float(m_iWidth);
			Value = Min + (Max - Min)*Ratio;
		}
		else
		{
			DoDrag = false;
		}
	}
}

void CSliderFloat::OnClick() {
	POINT a = GetAbsolutePos();
	RECT SliderRegion = { a.x, a.y, m_iWidth, 11 };
	if (GUI.IsMouseInRegion(SliderRegion))
	{
		DoDrag = true;
	}
}

float CSliderFloat::GetValue()
{
	return Value;
}

void CSliderFloat::SetValue(float v)
{
	Value = v;
}

void CSliderFloat::SetBoundaries(float min, float max)
{
	Min = min; Max = max;
}

void CSliderFloat::SetFormat(SliderFormat type)
{
	format = type;
}
#pragma endregion Implementations of the Slider functions

#pragma region ItemSelector
CItemSelector::CItemSelector()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	FileControlType = UIControlTypes::UIC_ItemSelector;
}

void CItemSelector::Draw(bool hover)
{
	POINT a = GetAbsolutePos();

	Render::Text(a.x, a.y, Color(244, 244, 244, 255), Render::Fonts::Menu, "-");
}

void CItemSelector::OnUpdate()
{
}

void CItemSelector::OnClick()
{
	POINT a = GetAbsolutePos();

	if (GUI.IsMouseInRegion(MinusRegion))
	{
		updateType = UPDATE_LEFT;
	}
}
#pragma endregion Implementation of sliders

#pragma region KeyBinders

char* KeyStrings[254] = { nullptr, "Left Mouse", "Right Mouse", "Control+Break", "Middle Mouse", "Mouse 4", "Mouse 5",
nullptr, "Backspace", "TAB", nullptr, nullptr, nullptr, "ENTER", nullptr, nullptr, "SHIFT", "CTRL", "ALT", "PAUSE",
"CAPS LOCK", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "ESC", nullptr, nullptr, nullptr, nullptr, "SPACEBAR",
"PG UP", "PG DOWN", "END", "HOME", "Left", "Up", "Right", "Down", nullptr, "Print", nullptr, "Print Screen", "Insert",
"Delete", nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
"Y", "Z", "Left Windows", "Right Windows", nullptr, nullptr, nullptr, "NUM 0", "NUM 1", "NUM 2", "NUM 3", "NUM 4", "NUM 5", "NUM 6",
"NUM 7", "NUM 8", "NUM 9", "*", "+", "_", "-", ".", "/", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
"F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, "NUM LOCK", "SCROLL LOCK", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, "LSHIFT", "RSHIFT", "LCONTROL", "RCONTROL", "LMENU", "RMENU", nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "Next Track", "Previous Track", "Stop", "Play/Pause", nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "/?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "[{", "\\|", "}]", "'\"", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

CKeyBind::CKeyBind()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	FileControlType = UIControlTypes::UIC_KeyBind;
}

void CKeyBind::Draw(bool hover)
{
	POINT a = GetAbsolutePos();

	Render::Outline(a.x, a.y, m_iWidth, m_iHeight, Color(2, 2, 2, 255));
	Render::GradientV(a.x + 2, a.y + 2, m_iWidth - 4, 12, Color(55, 55, 55, 255), Color(40, 40, 40, 245));
	if (hover)
		Render::GradientV(a.x + 2, a.y + 2, m_iWidth - 4, 12, Color(63, 63, 63, 255), Color(46, 46, 46, 245));
	bool GoodKeyName = false;
	char NameBuffer[128];
	char* KeyName = "Not Bound";

	if (IsGettingKey)
	{
		KeyName = "<Press A Key>";
	}
	else
	{
		if (Key >= 0)
		{
			KeyName = KeyStrings[Key];
			if (KeyName)
			{
				GoodKeyName = true;
			}
			else
			{
				if (GetKeyNameText(Key << 16, NameBuffer, 127))
				{
					KeyName = NameBuffer;
					GoodKeyName = true;
				}
			}
		}

		if (!GoodKeyName)
		{
			KeyName = "No Key Bound";
		}
	}


	Render::Text(a.x + 4, a.y + 2 - 1, Color(244, 244, 244, 255), Render::Fonts::Menu, KeyName);
}

void CKeyBind::OnUpdate() {
	m_iHeight = 16;
	POINT a = GetAbsolutePos();
	if (IsGettingKey)
	{
		for (int i = 0; i < 255; i++)
		{
			if (GUI.GetKeyPress(i))
			{
				if (i == VK_ESCAPE)
				{
					IsGettingKey = false;
					Key = -1;
					return;
				}

				Key = i;
				IsGettingKey = false;
				return;
			}
		}
	}
}

void CKeyBind::OnClick() {
	POINT a = GetAbsolutePos();
	if (!IsGettingKey)
	{
		IsGettingKey = true;
	}
}

int CKeyBind::GetKey()
{
	return Key;
}

void CKeyBind::SetKey(int key)
{
	Key = key;
}

#pragma endregion Implementations of the KeyBind Control functions

#pragma region Button
CButton::CButton()
{
	m_iWidth = 177;
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable;
	FileControlType == UIControlTypes::UIC_Button;
	Text = "Default";
	CallBack = nullptr;
	FileIdentifier = "Default";
}

void CButton::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	Render::Outline(a.x, a.y, m_iWidth, m_iHeight, Color(2, 2, 2, 255));
	if (hover)
		Render::GradientV(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, Color(40, 40, 40, 255), Color(40, 40, 40, 255));
	else
		Render::GradientV(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, Color(35, 35, 35, 255), Color(35, 35, 35, 255));

	RECT TextSize = Render::GetTextSize(Render::Fonts::MenuBold, Text.c_str());
	int TextX = a.x + (m_iWidth / 2) - (TextSize.left / 2);
	int TextY = a.y + (m_iHeight / 2) - (TextSize.bottom / 2);

	Render::Text(TextX, TextY - 1, Color(200, 200, 200, 255), Render::Fonts::MenuBold, Text.c_str());
}

void CButton::SetText(std::string text)
{
	Text = text;
}

void CButton::SetCallback(CButton::ButtonCallback_t callback)
{
	CallBack = callback;
}

void CButton::OnUpdate()
{
	m_iHeight = 26;
}

void CButton::OnClick()
{
	if (CallBack)
		CallBack();
}
#pragma endregion Implementations of the Button functions

#pragma region ComboBox
CComboBox::CComboBox()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_Focusable | UIFlags::UI_SaveFile;
	FileControlType = UIControlTypes::UIC_ComboBox;
	winWidth = 400;
	winHeight = 400;
}

void CComboBox::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	RECT Region = { a.x, a.y, m_iWidth, 20 };
	Render::Outline(a.x, a.y, m_iWidth, 20, Color(2, 2, 2, 255));
	Render::GradientV(a.x + 2, a.y + 2, m_iWidth - 4, 16, Color(35, 35, 35, 255), Color(35, 35, 35, 255));

	// Hover for the Top Box
	if (GUI.IsMouseInRegion(Region))
		Render::GradientV(a.x + 2, a.y + 2, m_iWidth - 4, 16, Color(40, 40, 40, 255), Color(40, 40, 40, 255));

	// If we have some items
	if (Items.size() > 0)
	{
		// The current item
		RECT txtSize = Render::GetTextSize(Render::Fonts::Menu, GetItem().c_str());
		Render::Text(a.x + 4, a.y + (Region.bottom / 2) - (txtSize.bottom / 2), Color(244, 244, 244, 255), Render::Fonts::Menu, GetItem().c_str());

		// If the drop down part is open
		if (IsOpen)
		{
			if (Menu::Window.isOpen())
			{
				/*Render::Clear(
				(Menu::Window.get_x() + 8 - 200 + Menu::Window.get_iWidth() + 8 - 200) / 2 - (winWidth / 2),
				(Menu::Window.get_y() + 1 + 28 * 2) + (Menu::Window.get_iHeight() + 1 + 28) * 2 / 2 - (winHeight / 2),
				winWidth,
				winHeight,
				Color(45,45,45,255));*/
			}

			Render::GradientV(a.x, a.y + 20, m_iWidth, Items.size() * 16, Color(35, 35, 35, 255), Color(35, 35, 35, 255));

			// Draw the items
			for (int i = 0; i < Items.size(); i++)
			{
				RECT ItemRegion = { a.x, a.y + 17 + i * 16, m_iWidth, 16 };

				// Hover
				if (GUI.IsMouseInRegion(ItemRegion))
				{
					Render::GradientV(a.x + 2, a.y + 20 + i * 16, m_iWidth - 4, 16, Color(40, 40, 40, 255), Color(40, 40, 40, 255));
				}

				Render::Text(a.x + 4, a.y + 19 + i * 16 - 1 + 4, Color(244, 244, 244, 255), Render::Fonts::Menu, Items[i].c_str());
			}
			Render::Outline(a.x, a.y + 20, m_iWidth, Items.size() * 16, Color(2, 2, 2, 255));
		}
	}
	Vertex_t Verts2[3];
	Verts2[0].m_Position.x = a.x + m_iWidth - 10;
	Verts2[0].m_Position.y = a.y + 8;
	Verts2[1].m_Position.x = a.x + m_iWidth - 5;
	Verts2[1].m_Position.y = a.y + 8;
	Verts2[2].m_Position.x = a.x + m_iWidth - 7.5;
	Verts2[2].m_Position.y = a.y + 11;

	Render::Polygon(3, Verts2, Color(2, 2, 2, 255));
}

void CComboBox::AddItem(std::string text)
{
	Items.push_back(text);
	SelectedIndex = 0;
}

void CComboBox::OnUpdate()
{
	if (IsOpen)
	{
		m_iHeight = 16 + 16 * Items.size();

		if (parent->GetFocus() != this)
			IsOpen = false;
	}
	else
	{
		m_iHeight = 16;
	}

}

void CComboBox::OnClick()
{
	POINT a = GetAbsolutePos();
	RECT Region = { a.x, a.y, m_iWidth, 16 };

	if (IsOpen)
	{
		// If we clicked one of the items(Not in the top bar)
		if (!GUI.IsMouseInRegion(Region))
		{
			// Draw the items
			for (int i = 0; i < Items.size(); i++)
			{
				RECT ItemRegion = { a.x, a.y + 16 + i * 16, m_iWidth, 16 };

				// Hover
				if (GUI.IsMouseInRegion(ItemRegion))
				{
					SelectedIndex = i;
				}
			}
		}

		// Close the drop down
		IsOpen = false;
	}
	else
	{
		IsOpen = true;
	}

}

int CComboBox::GetIndex()
{
	return SelectedIndex;
}

void CComboBox::SetIndex(int index)
{
	SelectedIndex = index;
}

std::string CComboBox::GetItem()
{
	if (SelectedIndex >= 0 && SelectedIndex < Items.size())
	{
		return Items[SelectedIndex];
	}

	return "Error";
}

void CComboBox::SelectIndex(int idx)
{
	if (idx >= 0 && idx < Items.size())
	{
		SelectedIndex = idx;
	}
}

#pragma endregion Implementations of the ComboBox functions

char* KeyDigits[254] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
"y", "z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "/?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "[", "\\", "]", "'", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

char* KeyDigitss[254] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
"Y", "Z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "{", "|", "}", "\"", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

CTextField::CTextField()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	FileControlType = UIControlTypes::UIC_KeyBind;
}

std::string CTextField::getText()
{
	return text;
}

void CTextField::SetText(std::string stext)
{
	text = stext;
}

void CTextField::Draw(bool hover)
{
	POINT a = GetAbsolutePos();

	Render::Outline(a.x, a.y, m_iWidth, 16, Color(2, 2, 2, 255));
	Render::GradientV(a.x + 2, a.y + 2, m_iWidth - 4, 12, Color(55, 55, 55, 255), Color(40, 40, 40, 245));
	if (hover || IsGettingKey)
		Render::GradientV(a.x + 2, a.y + 2, m_iWidth - 4, 12, Color(63, 63, 63, 255), Color(46, 46, 46, 245));

	const char *cstr = text.c_str();

	Render::Text(a.x + 2, a.y + 2 - 1, Color(244, 244, 244, 255), Render::Fonts::Menu, cstr);
}

void CTextField::OnUpdate()
{
	m_iHeight = 16;
	POINT a = GetAbsolutePos();
	POINT b;
	const char *strg = text.c_str();

	if (IsGettingKey)
	{
		b = GetAbsolutePos();
		for (int i = 0; i < 255; i++)
		{

			if (GUI.GetKeyPress(i))
			{
				if (i == VK_ESCAPE || i == VK_RETURN || i == VK_INSERT)
				{
					IsGettingKey = false;
					return;
				}

				if (i == VK_BACK && strlen(strg) != 0)
				{
					text = text.substr(0, strlen(strg) - 1);
				}

				if (strlen(strg) < 20 && i != NULL && KeyDigitss[i] != nullptr)
				{
					if (GetAsyncKeyState(VK_SHIFT))
					{
						text = text + KeyDigitss[i];
					}
					else
					{
						text = text + KeyDigits[i];
					}
					return;
				}

				if (strlen(strg) < 20 && i == 32)
				{
					text = text + " ";
					return;
				}
			}
		}
	}
}

void CTextField::OnClick()
{
	POINT a = GetAbsolutePos();
	if (!IsGettingKey)
	{
		IsGettingKey = true;
	}
}