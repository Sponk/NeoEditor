#ifndef SWIG_CASTS
#define SWIG_CASTS

#define OBJECT_CAST(from) shared_ptr<Neo2D::Object2D> toObject(shared_ptr<from> t) \
							{ return static_pointer_cast<Neo2D::Object2D>(t); }

#define WIDGET_CAST(from) shared_ptr<Neo2D::Gui::Widget> toWidget(shared_ptr<from> t) \
							{ return static_pointer_cast<Neo2D::Gui::Widget>(t); }

OBJECT_CAST(Neo2D::Gui::Widget);
OBJECT_CAST(Neo2D::Gui::Button);
OBJECT_CAST(Neo2D::Gui::CheckButton);
OBJECT_CAST(Neo2D::Gui::EditField);
OBJECT_CAST(Neo2D::Gui::ImageButton);
OBJECT_CAST(Neo2D::Gui::Label);
OBJECT_CAST(Neo2D::Sprite);

WIDGET_CAST(Neo2D::Gui::Widget);
WIDGET_CAST(Neo2D::Gui::Button);
WIDGET_CAST(Neo2D::Gui::CheckButton);
WIDGET_CAST(Neo2D::Gui::EditField);
WIDGET_CAST(Neo2D::Gui::ImageButton);
WIDGET_CAST(Neo2D::Gui::Label);

#endif