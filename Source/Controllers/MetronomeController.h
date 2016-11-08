/*
 * MetronomeController.h
 *
 *  Created on: 2 Oct 2016
 *      Author: jeremy
 */

#ifndef SOURCE_CONTROLLERS_METRONOMECONTROLLER_H_
#define SOURCE_CONTROLLERS_METRONOMECONTROLLER_H_

#include <Source/Api/eXaDrums.h>

#include <gtkmm/builder.h>
#include <gtkmm/window.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/scale.h>


using namespace eXaDrumsApi;

namespace Gui
{

	class MetronomeController
	{

	public:

		MetronomeController(Glib::RefPtr<Gtk::Builder> builder, std::shared_ptr<eXaDrums> drumKit);
		virtual ~MetronomeController();

		void ShowMetronomePrefs();

	private:

		std::vector<std::string> RetrieveClickTypes() const;
		std::vector<int> RetrieveRhythmList() const;
		std::vector<int> RetrieveBpmeasList() const;

		// Signals
		void EnableClick() const;
		void ChangeTempo() const;
		void SaveMetronomeConfig();


		Glib::RefPtr<Gtk::Builder> builder;
		std::shared_ptr<eXaDrums> drumKit;

		Gtk::Window* metronomeWindow;
		Gtk::ComboBoxText* clickTypes;
		Gtk::Scale* clickTempoScale;
		Gtk::Button* metronomeConfigSave;
		Gtk::CheckButton* enableClickButton;
		Gtk::ComboBoxText* rhythmList;
		Gtk::ComboBoxText* bpmeasList;

		std::vector<int> rhythms;
		std::vector<int> bpmeasValues;

	};

} /* namespace Gui */

#endif /* SOURCE_CONTROLLERS_METRONOMECONTROLLER_H_ */