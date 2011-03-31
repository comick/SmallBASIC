// $Id$
//
// Copyright(C) 2001-2011 Chris Warren-Smith. [http://tinyurl.com/ja2ss]
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//

#include "config.h"
#include "sys.h"
#include "var.h"
#include "kw.h"
#include "pproc.h"
#include "device.h"
#include "smbas.h"
#include "keymap.h"
#include "mainwindow.h"

#include <QAbstractItemModel>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QListView>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QWidget>

extern "C" {
#include "blib_ui.h"
}

struct Form : public QWidget {
  Form(int x1, int x2, int y1, int y2) : QWidget() {
    setGeometry(x1, x2, y1, y2);
    this->cmd = 0;
    this->var = 0;
    this->kb_handle = false;
  }
  ~Form() {}
  var_t* var; // form variable contains the value of the event widget
  int cmd; // doform argument by value
  bool kb_handle; // whether doform returns on a keyboard event
};

Form* form = 0;

// control types available using the BUTTON command
enum ControlType {
  ctrl_button,
  ctrl_radio,
  ctrl_check,
  ctrl_text,
  ctrl_label,
  ctrl_listbox,
  ctrl_dropdown
};

enum Mode { m_reset, m_init, m_active, m_selected } mode = m_reset;

// whether a widget event has fired
bool form_event() { return mode == m_selected; }

int prev_x;
int prev_y;

// width and height fudge factors for when button w+h specified as -1
#define BN_W  16
#define BN_H   8
#define RAD_W 22
#define RAD_H  0

struct WidgetInfo : public QVariant {
  WidgetInfo() : QVariant(1, this) {
  }

  ControlType type;
  var_t* var;
  bool is_group_radio;

  // startup value used to check if
  // exec has altered a bound variable
  union {
    long i;
    byte* ptr;
  } orig;

  void update_var_flag() {
    switch (var->type) {
    case V_STR:
      orig.ptr = var->v.p.ptr;
      break;
    case V_ARRAY:
      orig.ptr = var->v.a.ptr;
      break;
    case V_INT:
      orig.i = var->v.i;
      break;
    default:
      orig.i = 0;
    }
  }
};

// implements abstract StringList as a list of strings
struct DropListModel : QAbstractItemModel {
  QVariantList list;
  int focus_index;

  DropListModel(const char* items, var_t* v) : QAbstractItemModel() {
    focus_index = -1;

    if (v && v->type == V_ARRAY) {
      fromArray(items, v);
      return;
    }

    // construct from a string like "Easy|Medium|Hard"
    int item_index = 0;
    int len = items ? strlen(items) : 0;
    for (int i = 0; i < len; i++) {
      const char* c = strchr(items + i, '|');
      int end_index = c ? c - items : len;
      if (end_index > 0) {
        QString s = QString::fromUtf8(items + i, end_index - i);
        list.append(s);
        i = end_index;
        if (v != 0 && v->type == V_STR && v->v.p.ptr &&
            strcasecmp((const char*)v->v.p.ptr, s.toUtf8().data()) == 0) {
          focus_index = item_index;
        }
        item_index++;
      }
    }
  }

  virtual ~DropListModel() {}

  // construct from an array of values
  void fromArray(const char* caption, var_t* v) {
    for (int i = 0; i < v->v.a.size; i++) {
      var_t* el_p = (var_t*) (v->v.a.ptr + sizeof(var_t)* i);
      if (el_p->type == V_STR) {
        list << (const char*) el_p->v.p.ptr;
        if (caption && strcasecmp((const char*)el_p->v.p.ptr, caption) == 0) {
          focus_index = i;
        }
      }
      else if (el_p->type == V_INT) {
        char buff[40];
        sprintf(buff, VAR_INT_FMT, el_p->v.i);
        list << buff;
      }
      else if (el_p->type == V_ARRAY) {
        fromArray(caption, el_p);
      }
    }
  }

  // index of the item in the model 
  QModelIndex index(int row, int column, const QModelIndex& index) const {
    return createIndex(row, column);
  }

  // parent of the model item with the given index
  QModelIndex parent(const QModelIndex &index) const {
    return createIndex(-1, -1);
  }

  // return the number of rows under the given parent
  int rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : list.count();
  }

  // return the number of columns
  int columnCount(const QModelIndex& parent) const {
    return 1;
  }

  // return the data at the given index
  QVariant data(const QModelIndex& index, int role) const {
    return list.at(index.row());
  }
};

// convert a basic array into a QString
void array_to_string(QString& s, var_t* v) {
  for (int i = 0; i < v->v.a.size; i++) {
    var_t*el_p = (var_t*) (v->v.a.ptr + sizeof(var_t)* i);
    if (el_p->type == V_STR) {
      s.append((const char*)el_p->v.p.ptr);
      s.append("\n");
    }
    else if (el_p->type == V_INT) {
      char buff[40];
      sprintf(buff, VAR_INT_FMT "\n", el_p->v.i);
      s.append(buff);
    }
    else if (el_p->type == V_ARRAY) {
      array_to_string(s, el_p);
    }
  }
}

// set basic string variable to widget state when the variable has changed
bool update_gui(QWidget* w, WidgetInfo* inf) {
  QComboBox* dropdown;
  QListView* listbox;
  DropListModel* model;

  if (inf->var->type == V_INT && inf->var->v.i != inf->orig.i) {
    // update list control with new int variable
    switch (inf->type) {
    case ctrl_dropdown:
      ((QComboBox*) w)->setCurrentIndex(inf->var->v.i);
      return true;

    case ctrl_listbox: 
      {
        QAbstractItemModel* model = ((QListView*) w)->model(); 
        ((QListView*) w)->setCurrentIndex(model->index(inf->var->v.i, 0));
      }
      return true;

    default:
      return false;
    }
  }

  if (inf->var->type == V_ARRAY && inf->var->v.p.ptr != inf->orig.ptr) {
    // update list control with new array variable
    QString s;

    switch (inf->type) {
    case ctrl_dropdown:
      delete ((QComboBox*) w)->model();
      ((QComboBox*) w)->setModel(new DropListModel(0, inf->var));
      w->layout();
      return true;

    case ctrl_listbox:
      delete ((QListView*) w)->model();
      ((QListView*) w)->setModel(new DropListModel(0, inf->var));
      ((QListView*) w)->xposition(0);
      ((QListView*) w)->yposition(0);
      ((QListView*) w)->select(0);
      w->layout();
      return true;

    case ctrl_label:
      array_to_string(s, inf->var);
      w->copy_label(s.c_str());
      break;

    case ctrl_text:
      array_to_string(s, inf->var);
      ((QPlainTextEdit*) w)->text(s.c_str());
      break;

    default:
      return false;
    }
  }

  if (inf->var->type == V_STR && inf->orig.ptr != inf->var->v.p.ptr) {
    // update list control with new string variable
    switch (inf->type) {
    case ctrl_dropdown:
      dropdown = (QComboBox*) w;
      model = (DropListModel*) dropdown->model();
      if (strchr((const char*)inf->var->v.p.ptr, '|')) {
        // create a new list of items
        delete model;
        model = new DropListModel((const char*)inf->var->v.p.ptr, 0);
        dropdown->setModel(model);
      }
      else {
        // select one of the existing list items
        int selection = model->getPosition((const char*)inf->var->v.p.ptr);
        if (selection != -1) {
          dropdown->value(selection);
        }
      }
      break;

    case ctrl_listbox:
      listbox = (QListView*) w;
      model = (DropListModel*) listbox->model();
      if (strchr((const char*)inf->var->v.p.ptr, '|')) {
        // create a new list of items
        delete model;
        model = new DropListModel((const char*)inf->var->v.p.ptr, 0);
        listbox->setModel(model);
      }
      else {
        int selection = model->getPosition((const char*)inf->var->v.p.ptr);
        if (selection != -1) {
          listbox->value(selection);
        }
      }
      break;

    case ctrl_check:
    case ctrl_radio:
      ((QCheckBox*) w)->
        value(!strcasecmp((const char*)inf->var->v.p.ptr, w->label()));
      break;

    case ctrl_label:
      w->copy_label((const char*)inf->var->v.p.ptr);
      break;

    case ctrl_text:
      ((QPlainTextEdit*)w)->text((const char*)inf->var->v.p.ptr);
      break;

    case ctrl_button:
      w->copy_label((const char*)inf->var->v.p.ptr);
      break;

    default:
      break;
    }
    return true;
  }
  return false;
}

// synchronise basic variable and widget state
void transfer_data(QWidget* w, WidgetInfo* inf) {
  QComboBox* dropdown;
  QListView* listbox;
  DropListModel* model;

  if (update_gui(w, inf)) {
    inf->update_var_flag();
    return;
  }

  // set widget state to basic variable
  switch (inf->type) {
  case ctrl_check:
    if (((QCheckBox*) w)->value()) {
      v_setstr(inf->var, w->label());
    }
    else {
      v_zerostr(inf->var);
    }
    break;

  case ctrl_radio:
    if (((QRadioButton*) w)->value() && w->label() != 0) {
      v_setstr(inf->var, w->label());
    }
    else if (!inf->is_group_radio) {
      // reset radio variable for radio that is not part of a group
      v_zerostr(inf->var);
    }
    break;

  case ctrl_text:
    if (((QPlainTextEdit*)w)->text()) {
      v_setstr(inf->var, ((QPlainTextEdit*)w)->text());
    }
    else {
      v_zerostr(inf->var);
    }
    break;

  case ctrl_dropdown:
    dropdown = (QComboBox*) w;
    model = (DropListModel*) dropdown->model();
    if (dropdown->value() != -1) {
      QString* s = model->getElementAt(dropdown->value());
      if (s) {
        v_setstr(inf->var, s->c_str());
      }
    }
    break;

  case ctrl_listbox:
    listbox = (QListView*) w;
    model = (DropListModel*) listbox->model();
    if (listbox->value() != -1) {
      QString* s = model->getElementAt(listbox->value());
      if (s) {
        v_setstr(inf->var, s->c_str());
      }
    }
    break;

  case ctrl_button:
    // update the basic variable with the button label
    v_setstr(inf->var, w->label());
    break;

  default:
    break;
  }

  // only update the gui when the variable is changed in basic code
  inf->update_var_flag();
}

// find the radio group of the given variable from within the parent
QButtonGroup* findRadioGroup(QWidget* parent, var_t* v) {
  QButtonGroup* radioGroup = 0;
  int n = parent->children();
  for (int i = 0; i < n && !radioGroup; i++) {
    QWidget* w = parent->child(i);
    if (!w->user_data()) {
      radioGroup = findRadioGroup((QButtonGroup*) w, v);
    }
    else {
      WidgetInfo* inf = (WidgetInfo*) w->property();
      if (inf->type == ctrl_radio &&
          inf->var->type == V_STR &&
          (inf->var == v || inf->var->v.p.ptr == v->v.p.ptr)) {
        // another ctrl_radio is linked to the same variable
        inf->is_group_radio = true;
        radioGroup = parent;
      }
    }
  }
  return radioGroup;
}

// radio control's belong to the same group when they share
// a common basic variable
void update_radio_group(WidgetInfo* radioInf, QRadioButton* radio) {
  var_t* v = radioInf->var;

  if (v == 0 || v->type != V_STR) {
    return;
  }

  QButtonGroup* radioGroup = findRadioGroup(form, v);

  if (!radioGroup) {
    radioGroup = new QButtonGroup(0, 0, form->width(), form->height());
    form->add(radioGroup);
  }
  else {
    radioInf->is_group_radio = true;
  }

  radioGroup->add(radio);
}

void widget_cb(Widget* w, void* v) {
  if (wnd->isRunning()) {
    WidgetInfo* inf = (WidgetInfo*) v;
    transfer_data(w, (WidgetInfo*) v);

    mode = m_selected;

    if (form->var) {
      // array type cannot be used in program select statement
      if (inf->var->type == V_ARRAY) {
        v_zerostr(form->var);
      }
      else {
        // set the form variable from the widget var
        v_set(form->var, inf->var);
      }
    }
  }
}

void update_widget(QWidget* widget, WidgetInfo* inf, QRect& rect) {
  if (rect.width() != -1) {
    widget->setFixedWidth(rect.width());
  }

  if (rect.height() != -1) {
    widget->setFixedHeight(rect.height());
  }

  if (rect.x() < 0) {
    rect.setX(prev_x - rect.x());
  }

  if (rect.y() < 0) {
    rect.setY(prev_y - rect.y());
  }

  prev_x = rect.x() + rect.width();
  prev_y = rect.y() + rect.height();

  widget->setX(rect.x());
  widget->setY(rect.y());
  widget->callback(widget_cb);
  widget->user_data(inf);

  // allow form init to update widget from basic variable
  inf->orig.ptr = 0;
  inf->orig.i = 0;

  // copy output widget colors
  widget->color(wnd->out->color());
  widget->labelcolor(wnd->out->labelcolor());
  widget->textcolor(wnd->out->labelcolor());
}

void update_button(QWidget* widget, WidgetInfo* inf,
                   const char* caption, QRect& rect, int def_w, int def_h) {
  if (rect.w() < 0 && caption != 0) {
    rect.w((int) getwidth(caption) + def_w + (-rect.w() - 1));
  }

  if (rect.h() < 0) {
    rect.h((int) (getascent() + getdescent() + def_h + (-rect.h() - 1)));
  }

  update_widget(widget, inf, rect);
  widget->copy_label(caption);
}

// create a new form
void form_create() {
  if (form == 0) {
    wnd->outputGroup->begin();
    form = new Form(wnd->out->x() + 2,
                    wnd->out->y() + 2,
                    wnd->out->w() - 2,
                    wnd->out->h() - 2);
    form->resizable(0);
    wnd->outputGroup->end();
  }
  form->begin();
  mode = m_init;
}

// prepare the form for display
void form_init() {
  if (form) {
    form->resize(2, 2, form->w() + 4, form->h() + 4);
    form->take_focus();
    form->show();
  }
}

// copy all widget fields into variables
void form_update(Group* group) {
  if (group && wnd->isRunning()) {
    int n = group->children();
    for (int i = 0; i < n; i++) {
      QWidget* w = group->child(i);
      if (!w->user_data()) {
        form_update((Group*) w);
      }
      else {
        transfer_data(w, (WidgetInfo*) w->user_data());
      }
    }
  }
}

// close the form
void form_end() {
  if (form != 0) {
    form->end();
  }
}

// destroy the form
C_LINKAGE_BEGIN void ui_reset() {
  if (form != 0) {
    form->hide();
    QObjectList children = form->children();
    // qDeleteAll(list.begin(), list.end());
    // list.clear();

    int n = form->children().count();
    for (int i = 0; i < n; i++) {
      WidgetInfo* inf = (WidgetInfo*) form->child(i)->user_data();
      delete inf;
    }
    form->clear();
    wnd->outputGroup->remove(form);
    form->parent(0);
    delete form;
    form = 0;

    wnd->out->show();
    wnd->out->update();
    wnd->take_focus();
  }
  mode = m_reset;
}

// BUTTON x, y, w, h, variable, caption [,type]
//
void cmd_button() {
  var_int_t x, y, w, h;
  var_t* v = 0;
  char* caption = 0;
  char* type = 0;

  form_create();
  if (-1 != par_massget("IIIIPSs", &x, &y, &w, &h, &v, &caption, &type)) {
    WidgetInfo* inf = new WidgetInfo();
    inf->var = v;
    QRect rect(x, y, w, h);

    if (prog_error) {
      return;
    }
    if (type) {
      if (strcasecmp("radio", type) == 0) {
        inf->type = ctrl_radio;
        inf->is_group_radio = false;
        form_end(); // add widget to RadioGroup
        QRadioButton* widget = new QRadioButton(x, y, w, h);
        update_radio_group(inf, widget);
        update_button(widget, inf, caption, rect, RAD_W, RAD_H);
      }
      else if (strcasecmp("checkbox", type) == 0 || strcasecmp("check", type) == 0) {
        inf->type = ctrl_check;
        QCheckBox* widget = new QCheckBox(x, y, w, h);
        update_button(widget, inf, caption, rect, RAD_W, RAD_H);
      }
      else if (strcasecmp("button", type) == 0) {
        inf->type = ctrl_button;
        QPushButton* widget = new QPushButton(x, y, w, h);
        update_button(widget, inf, caption, rect, BN_W, BN_H);
      }
      else if (strcasecmp("label", type) == 0) {
        inf->type = ctrl_label;
        QWidget* widget = new QWidget();
        widget->setGeometry(x, y, w, h);
        widget->align(ALIGN_LEFT | ALIGN_INSIDE | ALIGN_CLIP);
        update_button(widget, inf, caption, rect, BN_W, BN_H);
      }
      else if (strcasecmp("listbox", type) == 0 || strcasecmp("list", type) == 0) {
        inf->type = ctrl_listbox;
        QListView* widget = new QListView(x, y, w, h);
        DropListModel* model = new DropListModel(caption, v);
        widget->setModel(model);
        if (model->focus_index != -1) {
          widget->value(model->focus_index);
        }
        update_widget(widget, inf, rect);
        widget->when(WHEN_RELEASE_ALWAYS);
      }
      else if (strcasecmp("dropdown", type) == 0 || strcasecmp("choice", type) == 0) {
        inf->type = ctrl_dropdown;
        QComboBox* widget = new QComboBox(x, y, w, h);
        DropListModel* model = new DropListModel(caption, v);
        widget->setModel(model);
        if (model->focus_index != -1) {
          widget->value(model->focus_index);
        }
        update_widget(widget, inf, rect);
      }
      else {
        ui_reset();
        rt_raise("UI: UNKNOWN BUTTON TYPE: %s", type);
      }
    }
    else {
      inf->type = ctrl_button;
      QPushButton* widget = new QPushButton(x, y, w, h);
      update_button(widget, inf, caption, rect, BN_W, BN_H);
    }
  }

  form_end();
  pfree2(caption, type);
}

// TEXT x, y, w, h, variable
// When DOFORM returns the variable contains the user entered value
void cmd_text() {
  var_int_t x, y, w, h;
  var_t* v = 0;

  if (-1 != par_massget("IIIIP", &x, &y, &w, &h, &v)) {
    form_create();
    QPlainTextEdit* widget = new QPlainTextEdit(x, y, w, h);
    QRect rect(x, y, w, h);
    WidgetInfo* inf = new WidgetInfo();
    inf->var = v;
    inf->type = ctrl_text;
    update_widget(widget, inf, rect);
    if (rect.h() > (getascent() + getdescent() + BN_H)) {
      widget->type(QPlainTextEdit::MULTILINE | QPlainTextEdit::WORDWRAP);
    }
    form->end();
  }
}

// DOFORM [FLAG|VAR]
// Executes the form
void cmd_doform() {
  if (form == 0) {
    rt_raise("UI: FORM NOT READY");
    return;
  }
  else if (mode == m_init) {
    form_init();
  }

  switch (code_peek()) {
  case kwTYPE_LINE:
  case kwTYPE_EOC:
  case kwTYPE_SEP:
    form->cmd = -1;
    form->var = 0;
    break;
  default:
    if (code_isvar()) {
      form->var = code_getvarptr();
      form->cmd = -1;
    }
    else {
      var_t var;
      v_init(&var);
      eval(&var);
      form->cmd = v_getint(&var);
      form->var = 0;
      v_free(&var);

      // apply any configuration options
      switch (form->cmd) {
      case 1:
        form->kb_handle = true;
        return;
      default:
        break;
      }
    }
    break;
  };

  form_update(form);

  if (!form->cmd) {
    ui_reset();
  }
  else if (wnd->penMode) {
    mode = m_active;
    fltk::wait();
  }
  else {
    // pump system messages until there is a widget callback
    mode = m_active;

    if (form->kb_handle) {
      dev_clrkb();
    }
    while (wnd->isRunning() && mode == m_active) {
      fltk::wait();

      if (form->kb_handle && keymap_kbhit()) {
        break;
      }
    }
    form_update(form);
  }
}

C_LINKAGE_END

// End of "$Id$".