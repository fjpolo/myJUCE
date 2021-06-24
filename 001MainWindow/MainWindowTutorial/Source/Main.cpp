/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MainComponent.h"

//==============================================================================
class MainWindowTutorialApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    MainWindowTutorialApplication() {}

    const juce::String getApplicationName() override       { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        // This method is where you should put your application's initialisation code..

        mainWindow.reset (new MainWindow (getApplicationName()));
    }

    void shutdown() override
    {
        // Add your application's shutdown code here..

        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name) : DocumentWindow(
                                                        name,
                                                        juce::Colours::lightgrey,
                                                        //juce::Colours::blue,
                                                        //juce::Colours::plum,
                                                        DocumentWindow::allButtons
                                                        //(DocumentWindow::minimiseButton | DocumentWindow::maximiseButton | DocumentWindow::closeButton)
                                                      )
        {
            /*300x200px centered window*/
            centreWithSize(300, 200);
            /*800x600 window at (50px,50px)*/
            // setBounds(50, 50, 800, 600);
            /*Resizable*/
            setResizable(true, true);
            /*Native Title bar yuck*/
            setUsingNativeTitleBar(false);
            /*Change background to black*/
            setBackgroundColour(juce::Colours::black);
            /*Make it happen*/
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (MainWindowTutorialApplication)
