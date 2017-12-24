// This file is part of SmallBASIC
//
// Copyright(C) 2001-2015 Chris Warren-Smith.
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//

#include "config.h"

#include "common/sbapp.h"
#include "common/fs_socket_client.h"
#include "ui/textedit.h"
#include "platform/sdl/runtime.h"
#include "platform/sdl/settings.h"

using namespace strlib;

#define MAX_MACRO 20

String g_exportAddr;
String g_exportToken;
int g_macro[MAX_MACRO];
int g_macro_size;
bool g_macro_record;
bool g_returnToLine;

struct StatusMessage {
  explicit StatusMessage(TextEditInput *editor) :
    _dirty(editor->isDirty()),
    _row(editor->getRow()),
    _col(editor->getCol()) {
  }

  void setFilename(String &loadPath) {
    int i = loadPath.lastIndexOf('/', 0);
    if (i != -1) {
      _fileName = loadPath.substring(i + 1);
    } else {
      _fileName = loadPath;
    }
  }

  bool update(TextEditInput *editor, AnsiWidget *out, bool force=false) {
    bool result;
    bool dirty = editor->isDirty();
    if (force
        || _dirty != dirty
        || _row != editor->getRow()
        || _col != editor->getCol()) {
      String message;
      result = true;
      if (dirty) {
        message.append(" * ");
      } else {
        message.append(" - ");
      }
      message.append(_fileName)
        .append(" Ctrl+h (C-h)=Help (")
        .append(editor->getRow())
        .append(",")
        .append(editor->getCol())
        .append(") ");
      int digits = snprintf(NULL, 0, "%d%d",
                            editor->getRow(), editor->getCol());
      int spaces = 6 - digits;
      for (int i = 0; i < spaces; i++) {
        message.append(' ');
      }
      if (!editor->getScroll()) {
        message.append("Top");
      } else if (editor->getLines() - editor->getScroll() < editor->getPageRows()) {
        message.append("Bot");
      } else {
        int pos = editor->getRow() * 100 / editor->getLines();
        message.append(pos).append("%");
      }
      out->setStatus(message);
      _dirty = dirty;
      _row = editor->getRow();
      _col = editor->getCol();
    } else {
      result = false;
    }
    return result;
  }

  bool _dirty;
  int _row;
  int _col;
  String _fileName;
};

void onlineHelp(Runtime *runtime, TextEditInput *widget) {
  char path[100];
  const char *nodeId = widget->getNodeId();
  if (nodeId != NULL && nodeId[0] != '0') {
    sprintf(path, "https://smallbasic.sourceforge.io?q=node/%s", nodeId);
  } else {
    char *selection = widget->getWordBeforeCursor();
    if (selection != NULL) {
      sprintf(path, "https://smallbasic.sourceforge.io?q=search/node/%s", selection);
      free(selection);
    } else {
      sprintf(path, "https://smallbasic.sourceforge.io");
    }
  }
  runtime->browseFile(path);
}

void showRecentFiles(TextEditHelpWidget *helpWidget, String &loadPath) {
  String fileList;
  helpWidget->createMessage();
  helpWidget->show();
  helpWidget->reload(NULL);
  getRecentFileList(fileList, loadPath);
  helpWidget->setText(fileList);
}

void showSelectionCount(AnsiWidget *out, TextEditInput *widget) {
  int lines, chars;
  widget->getSelectionCounts(&lines, &chars);
  String label = "Region has ";
  label.append(lines).append(" line");
  if (lines > 1) {
    label.append("s, ");
  } else {
    label.append(", ");
  }
  label.append(chars).append(" character");
  if (chars > 1) {
    label.append("s.");
  } else {
    label.append(".");
  }
  out->setStatus(label);
}

void exportBuffer(AnsiWidget *out, const char *text, String &dest, String &token) {
  char buffer[PATH_MAX];
  dev_file_t f;
  memset(&f, 0, sizeof(dev_file_t));

  sprintf(f.name, "SOCL:%s\n", dest.c_str());
  if (dest.indexOf(':', 0) != -1 && sockcl_open(&f)) {
    sprintf(buffer, "# %s\n", token.c_str());
    sockcl_write(&f, (byte *)buffer, strlen(buffer));
    if (!sockcl_write(&f, (byte *)text, strlen(text))) {
      sprintf(buffer, "Failed to write: %s", dest.c_str());
    } else {
      sprintf(buffer, "Exported file to %s", dest.c_str());
    }
    sockcl_close(&f);
  } else {
    sprintf(buffer, "Failed to open: %s", dest.c_str());
  }
  out->setStatus(buffer);
}

void publish(System *system, const char *text, const char *fileName, const char *description) {
  String gist = saveGist(text, fileName, description);
  if (gist.empty()) {
    system->alert("Publish", "Failed to save gist file.");
  } else {
    String command;
    var_t result;

    command.append("curl -X POST -d @")
      .append(gist)
      .append("  https://api.github.com/gists")
      .append(" --header \"Content-Type:application/json\"");
    v_init(&result);
    if (!dev_run(command, &result, 1)) {
      system->alert("Publish", "Failed to invoke curl.");
    } else {
      const char *str = v_str(&result);
      const char *field = "html_url";
      const char *url = strstr(str, field);
      String html;

      if (url != NULL) {
        const char *q1 = strchr(url + strlen(field) + 2, '\"');
        const char *q2 = q1 == NULL ? NULL : strchr(q1 + 1, '\"');
        if (q1 != NULL && q2 != NULL) {
          html.append(q1 + 1, q2 - q1 - 1);
        }
      }
      if (html.empty()) {
        system->alert("Publish", "Failed to publish gist.");
      } else {
        system->browseFile(html);
      }
    }
  }
}

void exportRun(Runtime *runtime, TextEditInput *editor) {
  char path[PATH_MAX];
  getScratchFile(path, sizeof(path));
  char *buffer = editor->getTextSelection();
  FILE *fp = fopen(path, "wb");
  if (fp) {
    fputs(buffer, fp);
    fputs("\npause\n", fp);
    fclose(fp);
    runtime->exportRun(path);
  } else {
    runtime->alert("Run", "Failed to save scratch file.");
  }
  free(buffer);
}

void System::editSource(String loadPath) {
  logEntered();

  int w = _output->getWidth();
  int h = _output->getHeight();
  int charWidth = _output->getCharWidth();
  int charHeight = _output->getCharHeight();
  int prevScreenId = _output->selectScreen(SOURCE_SCREEN);
  TextEditInput *editWidget;
  if (_editor != NULL) {
    editWidget = _editor;
    editWidget->_width = w;
    editWidget->_height = h;
  } else {
    editWidget = new TextEditInput(_programSrc, charWidth, charHeight, 0, 0, w, h);
  }
  TextEditHelpWidget *helpWidget = new TextEditHelpWidget(editWidget, charWidth, charHeight);
  TextEditInput *widget = editWidget;
  String recentFile;
  StatusMessage statusMessage(editWidget);
  enum InputMode {
    kInit, kExportAddr, kExportToken, kCommand, kPublish
  } inputMode = kInit;

  _modifiedTime = getModifiedTime();
  editWidget->updateUI(NULL, NULL);
  editWidget->setLineNumbers();
  editWidget->setFocus(true);
  statusMessage.setFilename(loadPath);

  _output->clearScreen();
  _output->addInput(editWidget);
  _output->addInput(helpWidget);

  if (isBreak() && g_returnToLine) {
    // break running program - position to last program line
    editWidget->setCursorRow(gsb_last_line);
    statusMessage.update(editWidget, _output, true);
  } else if (gsb_last_error && !isBack()) {
    // program stopped with an error
    editWidget->setCursorRow(gsb_last_line + editWidget->getSelectionRow() - 1);
    if (_stackTrace.size()) {
      helpWidget->setText(gsb_last_errmsg);
      helpWidget->createStackTrace(gsb_last_errmsg, gsb_last_line, _stackTrace);
      widget = helpWidget;
      helpWidget->show();
      _output->setStatus("Error. Esc=Close, Up/Down=Caller");
    } else {
      _output->setStatus(!gsb_last_errmsg[0] ? "Error" : gsb_last_errmsg);
    }
  } else {
    statusMessage.update(editWidget, _output, true);
  }

  _srcRendered = false;
  _stackTrace.removeAll();
  _output->redraw();
  _state = kEditState;

  showCursor(kIBeam);
  setRecentFile(loadPath.c_str());
  setWindowTitle(statusMessage._fileName);

  while (_state == kEditState) {
    MAEvent event = getNextEvent();
    switch (event.type) {
    case EVENT_TYPE_OPTIONS_BOX_BUTTON_CLICKED:
      if (statusMessage.update(editWidget, _output)) {
        _output->redraw();
      }
      break;
    case EVENT_TYPE_KEY_PRESSED:
      if (_userScreenId == -1) {
        dev_clrkb();
        int sw = _output->getScreenWidth();
        bool redraw = true;
        char *text;

        if (_modifiedTime != 0 && _modifiedTime != getModifiedTime()) {
          const char *msg = "Do you want to reload the file?";
          if (ask("File has changed on disk", msg, false) == 0) {
            loadSource(loadPath.c_str());
            editWidget->reload(_programSrc);
          }
          _modifiedTime = getModifiedTime();
          event.key = 0;
        }

        switch (event.key) {
        case SB_KEY_CTRL('5'):
          _output->setStatus("Recording keyboard macro");
          g_macro_record = true;
          g_macro_size = 0;
          break;
        case SB_KEY_CTRL('6'):
          g_macro_record = false;
          break;
        case SB_KEY_CTRL('7'):
          g_macro_record = false;
          for (int i = 0; i < g_macro_size; i++) {
            redraw |= widget->edit(g_macro[i], sw, charWidth);
          }
          break;
        case SB_KEY_F(12):
        case SB_KEY_MENU:
          redraw = false;
        break;
        case SB_KEY_ESCAPE:
          widget = editWidget;
          helpWidget->hide();
          debugStop();
          break;
        case SB_KEY_CTRL('s'):
          saveFile(editWidget, loadPath);
          break;
        case SB_KEY_CTRL('c'):
        case SB_KEY_CTRL('x'):
        case SB_KEY_CTRL(SB_KEY_INSERT):
          text = widget->copy(event.key == (int)SB_KEY_CTRL('x'));
        if (text) {
          setClipboardText(text);
          free(text);
        }
        break;
        case SB_KEY_F(1):
        case SB_KEY_ALT('h'):
          _output->setStatus("Keyword Help. F2=online, Esc=Close");
          widget = helpWidget;
          helpWidget->createKeywordIndex();
          helpWidget->show();
          break;
        case SB_KEY_F(2):
          redraw = false;
          onlineHelp((Runtime *)this, editWidget);
          break;
        case SB_KEY_F(4):
          if (editWidget->getTextLength() && !g_exportAddr.empty() && !g_exportToken.empty()) {
            exportBuffer(_output, editWidget->getText(), g_exportAddr, g_exportToken);
            break;
          }
          // else fallthru to F3 handler
        case SB_KEY_F(3):
          if (editWidget->getTextLength()) {
            saveFile(editWidget, loadPath);
            _output->setStatus("Export to mobile SmallBASIC. Enter <IP>:<Port>");
            widget = helpWidget;
            helpWidget->createLineEdit(g_exportAddr);
            helpWidget->show();
            inputMode = kExportAddr;
          }
          break;
        case SB_KEY_F(5):
          saveFile(editWidget, loadPath);
          _output->setStatus("Debug. F6=Step, F7=Continue, Esc=Close");
          widget = helpWidget;
          helpWidget->createMessage();
          helpWidget->show();
          debugStart(editWidget, loadPath.c_str());
          statusMessage._row = editWidget->getRow();
          statusMessage._col = editWidget->getCol();
          break;
        case SB_KEY_F(6):
          debugStep(editWidget, helpWidget, false);
          break;
        case SB_KEY_F(7):
          debugStep(editWidget, helpWidget, true);
          break;
        case SB_KEY_F(8):
          exportRun((Runtime *)this, editWidget);
          break;
        case SB_KEY_F(9):
        case SB_KEY_CTRL('r'):
          if (((Runtime *)this)->debugActive()) {
            exportRun((Runtime *)this, editWidget);
          } else {
            _state = kRunState;
          }
        break;
        case SB_KEY_F(10):
          _output->setStatus("Enter program command line, Esc=Close");
          widget = helpWidget;
          helpWidget->createLineEdit(opt_command);
          helpWidget->show();
          inputMode = kCommand;
          break;
        case SB_KEY_F(11):
          if (editWidget->getTextLength()) {
            _output->setStatus("Enter description, Esc=Close [Publish on GitHub]");
            widget = helpWidget;
            helpWidget->createLineEdit("");
            helpWidget->show();
            inputMode = kPublish;
          }
          break;
        case SB_KEY_CTRL('h'):
          _output->setStatus("Keystroke help. Esc=Close");
          widget = helpWidget;
          helpWidget->createHelp();
          helpWidget->show();
          break;
        case SB_KEY_CTRL('l'):
          _output->setStatus("Outline. Esc=Close");
          widget = helpWidget;
          helpWidget->createOutline();
          helpWidget->show();
          break;
        case SB_KEY_CTRL('f'):
          _output->setStatus("Find in buffer. Esc=Close");
          widget = helpWidget;
          helpWidget->createSearch(false);
          helpWidget->show();
          break;
        case SB_KEY_CTRL('n'):
          _output->setStatus("Replace string. Esc=Close");
          widget = helpWidget;
          helpWidget->createSearch(true);
          helpWidget->show();
          break;
        case SB_KEY_ALT('g'):
          _output->setStatus("Goto line. Esc=Close");
          widget = helpWidget;
          helpWidget->createGotoLine();
          helpWidget->show();
          break;
        case SB_KEY_CTRL(' '):
          _output->setStatus("Auto-complete. Esc=Close");
          widget = helpWidget;
          helpWidget->createCompletionHelp();
          helpWidget->show();
          break;
        case SB_KEY_CTRL('v'):
        case SB_KEY_SHIFT(SB_KEY_INSERT):
          text = getClipboardText();
          widget->paste(text);
          free(text);
          break;
        case SB_KEY_CTRL('o'):
          _output->selectScreen(USER_SCREEN1);
          showCompletion(true);
          _output->redraw();
          _state = kActiveState;
          waitForBack();
          _output->selectScreen(SOURCE_SCREEN);
          _state = kEditState;
          break;
        case SB_KEY_ALT('0'):
          _output->setStatus("Recent files. Esc=Close");
          widget = helpWidget;
          showRecentFiles(helpWidget, loadPath);
          break;
        case SB_KEY_ALT('.'):
          g_returnToLine = !g_returnToLine;
          _output->setStatus(g_returnToLine ?
                             "Position the cursor to the last program line after BREAK" :
                             "BREAK restores current cursor position");
          break;
        case SB_KEY_ALT('='):
          showSelectionCount(_output, editWidget);
          break;
        case SB_KEY_ALT('1'):
        case SB_KEY_ALT('2'):
        case SB_KEY_ALT('3'):
        case SB_KEY_ALT('4'):
        case SB_KEY_ALT('5'):
        case SB_KEY_ALT('6'):
        case SB_KEY_ALT('7'):
        case SB_KEY_ALT('8'):
        case SB_KEY_ALT('9'):
          if (editWidget->isDirty()) {
            saveFile(editWidget, loadPath);
          }
          if (getRecentFile(recentFile, event.key - SB_KEY_ALT('1'))) {
            if (loadSource(recentFile.c_str())) {
              editWidget->reload(_programSrc);
              statusMessage.setFilename(loadPath);
              setLoadPath(recentFile);
              setWindowTitle(statusMessage._fileName);
              loadPath = recentFile;
              if (helpWidget->messageMode() && helpWidget->isVisible()) {
                showRecentFiles(helpWidget, loadPath);
              }
            } else {
              String message("Failed to load recent file: ");
              message.append(recentFile);
              _output->setStatus(message);
            }
          }
          _modifiedTime = getModifiedTime();
          break;
        default:
          redraw = widget->edit(event.key, sw, charWidth);
          break;
        }
        if (g_macro_record && g_macro_size < MAX_MACRO &&
            event.key != (int)SB_KEY_CTRL('5')) {
          g_macro[g_macro_size++] = event.key;
        }
        if (event.key == SB_KEY_ENTER) {
          if (helpWidget->replaceMode()) {
            _output->setStatus("Replace string with. Esc=Close");
          } else if (helpWidget->lineEditMode() && helpWidget->getTextLength()) {
            switch (inputMode) {
            case kExportAddr:
              g_exportAddr = helpWidget->getText();
              inputMode = kExportToken;
              helpWidget->createLineEdit(g_exportToken);
              _output->setStatus("Enter token. Esc=Close");
              break;
            case kExportToken:
              g_exportToken = helpWidget->getText();
              inputMode = kInit;
              widget = editWidget;
              exportBuffer(_output, editWidget->getText(), g_exportAddr, g_exportToken);
              helpWidget->hide();
              break;
            case kCommand:
              strlcpy(opt_command, helpWidget->getText(), sizeof(opt_command));
              inputMode = kInit;
              widget = editWidget;
              helpWidget->hide();
              break;
            case kPublish:
              _output->setStatus("Sending gist...");
              _output->redraw();
              publish(this, editWidget->getText(), statusMessage._fileName, helpWidget->getText());
              inputMode = kInit;
              widget = editWidget;
              helpWidget->hide();
              statusMessage._dirty = !widget->isDirty();
              break;
            default:
              break;
            }
          } else if (helpWidget->closeOnEnter() && helpWidget->isVisible()) {
            statusMessage._dirty = !widget->isDirty();
            widget = editWidget;
            helpWidget->hide();
          }
          redraw = true;
        }

        helpWidget->setFocus(widget == helpWidget);
        editWidget->setFocus(widget == editWidget);
        redraw |= statusMessage.update(editWidget, _output);
        if (redraw) {
          _output->redraw();
        }
      }
    }
    if ((isBack() || isClosing()) && editWidget->isDirty()) {
      const char *message = "The current file has not been saved.\n"
                            "Would you like to save it now?";
      int choice = ask("Save changes?", message, isBack());
      if (choice == 0) {
        saveFile(editWidget, loadPath);
      } else if (choice == 2) {
        // cancel
        _state = kEditState;
      }
    }
  }

  if (_state == kRunState) {
    // allow the editor to be restored on return
    if (!_output->removeInput(editWidget)) {
      trace("Failed to remove editor input");
    }
    _editor = editWidget;
    _editor->setFocus(false);
  } else {
    _editor = NULL;
  }

  // deletes editWidget unless it has been removed
  _output->removeInputs();
  if (!isClosing()) {
    _output->selectScreen(prevScreenId, false);
  }
  logLeaving();
}
