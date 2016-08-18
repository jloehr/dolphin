// Copyright 2011 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include <cstddef>
#include <wx/bitmap.h>
#include <wx/checkbox.h>
#include <wx/dcmemory.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/statbmp.h>
#include <wx/textctrl.h>

#include "Common/Assert.h"
#include "Common/CommonTypes.h"
#include "Common/FileUtil.h"
#include "Common/IniFile.h"
#include "Common/Logging/Log.h"
#include "Core/Core.h"
#include "Core/Movie.h"
#include "DolphinWX/TASInputDlg.h"
#include "InputCommon/GCPadStatus.h"

wxDEFINE_EVENT(INVALIDATE_BUTTON_EVENT, wxCommandEvent);
wxDEFINE_EVENT(INVALIDATE_CONTROL_EVENT, wxCommandEvent);
wxDEFINE_EVENT(INVALIDATE_EXTENSION_EVENT, wxThreadEvent);

TASInputDlg::TASInputDlg(wxWindow* parent, wxWindowID id, const wxString& title,
                         const wxPoint& position, const wxSize& size, long style)
    : wxDialog(parent, id, title, position, size, style)
{
}

void TASInputDlg::CreateBaseLayout()
{
  for (unsigned int i = 0; i < ArraySize(m_controls); ++i)
    m_controls[i] = nullptr;
  for (unsigned int i = 0; i < ArraySize(m_buttons); ++i)
    m_buttons[i] = nullptr;
  for (unsigned int i = 0; i < ArraySize(m_cc_controls); ++i)
    m_cc_controls[i] = nullptr;

  m_buttons[0] = &m_dpad_down;
  m_buttons[1] = &m_dpad_up;
  m_buttons[2] = &m_dpad_left;
  m_buttons[3] = &m_dpad_right;
  m_buttons[4] = &m_a;
  m_buttons[5] = &m_b;
  m_controls[0] = &m_main_stick.x_cont;
  m_controls[1] = &m_main_stick.y_cont;

  m_a = CreateButton("A");
  m_a.checkbox->SetClientData(&m_a);
  m_b = CreateButton("B");
  m_b.checkbox->SetClientData(&m_b);
  m_dpad_up = CreateButton("Up");
  m_dpad_up.checkbox->SetClientData(&m_dpad_up);
  m_dpad_right = CreateButton("Right");
  m_dpad_right.checkbox->SetClientData(&m_dpad_right);
  m_dpad_down = CreateButton("Down");
  m_dpad_down.checkbox->SetClientData(&m_dpad_down);
  m_dpad_left = CreateButton("Left");
  m_dpad_left.checkbox->SetClientData(&m_dpad_left);

  m_buttons_dpad = new wxGridSizer(3);
  m_buttons_dpad->AddSpacer(20);
  m_buttons_dpad->Add(m_dpad_up.checkbox);
  m_buttons_dpad->AddSpacer(20);
  m_buttons_dpad->Add(m_dpad_left.checkbox);
  m_buttons_dpad->AddSpacer(20);
  m_buttons_dpad->Add(m_dpad_right.checkbox);
  m_buttons_dpad->AddSpacer(20);
  m_buttons_dpad->Add(m_dpad_down.checkbox);
  m_buttons_dpad->AddSpacer(20);
}

const int TASInputDlg::m_gc_pad_buttons_bitmask[12] = {
    PAD_BUTTON_DOWN, PAD_BUTTON_UP, PAD_BUTTON_LEFT, PAD_BUTTON_RIGHT,
    PAD_BUTTON_A,    PAD_BUTTON_B,  PAD_BUTTON_X,    PAD_BUTTON_Y,
    PAD_TRIGGER_Z,   PAD_TRIGGER_L, PAD_TRIGGER_R,   PAD_BUTTON_START};

const std::string TASInputDlg::m_cc_button_names[] = {
    "Down", "Up", "Left", "Right", "A", "B", "X", "Y", "+", "-", "L", "R", "ZR", "ZL", "Home"};

void TASInputDlg::CreateWiiLayout(int num)
{
  if (m_has_layout)
    return;

  CreateBaseLayout();

  m_buttons[6] = &m_one;
  m_buttons[7] = &m_two;
  m_buttons[8] = &m_plus;
  m_buttons[9] = &m_minus;
  m_buttons[10] = &m_home;

  m_controls[4] = &m_x_cont;
  m_controls[5] = &m_y_cont;
  m_controls[6] = &m_z_cont;

  m_main_stick = CreateStick(ID_MAIN_STICK, 1024, 768, 512, 384, true, false);
  m_main_stick_szr = CreateStickLayout(&m_main_stick, _("IR"));

  m_x_cont = CreateControl(wxSL_VERTICAL, -1, 100, false, 1023, 512);
  m_y_cont = CreateControl(wxSL_VERTICAL, -1, 100, false, 1023, 512);
  m_z_cont = CreateControl(wxSL_VERTICAL, -1, 100, false, 1023, 616);
  wxStaticBoxSizer* const axisBox =
      CreateAccelLayout(&m_x_cont, &m_y_cont, &m_z_cont, _("Orientation"));

  wxStaticBoxSizer* const m_buttons_box = new wxStaticBoxSizer(wxVERTICAL, this, _("Buttons"));
  wxGridSizer* const m_buttons_grid = new wxGridSizer(4);

  m_plus = CreateButton("+");
  m_plus.checkbox->SetClientData(&m_plus);
  m_minus = CreateButton("-");
  m_minus.checkbox->SetClientData(&m_minus);
  m_one = CreateButton("1");
  m_one.checkbox->SetClientData(&m_one);
  m_two = CreateButton("2");
  m_two.checkbox->SetClientData(&m_two);
  m_home = CreateButton("Home");
  m_home.checkbox->SetClientData(&m_home);

  m_main_szr = new wxBoxSizer(wxVERTICAL);
  m_wiimote_szr = new wxBoxSizer(wxHORIZONTAL);
  m_ext_szr = new wxBoxSizer(wxHORIZONTAL);
  m_cc_szr = CreateCCLayout();

  if (!Core::IsRunning())
  {
    IniFile ini;
    ini.Load(File::GetUserPath(D_CONFIG_IDX) + "WiimoteNew.ini");
    std::string extension;
    ini.GetIfExists("Wiimote" + std::to_string(num + 1), "Extension", &extension);

    if (extension == "Nunchuk")
      m_ext = 1;
    if (extension == "Classic")
      m_ext = 2;
  }

  m_buttons[11] = &m_c;
  m_buttons[12] = &m_z;
  m_controls[2] = &m_c_stick.x_cont;
  m_controls[3] = &m_c_stick.y_cont;
  m_controls[7] = &m_nx_cont;
  m_controls[8] = &m_ny_cont;
  m_controls[9] = &m_nz_cont;

  m_c_stick = CreateStick(ID_C_STICK, 255, 255, 128, 128, false, true);
  m_c_stick_szr = CreateStickLayout(&m_c_stick, _("Nunchuk stick"));

  m_nx_cont = CreateControl(wxSL_VERTICAL, -1, 100, false, 1023, 512);
  m_ny_cont = CreateControl(wxSL_VERTICAL, -1, 100, false, 1023, 512);
  m_nz_cont = CreateControl(wxSL_VERTICAL, -1, 100, false, 1023, 512);
  wxStaticBoxSizer* const nunchukaxisBox =
      CreateAccelLayout(&m_nx_cont, &m_ny_cont, &m_nz_cont, _("Nunchuk orientation"));

  m_c = CreateButton("C");
  m_c.checkbox->SetClientData(&m_c);
  m_z = CreateButton("Z");
  m_z.checkbox->SetClientData(&m_z);
  m_ext_szr->Add(m_c_stick_szr, 0, wxLEFT | wxBOTTOM | wxRIGHT, 5);
  m_ext_szr->Add(nunchukaxisBox);

  for (Control* const control : m_controls)
  {
    if (control != nullptr)
      control->slider->Bind(wxEVT_RIGHT_UP, &TASInputDlg::OnRightClickSlider, this);
  }

  for (unsigned int i = 4; i < ArraySize(m_buttons); ++i)
    if (m_buttons[i] != nullptr)
      m_buttons_grid->Add(m_buttons[i]->checkbox);
  m_buttons_grid->AddSpacer(5);

  m_buttons_box->Add(m_buttons_grid);
  m_buttons_box->Add(m_buttons_dpad);

  m_wiimote_szr->Add(m_main_stick_szr, 0, wxALL, 5);
  m_wiimote_szr->Add(axisBox, 0, wxTOP | wxRIGHT, 5);
  m_wiimote_szr->Add(m_buttons_box, 0, wxTOP | wxRIGHT, 5);
  m_main_szr->Add(m_wiimote_szr);
  m_main_szr->Add(m_ext_szr);
  m_main_szr->Add(m_cc_szr);

  HandleExtensionChange();
  FinishLayout();
}

void TASInputDlg::FinishLayout()
{
  Bind(wxEVT_CLOSE_WINDOW, &TASInputDlg::OnCloseWindow, this);
  Bind(INVALIDATE_BUTTON_EVENT, &TASInputDlg::UpdateFromInvalidatedButton, this);
  Bind(INVALIDATE_CONTROL_EVENT, &TASInputDlg::UpdateFromInvalidatedControl, this);
  Bind(INVALIDATE_EXTENSION_EVENT, &TASInputDlg::UpdateFromInvalidatedExtension, this);
  m_has_layout = true;
}

wxBoxSizer* TASInputDlg::CreateCCLayout()
{
  wxBoxSizer* const szr = new wxBoxSizer(wxHORIZONTAL);

  for (size_t i = 0; i < ArraySize(m_cc_buttons); ++i)
  {
    m_cc_buttons[i] = CreateButton(m_cc_button_names[i]);
    m_cc_buttons[i].checkbox->SetClientData(&m_cc_buttons[i]);
  }

  m_cc_controls[CC_L_STICK_X] = &m_cc_l_stick.x_cont;
  m_cc_controls[CC_L_STICK_Y] = &m_cc_l_stick.y_cont;
  m_cc_controls[CC_R_STICK_X] = &m_cc_r_stick.x_cont;
  m_cc_controls[CC_R_STICK_Y] = &m_cc_r_stick.y_cont;
  m_cc_controls[CC_L_TRIGGER] = &m_cc_l;
  m_cc_controls[CC_R_TRIGGER] = &m_cc_r;

  m_cc_l_stick_szr = CreateStickLayout(&m_cc_l_stick, _("Left stick"));
  m_cc_r_stick_szr = CreateStickLayout(&m_cc_r_stick, _("Right stick"));

  m_cc_l = CreateControl(wxSL_VERTICAL, -1, 100, false, 31, 0);
  m_cc_r = CreateControl(wxSL_VERTICAL, -1, 100, false, 31, 0);

  wxStaticBoxSizer* const shoulder_box =
      new wxStaticBoxSizer(wxHORIZONTAL, this, _("Shoulder Buttons"));
  shoulder_box->Add(m_cc_l.slider, 0, wxALIGN_CENTER_VERTICAL);
  shoulder_box->Add(m_cc_l.text, 0, wxALIGN_CENTER_VERTICAL);
  shoulder_box->Add(m_cc_r.slider, 0, wxALIGN_CENTER_VERTICAL);
  shoulder_box->Add(m_cc_r.text, 0, wxALIGN_CENTER_VERTICAL);

  wxStaticBoxSizer* const cc_buttons_box = new wxStaticBoxSizer(wxVERTICAL, this, _("Buttons"));
  wxGridSizer* const cc_buttons_grid = new wxGridSizer(4);
  wxGridSizer* const cc_buttons_dpad = new wxGridSizer(3);

  cc_buttons_dpad->AddSpacer(20);
  cc_buttons_dpad->Add(m_cc_buttons[1].checkbox);
  cc_buttons_dpad->AddSpacer(20);
  cc_buttons_dpad->Add(m_cc_buttons[2].checkbox);
  cc_buttons_dpad->AddSpacer(20);
  cc_buttons_dpad->Add(m_cc_buttons[3].checkbox);
  cc_buttons_dpad->AddSpacer(20);
  cc_buttons_dpad->Add(m_cc_buttons[0].checkbox);
  cc_buttons_dpad->AddSpacer(20);

  for (auto button : m_cc_buttons)
    if (!button.checkbox->GetContainingSizer())
      cc_buttons_grid->Add(button.checkbox);
  cc_buttons_grid->AddSpacer(5);

  cc_buttons_box->Add(cc_buttons_grid);
  cc_buttons_box->Add(cc_buttons_dpad);

  szr->Add(m_cc_l_stick_szr, 0, wxALL, 5);
  szr->Add(m_cc_r_stick_szr, 0, wxALL, 5);
  szr->Add(shoulder_box, 0, wxLEFT | wxRIGHT, 5);
  szr->Add(cc_buttons_box, 0, wxTOP | wxRIGHT, 5);

  for (Control* const control : m_cc_controls)
  {
    if (control != nullptr)
      control->slider->Bind(wxEVT_RIGHT_UP, &TASInputDlg::OnRightClickSlider, this);
  }
  return szr;
}

void TASInputDlg::HandleExtensionChange()
{
  if (m_ext == 1)
  {
    m_main_szr->Hide(m_cc_szr);
    m_main_szr->Show(m_wiimote_szr);
    m_main_szr->Show(m_ext_szr);
  }
  else if (m_ext == 2)
  {
    m_main_szr->Hide(m_ext_szr);
    m_main_szr->Hide(m_wiimote_szr);
    m_main_szr->Show(m_cc_szr);
  }
  else
  {
    m_main_szr->Hide(m_ext_szr);
    m_main_szr->Hide(m_cc_szr);
    m_main_szr->Show(m_wiimote_szr);
  }
  SetSizerAndFit(m_main_szr, true);
  ResetValues();
}

void TASInputDlg::CreateGCLayout()
{
  if (m_has_layout)
    return;

  CreateBaseLayout();

  m_buttons[6] = &m_x;
  m_buttons[7] = &m_y;
  m_buttons[8] = &m_z;
  m_buttons[9] = &m_l;
  m_buttons[10] = &m_r;
  m_buttons[11] = &m_start;

  m_controls[2] = &m_c_stick.x_cont;
  m_controls[3] = &m_c_stick.y_cont;
  m_controls[4] = &m_l_cont;
  m_controls[5] = &m_r_cont;

  wxBoxSizer* const top_box = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* const bottom_box = new wxBoxSizer(wxHORIZONTAL);
  m_main_stick = CreateStick(ID_MAIN_STICK, 255, 255, 128, 128, false, true);
  wxStaticBoxSizer* const main_box = CreateStickLayout(&m_main_stick, _("Main Stick"));

  m_c_stick = CreateStick(ID_C_STICK, 255, 255, 128, 128, false, true);
  wxStaticBoxSizer* const c_box = CreateStickLayout(&m_c_stick, _("C Stick"));

  wxStaticBoxSizer* const shoulder_box =
      new wxStaticBoxSizer(wxHORIZONTAL, this, _("Shoulder Buttons"));
  m_l_cont = CreateControl(wxSL_VERTICAL, -1, 100, false, 255, 0);
  m_r_cont = CreateControl(wxSL_VERTICAL, -1, 100, false, 255, 0);
  shoulder_box->Add(m_l_cont.slider, 0, wxALIGN_CENTER_VERTICAL);
  shoulder_box->Add(m_l_cont.text, 0, wxALIGN_CENTER_VERTICAL);
  shoulder_box->Add(m_r_cont.slider, 0, wxALIGN_CENTER_VERTICAL);
  shoulder_box->Add(m_r_cont.text, 0, wxALIGN_CENTER_VERTICAL);

  for (Control* const control : m_controls)
  {
    if (control != nullptr)
      control->slider->Bind(wxEVT_RIGHT_UP, &TASInputDlg::OnRightClickSlider, this);
  }

  wxStaticBoxSizer* const m_buttons_box = new wxStaticBoxSizer(wxVERTICAL, this, _("Buttons"));
  wxGridSizer* const m_buttons_grid = new wxGridSizer(4);

  m_x = CreateButton("X");
  m_x.checkbox->SetClientData(&m_x);
  m_y = CreateButton("Y");
  m_y.checkbox->SetClientData(&m_y);
  m_l = CreateButton("L");
  m_l.checkbox->SetClientData(&m_l);
  m_r = CreateButton("R");
  m_r.checkbox->SetClientData(&m_r);
  m_z = CreateButton("Z");
  m_z.checkbox->SetClientData(&m_z);
  m_start = CreateButton("Start");
  m_start.checkbox->SetClientData(&m_start);

  for (unsigned int i = 4; i < ArraySize(m_buttons); ++i)
    if (m_buttons[i] != nullptr)
      m_buttons_grid->Add(m_buttons[i]->checkbox, false);
  m_buttons_grid->AddSpacer(5);

  m_buttons_box->Add(m_buttons_grid);
  m_buttons_box->Add(m_buttons_dpad);

  wxBoxSizer* const main_szr = new wxBoxSizer(wxVERTICAL);

  top_box->Add(main_box, 0, wxALL, 5);
  top_box->Add(c_box, 0, wxTOP | wxRIGHT, 5);
  bottom_box->Add(shoulder_box, 0, wxLEFT | wxRIGHT, 5);
  bottom_box->Add(m_buttons_box, 0, wxBOTTOM, 5);
  main_szr->Add(top_box);
  main_szr->Add(bottom_box);
  SetSizerAndFit(main_szr);

  ResetValues();
  FinishLayout();
}

TASInputDlg::Control TASInputDlg::CreateControl(long style, int width, int height, bool reverse,
                                                u32 range, u32 default_value)
{
  Control tempCont;
  tempCont.range = range;
  tempCont.default_value = default_value;
  tempCont.slider = new wxSlider(this, m_eleID++, default_value, 0, range, wxDefaultPosition,
                                 wxDefaultSize, style);
  tempCont.slider->SetMinSize(wxSize(width, height));
  tempCont.slider->Bind(wxEVT_SLIDER, &TASInputDlg::UpdateFromSliders, this);
  tempCont.text = new wxTextCtrl(this, m_eleID++, std::to_string(default_value), wxDefaultPosition,
                                 wxSize(40, 20));
  tempCont.text->SetMaxLength(range > 999 ? 4 : 3);
  tempCont.text_id = m_eleID - 1;
  tempCont.text->Bind(wxEVT_TEXT, &TASInputDlg::UpdateFromText, this);
  tempCont.slider_id = m_eleID - 2;
  tempCont.reverse = reverse;
  return tempCont;
}

TASInputDlg::Stick TASInputDlg::CreateStick(int id_stick, int xRange, int yRange, u32 defaultX,
                                            u32 defaultY, bool reverseX, bool reverseY)
{
  Stick tempStick;
  tempStick.bitmap = new wxStaticBitmap(this, id_stick, CreateStickBitmap(128, 128));
  tempStick.bitmap->Bind(wxEVT_MOTION, &TASInputDlg::OnMouseDownL, this);
  tempStick.bitmap->Bind(wxEVT_LEFT_DOWN, &TASInputDlg::OnMouseDownL, this);
  tempStick.bitmap->Bind(wxEVT_RIGHT_UP, &TASInputDlg::OnMouseUpR, this);
  tempStick.x_cont = CreateControl(wxSL_HORIZONTAL | (reverseX ? wxSL_INVERSE : 0), 120, -1,
                                   reverseX, xRange, defaultX);
  tempStick.y_cont = CreateControl(wxSL_VERTICAL | wxSL_LEFT | (reverseY ? wxSL_INVERSE : 0), -1,
                                   120, reverseY, yRange, defaultY);
  return tempStick;
}

wxStaticBoxSizer* TASInputDlg::CreateStickLayout(Stick* tempStick, const wxString& title)
{
  wxStaticBoxSizer* const temp_box = new wxStaticBoxSizer(wxHORIZONTAL, this, title);
  wxFlexGridSizer* grid = new wxFlexGridSizer(2, 3, 3);

  grid->Add(tempStick->x_cont.slider, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM);
  grid->Add(tempStick->x_cont.text, 0, wxEXPAND);
  grid->Add(tempStick->bitmap, 0, wxALL | wxALIGN_CENTER, 3);
  grid->Add(tempStick->y_cont.slider, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
  grid->Add(1, 1);
  grid->Add(tempStick->y_cont.text, 0, wxEXPAND);
  temp_box->Add(grid, 1, wxEXPAND | wxALL, 3);
  return temp_box;
}

wxStaticBoxSizer* TASInputDlg::CreateAccelLayout(Control* x, Control* y, Control* z,
                                                 const wxString& title)
{
  wxStaticBoxSizer* const temp_box = new wxStaticBoxSizer(wxHORIZONTAL, this, title);
  wxStaticBoxSizer* const xBox = new wxStaticBoxSizer(wxVERTICAL, this, _("X"));
  wxStaticBoxSizer* const yBox = new wxStaticBoxSizer(wxVERTICAL, this, _("Y"));
  wxStaticBoxSizer* const zBox = new wxStaticBoxSizer(wxVERTICAL, this, _("Z"));

  xBox->Add(x->slider, 0, wxALIGN_CENTER_HORIZONTAL);
  xBox->Add(x->text, 0, wxALIGN_CENTER_HORIZONTAL);
  yBox->Add(y->slider, 0, wxALIGN_CENTER_HORIZONTAL);
  yBox->Add(y->text, 0, wxALIGN_CENTER_HORIZONTAL);
  zBox->Add(z->slider, 0, wxALIGN_CENTER_HORIZONTAL);
  zBox->Add(z->text, 0, wxALIGN_CENTER_HORIZONTAL);
  temp_box->Add(xBox, 0, wxLEFT | wxBOTTOM | wxRIGHT, 5);
  temp_box->Add(yBox, 0, wxRIGHT, 5);
  temp_box->Add(zBox, 0, wxRIGHT, 5);
  return temp_box;
}

TASInputDlg::Button TASInputDlg::CreateButton(const std::string& name)
{
  Button temp;
  wxCheckBox* checkbox = new wxCheckBox(this, m_eleID++, name);
  checkbox->Bind(wxEVT_RIGHT_DOWN, &TASInputDlg::SetTurbo, this);
  checkbox->Bind(wxEVT_LEFT_DOWN, &TASInputDlg::SetTurbo, this);
  checkbox->Bind(wxEVT_CHECKBOX, &TASInputDlg::OnCheckboxToggle, this);
  temp.checkbox = checkbox;
  temp.id = m_eleID - 1;
  return temp;
}

void TASInputDlg::OnCheckboxToggle(wxCommandEvent& event)
{
  auto cbox = static_cast<wxCheckBox*>(event.GetEventObject());
  static_cast<Button*>(cbox->GetClientData())->is_checked = event.IsChecked();
}

void TASInputDlg::ResetValues()
{
  for (Button* const button : m_buttons)
  {
    if (button != nullptr)
    {
      button->value = false;
      button->checkbox->SetValue(false);
    }
  }

  for (Control* const control : m_controls)
  {
    if (control != nullptr)
    {
      control->value = control->default_value;
      control->slider->SetValue(control->default_value);
      control->text->SetValue(std::to_string(control->default_value));
    }
  }
  if (m_ext == 2)
  {
    for (Button& button : m_cc_buttons)
    {
      button.value = false;
      button.checkbox->SetValue(false);
    }

    for (Control* control : m_cc_controls)
    {
      control->value = control->default_value;
      control->slider->SetValue(control->default_value);
      control->text->SetValue(std::to_string(control->default_value));
    }
  }
}

// NOTE: Host / CPU Thread
void TASInputDlg::SetStickValue(Control* control, int CurrentValue, int center)
{
  if (CurrentValue != center)
  {
    control->value = CurrentValue;
    control->set_by_keyboard = true;
  }
  else if (control->set_by_keyboard)
  {
    control->value = center;
    control->set_by_keyboard = false;
  }
  else
  {
    return;
  }

  InvalidateControl(control);
}

// NOTE: Host / CPU Thread
void TASInputDlg::SetSliderValue(Control* control, int CurrentValue)
{
  if (CurrentValue != (int)control->default_value)
  {
    control->value = CurrentValue;
    control->set_by_keyboard = true;
  }
  else if (control->set_by_keyboard)
  {
    control->value = control->default_value;
    control->set_by_keyboard = false;
  }
  else
  {
    return;
  }

  InvalidateControl(control);
}

// NOTE: Host / CPU Thread
void TASInputDlg::SetButtonValue(Button* button, bool CurrentState)
{
  if (CurrentState)
  {
    button->set_by_keyboard = true;
  }
  else if (button->set_by_keyboard)
  {
    button->set_by_keyboard = false;
  }
  else
  {
    return;
  }

  button->value = CurrentState;
  InvalidateButton(button);
}

// NOTE: Host / CPU Thread
void TASInputDlg::SetWiiButtons(u16* butt)
{
  ButtonTurbo();
}

// NOTE: Host / CPU Thread
void TASInputDlg::GetKeyBoardInput(GCPadStatus* PadStatus)
{
  SetStickValue(&m_main_stick.x_cont, PadStatus->stickX);
  SetStickValue(&m_main_stick.y_cont, PadStatus->stickY);

  SetStickValue(&m_c_stick.x_cont, PadStatus->substickX);
  SetStickValue(&m_c_stick.y_cont, PadStatus->substickY);
  SetSliderValue(&m_l_cont, PadStatus->triggerLeft);
  SetSliderValue(&m_r_cont, PadStatus->triggerRight);

  for (unsigned int i = 0; i < ArraySize(m_buttons); ++i)
  {
    if (m_buttons[i] != nullptr)
      SetButtonValue(m_buttons[i], ((PadStatus->button & m_gc_pad_buttons_bitmask[i]) != 0));
  }
  SetButtonValue(&m_l,
                 ((PadStatus->triggerLeft) == 255) || ((PadStatus->button & PAD_TRIGGER_L) != 0));
  SetButtonValue(&m_r,
                 ((PadStatus->triggerRight) == 255) || ((PadStatus->button & PAD_TRIGGER_R) != 0));
}

// NOTE: Host / CPU Thread
void TASInputDlg::GetValues(GCPadStatus* PadStatus)
{
  if (!IsShown() || !m_has_layout)
    return;

  // TODO:: Make this instant not when polled.
  GetKeyBoardInput(PadStatus);

  PadStatus->stickX = m_main_stick.x_cont.value;
  PadStatus->stickY = m_main_stick.y_cont.value;
  PadStatus->substickX = m_c_stick.x_cont.value;
  PadStatus->substickY = m_c_stick.y_cont.value;
  PadStatus->triggerLeft = m_l.is_checked ? 255 : m_l_cont.value;
  PadStatus->triggerRight = m_r.is_checked ? 255 : m_r_cont.value;

  for (unsigned int i = 0; i < ArraySize(m_buttons); ++i)
  {
    if (m_buttons[i] != nullptr)
    {
      if (m_buttons[i]->is_checked)
        PadStatus->button |= m_gc_pad_buttons_bitmask[i];
      else
        PadStatus->button &= ~m_gc_pad_buttons_bitmask[i];
    }
  }

  if (m_a.is_checked)
    PadStatus->analogA = 0xFF;
  else
    PadStatus->analogA = 0x00;

  if (m_b.is_checked)
    PadStatus->analogB = 0xFF;
  else
    PadStatus->analogB = 0x00;

  ButtonTurbo();
}

void TASInputDlg::UpdateFromSliders(wxCommandEvent& event)
{
  wxTextCtrl* text = nullptr;

  for (Control* const control : m_controls)
  {
    if (control != nullptr && event.GetId() == control->slider_id)
      text = control->text;
  }

  for (Control* const control : m_cc_controls)
  {
    if (control != nullptr && event.GetId() == control->slider_id)
      text = control->text;
  }
  int value = ((wxSlider*)event.GetEventObject())->GetValue();
  if (text)
    text->SetValue(std::to_string(value));
}

void TASInputDlg::UpdateFromText(wxCommandEvent& event)
{
  unsigned long value;

  if (!((wxTextCtrl*)event.GetEventObject())->GetValue().ToULong(&value))
    return;

  for (Control* const control : m_controls)
  {
    if (control != nullptr && event.GetId() == control->text_id)
    {
      int v = (value > control->range) ? control->range : value;
      control->slider->SetValue(v);
      control->text->ChangeValue(std::to_string(v));
      control->value = v;
    }
  }

  for (Control* const control : m_cc_controls)
  {
    if (control != nullptr && event.GetId() == control->text_id)
    {
      int v = (value > control->range) ? control->range : value;
      control->slider->SetValue(v);
      control->text->ChangeValue(std::to_string(v));
      control->value = v;
    }
  }

  if (m_controls[0] != nullptr)
    UpdateStickBitmap(m_main_stick);
  if (m_controls[2] != nullptr)
    UpdateStickBitmap(m_c_stick);
  if (m_cc_controls[CC_L_STICK_X] != nullptr)
    UpdateStickBitmap(m_cc_l_stick);
  if (m_cc_controls[CC_R_STICK_X] != nullptr)
    UpdateStickBitmap(m_cc_r_stick);
}

void TASInputDlg::UpdateStickBitmap(Stick stick)
{
  int x = (u8)(
      std::floor(((double)stick.x_cont.value / (double)(stick.x_cont.range + 1) * 255.0) + .5));
  int y = (u8)(
      std::floor(((double)stick.y_cont.value / (double)(stick.y_cont.range + 1) * 255.0) + .5));
  if (stick.x_cont.reverse)
    x = 256 - (u8)x;
  if (stick.y_cont.reverse)
    y = 256 - (u8)y;
  stick.bitmap->SetBitmap(CreateStickBitmap(x, y));
}

void TASInputDlg::OnCloseWindow(wxCloseEvent& event)
{
  if (event.CanVeto())
  {
    event.Skip(false);
    Show(false);
    ResetValues();
  }
}

TASInputDlg::Stick* TASInputDlg::FindStickByID(int id)
{
  if (id == ID_MAIN_STICK)
    return &m_main_stick;
  else if (id == ID_C_STICK)
    return &m_c_stick;
  else if (id == ID_CC_L_STICK)
    return &m_cc_l_stick;
  else if (id == ID_CC_R_STICK)
    return &m_cc_r_stick;
  else
    return nullptr;
}
void TASInputDlg::OnMouseUpR(wxMouseEvent& event)
{
  Stick* stick = FindStickByID(event.GetId());
  if (stick == nullptr)
    return;

  stick->x_cont.value = stick->x_cont.default_value;
  stick->y_cont.value = stick->y_cont.default_value;
  stick->bitmap->SetBitmap(CreateStickBitmap(128, 128));
  stick->x_cont.text->SetValue(std::to_string(stick->x_cont.default_value));
  stick->y_cont.text->SetValue(std::to_string(stick->y_cont.default_value));
  stick->x_cont.slider->SetValue(stick->x_cont.default_value);
  stick->y_cont.slider->SetValue(stick->y_cont.default_value);

  event.Skip();
}

void TASInputDlg::OnRightClickSlider(wxMouseEvent& event)
{
  for (Control* const control : m_controls)
  {
    if (control != nullptr && event.GetId() == control->slider_id)
    {
      control->value = control->default_value;
      control->slider->SetValue(control->default_value);
      control->text->SetValue(std::to_string(control->default_value));
      return;
    }
  }
  for (Control* const control : m_cc_controls)
  {
    if (control != nullptr && event.GetId() == control->slider_id)
    {
      control->value = control->default_value;
      control->slider->SetValue(control->default_value);
      control->text->SetValue(std::to_string(control->default_value));
      return;
    }
  }
}

void TASInputDlg::OnMouseDownL(wxMouseEvent& event)
{
  if (!event.LeftIsDown())
    return;

  Stick* stick = FindStickByID(event.GetId());
  if (stick == nullptr)
    return;

  wxPoint ptM(event.GetPosition());
  stick->x_cont.value = ptM.x * stick->x_cont.range / 127;
  stick->y_cont.value = ptM.y * stick->y_cont.range / 127;

  if ((unsigned)stick->y_cont.value > stick->y_cont.range)
    stick->y_cont.value = stick->y_cont.range;
  if ((unsigned)stick->x_cont.value > stick->x_cont.range)
    stick->x_cont.value = stick->x_cont.range;

  if (stick->y_cont.reverse)
    stick->y_cont.value = stick->y_cont.range - (u16)stick->y_cont.value;
  if (stick->x_cont.reverse)
    stick->x_cont.value = stick->x_cont.range - (u16)stick->x_cont.value;

  stick->x_cont.value = (unsigned int)stick->x_cont.value > stick->x_cont.range ?
                            stick->x_cont.range :
                            stick->x_cont.value;
  stick->y_cont.value = (unsigned int)stick->y_cont.value > stick->y_cont.range ?
                            stick->y_cont.range :
                            stick->y_cont.value;

  // This updates sliders and the bitmap too.
  stick->x_cont.text->SetValue(std::to_string(stick->x_cont.value));
  stick->y_cont.text->SetValue(std::to_string(stick->y_cont.value));

  event.Skip();
}

void TASInputDlg::SetTurbo(wxMouseEvent& event)
{
  Button* button = nullptr;

  for (Button* const btn : m_buttons)
  {
    if (btn != nullptr && event.GetId() == btn->id)
      button = btn;
  }
  if (m_ext == 2)
  {
    for (size_t i = 0; i < ArraySize(m_cc_buttons); ++i)
    {
      if (event.GetId() == m_cc_buttons[i].id)
        button = &m_cc_buttons[i];
    }
  }

  if (event.LeftDown())
  {
    if (button)
      button->turbo_on = false;

    event.Skip();
    return;
  }

  if (button)
  {
    button->checkbox->SetValue(true);
    button->turbo_on = !button->turbo_on;
  }

  event.Skip();
}

// NOTE: Host / CPU Thread
void TASInputDlg::ButtonTurbo()
{
  static u64 frame = Movie::GetCurrentFrame();

  if (frame != Movie::GetCurrentFrame())
  {
    frame = Movie::GetCurrentFrame();
    for (Button* const button : m_buttons)
    {
      if (button != nullptr && button->turbo_on)
      {
        button->value = !button->is_checked;
        InvalidateButton(button);
      }
    }
    if (m_ext == 2)
    {
      for (Button& button : m_cc_buttons)
      {
        if (button.turbo_on)
        {
          button.value = !button.is_checked;
          InvalidateButton(&button);
        }
      }
    }
  }
}

void TASInputDlg::InvalidateButton(Button* button)
{
  if (!wxIsMainThread())
  {
    wxCommandEvent* evt = new wxCommandEvent(INVALIDATE_BUTTON_EVENT, button->id);
    evt->SetClientData(button);
    wxQueueEvent(this, evt);
    return;
  }

  button->checkbox->SetValue(button->value);
  button->is_checked = button->value;
}

void TASInputDlg::InvalidateControl(Control* control)
{
  if (!wxIsMainThread())
  {
    wxCommandEvent* evt = new wxCommandEvent(INVALIDATE_CONTROL_EVENT, control->text_id);
    evt->SetClientData(control);
    wxQueueEvent(this, evt);
    return;
  }

  control->text->SetValue(std::to_string(control->value));
}

void TASInputDlg::InvalidateExtension()
{
  if (!wxIsMainThread())
  {
    GetEventHandler()->QueueEvent(new wxThreadEvent(INVALIDATE_EXTENSION_EVENT));
    return;
  }

  HandleExtensionChange();
}

void TASInputDlg::UpdateFromInvalidatedButton(wxCommandEvent& event)
{
  Button* button = static_cast<Button*>(event.GetClientData());
  _assert_msg_(PAD, button->id == button->checkbox->GetId(), "Button ids do not match: %i != %i",
               button->id, button->checkbox->GetId());
  button->checkbox->SetValue(button->value);
  button->is_checked = button->value;
}

void TASInputDlg::UpdateFromInvalidatedControl(wxCommandEvent& event)
{
  Control* control = static_cast<Control*>(event.GetClientData());
  _assert_msg_(PAD, control->text_id == control->text->GetId(),
               "Control ids do not match: %i != %i", control->text_id, control->text->GetId());
  control->text->SetValue(std::to_string(control->value));
}

void TASInputDlg::UpdateFromInvalidatedExtension(wxThreadEvent&)
{
  HandleExtensionChange();
}

wxBitmap TASInputDlg::CreateStickBitmap(int x, int y)
{
  x = x / 2;
  y = y / 2;

  wxMemoryDC memDC;
  wxBitmap bitmap(129, 129);
  memDC.SelectObject(bitmap);
  memDC.SetBackground(*wxLIGHT_GREY_BRUSH);
  memDC.Clear();
  memDC.SetBrush(*wxWHITE_BRUSH);
  memDC.DrawCircle(64, 64, 64);
  memDC.SetPen(wxPen(*wxBLACK, 3, wxPENSTYLE_SOLID));
  memDC.DrawLine(64, 64, x, y);
  memDC.SetPen(*wxBLACK_PEN);
  memDC.CrossHair(64, 64);
  memDC.SetBrush(*wxBLUE_BRUSH);
  memDC.DrawCircle(x, y, 5);
  memDC.SelectObject(wxNullBitmap);
  return bitmap;
}
