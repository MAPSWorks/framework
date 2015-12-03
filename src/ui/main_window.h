#ifndef MainWindow_H
#define MainWindow_H

#include <QSettings>
#include <string>
#include <cassert>

#include <QMainWindow>
#include <QAbstractListModel>

#include "ui_main_window.h"

using namespace std;

class SceneWidget;
class QFileSystemModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    void init(void);
    static MainWindow& getInstance(){
        static MainWindow my_window;
        return my_window;
    }
    
	void showInformation(const string& information);
    void updateScene();
    
	const string& getWorkspace(void) const {return workspace_;}
    
    public slots:
	bool slotShowYesNoMessageBox(const string& text, const string& informative_text);
    
    unique_ptr<Ui::MainWindowClass>& getUi() { return ui_; }
    
signals:
    void keyDownPressed(void);
	void showInformationRequested(const QString& information);
    void showStatusInformation(const QString &str);
	void showStatusRequested(const QString& status, int timeout);
    
protected:
    virtual void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    
private slots:
	void slotShowInformation(const QString& information);
    bool slotSetWorkspace(void);
    
private:
    MainWindow(QWidget* parent=0);
    virtual ~MainWindow();

    void loadSettings();
    void saveSettings();
    
    unique_ptr<Ui::MainWindowClass>            ui_;
    QSettings                                  settings_;
    string                                     workspace_;
};

#endif // MainWindow_H
