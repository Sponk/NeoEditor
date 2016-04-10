#include <RadioButton.h>

Neo2D::Gui::RadioButton::RadioButton(int x,
									 int y,
									 unsigned int w,
									 unsigned int h,
									 const char* label,
									 const shared_ptr<Neo2D::Object2D>& parent)
	: CheckButton(x, y, w, h, label, parent)
{
	getButtonWidget().setCallback([](Widget& w, void* d)
								  {
									  RadioButton* self = reinterpret_cast<RadioButton*>(d);
									  bool newValue = !self->getValue();

									  // Set everything to false if the widget has a parent
									  shared_ptr<RadioGroup> group;
									  if (group = dynamic_pointer_cast<RadioGroup>(self->getParent().lock()))
									  {
										  group->reset();
									  }

									  self->setValue(newValue);
								  }, this);
}