/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
#ifndef APP_POWERTABEDITOR_H
#define APP_POWERTABEDITOR_H

#include <QMainWindow>

#include <boost/scoped_ptr.hpp>
#include <score/position.h>
#include <string>
#include <vector>

class Caret;
class Command;
class DocumentManager;
class FileFormatManager;
class RecentFiles;
class ScoreArea;
class ScoreLocation;
class UndoManager;

class PowerTabEditor : public QMainWindow
{
    Q_OBJECT

public:
    PowerTabEditor();
    ~PowerTabEditor();

    /// Opens the given list of files.
    void openFiles(const std::vector<std::string> &files);

private slots:
    /// Creates a new (blank) document.
    void createNewDocument();

    /// Opens a new file. If 'filename' is empty, the user will be prompted
    /// to select a filename.
    void openFile(QString filename = "");

    /// Handle when the active tab is changed.
    void switchTab(int index);

    /// Closes the specified tab.
    /// @return True if the document was closed successfully.
    bool closeTab(int index);

    /// Closes the current document.
    /// @return True if the document was closed successfully.
    bool closeCurrentTab();

    /// Saves the current document to a new filename.
    /// @return True if the file was successfully saved.
    bool saveFileAs();

    /// Cycles through the tabs in the tab bar.
    /// @param offset Direction and number of tabs to move by
    /// (i.e. -1 moves back one tab).
    void cycleTab(int offset);

    /// Launches a dialog for the user to edit keyboard shortcuts.
    void editKeyboardShortcuts();

    /// Launches the preferences dialog.
    void editPreferences();

    /// Redraws only the given system.
    void redrawSystem(int);
    /// Redraws the entire score.
    void redrawScore();

    /// Moves the caret to the first position in the staff.
    void moveCaretToStart();
    /// Moves the caret to the right by one position.
    void moveCaretRight();
    /// Moves the caret to the left by one position.
    void moveCaretLeft();
    /// Moves the caret down by one string.
    void moveCaretDown();
    /// Moves the caret up by one string.
    void moveCaretUp();
    /// Moves the caret to the last position in the staff.
    void moveCaretToEnd();
    /// Moves the caret to the first system in the score.
    void moveCaretToFirstSection();
    /// Moves the caret to the next system in the score.
    void moveCaretToNextSection();
    /// Moves the caret to the previous system in the score.
    void moveCaretToPrevSection();
    /// Moves the caret to the last system in the score.
    void moveCaretToLastSection();
    /// Moves the caret to the next staff in the system.
    void moveCaretToNextStaff();
    /// Moves the caret to the previous staff in the system.
    void moveCaretToPrevStaff();
    /// Moves the caret to the next bar after the current position.
    void moveCaretToNextBar();
    /// Moves the caret to the last bar before the current position.
    void moveCaretToPrevBar();
    /// Moves the caret to a specific rehearsal sign.
    void gotoRehearsalSign();

    /// Inserts a new system at the end of the score.
    void insertSystemAtEnd();
    /// Inserts a new system before the current system.
    void insertSystemBefore();
    /// Inserts a new system after the current system.
    void insertSystemAfter();

    /// Adds or removes a tapped harmonic for the current note.
    void editTappedHarmonic();
    /// Adds or removes a trill for the current note.
    void editTrill();

    /// Adds or removes a player change at the current location.
    void editPlayerChange();

private:
    /// Returns the application name & version (e.g. 'Power Tab Editor 2.0').
    QString getApplicationName() const;
    /// Updates the window title with the file path of the active document.
    void updateWindowTitle();

    /// Create all of the commands for the application.
    void createCommands();
    /// Set up the menus for the application.
    void createMenus();
    /// Create the tab widget and score area.
    void createTabArea();
    /// Updates the last directory that a file was opened from.
    void setPreviousDirectory(const QString &fileName);
    /// Sets up the UI for the current document after it has been opened.
    void setupNewTab();
    /// Updates whether menu items are enabled, checked, etc. depending on the
    /// current location.
    void updateCommands();

    /// Toggles a simple position property.
    void editSimpleProperty(Command *command, Position::SimpleProperty property);

    /// Helper function to insert a system at the given index.
    void insertSystem(int index);
    /// Increases or decreases the line spacing by the given amount.
    void adjustLineSpacing(int amount);

    /// Returns the score area for the active document.
    ScoreArea *getScoreArea();
    /// Returns the caret for the active document.
    Caret &getCaret();
    /// Returns the location of the caret within the active document.
    ScoreLocation &getLocation();

    boost::scoped_ptr<DocumentManager> myDocumentManager;
    boost::scoped_ptr<FileFormatManager> myFileFormatManager;
    boost::scoped_ptr<UndoManager> myUndoManager;
    /// Tracks the last directory that a file was opened from.
    QString myPreviousDirectory;
    RecentFiles *myRecentFiles;

    QTabWidget *myTabWidget;

    QMenu *myFileMenu;
    Command *myNewDocumentCommand;
    Command *myOpenFileCommand;
    Command *myCloseTabCommand;
    Command *mySaveAsCommand;
    QMenu *myRecentFilesMenu;
    Command *myEditShortcutsCommand;
    Command *myEditPreferencesCommand;
    Command *myExitCommand;

    QMenu *myEditMenu;
    QAction *myUndoAction;
    QAction *myRedoAction;

    QMenu *myPositionMenu;
    QMenu *myPositionSectionMenu;
    Command *myFirstSectionCommand;
    Command *myNextSectionCommand;
    Command *myPrevSectionCommand;
    Command *myLastSectionCommand;
    QMenu *myPositionStaffMenu;
    Command *myStartPositionCommand;
    Command *myNextPositionCommand;
    Command *myPrevPositionCommand;
    Command *myNextStringCommand;
    Command *myPrevStringCommand;
    Command *myLastPositionCommand;    
    Command *myNextStaffCommand;
    Command *myPrevStaffCommand;
    Command *myNextBarCommand;
    Command *myPrevBarCommand;
    Command *myGoToRehearsalSignCommand;

    QMenu *mySectionMenu;
    Command *myInsertSystemAtEndCommand;
    Command *myInsertSystemBeforeCommand;
    Command *myInsertSystemAfterCommand;
    QMenu *myLineSpacingMenu;
    Command *myIncreaseLineSpacingCommand;
    Command *myDecreaseLineSpacingCommand;

    QMenu *myTabSymbolsMenu;
    Command* myTappedHarmonicCommand;
    Command *myTrillCommand;
    Command *myTapCommand;
    Command *myPickStrokeUpCommand;
    Command *myPickStrokeDownCommand;

    QMenu *myPlayerMenu;
    Command *myPlayerChangeCommand;

    QMenu *myWindowMenu;
    Command *myNextTabCommand;
    Command *myPrevTabCommand;

#if 0
    void registerCaret(Caret* caret);

protected:
    virtual void closeEvent(QCloseEvent*);

private:
    void updateScoreAreaActions(bool enable);
    bool eventFilter(QObject *obj, QEvent *ev);
    void changePositionSpacing(int offset);
    void changeNoteDuration(bool increase);
    void performSystemInsert(size_t index);
    int getCurrentPlaybackSpeed() const;

    void editSlideInto(uint8_t newSlideIntoType);
    void editSlideOutOf(uint8_t newSlideType);
    void shiftTabNumber(int direction);
    void updateNoteDuration(uint8_t duration);
    void addRest();
    void editRest(uint8_t duration);
    void editKeySignature(const SystemLocation &location);
    void editTimeSignature(const SystemLocation &location);
    void editBarline(const SystemLocation &location);

    static std::vector<Position*> getSelectedPositions();
    static std::vector<Note*> getSelectedNotes();
    PlaybackWidget* getCurrentPlaybackWidget() const;
    Mixer* getCurrentMixer();

private slots:
    void updateActions();
    void updateLocationLabel();
    void updateModified(bool);
    void updateActiveVoice(int);
    void openFileInformation();
    void performFullRedraw();
    void startStopPlayback();
    void rewindPlaybackToStart();
    bool moveCaretLeft();
    void moveCaretDown();
    void moveCaretUp();

    void gotoBarline();

    void editChordName();
    void editRehearsalSign();
    void editTempoMarker();
    void editMusicalDirection();
    void editRepeatEnding();
    void editTiedNote();
    void editHammerPull();
    void editTrill();
    void editBarlineFromCaret();
    void insertStandardBarline();
    void editArtificialHarmonic();
    void editKeySignatureFromCaret();
    void editTimeSignatureFromCaret();
    void editDynamic();
    void editVolumeSwell();
    void editIrregularGrouping(bool setAsTriplet = false);

    void removeCurrentSystem();

    void shiftForward();
    void shiftBackward();

    void clearNote();
    void clearCurrentPosition();

    void addGuitar();
    void showTuningDictionary();
    void toggleGuitarVisible(uint32_t trackIndex, bool isVisible);

    void doPaste();
    void copySelectedNotes();
    void cutSelectedNotes();

private:
    bool isPlaying;

    uint8_t activeDuration;

    Toolbox* toolBox;
    QSplitter* vertSplitter;
    QSplitter* horSplitter;

    Command* cutAct;
    Command* copyAct;
    Command* pasteAct;
    Command* fileInfoAct;

    QMenu* playbackMenu;
    Command* playPauseAct;

    Command* shiftForwardAct;
    Command* shiftBackwardAct;
    Command* clearNoteAct; // clears the active note
    Command* clearCurrentPositionAct; // clears out the entire position (either notes or barline)
    Command* gotoBarlineAct;

    Command* shiftTabNumUp; // shift tab numbers up/down by a string
    Command* shiftTabNumDown;

    QMenu* textMenu;
    Command* chordNameAct; // add/remove a chord name

    Command* increasePositionSpacingAct;
    Command* decreasePositionSpacingAct;
    Command* removeCurrentSystemAct;

    QMenu* notesMenu;
    QActionGroup* noteDurationActGroup; // only one duration can be checked at a time
    Command* wholeNoteAct; // actions for modifying the duration of a note
    Command* halfNoteAct;
    Command* quarterNoteAct;
    Command* eighthNoteAct;
    Command* sixteenthNoteAct;
    Command* thirtySecondNoteAct;
    Command* sixtyFourthNoteAct;
    Command* increaseDurationAct;
    Command* decreaseDurationAct;
    Command* dottedNoteAct; // sets a note to be dotted
    Command* doubleDottedNoteAct;
    Command* addDotAct;
    Command* removeDotAct;
    Command* tiedNoteAct; // sets a note to be tied to the previous note
    Command* noteMutedAct; // sets the note to be muted
    Command* ghostNoteAct; // set a note to be a ghost note
    Command* letRingAct;
    Command* fermataAct; // set a position as fermata
    Command* graceNoteAct;
    Command* staccatoNoteAct; // set the notes of a position to be staccato
    Command* marcatoAct; // set an accent
    Command* sforzandoAct; // set a heavy accent
    Command* tripletAct;
    Command* irregularGroupingAct;

    QMenu* octaveMenu;
    Command* octave8vaAct;
    Command* octave15maAct;
    Command* octave8vbAct;
    Command* octave15mbAct;

    QMenu* restsMenu;
    QActionGroup* restDurationsGroup; // allow only one rest duration to be selected at a time
    Command* wholeRestAct;
    Command* halfRestAct;
    Command* quarterRestAct;
    Command* eighthRestAct;
    Command* sixteenthRestAct;
    Command* thirtySecondRestAct;
    Command* sixtyFourthRestAct;
    Command* addRestAct;

    QMenu* musicSymbolsMenu;
    Command* rehearsalSignAct; // add/remove rehearsal signs
    Command* tempoMarkerAct;
    Command* keySignatureAct;
    Command* timeSignatureAct;
    Command* barlineAct;
    Command* standardBarlineAct;
    Command* musicalDirectionAct;
    Command* repeatEndingAct;
    Command* dynamicAct;
    Command* volumeSwellAct;

    Command* hammerPullAct; // add/remove hammer-on or pull-off
    Command* naturalHarmonicAct; // add/remove natural harmonics
    Command* artificialHarmonicAct;
    Command* shiftSlideAct;
    Command* legatoSlideAct;
    Command* vibratoAct; // add/remove vibrato
    Command* wideVibratoAct;
    Command* palmMuteAct; // toggle palm-muting
    Command* tremoloPickingAct; // toggle tremolo picking
    Command* arpeggioUpAct;
    Command* arpeggioDownAct;

    QMenu* slideIntoMenu;
    Command* slideIntoFromAboveAct;
    Command* slideIntoFromBelowAct;

    QMenu* slideOutOfMenu;
    Command* slideOutOfDownwardsAct;
    Command* slideOutOfUpwardsAct;

    QMenu* guitarMenu;
    Command* addGuitarAct;
    Command* tuningDictionaryAct;

    boost::scoped_ptr<QStackedWidget> mixerList;
    boost::scoped_ptr<QStackedWidget> playbackToolbarList;

    boost::shared_ptr<SkinManager> skinManager;
    boost::scoped_ptr<MidiPlayer> midiPlayer;

    boost::shared_ptr<SettingsPubSub> settingsPubSub;
    boost::shared_ptr<TuningDictionary> tuningDictionary;

#endif
};

#endif
